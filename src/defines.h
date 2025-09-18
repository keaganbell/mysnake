#pragma once
#pragma warning(disable:4244) // conversion loss of data
#pragma warning(disable:4505) // unreferenced operator
#pragma warning(disable:4201) // anonymous structs (msvc specific)

#include "stdint.h"
#include "stddef.h"

typedef int8_t  i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef float  r32;
typedef double r64;

typedef u8 b8;
typedef u32 b32;
#define true  1
#define false 0

#define UNUSED(x) (void)(x)

#define INVALID_CODEPATH assert(0);
#define INVALID_DEFAULT_CASE default: assert(0);

#ifdef SNAKE_ENGINE
#define SNAPI static
#else
#define SNAPI extern
#endif
