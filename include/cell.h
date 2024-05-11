#ifndef FS_CELL_H
#define FS_CELL_H

#include "./utils.h"

#define MOVES_DOWN_MASK         0b00000001
#define MOVES_DOWN_SIDES_MASK   0b00000010
#define MOVES_SIDES_MASK        0b00000100
#define MOVES_UP_SIDES_MASK     0b00001000
#define MOVES_UP_MASK           0b00010000

#define ID_EMPTY 0x0000
#define ID_SAND  0x0100 | MOVES_DOWN_MASK | MOVES_DOWN_SIDES_MASK
#define ID_STONE 0x0200

#define IS_EMPTY(_cell) ((_cell)->id == ID_EMPTY)

#define MOVES_DOWN(_cell)       ((_cell)->id & MOVES_DOWN_MASK)
#define MOVES_DOWN_SIDES(_cell) ((_cell)->id & MOVES_DOWN_SIDES_MASK)
#define MOVES_SIDES(_cell)      ((_cell)->id & MOVES_SIDES_MASK)
#define MOVES_UP_SIDES(_cell)   ((_cell)->id & MOVES_UP_SIDES_MASK)
#define MOVES_UP(_cell)         ((_cell)->id & MOVES_UP_MASK)

typedef struct Cell {
    u16 id;
    f32 tempreture;
} Cell;

void cell_copy(const Cell *src, Cell *dest);

#endif
