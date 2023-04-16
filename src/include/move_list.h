#ifndef FS_ARRAY_LIST_H
#define FS_ARRAY_LIST_H

#include "c.h"

#define MOVE_LIST_INITIAL_SIZE 1024

typedef struct Move {
	Vec2i from;
	Vec2i to;
} Move;

typedef struct MoveList {
	Move *moves;
	u32 allocated_size;
	u32 size;
} MoveList;

MoveList *ml_create();
void ml_free(MoveList *ml);

void ml_clear(MoveList *ml);

void ml_add(MoveList *ml, Move move);
Move ml_get(MoveList *ml, u32 index);

i32 move_cmp(Move a, Move b);

void ml_sort(MoveList *ml);

#endif
