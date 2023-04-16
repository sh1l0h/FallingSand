#ifndef FS_WORLD_H
#define FS_WORLD_H

#include "c.h"
#include "move_list.h"
#include "chunk.h"

typedef struct World{
	Chunk **chunks;
	i32 width;
	i32 height;
	MoveList *ml;
} World;

extern World world;

void create_world(i32 width, i32 height);

Chunk *world_get_chunk(i32 x, i32 y);
Chunk *get_chunk(i32 x, i32 y);

Cell delete_cell(i32 x, i32 y);
Cell get_cell(i32 x, i32 y);
void set_cell(i32 x, i32 y, Cell cell);
i32 in_bounds(i32 x, i32 y);

void update(f32 dt);
void render(u32 *pixels, f32 dt);

void render_debug();

#endif
