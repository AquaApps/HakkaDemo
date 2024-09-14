#include "hakkaHelper.h"

#include <algorithm>
#include <string>

#include <istream>
#include <sstream>
#include <cctype>
#include <unistd.h>

auto get_legal_pages(std::shared_ptr<hakka::Target> &process, std::shared_ptr<hakka::ProcMaps> &maps,
                     bool ignoreSwapped, bool ignoreMissing,
                     std::vector<std::pair<ptr_t, ptr_t>> &dest) -> void {
    if (!maps->readable) {
        return;
    }
    auto page_size = getpagesize();
    for (int i = 0; i < ((maps->end() - maps->start()) / page_size); ++i) {
        auto start = maps->start() + (i * page_size);
        auto entry = process->getPageEntry(start);
        if (!entry.present && !ignoreMissing) {
            continue;
        }
        if (!entry.swapped && !ignoreSwapped) {
            continue;
        }
        auto end = start + page_size;
        dest.emplace_back(start, end);
    }
}

void hakka::MemorySearcher::organizeMemoryPageGroups(std::vector<std::pair<ptr_t, ptr_t>> &dest) {
    std::vector<std::pair<ptr_t, ptr_t>> pages;
    auto maps = this->process->getMaps(this->range);
    do {
        get_legal_pages(process, maps,
                        this->ignoreSwappedPage,
                        this->ignoreMissingPage,
                        pages);
    } while ((maps = maps->next()));
    ptr_t _start = 0;
    ptr_t _end = 0;
    std::for_each(pages.begin(), pages.end(), [&](const std::pair<ptr_t, ptr_t> &item) {
        if (this->end == 0 ||
            (item.first >= this->start && item.second <= this->end)) {
            if (_start == 0) {
                _start = item.first;
                _end = item.second;
            } else {
                if (_end == item.first) {
                    _end = item.second;
                } else {
                    dest.emplace_back(_start, _end);
                    _start = item.first;
                    _end = item.second;
                }
            }
        }
    });
}

auto inline matchValue(std::shared_ptr<hakka::Target> &process, size_t valueSize, ptr_t addr,
                       hakka::MatchSign matchSign) -> bool {
    // todo
}

hakka::MemorySearcher::MemorySearcher(std::shared_ptr<hakka::Target> target) {
    this->process = std::move(target);
}

void hakka::MemorySearcher::setMemoryRange(i32 _range) {
    this->range = _range;
}

void hakka::MemorySearcher::setPageConfig(bool ignoreSwapped, bool ignoreMissing) {
    this->ignoreSwappedPage = ignoreSwapped;
    this->ignoreMissingPage = ignoreMissing;
}

void hakka::MemorySearcher::setSearchRange(ptr_t _start, ptr_t _end) {
    this->start = _start;
    this->end = _end;
}

auto hakka::MemorySearcher::searchValue(const std::string &expr, size_t valueSize,
                                        hakka::MatchSign sign) -> size_t {
    if (!this->results.empty()) {
        this->clearResults();
    }

    std::vector<std::pair<ptr_t, ptr_t>> pages;
    this->organizeMemoryPageGroups(pages);
    std::for_each(pages.begin(), pages.end(), [&](const std::pair<ptr_t, ptr_t> &item) {
        ptr_t _addr = item.first;
        auto _end = item.second;
        do {
            if (matchValue(this->process, valueSize, _addr, sign)) {
                this->results.insert(_addr);
            }
            _addr += valueSize;
        } while (_addr < _end && (_end - _addr) >= valueSize);
    });
    return this->results.size();
}

auto hakka::MemorySearcher::getResults() -> const std::unordered_set<ptr_t> & {
    return this->results;
}

void hakka::MemorySearcher::clearResults() {
    this->results.clear();
}
