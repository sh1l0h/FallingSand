#ifndef FS_ARRAY_LIST_H
#define FS_ARRAY_LIST_H

#include "utils.h"
#include "cell.h"

#define MOVE_LIST_INITIAL_SIZE 1024

typedef struct Move {
    Vec2i src;
	Vec2i dest;
    Cell cell;
} Move;

typedef struct MoveList {
	Move *moves;
	u32 allocated_size;
	u32 size;
} MoveList;

MoveList *ml_create();
void ml_free(MoveList *ml);

void ml_clear(MoveList *ml);

void ml_add(MoveList *ml, const Vec2i *src, const Vec2i *dest, const Cell *cell);
Move *ml_get(const MoveList *ml, const u32 index);

i32 move_cmp(const Move *a, const Move *b);
void ml_sort(MoveList *ml);

#endif
