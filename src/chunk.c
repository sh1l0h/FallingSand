#include "../include/chunk.h"
#include "../include/world.h"
#include "../include/camera.h"

Chunk *chunk_create(const Vec2i *position)
{
	Chunk *result = malloc(sizeof *result);
    zinc_vec2i_copy(position, &result->position);
	result->should_be_updated = 0;
	result->min_update = ZINC_VEC2I(CHUNK_DIM, CHUNK_DIM);
	result->max_update = ZINC_VEC2I(-1, -1);
	memset(result->cells, 0, CHUNK_DIM * CHUNK_DIM * sizeof *result->cells);
    result->next = NULL;

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

static bool chunk_add_move(const Vec2i *src,
                           const Vec2i *dest,
                           const Cell *cell)
{

    Chunk *dest_chunk = world_get_chunk_by_cell(dest);
    const Vec2i cell_pos_in_chunk = 
        ZINC_VEC2I_INIT(MOD(dest->x, CHUNK_DIM),
                        MOD(dest->y, CHUNK_DIM));

    if (dest_chunk == NULL || 
        !IS_EMPTY(chunk_get_cell(dest_chunk, &cell_pos_in_chunk)))
        return false;

    ml_add(src, dest, cell);
    return true;
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
            Vec2i cell_pos = ZINC_VEC2I_INIT(x, y);

            const Cell *curr_cell = chunk_get_cell(chunk, &cell_pos); 
            if (IS_EMPTY(curr_cell))
                continue;

            zinc_vec2i_add(&global_chunk_pos, &cell_pos, &cell_pos);            

            Vec2i cell_down_pos = ZINC_VEC2I_INIT(0, -1);
            zinc_vec2i_add(&cell_pos, &cell_down_pos, &cell_down_pos);

            if (MOVES_DOWN(curr_cell) && 
                chunk_add_move(&cell_pos, &cell_down_pos, curr_cell)) {
                move_count += 1;
                continue;
            }

            Vec2i cell_up_pos = ZINC_VEC2I_INIT(0, 1);
            zinc_vec2i_add(&cell_pos, &cell_up_pos, &cell_up_pos);

            if (MOVES_UP(curr_cell) && 
                chunk_add_move(&cell_pos, &cell_up_pos, curr_cell)) {
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

                    if (!chunk_add_move(&cell_pos, &down_side_pos, curr_cell))
                        continue;

                    move_count += 1;
                    goto next_cell;
                }
            }

            if (MOVES_SIDES(curr_cell)) {
                for (i32 i = 0; i < 2; i++) {
                    Vec2i side_pos;
                    zinc_vec2i_add(order + i, &cell_pos, &side_pos);

                    if (!chunk_add_move(&cell_pos, &side_pos, curr_cell))
                        continue;

                    move_count += 1;
                    goto next_cell;
                }
            }

            if (MOVES_UP_SIDES(curr_cell)) {
                for (i32 i = 0; i < 2; i++) {
                    Vec2i up_side_pos;
                    zinc_vec2i_add(order + i, &cell_up_pos, &up_side_pos);

                    if (!chunk_add_move(&cell_pos, &up_side_pos, curr_cell))
                        continue;

                    ml_add(&cell_pos, &up_side_pos, curr_cell);
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

void chunk_render(Chunk *chunk, SDL_Renderer *renderer)
{
    Vec2i chunk_pos_in_cells;
    zinc_vec2i_scale(&chunk->position, CHUNK_DIM, &chunk_pos_in_cells);
    for (i32 y = 0; y < CHUNK_DIM; y++) {
        for (i32 x = 0; x < CHUNK_DIM; x++) {
            Vec2i cell_pos = ZINC_VEC2I_INIT(x, y);
            Cell *curr = chunk_get_cell(chunk, &cell_pos);
            if (IS_EMPTY(curr))
                continue;

            zinc_vec2i_add(&cell_pos, &chunk_pos_in_cells, &cell_pos);
            camera_global_to_screen_pos(&cell_pos, &cell_pos);
            const SDL_Rect rect = {
                .x = cell_pos.x - camera.scale / 2,
                .y = cell_pos.y - camera.scale / 2,
                .w = camera.scale,
                .h = camera.scale
            };

            switch (curr->id) {
            case ID_SAND:
                SDL_SetRenderDrawColor(renderer, 236, 204, 162, 255);
                break;
            }

            SDL_RenderFillRect(renderer, &rect);
        }
    }
}
