#ifndef FS_UTILS_H
#define FS_UTILS_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <math.h>
#include "../libs/ZINC/include/zinc.h"

#define MAX(_a, _b) ((_a) > (_b) ? (_a) : (_b))
#define MIN(_a, _b) ((_a) < (_b) ? (_a) : (_b))
#define MOD(_a, _mod) ((((_a) % (_mod)) + (_mod)) % (_mod))
#define SIGN(_a) (((_a) > 0) ? 1 : (((_a) < 0) ? -1 : 0))
#define CLAMP(_v, _min, _max) (MIN((_max), MAX((_min), (_v))))

#define FIXED_DELTA (1.0f / 60.0f)
#define GRAVITY -30.0f

u64 vec2i_hash(const Vec2i *vec);

#endif
