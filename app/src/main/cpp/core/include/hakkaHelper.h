#ifndef MENDER_HAKKAHELPER_H
#define MENDER_HAKKAHELPER_H

#include "hakka.h"
#include <unordered_set>

namespace hakka {
    class Proc {
    public:
        pid_t pid;
        std::string comm;
        char state;
        pid_t ppid;
    };

    auto getPidList() -> std::vector<pid_t>;

    auto findPidByPackage(std::string &package) -> pid_t;

    auto getProcessList() -> std::vector<hakka::Proc>;


    class MemorySearcher {
    protected:
        std::shared_ptr<hakka::Target> process;
        std::unordered_set<ptr_t> results;

        i32 range = MemoryRange::ALL;
        ptr_t start = 0;
        ptr_t end = 0;
        // 忽略换出的页面
        bool ignoreSwappedPage = false;
        // 忽略缺少的页面
        bool ignoreMissingPage = true;

        void organizeMemoryPageGroups(std::vector<std::pair<ptr_t, ptr_t>> &dest);

    public:
        explicit MemorySearcher(std::shared_ptr<hakka::Target> target);

        void setMemoryRange(i32 _range);

        void setPageConfig(bool ignoreSwapped, bool ignoreMissing);

        void setSearchRange(ptr_t start, ptr_t end);

        auto searchValue(const std::string &expr, ptr_t bandSize) -> size_t;

        auto getResults() -> const std::unordered_set<ptr_t> &;

    private:
        void clearResults();
    };
}

#endif //MENDER_HAKKAHELPER_H
