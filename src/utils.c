#include "../include/utils.h"

u64 vec2i_hash(const Vec2i *vec)
{
    return vec->x * 73856093 ^ vec->y * 19349663;
}
