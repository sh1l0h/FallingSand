#ifndef FS_C_H
#define FS_C_H

#include <stdint.h>

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t  i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef float f32;
typedef double f64;

typedef union Vec2f {
	struct {
		f32 x;
		f32 y;
	};
	f32 cord[2];
} Vec2f;

typedef union Vec2i {
	struct {
		i32 x;
		i32 y;
	};
	i32 cord[2];
} Vec2i;

#endif
