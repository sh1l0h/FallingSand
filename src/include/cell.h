#ifndef FS_CELL_H
#define FS_CELL_H

#include "c.h"

#define MOVES_DOWN 0x0001
#define MOVES_DOWN_SIDES 0x0002
#define MOVES_SIDES 0x0004
#define MOVES_UP_SIDES 0x0008
#define MOVES_UP 0x0010

#define EMPTY_ID 0x0000
#define SAND_ID  0x0100 | MOVES_DOWN | MOVES_DOWN_SIDES
#define STONE_ID 0x0200

typedef struct Cell {
	u16 id;
	Vec2f acc;
	Vec2f vel;
} Cell;

extern Cell empty;
extern Cell sand;

void init_cells();
Cell create_cell(u16 id);

#endif
