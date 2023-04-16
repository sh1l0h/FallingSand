#include "include/world.h"
#include <SDL2/SDL.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

World world;

extern SDL_PixelFormat *format;

extern SDL_Renderer *renderer;

void create_world(i32 width, i32 height)
{
	world.width = width;
	world.height = height;
	world.chunks = malloc(sizeof(Chunk)*width*height);
	world.ml = ml_create();

	for(i32 x = 0; x < width; x++){
		for(i32 y = 0; y < height; y++){
			world.chunks[y*width + x] = chunk_create(x, y);
		}
	}
}

inline Chunk *world_get_chunk(i32 x, i32 y)
{
	return world.chunks[y*world.width + x];
}

inline Chunk *get_chunk(i32 x, i32 y)
{
	i32 chunk_x = x / CHUNK_DIM;
	i32 chunk_y = y / CHUNK_DIM;
	
	return world_get_chunk(chunk_x, chunk_y); 
}

inline Cell delete_cell(i32 x, i32 y)
{

#ifdef DEBUG
	assert(in_bounds(x, y) && "Out of bounds in delete_cell");
#endif

	return chunk_delete_cell(get_chunk(x,y), x % CHUNK_DIM, y % CHUNK_DIM);
}

inline Cell get_cell(i32 x, i32 y)
{

#ifdef DEBUG
	assert(in_bounds(x, y) && "Out of bounds in get_cell");
#endif

	return chunk_get_cell(get_chunk(x, y), x % CHUNK_DIM, y % CHUNK_DIM);
}

inline void set_cell(i32 x, i32 y, Cell cell)
{

#ifdef DEBUG
	assert(in_bounds(x, y) && "Out of bounds in set_cell");
#endif

	chunk_set_cell(get_chunk(x, y), x % CHUNK_DIM, y % CHUNK_DIM, cell);
}

i32 in_bounds(i32 x, i32 y)
{
	return x >= 0 && x < world.width * CHUNK_DIM && y >= 0 && y < world.height * CHUNK_DIM;
}

void update(f32 dt)
{
	// Collecting moves from chunks
	for(i32 chunk_y = 0; chunk_y < world.height; chunk_y++){
		for(i32 chunk_x = 0; chunk_x < world.width; chunk_x++){

			Chunk *curr = world_get_chunk(chunk_x, chunk_y);
			if(!curr->should_be_updated) continue;

			u32 move_count = 0;
			for(i32 y = curr->min_update_zone.y; y <= curr->max_update_zone.y; y++){
				for(i32 x = curr->min_update_zone.x; x <= curr->max_update_zone.x; x++){
					Cell curr_cell = chunk_get_cell(curr, x, y); 
					if(curr_cell.id == 0) continue;

					i32 global_x = chunk_x*CHUNK_DIM + x;
					i32 global_y = chunk_y*CHUNK_DIM + y;

					Move move;

					if(curr_cell.id & MOVES_DOWN && in_bounds(global_x, global_y + 1) && get_cell(global_x, global_y + 1).id == EMPTY_ID){
						move.from = (Vec2i) {{global_x, global_y}};
						move.to = (Vec2i) {{global_x, global_y + 1}};
						ml_add(world.ml, move);

						move_count = 1;
						continue;
					}

					if(curr_cell.id & MOVES_DOWN_SIDES){
						if(in_bounds(global_x + 1, global_y + 1) && get_cell(global_x + 1, global_y + 1).id == EMPTY_ID){
							move.from = (Vec2i) {{global_x, global_y}};
							move.to = (Vec2i) {{global_x + 1, global_y + 1}};
							ml_add(world.ml, move);
							move_count = 1;
							continue;
						}
						if(in_bounds(global_x - 1, global_y + 1) && get_cell(global_x - 1, global_y + 1).id == EMPTY_ID){
							move.from = (Vec2i) {{global_x, global_y}};
							move.to = (Vec2i) {{global_x - 1, global_y + 1}};
							ml_add(world.ml, move);
							move_count = 1;
							continue;
						}
					}
				}
			}

			curr->should_be_updated = move_count;
			curr->min_update_zone = (Vec2i) {{CHUNK_DIM, CHUNK_DIM}};
			curr->max_update_zone = (Vec2i) {{-1, -1}};
		}
	}

	// Executing the collected moves 
	ml_sort(world.ml);

	u32 group_size = 1;
	for(u32 i = 0; i < world.ml->size; i++){
		if(i + 1 < world.ml->size && !move_cmp(ml_get(world.ml, i), ml_get(world.ml, i + 1))){
			group_size++;
			continue;
		}

		u32 random_number = rand() % group_size;
		Move move_to_execute = ml_get(world.ml, i - random_number); 

		Cell cell_to_move = delete_cell(move_to_execute.from.x, move_to_execute.from.y);

		Chunk *dest_chunk = get_chunk(move_to_execute.to.x, move_to_execute.to.y);
		dest_chunk->should_be_updated = 1;

		chunk_set_cell(dest_chunk, move_to_execute.to.x % CHUNK_DIM, move_to_execute.to.y % CHUNK_DIM, cell_to_move);

		group_size = 1;
	}

	ml_clear(world.ml);
}
	
void render(u32 *pixels, f32 dt)
{
	for(i32 y = 0; y < world.height*CHUNK_DIM; y++){
		for(i32 x = 0; x < world.width*CHUNK_DIM; x++){
			Cell curr = get_cell(x, y);
			switch(curr.id){
			case EMPTY_ID:
				pixels[y*640 + x] = SDL_MapRGB(format, 0, 0, 0);
				break;
			case SAND_ID:
				pixels[y*640 + x] = SDL_MapRGB(format, 236, 204, 162);
				break;
				
			}

		}
	}
}

void render_debug()
{
	for(i32 chunk_y = 0; chunk_y < world.height; chunk_y++){
		for(i32 chunk_x = 0; chunk_x < world.width; chunk_x++){

			Chunk *curr_chunk = world_get_chunk(chunk_x, chunk_y);

			SDL_Rect chunk_border = {.x = chunk_x*CHUNK_DIM, .y = chunk_y*CHUNK_DIM, .w = CHUNK_DIM, .h = CHUNK_DIM};

			if(curr_chunk->should_be_updated){
				SDL_SetRenderDrawColor(renderer, 0, 255, 0, 100);
				SDL_Rect update_border = {
					.x = chunk_border.x + curr_chunk->min_update_zone.x,
					.y = chunk_border.y + curr_chunk->min_update_zone.y,
					.w = curr_chunk->max_update_zone.x - curr_chunk->min_update_zone.x,
					.h = curr_chunk->max_update_zone.y - curr_chunk->min_update_zone.y
				};

				SDL_RenderDrawRect(renderer, &update_border);

				SDL_SetRenderDrawColor(renderer, 255, 0, 0, 100);
			}
			else SDL_SetRenderDrawColor(renderer, 255, 255, 255, 100);

			SDL_RenderDrawRect(renderer, &chunk_border);
		}
	}
}
