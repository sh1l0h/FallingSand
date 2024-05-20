#include "../include/world.h"
#include "../include/camera.h"
#include <SDL2/SDL.h>

World world = { 
    .buckets = {NULL},
    .loaded_chunks = 0
};

extern SDL_PixelFormat *format;
extern SDL_Renderer *renderer;

void world_add_chunk(const Vec2i *chunk_pos)
{
    u64 index = vec2i_hash(chunk_pos) & (CHUNK_BUCKETS_SIZE - 1);
    Chunk *curr = world.buckets[index];
    Chunk *new = chunk_create(chunk_pos);
    new->next = curr;
    world.buckets[index] = new;
}

inline Chunk *world_get_chunk(const Vec2i *chunk_pos)
{
    u64 index = vec2i_hash(chunk_pos) & (CHUNK_BUCKETS_SIZE - 1);

    Chunk *curr = world.buckets[index];
    while (curr != NULL) {
        if (curr->position.x == chunk_pos->x && 
            curr->position.y == chunk_pos->y)
            return curr;

        curr = curr->next;
    }

	return NULL;
}

inline void world_cell_to_chunk_pos(const Vec2i *cell_pos, Vec2i *chunk_pos)
{
    chunk_pos->x = cell_pos->x < 0 ? 
        (cell_pos->x - CHUNK_DIM + 1) / CHUNK_DIM : cell_pos->x / CHUNK_DIM;
    chunk_pos->y = cell_pos->y < 0 ? 
        (cell_pos->y - CHUNK_DIM + 1) / CHUNK_DIM : cell_pos->y / CHUNK_DIM;
}

inline Chunk *world_get_chunk_by_cell(const Vec2i *cell_pos)
{
    Vec2i chunk_pos;
    world_cell_to_chunk_pos(cell_pos, &chunk_pos);
	
	return world_get_chunk(&chunk_pos);
}

inline bool world_delete_cell(const Vec2i *cell_pos)
{
    Cell cell = {.id = ID_EMPTY};
    return world_set_cell(cell_pos, &cell); 
}

inline Cell *world_get_cell(const Vec2i *cell_pos)
{
    const Vec2i cell_pos_in_chunk = 
        ZINC_VEC2I_INIT(MOD(cell_pos->x, CHUNK_DIM),
                        MOD(cell_pos->y, CHUNK_DIM));

    Chunk *chunk = world_get_chunk_by_cell(cell_pos); 

    if (chunk == NULL)
        return NULL;

    return chunk_get_cell(chunk, &cell_pos_in_chunk);
}

static void world_update_neighboring_chunk(const Vec2i *chunk_pos, 
                                           const Vec2i *cell_pos, 
                                           const Vec2i *offset)
{
    Vec2i neighbor_pos;
    zinc_vec2i_add(offset, chunk_pos, &neighbor_pos);

    Chunk *c = world_get_chunk(&neighbor_pos);
    if (c == NULL)
        return;

    const Vec2i update_pos = 
        ZINC_VEC2I_INIT(cell_pos->x + (-offset->x) * CHUNK_DIM,
                        cell_pos->y + (-offset->y) * CHUNK_DIM);

    chunk_update_zone(c, &update_pos);
}

bool world_set_cell(const Vec2i *cell_pos, const Cell *cell)
{
    const Vec2i cell_pos_in_chunk = 
        ZINC_VEC2I_INIT(MOD(cell_pos->x, CHUNK_DIM),
                        MOD(cell_pos->y, CHUNK_DIM));

    Chunk *chunk = world_get_chunk_by_cell(cell_pos);
    if (chunk == NULL)
        return false;

    chunk_set_cell(chunk, &cell_pos_in_chunk, cell);

    Vec2i chunk_pos;
    world_cell_to_chunk_pos(cell_pos, &chunk_pos);

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

    return true;
}

void world_update()
{
    Vec2i min = ZINC_VEC2I_INIT(0, camera.half_width * 2 - 1);
    Vec2i max = ZINC_VEC2I_INIT(camera.half_width * 2 - 1, 0);
    camera_screen_to_global_pos(&min, &min);
    camera_screen_to_global_pos(&max, &max);
    world_cell_to_chunk_pos(&min, &min);
    world_cell_to_chunk_pos(&max, &max);

    // Collecting moves from chunks
    for (i32 chunk_x = min.x; chunk_x <= max.x; chunk_x++) {
        for (i32 chunk_y = min.y; chunk_y <= max.y; chunk_y++) {
            Vec2i chunk_pos = ZINC_VEC2I_INIT(chunk_x, chunk_y);
            Chunk *c = world_get_chunk(&chunk_pos);
            if (c == NULL)
                continue;
            chunk_collect_moves(c);
        }
    }

    // Executing the collected moves 
    ml_sort();

    u32 group_size = 1;
    for (u32 i = 0; i < ml.size; i++) {
        if (i + 1 < ml.size && 
            !move_cmp(ml_get(i), ml_get(i + 1))) {
            group_size++;
            continue;
        }

        u32 random_number = rand() % group_size;
        Move *move_to_execute = ml_get(i - random_number); 
        world_delete_cell(&move_to_execute->src);
        world_set_cell(&move_to_execute->dest, &move_to_execute->cell);

        group_size = 1;
    }

    ml_clear();
}

void world_render()
{
    Vec2i min = ZINC_VEC2I_INIT(0, camera.half_width * 2 - 1);
    Vec2i max = ZINC_VEC2I_INIT(camera.half_width * 2 - 1, 0);
                                
    camera_screen_to_global_pos(&min, &min);
    camera_screen_to_global_pos(&max, &max);
    world_cell_to_chunk_pos(&min, &min);
    world_cell_to_chunk_pos(&max, &max);

    for (i32 chunk_x = min.x; chunk_x <= max.x; chunk_x++) {
        for (i32 chunk_y = min.y; chunk_y <= max.y; chunk_y++) {
            Vec2i chunk_pos = ZINC_VEC2I_INIT(chunk_x, chunk_y);
            Chunk *chunk = world_get_chunk(&chunk_pos);
            if (chunk == NULL)
                continue;
            chunk_render(chunk, renderer);
        }
    }
}

/*
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
*/
