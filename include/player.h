#ifndef FS_PLAYER_H
#define FS_PLAYER_H

#include "./utils.h"

#define PLAYER_HEIGHT 19
#define PLAYER_WIDTH 7

#define PLAYER_DRAG 17.0f
#define PLAYER_VERTICAL_LEAP 4

typedef struct Player {
    Vec2 pos, vel, acc;
} Player;

extern Player *player;

void player_init(const Vec2 *pos);

void player_update();

void player_render();

#endif
