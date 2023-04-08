#include <SDL2/SDL.h>
#include "include/c.h"

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;

SDL_PixelFormat *format = NULL;

void clean()
{
	if(renderer) SDL_DestroyRenderer(renderer);
	if(window) SDL_DestroyWindow(window);

	SDL_Quit();
}
	

int main()
{
	if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER)){
		printf("%s\n", SDL_GetError());
		return 1;
	}

	window = SDL_CreateWindow("Falling Sand Game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 800, 0);

	if(!window){
		printf("%s\n", SDL_GetError());
		clean();
		return 1;
	}

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	if(!renderer){
		printf("%s\n", SDL_GetError());
		clean();
		return 1;
	}

	SDL_Texture *texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB888, SDL_TEXTUREACCESS_STREAMING, 800, 800);

	format = SDL_AllocFormat(SDL_PIXELFORMAT_RGB888);

	u32 ms_per_update = 1000 / 100;

	u32 ms_per_frame = 1000 / 30;

	u32 second_count = 0;
	u32 fps = 0;
	u32 frame_count = 0;

	u32 last_time = SDL_GetTicks();
	u32 time_to_process = 0;

	u32 delay = 0;

	u8 should_continue = 1;
	while(should_continue){
		u32 curr_time = SDL_GetTicks();
		u32 delta = curr_time - last_time;
		last_time = curr_time;
		time_to_process += delta;
		second_count += delta;

		SDL_Event event;
		while(SDL_PollEvent(&event)){
			switch(event.type){
			case SDL_QUIT:
				should_continue = 0;
				break;
			}
		}

#ifdef DEBUG
		printf("fps = %d, delta = %d ms, time to process = %d, delay = %d\n", fps, delta, time_to_process, delay);
#endif
		while(time_to_process >= ms_per_update){
			//Update

			time_to_process -= ms_per_update;
		}

		if(second_count >= 1000){
			fps = frame_count;
			frame_count = 0;
			second_count -= 1000;
		}

		if(delta > delay){
			SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
			SDL_RenderClear(renderer);

			u32 *pixels;
			i32 pitch;

			SDL_LockTexture(texture, NULL, (void **) &pixels, &pitch);

			//render

			SDL_UnlockTexture(texture);

			SDL_RenderCopy(renderer, texture, NULL, NULL);
			SDL_RenderPresent(renderer);

			frame_count++;

			delay = ms_per_frame - ((delta - delay) % ms_per_frame);
		}
		else delay -= delta;


		SDL_Delay(1);
	}

	clean();
	return 0;
}
