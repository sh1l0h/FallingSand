#ifndef FS_CHUNK_H
#define FS_CHUNK_H

#include "utils.h"
#include "cell.h"
#include "move_list.h"

#define CHUNK_DIM 64
#define UPDATE_ZONE_OFFSET 3

typedef struct Chunk {
    Vec2i position;
	Vec2i min_update;
	Vec2i max_update;
	Cell cells[CHUNK_DIM * CHUNK_DIM];
	bool should_be_updated;
} Chunk;

Chunk *chunk_create(const Vec2i *position);

Cell *chunk_get_cell(Chunk *chunk, const Vec2i *cell_pos);
void chunk_set_cell(Chunk *chunk, const Vec2i *cell_pos, const Cell *cell);
void chunk_update_zone(Chunk *chunk, const Vec2i *update_pos);

void chunk_collect_moves(Chunk *chunk);

#endif 
