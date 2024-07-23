#include "../include/player.h"
#include "../include/world.h"
#include "../include/camera.h"
#include "../include/particle.h"

Player *player = NULL;

void player_init(const Vec2 *pos)
{
    player = calloc(1, sizeof *player);
    zinc_vec2_copy(pos, &player->pos);
    player->acc.y = GRAVITY;
}

static bool player_check_horizontal_collision(f32 *offset, f32 *vertical_offset)
{
    i32 start_y = floorf(player->pos.y + PLAYER_HEIGHT / 2.0f
                         - PLAYER_SKIN_WIDTH);
    i32 end_y = floorf(player->pos.y - PLAYER_HEIGHT / 2.0f
                       + PLAYER_SKIN_WIDTH);
    if (*offset < 0.0f) {
        i32 start_x = floorf(player->pos.x - PLAYER_WIDTH / 2.0f);
        i32 end_x = floorf(player->pos.x - PLAYER_WIDTH / 2.0f + *offset
                           + PLAYER_SKIN_WIDTH);
        for (i32 x = start_x; x >= end_x; x--) {
            for (i32 y = start_y; y >= end_y; y--) {
                Vec2i curr_cell = ZINC_VEC2I_INIT(x, y);
                Cell *c = world_get_cell(&curr_cell);
                if (c != NULL && IS_EMPTY(c))
                    continue;

                f32 diff = player->pos.x;
                player->pos.x = x + 1.0f + PLAYER_WIDTH / 2.0f;
                diff = player->pos.x - diff; 

                *offset -= diff;
                i32 step_height = y - end_y + 1;

                if (step_height <= PLAYER_STEP_HEIGHT) {
                    *vertical_offset = step_height;
                    return true;
                }

                player->vel.x = 0.0f;
                return false;
            }
        }
    }
    else {
        i32 start_x = floorf(player->pos.x + PLAYER_WIDTH / 2.0f);
        i32 end_x = floorf(player->pos.x + PLAYER_WIDTH / 2.0f + *offset
                           - PLAYER_SKIN_WIDTH);
        for (i32 x = start_x; x <= end_x; x++) {
            for (i32 y = start_y; y >= end_y; y--) {
                Vec2i curr_cell = ZINC_VEC2I_INIT(x, y);
                Cell *c = world_get_cell(&curr_cell);
                if (c != NULL && IS_EMPTY(c))
                    continue;

                f32 diff = player->pos.x;
                player->pos.x = x - PLAYER_WIDTH / 2.0f;
                diff = player->pos.x - diff; 

                *offset -= diff;
                i32 step_height = y - end_y + 1;
                if (step_height <= PLAYER_STEP_HEIGHT) {
                    *vertical_offset = step_height;
                    return true;
                }

                player->vel.x = 0.0f;
                return false;
            }
        }
    }

    player->pos.x += *offset;
    return false;
}

static bool player_check_vertical_collision(f32 offset)
{
    i32 start_x = floorf(player->pos.x - PLAYER_WIDTH / 2.0f
                         + PLAYER_SKIN_WIDTH);
    i32 end_x = floorf(player->pos.x + PLAYER_WIDTH / 2.0f 
                       - PLAYER_SKIN_WIDTH);
    if (offset < 0.0f) {
        i32 start_y = floorf(player->pos.y - PLAYER_HEIGHT / 2.0f);
        i32 end_y = floorf(player->pos.y - PLAYER_HEIGHT / 2.0f + offset
                           + PLAYER_SKIN_WIDTH);
        for (i32 y = start_y; y >= end_y; y--) {
            for (i32 x = start_x; x <= end_x; x++) {
                Vec2i curr_cell = ZINC_VEC2I_INIT(x, y);
                Cell *c = world_get_cell(&curr_cell);
                if (c != NULL && IS_EMPTY(c))
                    continue;

                player->pos.y = y + 1.0f + PLAYER_HEIGHT / 2.0f;
                player->vel.y = 0.0f;
                player->on_ground = true;
                return false;
            }
        }
    }
    else {
        i32 start_y = floorf(player->pos.y + PLAYER_HEIGHT / 2.0f);
        i32 end_y = floorf(player->pos.y + PLAYER_HEIGHT / 2.0f + offset
                           - PLAYER_SKIN_WIDTH);
        for (i32 y = start_y; y <= end_y; y++) {
            for (i32 x = start_x; x <= end_x; x++) {
                Vec2i curr_cell = ZINC_VEC2I_INIT(x, y);

                Cell *c = world_get_cell(&curr_cell);
                if (c != NULL && IS_EMPTY(c))
                    continue;

                player->pos.y = y - PLAYER_HEIGHT / 2.0f;
                player->vel.y = 0.0f;
                player->on_ground = true;
                return false;
            }
        }
    }

    player->on_ground = false;
    player->pos.y += offset;
    return true;
}

void player_update()
{
    Vec2i border_min;
    border_min.x = floorf(player->pos.x - PLAYER_WIDTH / 2.0f 
                          + PLAYER_SKIN_WIDTH);
    border_min.y = floorf(player->pos.y - PLAYER_HEIGHT / 2.0f
                          + PLAYER_SKIN_WIDTH);

    Vec2i border_max;
    border_max.x = floorf(player->pos.x + PLAYER_WIDTH / 2.0f
                          - PLAYER_SKIN_WIDTH);
    border_max.y = floorf(player->pos.y + PLAYER_HEIGHT / 2.0f
                          - PLAYER_SKIN_WIDTH);

    for (i32 y = border_min.y; y <= border_max.y; y++) {
        for (i32 x = border_min.x; x <= border_max.x; x++) {
            Vec2i curr_cell = ZINC_VEC2I_INIT(x, y);
            Cell *c = world_get_cell(&curr_cell);
            if (c != NULL && IS_EMPTY(c))
                continue;

            Vec2 particle_pos = ZINC_VEC2_INIT(x + 0.5f, y + 0.5f);
            Vec2 particle_vel; 
            zinc_vec2_sub(&particle_pos, &player->pos, &particle_vel);
            zinc_vec2_scale(&particle_vel, 1.5f, &particle_vel);

            particle_add(c, &particle_pos, &particle_vel);
            world_delete_cell(&curr_cell);
        }
    }

    player->vel.x += (player->acc.x - 0.4f * player->vel.x) * FIXED_DELTA;
    player->vel.y += player->acc.y * FIXED_DELTA;

    f32 speed = zinc_vec2_len(&player->vel);
    if (speed < PLAYER_MIN_SPEED)
        return;

    if (player->vel.y != 0.0f)
        player_check_vertical_collision(player->vel.y * FIXED_DELTA);

    f32 offset = player->vel.x * FIXED_DELTA;
    f32 vertical_offset = 0.0f;
    while (offset != 0.0f && 
           player_check_horizontal_collision(&offset, &vertical_offset)) {

        if (!player_check_vertical_collision(vertical_offset))
            break;
    }
}

void player_render()
{
    Vec2i center = ZINC_VEC2I_INIT(floor(player->pos.x), 
                                   floor(player->pos.y));
    camera_global_to_screen_pos(&center, &center);
    const SDL_Rect rect = {
        .x = center.x - PLAYER_WIDTH * camera->scale / 2,
        .y = center.y - PLAYER_HEIGHT * camera->scale / 2,
        .w = camera->scale * PLAYER_WIDTH,
        .h = camera->scale * PLAYER_HEIGHT
    };

    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderFillRect(renderer, &rect);
}
