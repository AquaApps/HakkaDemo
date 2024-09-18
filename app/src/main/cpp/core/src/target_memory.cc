#pragma clang diagnostic push
#pragma ide diagnostic ignored "bugprone-easily-swappable-parameters"

#include <fstream>
#include <sstream>
#include <vector>
#include <asm-generic/fcntl.h>
#include <fcntl.h>
#include <unistd.h>
#include "hakka.h"
#include "exception.h"

void hakka::Target::readByDirect(ptr_t addr, void *data, size_t len) {
    memcpy(data, (void *) addr, len);
}

void hakka::Target::writeByDirect(ptr_t addr, void *data, size_t len) {
    memcpy((void *) addr, data, len);
}

void hakka::Target::readByMem(ptr_t addr, void *data, size_t len) const {
    pread64(this->memFd, data, len, addr);
}

void hakka::Target::writeByMem(ptr_t addr, void *data, size_t len) const {
    pwrite64(this->memFd, data, len, addr);
}

void hakka::Target::readBySyscall(ptr_t addr, void *data, size_t len) const {
    iovec local{data, len};
    iovec remote{(void *) addr, len};
    if (process_vm_readv(this->pid, &local, 1, &remote, 1, 0) != len) {
        std::string hexStr = (std::stringstream() << "Memory read Error: " << std::hex << addr
                                                  << ".").str();
        throw hakka::memory_operate_error(hexStr);
    }
}

void hakka::Target::writeBySyscall(ptr_t addr, void *data, size_t len) const {
    iovec local{data, len};
    iovec remote{(void *) addr, len};
    if (process_vm_writev(this->pid, &local, 1, &remote, 1, 0) != len) {
        throw hakka::memory_operate_error("Memory write error.");
    }
}

void hakka::Target::readByMmap(ptr_t addr, void *data, size_t len) const {
    iovec local{data, len};
    iovec remote{(void *) addr, len};
    if (process_vm_readv(this->pid, &local, 1, &remote, 1, 0) != len) {
        std::string hexStr = (std::stringstream() << "Memory read Error: " << std::hex << addr
                                                  << ".").str();
        throw hakka::memory_operate_error(hexStr);
    }
}

void hakka::Target::writeByMmap(ptr_t addr, void *data, size_t len) const {
    iovec local{data, len};
    iovec remote{(void *) addr, len};
    if (process_vm_writev(this->pid, &local, 1, &remote, 1, 0) != len) {
        throw hakka::memory_operate_error("Memory write error.");
    }
}

#pragma clang diagnostic pop