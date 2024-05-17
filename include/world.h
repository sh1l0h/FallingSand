#ifndef FS_WORLD_H
#define FS_WORLD_H

#include "utils.h"
#include "move_list.h"
#include "chunk.h"

typedef struct World{
	Chunk **chunks;
	i32 width;
	i32 height;
	MoveList *ml;
} World;

extern World world;

void world_create(i32 width, i32 height);

Chunk *world_get_chunk(const Vec2i *chunk_pos);
Chunk *world_get_chunk_by_cell(const Vec2i *cell_pos);

void world_delete_cell(const Vec2i *cell_pos);
Cell *world_get_cell(const Vec2i *cell_pos);
void world_set_cell(const Vec2i *cell_pos, const Cell *cell);
bool world_in_bounds(const Vec2i *pos);
bool world_cell_in_bounds(const Vec2i *cell_pos);

void world_update();
void world_render(u32 *pixels);

void world_render_debug();

#endif
