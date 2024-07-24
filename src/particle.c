#include "../include/particle.h"
#include "../include/world.h"
#include "../include/camera.h"

static size_t allocated = 8;
static size_t size = 0;
static Particle *particles = NULL;

void particle_init()
{
    particles = malloc(allocated * sizeof *particles);
}

void particle_deinit()
{
    free(particles);
}

void particle_add(const Cell *cell, const Vec2 *pos, const Vec2 *vel)
{
    cell_copy(cell, &particles[size].cell);
    zinc_vec2_copy(pos, &particles[size].pos);
    zinc_vec2_copy(vel, &particles[size].vel);
    size += 1;
    
    if (size >= allocated) {
        allocated *= 2;
        particles = realloc(particles, allocated * sizeof *particles);
    }
}

static bool particle_check_collision(const Vec2 *origin,
                                     const Vec2 *dir,
                                     const f32 t,
                                     f32 *hit_dis,
                                     Vec2 *hit_norm)
{
    Vec2i cell_pos = ZINC_VEC2I_INIT(floor(origin->x), floor(origin->y));

    Vec2 scale;  
    scale.x = sqrt(1 + (dir->y / dir->x) * (dir->y / dir->x));
    scale.y = sqrt(1 + (dir->x / dir->y) * (dir->x / dir->y));

    Vec2 mag = ZINC_VEC2_INIT(FLT_MAX, FLT_MAX);
    Vec2i step = ZINC_VEC2I_INIT(SIGN(dir->x), SIGN(dir->y));

    if (step.x < 0)
        mag.x = (origin->x - cell_pos.x) * scale.x;
    else
        mag.x = (cell_pos.x - origin->x + 1) * scale.x;

    if (step.y < 0) 
        mag.y = (origin->y - cell_pos.y) * scale.y;
    else
        mag.y = (cell_pos.y - origin->y + 1) * scale.y;

    while (mag.x <= t || mag.y <= t) {
        if (mag.x < mag.y) {
            cell_pos.x += step.x;
            *hit_dis = mag.x;
            mag.x += scale.x;
            hit_norm->x = -step.x;
            hit_norm->y = 0;
        }
        else {
            cell_pos.y += step.y;
            *hit_dis = mag.y;
            mag.y += scale.y; 
            hit_norm->y = -step.y;
            hit_norm->x = 0;
        }

        Cell *c = world_get_cell(&cell_pos);
        if (c == NULL || !IS_EMPTY(c))
            return true;
    }

    *hit_dis = t;
    return false;
}

void particle_update()
{
    for (size_t i = 0; i < size; i++) {
        Particle *curr = particles + i;
        curr->vel.y += GRAVITY * FIXED_DELTA;

        f32 t = FIXED_DELTA;

        while (t > 0) {
            Vec2 dir;
            zinc_vec2_copy(&curr->vel, &dir);
            zinc_vec2_normalize(&dir);

            f32 speed = zinc_vec2_len(&curr->vel);

            f32 hit_dis;
            Vec2 hit_norm;
            bool hit = particle_check_collision(&curr->pos, 
                                                &dir,
                                                speed * t,
                                                &hit_dis,
                                                &hit_norm);

            zinc_vec2_scale(&dir, hit_dis, &dir);
            zinc_vec2_add(&curr->pos, &dir, &curr->pos);

            if (!hit) 
                break;

            t -= hit_dis / speed;

            Vec2 tmp;
            zinc_vec2_scale(&hit_norm, 
                            2 * zinc_vec2_dot(&curr->vel, &hit_norm), 
                            &tmp);
            zinc_vec2_sub(&curr->vel, &tmp, &curr->vel); 
            zinc_vec2_scale(&curr->vel, 0.5f, &curr->vel);

            if (zinc_vec2_len(&curr->vel) < PARTICLE_MIN_SPEED) {
                Vec2i cell_pos = ZINC_VEC2I_INIT(floor(curr->pos.x), 
                                                 floor(curr->pos.y));

                if (hit_norm.x < 0)
                    cell_pos.x -= 1;

                if (hit_norm.y < 0)
                    cell_pos.y -= 1;

                world_set_cell(&cell_pos, &curr->cell);

                if (size > 1) {
                    cell_copy(&particles[size - 1].cell, &curr->cell);
                    zinc_vec2_copy(&particles[size - 1].pos, &curr->pos);
                    zinc_vec2_copy(&particles[size - 1].vel, &curr->vel);
                    i -= 1;
                }

                size -= 1;
                break;
            }
        }

    }
}

void particle_render() 
{
    for (size_t i = 0; i < size; i++) {
        Particle *curr = particles + i;

        Vec2i cell_pos = ZINC_VEC2I_INIT(floor(curr->pos.x), 
                                         floor(curr->pos.y));
        camera_global_to_screen_pos(&cell_pos, &cell_pos);
        const SDL_Rect rect = {
            .x = cell_pos.x - camera->scale / 2,
            .y = cell_pos.y - camera->scale / 2,
            .w = camera->scale,
            .h = camera->scale
        };

        switch (curr->cell.id) {
        case ID_SAND:
            SDL_SetRenderDrawColor(renderer, 236, 204, 162, 255);
            break;
        }

        SDL_RenderFillRect(renderer, &rect);
    }
}
