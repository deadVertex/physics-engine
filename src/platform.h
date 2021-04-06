#pragma once

#define ENABLE_ASSERTIONS

#include <cstdint>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef u32 b32;

typedef float f32;
typedef double f64;

#define global static
#define internal static

#define ArrayCount(ARRAY) (sizeof(ARRAY) / sizeof(ARRAY[0]))

#ifdef ENABLE_ASSERTIONS
#define Assert(COND)                                                           \
    if (!(COND))                                                               \
    {                                                                          \
        *(int *)0 = 0;                                                         \
    }
#else
#define Assert(COND)
#endif
