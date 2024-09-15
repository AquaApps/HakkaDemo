#pragma once

#include <cmath>
#include <variant>

using handle = int;

using i64 = int64_t;
using i32 = int32_t;
using i16 = int16_t;
using i8 = int8_t;
using u64 = uint64_t;
using u32 = uint32_t;
using u16 = uint16_t;
using u8 = uint8_t;

using ptr_t = std::int64_t;

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