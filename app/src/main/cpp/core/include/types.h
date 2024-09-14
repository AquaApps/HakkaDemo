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

#ifdef HAKUTAKU_64BIT
using ptr_t = std::int64_t;
#else
using ptr_t = std::int32_t;
#endif