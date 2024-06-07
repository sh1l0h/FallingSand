#include "../include/camera.h"

Camera *camera = NULL;
SDL_Renderer *renderer = NULL;

void camera_init(const Vec2i *pos, u32 scale, u32 half_width, u32 half_height)
{
    camera = malloc(sizeof *camera);
    zinc_vec2i_copy(pos, &camera->position);
    camera->scale = scale;
    camera->half_width = half_width;
    camera->half_height = half_height;
}

void camera_screen_to_global_pos(const Vec2i *screen_pos, Vec2i *global_pos)
{
    Vec2i t = ZINC_VEC2I_INIT(camera->half_width, camera->half_height);
    zinc_vec2i_sub(screen_pos, &t, &t);
    t.y *= -1;
    zinc_vec2i_div(&t, camera->scale, &t);
    zinc_vec2i_add(&t, &camera->position, global_pos);
}

void camera_global_to_screen_pos(const Vec2i *global_pos, Vec2i *screen_pos)
{
    zinc_vec2i_sub(global_pos, &camera->position, screen_pos);
    zinc_vec2i_scale(screen_pos, camera->scale, screen_pos);
    screen_pos->y *= -1;
    zinc_vec2i_add(screen_pos, 
                   &ZINC_VEC2I(camera->half_width, camera->half_height),
                   screen_pos);
}
