#ifndef FS_PARTICLE_H
#define FS_PARTICLE_H

#include "utils.h"
#include "cell.h"

#define PARTICLE_MIN_SPEED 1.0

typedef struct Particle {
    Cell cell; 
	Vec2 pos;
	Vec2 vel;
} Particle;

void particle_init();
void particle_deinit();

void particle_add(const Cell *cell, const Vec2 *pos, const Vec2 *vel);

void particle_update();
void particle_render();

#endif
