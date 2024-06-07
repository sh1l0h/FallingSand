#ifndef FS_WORLD_H
#define FS_WORLD_H

#include "utils.h"
#include "move_list.h"
#include "chunk.h"

#define CHUNK_BUCKETS_SIZE 512

typedef struct World{
	Chunk *buckets[CHUNK_BUCKETS_SIZE];
    u64 loaded_chunks;
} World;

extern World *world;

void world_init();
void world_add_chunk(const Vec2i *chunk_pos);

void world_cell_to_chunk_pos(const Vec2i *cell_pos, Vec2i *chunk_pos);

Chunk *world_get_chunk(const Vec2i *chunk_pos);
Chunk *world_get_chunk_by_cell(const Vec2i *cell_pos);

bool world_delete_cell(const Vec2i *cell_pos);
Cell *world_get_cell(const Vec2i *cell_pos);
bool world_set_cell(const Vec2i *cell_pos, const Cell *cell);

void world_update();
void world_render();

void world_render_debug();

#endif
