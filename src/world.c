#include "../include/world.h"
#include <SDL2/SDL.h>

World world;

extern SDL_PixelFormat *format;
extern SDL_Renderer *renderer;

void world_create(i32 width, i32 height)
{
	world.width = width;
	world.height = height;
	world.chunks = malloc(width * height * sizeof *world.chunks);
	world.ml = ml_create();

    for (i32 y = 0; y < height; y++) {
        for (i32 x = 0; x < width; x++) {
            Vec2i chunk_pos = ZINC_VEC2I_INIT(x, y);
			world.chunks[y * width + x] = chunk_create(&chunk_pos);
        }
	}
}

inline Chunk *world_get_chunk(const Vec2i *chunk_pos)
{
	return world.chunks[chunk_pos->y * world.width + chunk_pos->x];
}

inline Chunk *world_get_chunk_by_cell(const Vec2i *cell_pos)
{
    const Vec2i chunk_pos = ZINC_VEC2I_INIT(cell_pos->x / CHUNK_DIM, 
                                            cell_pos->y / CHUNK_DIM);
	
	return world_get_chunk(&chunk_pos);
}

inline void world_delete_cell(const Vec2i *cell_pos)
{
    Cell cell = {ID_EMPTY, 0.0f};
    world_set_cell(cell_pos, &cell); 
}

inline Cell *world_get_cell(const Vec2i *cell_pos)
{
    const Vec2i cell_pos_in_chunk = ZINC_VEC2I_INIT(cell_pos->x % CHUNK_DIM,
                                                    cell_pos->y % CHUNK_DIM);
	return chunk_get_cell(world_get_chunk_by_cell(cell_pos), 
                          &cell_pos_in_chunk);
}

static void world_update_neighboring_chunk(const Vec2i *chunk_pos, 
                                           const Vec2i *cell_pos, 
                                           const Vec2i *offset)
{
    Vec2i neighbor_pos;
    zinc_vec2i_add(offset, chunk_pos, &neighbor_pos);
    if (!world_in_bounds(&neighbor_pos)) 
        return;

    Chunk *c = world_get_chunk(&neighbor_pos);
    const Vec2i update_pos = 
        ZINC_VEC2I_INIT(cell_pos->x + (-offset->x) * CHUNK_DIM,
                        cell_pos->y + (-offset->y) * CHUNK_DIM);

    chunk_update_zone(c, &update_pos);
}

void world_set_cell(const Vec2i *cell_pos, const Cell *cell)
{
    const Vec2i cell_pos_in_chunk = ZINC_VEC2I_INIT(cell_pos->x % CHUNK_DIM,
                                                    cell_pos->y % CHUNK_DIM);

    const Vec2i chunk_pos = ZINC_VEC2I_INIT(cell_pos->x / CHUNK_DIM, 
                                            cell_pos->y / CHUNK_DIM);

    if (cell_pos_in_chunk.x < UPDATE_ZONE_OFFSET) {
        world_update_neighboring_chunk(&chunk_pos, 
                                       &cell_pos_in_chunk, 
                                       &ZINC_VEC2I(-1, 0));
    }
    else if (cell_pos_in_chunk.x >= CHUNK_DIM - UPDATE_ZONE_OFFSET) {
        world_update_neighboring_chunk(&chunk_pos, 
                                       &cell_pos_in_chunk, 
                                       &ZINC_VEC2I(1, 0));
    }

    if (cell_pos_in_chunk.y < UPDATE_ZONE_OFFSET) {
        world_update_neighboring_chunk(&chunk_pos, 
                                       &cell_pos_in_chunk, 
                                       &ZINC_VEC2I(0, -1));
    }
    else if (cell_pos_in_chunk.y >= CHUNK_DIM - UPDATE_ZONE_OFFSET) {
        world_update_neighboring_chunk(&chunk_pos, 
                                       &cell_pos_in_chunk, 
                                       &ZINC_VEC2I(0, 1));
    }

    Chunk *chunk = world_get_chunk_by_cell(cell_pos);
    chunk_set_cell(chunk, &cell_pos_in_chunk, cell);
}

inline bool world_in_bounds(const Vec2i *cell_pos)
{
    return cell_pos->x >= 0 && cell_pos->x < world.width && 
        cell_pos->y >= 0 && cell_pos->y < world.height;
}

inline bool world_cell_in_bounds(const Vec2i *cell_pos)
{
    return cell_pos->x >= 0 && cell_pos->x < world.width * CHUNK_DIM && 
        cell_pos->y >= 0 && cell_pos->y < world.height * CHUNK_DIM;
}

void world_update()
{
    // Collecting moves from chunks
    for (i32 chunk_y = 0; chunk_y < world.height; chunk_y++) {
        for (i32 chunk_x = 0; chunk_x < world.width; chunk_x++) {
            Vec2i chunk_pos = ZINC_VEC2I_INIT(chunk_x, chunk_y);
            chunk_collect_moves(world_get_chunk(&chunk_pos));
        }
    }

    // Executing the collected moves 
    ml_sort(world.ml);

    u32 group_size = 1;
    for (u32 i = 0; i < world.ml->size; i++) {
        if (i + 1 < world.ml->size && 
            !move_cmp(ml_get(world.ml, i), ml_get(world.ml, i + 1))) {
            group_size++;
            continue;
        }

        u32 random_number = rand() % group_size;
        Move *move_to_execute = ml_get(world.ml, i - random_number); 
        world_delete_cell(&move_to_execute->src);
        world_set_cell(&move_to_execute->dest, &move_to_execute->cell);

        group_size = 1;
    }

    ml_clear(world.ml);
}

void world_render(u32 *pixels)
{
    for (i32 y = 0; y < world.height*CHUNK_DIM; y++) {
        for (i32 x = 0; x < world.width*CHUNK_DIM; x++) {
            Vec2i cell_pos = ZINC_VEC2I_INIT(x, y);
            Cell *curr = world_get_cell(&cell_pos);
            switch (curr->id) {
            case ID_EMPTY:
                pixels[y*640 + x] = SDL_MapRGB(format, 0, 0, 0);
                break;

            case ID_SAND:
                pixels[y*640 + x] = SDL_MapRGB(format, 236, 204, 162);
                break;

            }

        }
    }
}

void world_render_debug()
{
    for(i32 chunk_y = 0; chunk_y < world.height; chunk_y++){
        for(i32 chunk_x = 0; chunk_x < world.width; chunk_x++){
            Vec2i chunk_pos = ZINC_VEC2I_INIT(chunk_x, chunk_y);
            Chunk *curr_chunk = world_get_chunk(&chunk_pos);

            SDL_Rect chunk_border = {.x = chunk_x*CHUNK_DIM, .y = chunk_y*CHUNK_DIM, .w = CHUNK_DIM, .h = CHUNK_DIM};

            if(curr_chunk->should_be_updated){
                SDL_SetRenderDrawColor(renderer, 0, 255, 0, 100);
                SDL_Rect update_border = {
                    .x = chunk_border.x + curr_chunk->min_update.x,
                    .y = chunk_border.y + curr_chunk->min_update.y,
                    .w = curr_chunk->max_update.x - curr_chunk->min_update.x,
                    .h = curr_chunk->max_update.y - curr_chunk->min_update.y
                };

                SDL_RenderDrawRect(renderer, &update_border);

                SDL_SetRenderDrawColor(renderer, 255, 0, 0, 100);
            }
            else SDL_SetRenderDrawColor(renderer, 255, 255, 255, 100);

            SDL_RenderDrawRect(renderer, &chunk_border);
        }
    }
}
