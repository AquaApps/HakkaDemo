#include <fstream>
#include <sstream>
#include <vector>
#include <asm-generic/fcntl.h>
#include <fcntl.h>
#include <unistd.h>
#include "exception.h"
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
        case hakka::MMAP: {
            readByMmap(addr, data, len);
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
        case hakka::MMAP: {
            writeByMmap(addr, data, len);
            break;
        }
        default: {
            throw std::runtime_error("Not support the MemoryMode.");
        }
    }
}


auto hakka::Target::getAllMaps() const -> std::vector<std::shared_ptr<hakka::ProcMap>> {
    std::ifstream maps(std::string("/proc/") + std::to_string(pid) + "/maps");
    if (!maps.is_open()) {
        throw hakka::file_not_found();
    }
    std::string line;
    bool lastIsCd = false;
    std::vector<std::shared_ptr<hakka::ProcMap>> result;
    auto callback = [&](std::shared_ptr<ProcMap> maps) { // NOLINT(*-unnecessary-value-param)
        result.push_back(maps);
    };

    while (getline(maps, line)) {
        std::istringstream iss(line);
        std::vector<std::string> tokens;
        std::string token;

        while (getline(iss, token, ' ')) {
            tokens.push_back(token);
        }

        auto address = tokens[0];
        std::string::size_type pos = address.find('-');
        ptr_t _start = std::stol(address.substr(0, pos), nullptr, 16);
        ptr_t _end = std::stol(address.substr(pos + 1), nullptr, 16);

        auto perms = tokens[1];
        auto readable = perms[0] == 'r';
        auto writable = perms[1] == 'w';
        auto executable = perms[2] == 'x';
        std::string module_name;
        if (tokens.size() > 5) {
            for (int i = 5; i < tokens.size(); i++) {
                module_name += tokens[i];
            }
        }
        // todo check length
        auto map = std::make_shared<hakka::ProcMap>(_start, _end,
                                                    readable, writable, executable,
                                                    module_name.c_str());
        map->setLastRange(lastIsCd);
        lastIsCd = map->range() == hakka::MemoryRange::CD;
        callback(map);
    }
    maps.close();
    return std::move(result);
}

auto hakka::Target::getPageEntry(ptr_t address) -> hakka::PageEntry {
    this->initPagemapFd();
    return hakka::PageEntry::getPagemapEntry(this->pagemapFd, address);
}

void hakka::Target::initMemFd() {
    if (this->memFd != 0) return;
    std::string path = "/proc/" + std::to_string(pid) + "/mem";
    this->memFd = open(path.c_str(), O_RDWR);
}

void hakka::Target::initPagemapFd() {
    if (this->pagemapFd != 0) return;
    auto pid_str = std::to_string(this->pid);
    std::string path = "/proc/" + pid_str + "/task/" + pid_str + "/pagemap";
    this->pagemapFd = open(path.c_str(), O_RDONLY);
    if (this->pagemapFd == 0) {
        throw std::runtime_error("Open /proc/pid/pagemap error.");
    }
}
