#include "../include/player.h"
#include "../include/world.h"
#include "../include/camera.h"

Player *player = NULL;

void player_init(const Vec2 *pos)
{
    player = calloc(1, sizeof *player);
    zinc_vec2_copy(pos, &player->pos);
    player->acc.y = GRAVITY;
}

static bool player_check_collision(f32 t, Vec2i *offset, f32 *hit_dis)
{
    Vec2i center = ZINC_VEC2I_INIT(floor(player->pos.x), floor(player->pos.y));

    Vec2 scale = ZINC_VEC2_ZERO_INIT;
    Vec2 mag = ZINC_VEC2_INIT(FLT_MAX, FLT_MAX);
    Vec2i step = ZINC_VEC2I_ZERO_INIT;

    if (player->vel.x < 0) {
        step.x = -1;
        mag.x = (player->pos.x - center.x) * scale.x;
        scale.x = sqrt(1 + (player->vel.y / player->vel.x) *
                       (player->vel.y / player->vel.x));
    }
    else if (player->vel.x > 0){
        step.x = 1;
        mag.x = (center.x - player->pos.x + 1) * scale.x;
        scale.x = sqrt(1 + (player->vel.y / player->vel.x) *
                       (player->vel.y / player->vel.x));
    }

    if (player->vel.y < 0) {
        step.y = -1;
        mag.y = (player->pos.y - center.y) * scale.y;
        scale.y = sqrt(1 + (player->vel.x / player->vel.y) * 
                       (player->vel.x / player->vel.y));
    }
    else if (player->vel.y > 0) {
        step.y = 1;
        mag.y = (center.y - player->pos.y + 1) * scale.y;
        scale.y = sqrt(1 + (player->vel.x / player->vel.y) * 
                       (player->vel.x / player->vel.y));
    }

    while (mag.x <= t || mag.y <= t) {
        if (mag.x < mag.y) {
            center.x += step.x;
            *hit_dis = mag.x;
            mag.x += scale.x;

            for (i32 i = PLAYER_HEIGHT - 1; i >= 0; i--) {

                Vec2i cell_pos = 
                    ZINC_VEC2I_INIT(center.x + step.x * PLAYER_WIDTH / 2, 
                                    center.y - PLAYER_HEIGHT / 2 + i);

                Cell *c = world_get_cell(&cell_pos);
                if (c != NULL && IS_EMPTY(c))
                    continue;

                if (i <= PLAYER_STEP_HEIGHT) {
                    offset->y += i + 1;
                    offset->x += step.x;
                    player->vel.y = 0.0;
                    return true;
                }

                player->step_time = 0.0f;
                player->vel.x = 0.0;
                return true;
            }

            continue;
        }

        center.y += step.y;
        *hit_dis = mag.y;
        mag.y += scale.y;

        for (i32 i = center.x - PLAYER_WIDTH / 2; 
             i <= center.x + PLAYER_WIDTH / 2; i++) {
            Vec2i cell_pos = 
                ZINC_VEC2I_INIT(i, center.y + step.y * PLAYER_HEIGHT / 2);

            Cell *c = world_get_cell(&cell_pos);
            if (c != NULL && IS_EMPTY(c))
                continue;

            player->vel.y = 0.0;
            return true;
        }
    }

    *hit_dis = t;
    player->step_time = 0.0f;
    return false;
}

void player_update()
{
    player->vel.x += (player->acc.x - 0.4f * player->vel.x) * FIXED_DELTA;
    player->vel.y += player->acc.y * FIXED_DELTA;

    f32 dt = FIXED_DELTA;

    while (dt > 0) {
        if (zinc_vec2_squared_len(&player->vel) == 0) {
            break;
        }

        f32 speed = zinc_vec2_len(&player->vel);
        Vec2 dir;
        zinc_vec2_copy(&player->vel, &dir);
        zinc_vec2_normalize(&dir);

        f32 hit_dis;
        Vec2i offset = ZINC_VEC2I_ZERO_INIT;
        bool hit = player_check_collision(speed * dt, &offset, &hit_dis);

        dt -= hit_dis / speed;

        zinc_vec2_scale(&dir, hit_dis, &dir);
        zinc_vec2_add(&player->pos, &dir, &player->pos);

        if (!hit) 
            break;

        f32 step_time = zinc_vec2i_len(&offset) / speed;
        if (offset.x != 0) {
            if (dt + player->step_time < step_time) {
                player->step_time += dt;
                break;
            }

            dt += player->step_time - step_time;
            player->step_time = 0.0f;
            player->pos.y += offset.y;
            player->pos.x += offset.x;
        }
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
