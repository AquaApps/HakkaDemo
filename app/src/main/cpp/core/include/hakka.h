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
        DIRECT, MEM_FILE, SYSCALL
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

    class PagemapEntry {
    public:
        u64 pfn: 54;
        u32 soft_dirty: 1;
        u32 file_page: 1;
        u32 swapped: 1;
        u32 present: 1;

        static auto getPagemapEntry(handle pagemapFd, ptr_t vaddr) -> PagemapEntry;
    };

    class ProcMaps : public std::enable_shared_from_this<ProcMaps> {
        std::shared_ptr<ProcMaps> _head;
        std::shared_ptr<ProcMaps> _tail;

        ptr_t _start;
        ptr_t _end;
    public:
        ProcMaps(ptr_t start, ptr_t end);

        hakka::MemoryRange range = ALL;
        bool readable = false;
        bool writable = false;
        bool executable = false;
        bool is_private = false;
        u32 inode = -1;
        i64 offset = 0;
        char module_name[128];

        void insert(std::shared_ptr<ProcMaps> maps);

        void remove();

        auto size() -> size_t;

        [[nodiscard]] auto start() const -> ptr_t;

        [[nodiscard]] auto end() const -> ptr_t;

        auto last() -> std::shared_ptr<ProcMaps>;

        auto next() -> std::shared_ptr<ProcMaps> &;

        static void determineRange(hakka::ProcMaps *maps, bool last_is_cd);

        static auto getMaps(pid_t pid, i32 range = MemoryRange::ALL) -> std::shared_ptr<ProcMaps>;
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

        auto
        getMaps(i32 range = hakka::MemoryRange::ALL) const -> std::shared_ptr<hakka::ProcMaps>;

        auto getPageEntry(ptr_t address) -> hakka::PagemapEntry;

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
    };


}

#endif //MENDER_HAKKA_H
