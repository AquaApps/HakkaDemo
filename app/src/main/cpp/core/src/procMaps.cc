#include "exception.h"
#include "hakka.h"

#include <string>
#include <vector>
#include <functional>
#include <fstream>
#include <sstream>

namespace fs = std::filesystem;

hakka::ProcMap::ProcMap(ptr_t start, ptr_t end,
                        bool readable, bool writable, bool executable,
                        const char *moduleName)
        : _start(start), _end(end),
          _readable(readable), _writable(writable), _executable(executable) {
    strcpy(_moduleName, moduleName);
}

void hakka::ProcMap::setLastRange(bool isCD) {
    determineRange(isCD);
}

auto hakka::ProcMap::size() const -> size_t {
    return _end - _start;
}

auto hakka::ProcMap::start() const -> ptr_t {
    return _start;
}

auto hakka::ProcMap::end() const -> ptr_t {
    return _end;
}

auto hakka::ProcMap::range() const -> hakka::MemoryRange {
    return _range;
}

auto hakka::ProcMap::readable() const -> bool {
    return _readable;
}

auto hakka::ProcMap::writable() const -> bool {
    return _writable;
}

auto hakka::ProcMap::executable() const -> bool {
    return _executable;
}

auto hakka::ProcMap::moduleName() const -> const char * {
    return _moduleName;
}

void hakka::ProcMap::determineRange(bool lastIsCd) {
    using namespace hakka;
    auto *module_name = this->_moduleName;
    if (this->_executable) {
        if (module_name[0] == '\0' || (strstr(module_name, "/data/app") != nullptr) ||
            (strstr(module_name, "/data/user") != nullptr)) {
            this->_range = XA;
        } else {
            this->_range = XS;
        }
        return;
    }
    if (module_name[0] != '\0') {
        if ((strncmp(module_name, "/dev/", 5) == 0)
            && ((strstr(module_name, "/dev/mali") != nullptr)
                || (strstr(module_name, "/dev/kgsl") != nullptr)
                || (strstr(module_name, "/dev/nv") != nullptr)
                || (strstr(module_name, "/dev/tegra") != nullptr)
                || (strstr(module_name, "/dev/ion") != nullptr)
                || (strstr(module_name, "/dev/pvr") != nullptr)
                || (strstr(module_name, "/dev/render") != nullptr)
                || (strstr(module_name, "/dev/galcore") != nullptr)
                || (strstr(module_name, "/dev/fimg2d") != nullptr)
                || (strstr(module_name, "/dev/quadd") != nullptr)
                || (strstr(module_name, "/dev/graphics") != nullptr)
                || (strstr(module_name, "/dev/mm_") != nullptr)
                || (strstr(module_name, "/dev/dri/") != nullptr))) {
            this->_range = V;
            return;
        }
        if (((strncmp(module_name, "/dev/", 5) == 0) &&
             (strstr(module_name, "/dev/xLog") != nullptr))
            || (strncmp(module_name, "/system/fonts/", 0xe) == 0)
            || (strncmp(module_name, "anon_inode:dmabuf", 0x11) == 0)) {
            this->_range = BAD;
            return;
        }
        if (strstr(module_name, "[anon:.bss]") != nullptr) {
            this->_range = lastIsCd ? CB : OTHER;
            return;
        }
        if (strncmp(module_name, "/system/", 8) == 0) {
            this->_range = OTHER;
            return;
        }
        if (strstr(module_name, "/dev/zero") != nullptr) {
            this->_range = CA;
            return;
        }
        if (strstr(module_name, "PPSSPP_RAM") != nullptr) {
            this->_range = PS;
            return;
        }
        if ((strstr(module_name, "system@") == nullptr)
            && (strstr(module_name, "gralloc") == nullptr)
            && strncmp(module_name, "[vdso]", 6) != 0
            && strncmp(module_name, "[vectors]", 9) != 0
            && (strncmp(module_name, "/dev/", 5) != 0 ||
                (strncmp(module_name, "/dev/ashmem", 0xB) == 0))) {
            if (strstr(module_name, "dalvik") != nullptr) {
                if (((strstr(module_name, "eap") != nullptr)
                     || (strstr(module_name, "dalvik-alloc") != nullptr)
                     || (strstr(module_name, "dalvik-main") != nullptr)
                     || (strstr(module_name, "dalvik-large") != nullptr)
                     || (strstr(module_name, "dalvik-free") != nullptr))
                    && (strstr(module_name, "itmap") == nullptr)
                    && (strstr(module_name, "ygote") == nullptr)
                    && (strstr(module_name, "ard") == nullptr)
                    && (strstr(module_name, "jit") == nullptr)
                    && (strstr(module_name, "inear") == nullptr)) {
                    this->_range = JH;
                    return;
                }
                this->_range = J;
                return;
            }
            if ((strstr(module_name, "/lib") != nullptr) &&
                (strstr(module_name, ".so") != nullptr)) {
                if (strstr(module_name, "/data/") != nullptr ||
                    (strstr(module_name, "/mnt/") != nullptr)) {
                    this->_range = CD;
                    return;
                }
            }
            if (strstr(module_name, "malloc") != nullptr) {
                this->_range = CA;
                return;
            }
            if (strstr(module_name, "[heap]") != nullptr) {
                this->_range = CH;
                return;
            }
            if (strstr(module_name, "[stack") != nullptr) {
                this->_range = S;
                return;
            }
            if ((strncmp(module_name, "/dev/ashmem", 0xB) == 0) &&
                (strstr(module_name, "MemoryHeapBase") == nullptr)) {
                this->_range = AS;
                return;
            }
        }
        this->_range = OTHER;
        return;
    }
    if (this->_readable && this->_writable) {
        this->_range = A;
        return;
    }
    this->_range = OTHER;
}