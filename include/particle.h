#ifndef FS_PARTICLE_H
#define FS_PARTICLE_H

#include "cell.h"

typedef struct Particle {
	u16 id;
	Vec2i pos;
	Vec2 acc;
	Vec2 vel;
} Particle;

#endif
