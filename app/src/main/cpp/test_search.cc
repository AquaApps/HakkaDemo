#include <algorithm>
#include <iostream>
#include <list>
#include <map>
#include <memory>
#include <unordered_set>
#include <variant>
#include <vector>
using i64 = int64_t;
using i32 = int32_t;
using i16 = int16_t;
using i8 = int8_t;
using u64 = uint64_t;
using u32 = uint32_t;
using u16 = uint16_t;
using u8 = uint8_t;
using ptr_t = int64_t;

enum ValueType {
    type_i32 = 0,
    type_float = 1,
    type_unknown = 2
};

using BasicValue = std::variant<i32, float, u32>;

class ValueRange {
public:
    BasicValue start;
    BasicValue end;
    ValueType type = type_unknown;

    bool operator<(const ValueRange &other) const {
        if (type != other.type)
            return type < other.type;
        if (start < other.start)
            return true;
        if (other.start < start)
            return false;
        return end < other.end;
    }

    [[nodiscard]] bool match(BasicValue val) const {
        return val >= start && val <= end;
    }
};

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

#define a 1111,
#define b 2222,
#define c 3333,
#define d 4444,

#define len 16
#define detectA true
#define detectB true
#define detectC false
// 对于a;b::4     结果0 3 6
// 对于a;b;c::4   结果
// 对于a;c::4     结果6 7
// 对于b;c::4     结果
i32 memoryA[14] = {a d d b d d a c d d d d d d};
// 对于a;b::4     结果0 3 5
// 对于a;b;c::4   结果3 5 6
// 对于a;c::4     结果5 6
// 对于b;c::4     结果3 6
i32 memoryB[12] = {a d d b d a c d d d d d};
// 对于a;b::4     结果0 3 5
// 对于a;b;c::4   结果3 5 6
// 对于a;c::4     结果5 6 8
// 对于b;c::4     结果3 6 8 10
i32 memoryC[13] = {a d d b d a c d c d b d d};
// 对于a;b::4     结果1 4
i32 memoryD[13] = {c a c c b c c c c a c c c};
// 对于a;b::4     结果1 2 4 9 12
i32 memoryE[13] = {c a b c b c c c c a c c b};
// 对于a;b::4     结果
i32 memoryF[8] = {c c c a c c c b};
// 对于a;b::4     结果4 5 7
i32 memoryG[16] = {c c c a a a c b c c c a c c c c};
// 对于a;b::4     结果
i32 memoryH[14] = {c c c a c c c b b c c c c c};

void readA(ptr_t addr, i32 *data, size_t lenth) {
    *data = memoryG[addr];
}

int main() {
    std::vector<ValueRange> ranges;
    ValueRange aRange;
    aRange.start = 1111;
    aRange.end = 1111;
    aRange.type = type_i32;
    ValueRange bRange;
    bRange.start = 2222;
    bRange.end = 2222;
    bRange.type = type_i32;
    ValueRange cRange;
    cRange.start = 3333;
    cRange.end = 3333;
    cRange.type = type_i32;
    if (detectA)
        ranges.push_back(aRange);
    if (detectB)
        ranges.push_back(bRange);
    if (detectC)
        ranges.push_back(cRange);
    std::pair<ptr_t, ptr_t> item = {0, len};
    std::unordered_set<ptr_t> results;

    Band band(ranges, item.first);
    ptr_t _end = item.second;
    i32 tmp;
    int bandSize = 4;
    ptr_t valueSize = 1;

    // 读取第一个bandSize
    do {
        readA(band.index, &tmp, valueSize);
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
        readA(last, &tmp, valueSize);
        for (const auto &valueRange: ranges) {
            if (valueRange.match(tmp)) {
                if (band.isMatch()) {
                    band.allToResult(results);
                }
                band.popPtr(valueRange, last);
                break;
            }
        }

        readA(band.index, &tmp, valueSize);
        for (const auto &valueRange: ranges)
            if (valueRange.match(tmp)) {
                band.pushPtr(valueRange, band.index);
                break;
            }
        band.index += valueSize;
    }

    //最后一个bandSize如果匹配则将所有地址返回
    if (band.isMatch()) band.allToResult(results);

    for (const auto &ptr: results) {
        readA(ptr, &tmp, sizeof(tmp));
        std::cout << tmp << " in " << ptr << std::endl;
    }

    return 0;
}
