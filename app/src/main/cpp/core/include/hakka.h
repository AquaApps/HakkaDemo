#ifndef MENDER_HAKKA_H
#define MENDER_HAKKA_H

#include <sys/uio.h>
#include <filesystem>
#include <dirent.h>
#include <string>
#include <vector>
#include <memory>

#include "types.h"

namespace hakka {
    /**
     * DIRECT: 适用于将本so注入到进程中的情况。
     * MEM_FILE：直接读写/proc/{pid}/mem。
     * SYSCALL: 借助process_vm_readv/writev读写指定pid的数据。
     */
    enum MemoryMode {
        DIRECT, MEM_FILE, SYSCALL, MMAP
    };

    enum MemoryRange {
        ALL = 0b1111111111111110,
        BAD = 0b0000000000000010,
        V = 0b0000000000000100, // video
        CA = 0b0000000000001000,
        CB = 0b0000000000010000,
        CD = 0b0000000000100000,
        CH = 0b0000000001000000,
        JH = 0b0000000010000000, // Java heap
        J = 0b0000000100000000, // Java
        A = 0b0000001000000000,
        XS = 0b0000010000000000, // 系统空间代码
        S = 0b0000100000000000, // 栈区
        AS = 0b0001000000000000,
        OTHER = 0b0010000000000000,
        XA = 0b0100000000000000, // 用户空间代码
        PS = 0b1000000000000000
    };

    class PageEntry {
    public:
        u64 pfn: 54;
        u32 soft_dirty: 1;
        u32 file_page: 1;
        u32 swapped: 1;
        u32 present: 1;

        static auto getPagemapEntry(handle pagemapFd, ptr_t vaddr) -> PageEntry;
    };

    class ProcMap : public std::enable_shared_from_this<ProcMap> {
        ptr_t _start;
        ptr_t _end;
        hakka::MemoryRange _range = ALL;
        char _moduleName[128]{0,};
        bool _readable = false;
        bool _writable = false;
        bool _executable = false;

        void determineRange(bool lastIsCd);

    public:
        ProcMap(ptr_t start, ptr_t end,
                bool readable, bool writable, bool executable,
                const char *moduleName);

        void setLastRange(bool isCD);

        auto size() const -> size_t;

        auto start() const -> ptr_t;

        auto end() const -> ptr_t;

        auto range() const -> hakka::MemoryRange;

        auto readable() const -> bool;

        auto writable() const -> bool;

        auto executable() const -> bool;

        auto moduleName() const -> const char *;
    };

    /**
     * 绑定pid,设置访问模式，读写内存，判断是否存活，读取page
     */
    class Target : public std::enable_shared_from_this<Target> {
        pid_t pid;
        MemoryMode memoryMode;

        handle memFd;
        handle pagemapFd;
    public:
        explicit Target(pid_t pid, MemoryMode mode);

        ~Target();

        auto isAlive() const -> bool;

        void read(ptr_t addr, void *data, size_t len);

        void write(ptr_t addr, void *data, size_t len);

        auto getAllMaps() const -> std::vector<std::shared_ptr<hakka::ProcMap>>;

        auto getPageEntry(ptr_t address) -> hakka::PageEntry;

    private:
        void initMemFd();

        void initPagemapFd();

        // Todo: 策略模式改进。
        static void readByDirect(ptr_t addr, void *data, size_t len);

        static void writeByDirect(ptr_t addr, void *data, size_t len);

        void readByMem(ptr_t addr, void *data, size_t len) const;

        void writeByMem(ptr_t addr, void *data, size_t len) const;

        void readBySyscall(ptr_t addr, void *data, size_t len) const;

        void writeBySyscall(ptr_t addr, void *data, size_t len) const;

        void readByMmap(ptr_t addr, void *data, size_t len) const;

        void writeByMmap(ptr_t addr, void *data, size_t len) const;
    };


}

#endif //MENDER_HAKKA_H
