#include "hakkaHelper.h"

#include <algorithm>
#include <map>
#include <string>
#include <list>
#include <istream>
#include <sstream>
#include <cctype>
#include <unistd.h>
#include <thread>
#include <future>


void parallel_for_each(std::vector<std::pair<ptr_t, ptr_t>> &vec, std::atomic<bool> &stop_flag,
                       const std::function<void(const std::pair<ptr_t, ptr_t> &)> &callback) {
    auto num_threads = std::thread::hardware_concurrency(); // 获取硬件并发线程数
    std::vector<std::future<void>> futures;
    size_t length = vec.size();
    size_t block_size = length / num_threads;

    for (size_t i = 0; i < num_threads; ++i) {
        size_t start = i * block_size;
        size_t end = (i == num_threads - 1) ? length : start + block_size;

        // 创建线程并执行任务
        futures.emplace_back(std::async(std::launch::async, [&, start, end]() {
            for (size_t j = start; j < end; ++j) {
                if (stop_flag.load()) {
                    return;
                }
                callback(vec[j]);
            }
        }));
    }

    // 等待所有线程完成
    for (auto &fut: futures) {
        fut.get();
    }
}


static ValueRange convertToValueRange(const std::string &input) {
    ValueRange valueRange;

    size_t position = input.find('-');
    const char *_start;
    const char *_end;
    if (position != std::string::npos) {
        _start = input.substr(0, position).c_str();
        _end = input.substr(position + 1, input.size() - position - 2).c_str();
    } else {
        _end = input.substr(0, input.size() - 1).c_str();
        _start = _end;
    }
    switch (input.back()) {
        case 'D':
            valueRange.type = type_i32;
            valueRange.start = std::stoi(_start);
            valueRange.end = std::stoi(_end);
            break;
        case 'F':
            valueRange.type = type_float;
            valueRange.start = std::stof(_start);
            valueRange.end = std::stof(_end);
            break;
        default:
            // todo throw
            valueRange.type = type_unknown;
            break;
    }
    return valueRange;
}

static auto parseExpr(const std::string &expr) -> std::vector<ValueRange> {
    std::vector<ValueRange> ranges;
    auto splitString = [](const std::string &str) {
        std::vector<std::string> result;
        std::istringstream iss(str);
        std::string segment;

        while (std::getline(iss, segment, ';')) {
            result.push_back(segment);
        }

        return result;
    };
    // 先以';'对字符串进行分割
    std::vector<std::string> segments = splitString(expr);
    // 把分割后的片段转为ValueRange
    std::for_each(segments.begin(), segments.end(), [&ranges](const std::string &segment) {
        ranges.push_back(convertToValueRange(segment));
    });
    return std::move(ranges);
}

class Band : public std::enable_shared_from_this<Band> {
    bool dirty = true;
    bool lastMatch = false;
    // pair解读：first是列表，second是该列表是否已经被merge过。
    std::map<ValueRange, std::pair<std::list<ptr_t>, bool> > valuesMap;

public:
    ptr_t index;

    explicit Band(const std::vector<ValueRange> &ranges, ptr_t initinalIndex) : index(
            initinalIndex) {
        for (const auto &range: ranges)
            valuesMap[range] = {std::list<ptr_t>(), false};
    }

    void pushPtr(const ValueRange range, const ptr_t ptr) {
        dirty = true;
        valuesMap[range].first.push_back(ptr);
        valuesMap[range].second = false;
    }

    void popPtr(const ValueRange range, const ptr_t ptr) {
        dirty = true;
        valuesMap[range].first.remove(ptr);
        valuesMap[range].second = false;
    }

    auto allToResult(std::unordered_set<ptr_t> &results) {
        std::for_each(valuesMap.begin(), valuesMap.end(), [&results](auto &entry) {
            if (!entry.second.second) {
                entry.second.second = true;
                std::for_each(entry.second.first.begin(),
                              entry.second.first.end(),
                              [&results](const ptr_t &item) {
                                  results.insert(item);
                              });
            }
        });
    }

    // 临时表中是否所有的范围都有值
    bool isMatch() {
        if (dirty) {
            lastMatch = !std::any_of(valuesMap.begin(), valuesMap.end(),
                                     [](const auto &entry) {
                                         return entry.second.first.empty();
                                     });
            dirty = false;
        }
        return lastMatch;
    }
};


void hakka::MemorySearcher::organizeMemoryPageGroups(std::vector<std::pair<ptr_t, ptr_t>> &dest,
                                                     std::vector<std::shared_ptr<ProcMap>> &targetMaps) {
    // 拿到所有的Map，过滤留下需要的
    if (targetMaps.empty()) {
        targetMaps = this->process->getAllMaps();
    }
    std::vector<std::shared_ptr<ProcMap>> rangeMaps;
    auto filter = [this](const std::shared_ptr<ProcMap> &map) {
        if (!map->readable())return false;
        return (this->range & map->range()) == map->range();
    };
    std::copy_if(targetMaps.begin(), targetMaps.end(), std::back_inserter(rangeMaps), filter);

    std::list<std::pair<ptr_t, ptr_t>> unMergePages;
    auto pageSize = getpagesize();
    std::for_each(rangeMaps.begin(), rangeMaps.end(),
                  [this, pageSize, &unMergePages](const std::shared_ptr<ProcMap> &map) {
                      for (int i = 0; i < ((map->end() - map->start()) / pageSize); ++i) {
                          auto _start = map->start() + (i * pageSize);

                          auto entry = process->getPageEntry(map->start());
                          if (ignoreSwappedPage && entry.swapped)continue;
                          if (ignoreMissingPage && !entry.present)continue;

                          auto pair = std::make_pair(_start, _start + pageSize);
                          unMergePages.push_back(pair);
                      }
                  });
    // 将 list 转换为 vector 并进行排序
    std::vector<std::pair<ptr_t, ptr_t>> lines(unMergePages.begin(), unMergePages.end());
    std::sort(lines.begin(), lines.end());

    // 合并线段
    std::pair<ptr_t, ptr_t> currentLine = lines[0];

    for (size_t i = 1; i < lines.size(); ++i) {
        // 如果当前线段与下一条线段重叠或相连
        if (currentLine.second >= lines[i].first) {
            // 合并线段
            currentLine.second = std::max(currentLine.second, lines[i].second);
        } else {
            // 将当前线段添加到结果并更新当前线段
            dest.push_back(currentLine);
            currentLine = lines[i];
        }
    }

    // 添加最后一条线段
    dest.push_back(currentLine);
}

hakka::MemorySearcher::MemorySearcher(std::shared_ptr<hakka::Target> target) {
    this->process = std::move(target);
    this->stopFlag = false;
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

auto hakka::MemorySearcher::searchValue(const std::string &expr, ptr_t bandSize) -> size_t {
    std::vector<std::shared_ptr<ProcMap>> empty;
    return searchValue(expr, bandSize, empty);
}

#include <android/log.h>

#define LOG_TAG "simonServer"

auto hakka::MemorySearcher::searchValue(const std::string &expr, ptr_t bandSize,
                                        std::vector<std::shared_ptr<ProcMap>> targetMaps) -> size_t {
    if (!this->results.empty()) {
        this->clearResults();
    }
    auto ranges = parseExpr(expr);

    // 遍历所有满足条件的内存页
    std::vector<std::pair<ptr_t, ptr_t>> pages;
    this->organizeMemoryPageGroups(pages, targetMaps);

    parallel_for_each(pages, this->stopFlag,
                      [&bandSize, &ranges, this](const std::pair<ptr_t, ptr_t> &item) {
                          Band band(ranges, item.first);
                          ptr_t _end = item.second;
                          i32 tmp;
                          ptr_t valueSize = sizeof(i32);

                          __android_log_print(ANDROID_LOG_ERROR, LOG_TAG,
                                              "page range 0x%llx-0x%llx",
                                              item.first, item.second);
                          // 读取第一个bandSize
                          do {
                              this->process->read(band.index, &tmp, valueSize);
                              for (const auto &valueRange: ranges)
                                  if (valueRange.match(tmp)) {
                                      band.pushPtr(valueRange, band.index);
                                      break;
                                  }
                              band.index += valueSize;
                          } while (band.index < (item.first + bandSize) && band.index < _end);

                          // bandSize每次移动一个ptr_t
                          while (band.index <= _end && (_end - band.index) >= valueSize) {
                              ptr_t last = band.index - bandSize;
                              this->process->read(last, &tmp, valueSize);
                              for (const auto &valueRange: ranges) {
                                  if (valueRange.match(tmp)) {
                                      if (band.isMatch()) {
                                          __android_log_print(ANDROID_LOG_ERROR, LOG_TAG,
                                                              "find %d [%llx]", tmp, last);
                                          band.allToResult(results);
                                      }
                                      band.popPtr(valueRange, last);
                                      break;
                                  }
                              }

                              this->process->read(band.index, &tmp, valueSize);
                              for (const auto &valueRange: ranges)
                                  if (valueRange.match(tmp)) {
                                      band.pushPtr(valueRange, band.index);
                                      break;
                                  }
                              band.index += valueSize;
                          }

                          //最后一个bandSize如果匹配则将所有地址返回
                          if (band.isMatch()) band.allToResult(results);
                      });
    return this->results.size();
}

auto hakka::MemorySearcher::getResults() -> const std::unordered_set<ptr_t> & {
    return this->results;
}

void hakka::MemorySearcher::clearResults() {
    this->results.clear();
}
