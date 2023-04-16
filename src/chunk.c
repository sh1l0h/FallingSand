#include "include/chunk.h"
#include <stdlib.h>
#include <assert.h>
#include <string.h>

Chunk *chunk_create()
{
	Chunk *result = malloc(sizeof(Chunk));
	result->should_be_updated = 0;
	result->min_update_zone = (Vec2i) {{CHUNK_DIM, CHUNK_DIM}};
	result->max_update_zone = (Vec2i) {{-1, -1}};
	memset(result->cells, 0, CHUNK_DIM*CHUNK_DIM*sizeof(Cell));

	return result;
}

Cell chunk_delete_cell(Chunk *chunk, i32 x, i32 y)
{
#ifdef DEBUG
	assert(x >= 0 && x < CHUNK_DIM && y >= 0 && y < CHUNK_DIM && "Out of bounds in chunk_delete_cell");
#endif

	Cell result = chunk_get_cell(chunk, x, y);

	chunk_set_cell(chunk, x, y, empty);
	return result;
}

inline Cell chunk_get_cell(Chunk *chunk, i32 x, i32 y)
{
#ifdef DEBUG
	assert(x >= 0 && x < CHUNK_DIM && y >= 0 && y < CHUNK_DIM && "Out of bounds in chunk_get_cell");
#endif

	return chunk->cells[y*CHUNK_DIM + x];
}

void chunk_set_cell(Chunk *chunk, i32 x, i32 y, Cell cell)
{
#ifdef DEBUG
	assert(x >= 0 && x < CHUNK_DIM && y >= 0 && y < CHUNK_DIM && "Out of bounds in chunk_set_cell");
#endif

	if(x < chunk->min_update_zone.x + UPDATE_ZONE_OFFSET)
		chunk->min_update_zone.x = x - UPDATE_ZONE_OFFSET <= 0 ? 0 : x - UPDATE_ZONE_OFFSET;
	if(x > chunk->max_update_zone.x - UPDATE_ZONE_OFFSET)
		chunk->max_update_zone.x = x + UPDATE_ZONE_OFFSET >= CHUNK_DIM ? CHUNK_DIM - 1 : x + UPDATE_ZONE_OFFSET;
	if(y < chunk->min_update_zone.y + UPDATE_ZONE_OFFSET)
		chunk->min_update_zone.y = y - UPDATE_ZONE_OFFSET <= 0 ? 0 : y - UPDATE_ZONE_OFFSET;
	if(y > chunk->max_update_zone.y - UPDATE_ZONE_OFFSET)
		chunk->max_update_zone.y = y + UPDATE_ZONE_OFFSET >= CHUNK_DIM ? CHUNK_DIM - 1 : y + UPDATE_ZONE_OFFSET;

	chunk->cells[x + y*CHUNK_DIM] = cell;
}
