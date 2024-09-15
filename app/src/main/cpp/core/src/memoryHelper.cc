#include "hakkaHelper.h"

#include <algorithm>
#include <map>
#include <string>

#include <istream>
#include <sstream>
#include <cctype>
#include <unistd.h>
#include <android/log.h>

#define LOG_TAG "simonServer"

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
public:
    // todo 这里改成链表
    std::map<ValueRange, int> valuesMap;
    ptr_t index;

    explicit Band(const std::vector<ValueRange> &ranges) {
        for (const auto &range: ranges)
            valuesMap[range] = 0;
    }

    bool isMatch() {
        return std::all_of(this->valuesMap.begin(), this->valuesMap.end(),
                           [](const auto &pair) { return pair.second != 0; });
    }
};

#include <android/log.h>

#define LOG_TAG "simonServer"

void hakka::MemorySearcher::organizeMemoryPageGroups(std::vector<std::pair<ptr_t, ptr_t>> &dest) {

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

#include <android/log.h>

#define LOG_TAG "simonServer"

auto hakka::MemorySearcher::searchValue(const std::string &expr, ptr_t bandSize) -> size_t {
    if (!this->results.empty()) {
        this->clearResults();
    }
    auto ranges = parseExpr(expr);

    __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "range count %d", ranges.size());
    // 遍历所有满足条件的内存页
    std::vector<std::pair<ptr_t, ptr_t>> pages;


    pages.emplace_back(0x78ec38f000, 0x78ec38ffff);


//    this->organizeMemoryPageGroups(pages);
    __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "page count %d", pages.size());

    std::for_each(pages.begin(), pages.end(),
                  [&bandSize, &ranges, this](const std::pair<ptr_t, ptr_t> &item) {
                      // todo 并行
                      Band band(ranges);
                      ptr_t _end = item.second;
                      band.index = item.first;
                      i32 tmp;

                      __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "page range 0x%llx-0x%llx",
                                          item.first, item.second);

                      // 读取第一个bandSize
                      do {
                          this->process->read(band.index, &tmp, sizeof(tmp));
                          for (const auto &valueRange: ranges)
                              if (valueRange.match(tmp)) {
                                  band.valuesMap[valueRange]++;
                                  break;
                              }
                          band.index += sizeof(tmp);
                      } while (band.index <= (item.first + bandSize) && band.index <= _end);

                      // bandSize每次移动一个ptr_t
                      while (band.index <= _end && (_end - band.index) >= sizeof(tmp)) {
                          this->process->read(band.index, &tmp, sizeof(tmp));
                          for (const auto &valueRange: ranges)
                              if (valueRange.match(tmp)) {
                                  __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "match ");
                                  band.valuesMap[valueRange]++;
                                  break;
                              }
                          // 如何此时band内已经满足联合条件，判断出去的点是否是条件之一，如果是则添加到结果
                          // todo 将所有的都加进去，用setb保证唯一性
                          // todo 先match再移动，这里要改，现在是先移动再match
                          if (band.isMatch()) {
                              ptr_t last = band.index - bandSize;
                              this->process->read(last, &tmp, sizeof(tmp));
                              for (const auto &valueRange: ranges)
                                  if (valueRange.match(tmp)) {
                                      band.valuesMap[valueRange]--;
                                      results.insert(last);
                                      break;
                                  }
                          }
                          band.index += sizeof(tmp);
                      }

                      // 收尾工作
                      band.index -= bandSize;
                      while (band.index <= _end && (_end - band.index) >= sizeof(tmp)) {
                          if (band.isMatch()) {
                              ptr_t last = band.index - bandSize;
                              this->process->read(last, &tmp, sizeof(tmp));
                              for (const auto &valueRange: ranges)
                                  if (valueRange.match(tmp)) {
                                      band.valuesMap[valueRange]--;
                                      results.insert(last);
                                      break;
                                  }
                          }
                          band.index += sizeof(tmp);
                      }

                  });
    return this->results.size();
}

auto hakka::MemorySearcher::getResults() -> const std::unordered_set<ptr_t> & {
    return this->results;
}

void hakka::MemorySearcher::clearResults() {
    this->results.clear();
}
