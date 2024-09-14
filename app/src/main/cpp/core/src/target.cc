#include <fstream>
#include <sstream>
#include <vector>
#include <asm-generic/fcntl.h>
#include <fcntl.h>
#include <unistd.h>
#include "hakka.h"

hakka::Target::Target(pid_t pid, MemoryMode mode) {
    this->pid = pid;
    this->memoryMode = mode;
    this->memFd = 0;
    this->pagemapFd = 0;
}

hakka::Target::~Target() {
    if (this->memFd != 0) {
        close(this->memFd);
        this->memFd = 0;
    }
    if (this->pagemapFd != 0) {
        close(this->pagemapFd);
        this->pagemapFd = 0;
    }
}

auto hakka::Target::isAlive() const -> bool {
    char stat[256];
    std::string filePath = "/proc/" + std::to_string(this->pid) + "/stat";
    FILE *_fp = fopen(filePath.c_str(), "r");
    if (_fp != nullptr) {
        std::fgets(stat, sizeof(stat), _fp);
        std::fclose(_fp);
        std::istringstream iss((std::string(stat)));
        std::string token;
        for (int i = 0; i < 3; ++i) {
            iss >> token;
        }
        // Running || Sleeping || Uninterruptible Sleep
        if (token == "R" || token == "S" || token == "D") {
            return true;
        }
    }
    return false;
}

void hakka::Target::read(ptr_t addr, void *data, size_t len) {
    auto entry = getPageEntry(addr);
    if (!entry.present) {
        throw std::runtime_error("The page is not present.");
    }
    switch (this->memoryMode) {
        case hakka::DIRECT: {
            readByDirect(addr, data, len);
            break;
        }
        case hakka::MEM_FILE: {
            initMemFd();
            readByMem(addr, data, len);
            break;
        }
        case hakka::SYSCALL: {
            readBySyscall(addr, data, len);
            break;
        }
        default: {
            throw std::runtime_error("Not support the MemoryMode.");
        }
    }
}

void hakka::Target::write(ptr_t addr, void *data, size_t len) {
    switch (this->memoryMode) {
        case hakka::DIRECT: {
            writeByDirect(addr, data, len);
            break;
        }
        case hakka::MEM_FILE: {
            initMemFd();
            writeByMem(addr, data, len);
            break;
        }
        case hakka::SYSCALL: {
            writeBySyscall(addr, data, len);
            break;
        }
        default: {
            throw std::runtime_error("Not support the MemoryMode.");
        }
    }
}

auto hakka::Target::getMaps(i32 range) const -> std::shared_ptr<hakka::ProcMaps> {
    return hakka::ProcMaps::getMaps(this->pid, range);
}

auto hakka::Target::getPageEntry(ptr_t address) -> hakka::PagemapEntry {
    this->initPagemapFd();
    return hakka::PagemapEntry::getPagemapEntry(this->pagemapFd, address);
}

void hakka::Target::initMemFd() {
    if (this->memFd != 0) return;
    std::string path = "/proc/" + std::to_string(pid) + "/mem";
    this->memFd = open(path.c_str(), O_RDWR);
}

void hakka::Target::initPagemapFd() {
    if (this->pagemapFd == 0) return;
    auto pid_str = std::to_string(this->pid);
    std::string path = "/proc/" + pid_str + "/task/" + pid_str + "/pagemap";
    this->pagemapFd = open(path.c_str(), O_RDONLY);
    if (this->pagemapFd == 0) {
        throw std::runtime_error("Open /proc/pid/pagemap error.");
    }
}
