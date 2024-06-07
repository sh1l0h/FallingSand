#ifndef FS_CAMERA_H
#define FS_CAMERA_H

#include "./utils.h"
#include <SDL2/SDL.h>

typedef struct Camera {
    Vec2i position;
    u32 scale, half_width, half_height;
} Camera;

extern Camera *camera;
extern SDL_Renderer *renderer;

void camera_init(const Vec2i *pos, u32 scale, u32 half_width, u32 half_height);

void camera_screen_to_global_pos(const Vec2i *screen_pos, Vec2i *global_pos);
void camera_global_to_screen_pos(const Vec2i *global_pos, Vec2i *screen_pos);

#endif
