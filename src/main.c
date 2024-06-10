#include <SDL2/SDL.h>
#include "../include/utils.h"
#include "../include/world.h"
#include "../include/camera.h"
#include "../include/particle.h"
#include "../include/player.h"

#define WINDOW_WIDTH 1200
#define WINDOW_HEIGHT 800

SDL_Window *window = NULL;

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

    world_init();
    ml_init();
    camera_init(&ZINC_VEC2I(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2), 
                1, 
                WINDOW_WIDTH / 2, 
                WINDOW_HEIGHT / 2);
    particle_init();
    player_init(&ZINC_VEC2(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2));
    particle_add(&(Cell) {ID_SAND, 0.0f}, 
                 &ZINC_VEC2(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2), 
                 &ZINC_VEC2(1.0, -2.0));

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

    u32 second_count = 0;
    u32 frame_count = 0;

    u8 should_continue = 1;
    while (should_continue) {
        u32 curr_time = SDL_GetTicks();
        u32 delta = curr_time - last_time;
        last_time = curr_time;
        time_to_process += delta;
        second_count += delta;

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch(event.type){
            case SDL_QUIT:
                should_continue = 0;
                break;

            case SDL_MOUSEMOTION:
                {
                    SDL_MouseMotionEvent *motion = &event.motion;
                    mouse_x = motion->x;
                    mouse_y = motion->y;
                }
                break;

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

            case SDL_MOUSEWHEEL:
                {
                    SDL_MouseWheelEvent *wheel = &event.wheel;
                    if (wheel->y + camera->scale > 0)
                        camera->scale += wheel->y;
                }
                break;
            case SDL_KEYDOWN:
                {
                    SDL_KeyboardEvent *key = &event.key;
                    if (key->keysym.scancode == SDL_SCANCODE_A) {
                        player->acc.x = -100.0f;
                    }
                    else if (key->keysym.scancode == SDL_SCANCODE_D) {
                        player->acc.x = 100.0f;
                    }
                }
                break;
            case SDL_KEYUP:
                {
                    SDL_KeyboardEvent *key = &event.key;
                    if (key->keysym.scancode == SDL_SCANCODE_A || key->keysym.scancode == SDL_SCANCODE_D) {
                        player->acc.x = 0;
                    }
                }
                break;
            }
        }

        while (time_to_process >= ms_per_update) {
            // Fixed update
            world_update();
            player_update();
            particle_update();

            time_to_process -= ms_per_update;
        }

        camera->position.x = player->pos.x;
        camera->position.y = player->pos.y;

        frame_count += 1;
        if (second_count >= 1000) {
            printf("FPS: %d\n", frame_count * 1000 / second_count);
            frame_count = 0;
            second_count -= 1000;
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // Render
        world_render();
        particle_render();
        player_render();

        SDL_RenderPresent(renderer);
        SDL_Delay(1);
    }

    clean();
    return 0;
}
