#include <SDL2/SDL.h>
#include "../include/utils.h"
#include "../include/world.h"
#include "../include/camera.h"

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 640

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;

SDL_PixelFormat *format = NULL;

void clean()
{
	if (renderer) 
        SDL_DestroyRenderer(renderer);
	if (window) 
        SDL_DestroyWindow(window);

	SDL_Quit();
}

int main()
{
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER)) {
		printf("%s\n", SDL_GetError());
		return 1;
	}

	window = SDL_CreateWindow("Falling Sand Game", 
                              SDL_WINDOWPOS_UNDEFINED,
                              SDL_WINDOWPOS_UNDEFINED,
                              WINDOW_WIDTH,
                              WINDOW_HEIGHT,
                              0);

	if (window == NULL) {
		printf("%s\n", SDL_GetError());
		clean();
		return 1;
	}

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	if (renderer == NULL) {
		printf("%s\n", SDL_GetError());
		clean();
		return 1;
	}

    ml_init();

    camera_init(&ZINC_VEC2I(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2), 
                3, 
                WINDOW_WIDTH / 2, 
                WINDOW_HEIGHT / 2);

    for (i32 i = 0; i < WINDOW_WIDTH / CHUNK_DIM; i++) {
        for (i32 j = 0; j < WINDOW_WIDTH / CHUNK_DIM; j++) {
            const Vec2i chunk_pos = ZINC_VEC2I_INIT(i, j);
            world_add_chunk(&chunk_pos);
        }
    }

    u32 ms_per_update = 1000 / 60;

    u32 last_time = SDL_GetTicks();
    u32 time_to_process = 0;

    i32 mouse_x = 0;
    i32 mouse_y = 0;

    u8 should_continue = 1;
    while (should_continue) {
        u32 curr_time = SDL_GetTicks();
        u32 delta = curr_time - last_time;
        last_time = curr_time;
        time_to_process += delta;

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch(event.type){
            case SDL_QUIT:
                should_continue = 0;
                break;

            case SDL_MOUSEMOTION:
                {
                    SDL_MouseMotionEvent motion = event.motion;
                    mouse_x = motion.x;
                    mouse_y = motion.y;
                    break;
                }
            case SDL_MOUSEBUTTONDOWN:
                {
                    for (int j = MAX(mouse_y - 10, 0); 
                         j < MIN(mouse_y + 10, WINDOW_HEIGHT); 
                         j++) {
                        for (int i = MAX(mouse_x - 10, 0); 
                             i < MIN(mouse_x + 10, WINDOW_WIDTH);
                             i++) {
                            Vec2i cell_pos = ZINC_VEC2I_INIT(i, j);
                            camera_screen_to_global_pos(&cell_pos, &cell_pos);
                            world_get_chunk_by_cell(&cell_pos)->should_be_updated = 1;
                            world_set_cell(&cell_pos, &(Cell) {ID_SAND, 0.0f});
                        }
                    }

                }
                break;
            }
        }

        printf("fps = %dms\n", delta);
        while (time_to_process >= ms_per_update) {
            // Fixed update
            world_update(ms_per_update);

            time_to_process -= ms_per_update;
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // Render
        world_render();

        SDL_RenderPresent(renderer);
        SDL_Delay(1);
    }

    clean();
    return 0;
}
