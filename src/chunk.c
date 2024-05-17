#include "../include/chunk.h"
#include "../include/world.h"

Chunk *chunk_create(const Vec2i *position)
{
	Chunk *result = malloc(sizeof *result);
    zinc_vec2i_copy(position, &result->position);
	result->should_be_updated = 0;
	result->min_update = ZINC_VEC2I(CHUNK_DIM, CHUNK_DIM);
	result->max_update = ZINC_VEC2I(-1, -1);
	memset(result->cells, 0, CHUNK_DIM * CHUNK_DIM * sizeof *result->cells);

	return result;
}

Cell *chunk_get_cell(Chunk *chunk, const Vec2i *cell_pos)
{
	return chunk->cells + cell_pos->y * CHUNK_DIM + cell_pos->x;
}

void chunk_set_cell(Chunk *chunk, const Vec2i *cell_pos, const Cell *cell)
{
    chunk_update_zone(chunk, cell_pos);

    cell_copy(cell, chunk->cells + cell_pos->x + cell_pos->y * CHUNK_DIM);
}

void chunk_update_zone(Chunk *chunk, const Vec2i *update_pos)
{
    chunk->min_update.x = MAX(0, MIN(update_pos->x - UPDATE_ZONE_OFFSET, 
                                     chunk->min_update.x));

    chunk->max_update.x = MIN(CHUNK_DIM - 1, 
                              MAX(update_pos->x + UPDATE_ZONE_OFFSET,
                                  chunk->max_update.x));

    chunk->min_update.y = MAX(0, MIN(update_pos->y - UPDATE_ZONE_OFFSET, 
                                     chunk->min_update.y));

    chunk->max_update.y = MIN(CHUNK_DIM - 1,
                              MAX(update_pos->y + UPDATE_ZONE_OFFSET, 
                                  chunk->max_update.y));
    chunk->should_be_updated = true;
}

void chunk_collect_moves(Chunk *chunk)
{
    Vec2i global_chunk_pos;
    zinc_vec2i_scale(&chunk->position, CHUNK_DIM, &global_chunk_pos);

    if (!chunk->should_be_updated)
        return; 

    u32 move_count = 0;
    for (i32 y = chunk->min_update.y; y <= chunk->max_update.y; y++) {
        for (i32 x = chunk->min_update.x; 
             x <= chunk->max_update.x; x++) {
            const Vec2i cell_pos = ZINC_VEC2I_INIT(x, y);

            Cell *curr_cell = chunk_get_cell(chunk, &cell_pos); 
            if (IS_EMPTY(curr_cell))
                continue;

            Vec2i global_cell_pos;
            zinc_vec2i_add(&global_chunk_pos, &cell_pos, &global_cell_pos);            

            Vec2i cell_down_pos = ZINC_VEC2I_INIT(0, 1);
            zinc_vec2i_add(&global_cell_pos, &cell_down_pos, &cell_down_pos);

            if (MOVES_DOWN(curr_cell) && world_cell_in_bounds(&cell_down_pos) && 
                IS_EMPTY(world_get_cell(&cell_down_pos))) {
                ml_add(world.ml, &global_cell_pos, &cell_down_pos, curr_cell);
                move_count += 1;
                continue;
            }

            Vec2i cell_up_pos = ZINC_VEC2I_INIT(0, -1);
            zinc_vec2i_add(&global_cell_pos, &cell_up_pos, &cell_up_pos);

            if (MOVES_UP(curr_cell) && world_cell_in_bounds(&cell_up_pos) && 
                IS_EMPTY(world_get_cell(&cell_up_pos))) {
                ml_add(world.ml, &global_cell_pos, &cell_up_pos, curr_cell);
                move_count += 1;
                continue;
            }

            i32 random = rand() % 2;
            const Vec2i order[] = {
                ZINC_VEC2I_INIT(random == 1 ? 1 : -1, 0),
                ZINC_VEC2I_INIT(random == 0 ? 1 : -1, 0)
            };

            if (MOVES_DOWN_SIDES(curr_cell)) {
                for (i32 i = 0; i < 2; i++) {
                    Vec2i down_side_pos;
                    zinc_vec2i_add(order + i, &cell_down_pos, &down_side_pos);

                    if (!world_cell_in_bounds(&down_side_pos) ||
                        !IS_EMPTY(world_get_cell(&down_side_pos))) 
                        continue;

                    ml_add(world.ml, 
                           &global_cell_pos, 
                           &down_side_pos, 
                           curr_cell);
                    move_count += 1;
                    goto next_cell;
                }
            }

            if (MOVES_SIDES(curr_cell)) {
                for (i32 i = 0; i < 2; i++) {
                    Vec2i side_pos;
                    zinc_vec2i_add(order + i, &global_cell_pos, &side_pos);

                    if (!world_cell_in_bounds(&side_pos) ||
                        !IS_EMPTY(world_get_cell(&side_pos))) 
                        continue;

                    ml_add(world.ml, &global_cell_pos, &side_pos, curr_cell);
                    move_count += 1;
                    goto next_cell;
                }
            }

            if (MOVES_UP_SIDES(curr_cell)) {
                for (i32 i = 0; i < 2; i++) {
                    Vec2i up_side_pos;
                    zinc_vec2i_add(order + i, &cell_up_pos, &up_side_pos);

                    if (!world_cell_in_bounds(&up_side_pos) ||
                        !IS_EMPTY(world_get_cell(&up_side_pos))) 
                        continue;

                    ml_add(world.ml, 
                           &global_cell_pos, 
                           &up_side_pos, 
                           curr_cell);
                    move_count += 1;
                    break;
                }
            }

next_cell:;
        }
    }

    chunk->should_be_updated = move_count > 0;
    chunk->min_update = ZINC_VEC2I(CHUNK_DIM, CHUNK_DIM);
    chunk->max_update = ZINC_VEC2I(-1, -1);
}
