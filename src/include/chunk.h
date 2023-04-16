#ifndef FS_CHUNK_H
#define FS_CHUNK_H

#include "c.h"
#include "cell.h"
#include "move_list.h"

#define CHUNK_DIM 64
#define UPDATE_ZONE_OFFSET 2

typedef struct Chunk {
	Vec2i min_update_zone;
	Vec2i max_update_zone;
	Cell cells[CHUNK_DIM*CHUNK_DIM];
	u8 should_be_updated;
} Chunk;

Chunk *chunk_create();

Cell chunk_delete_cell(Chunk *chunk, i32 x, i32 y);
Cell chunk_get_cell(Chunk *chunk, i32 x, i32 y);
void chunk_set_cell(Chunk *chunk, i32 x, i32 y, Cell cell);

#endif 
