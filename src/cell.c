#include "../include/cell.h"

inline void cell_copy(const Cell *src, Cell *dest)
{
    memcpy(dest, src, sizeof *src);
}
