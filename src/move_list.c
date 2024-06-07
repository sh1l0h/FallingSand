#include "../include/move_list.h"

MoveList *ml = NULL;

inline void ml_init()
{
    ml = malloc(sizeof *ml);
	ml->moves = malloc(MOVE_LIST_INITIAL_SIZE * sizeof *ml->moves);
	ml->allocated_size = MOVE_LIST_INITIAL_SIZE;
	ml->size = 0;
}
	
inline void ml_deinit()
{
	free(ml->moves);
}

inline void ml_clear()
{
	ml->size = 0;
}

void ml_add(const Vec2i *src, const Vec2i *dest, const Cell *cell)
{
    zinc_vec2i_copy(src, &ml->moves[ml->size].src);
    zinc_vec2i_copy(dest, &ml->moves[ml->size].dest);
    cell_copy(cell, &ml->moves[ml->size].cell);
    
	ml->size += 1;
	if (ml->size >= ml->allocated_size) {
        ml->allocated_size *= 2;
        ml->moves = realloc(ml->moves, ml->allocated_size * sizeof *ml->moves);
    }
}

inline Move *ml_get(const u32 index)
{
	return ml->moves + index;
}

i32 move_cmp(const Move *a, const Move *b)
{
	return a->dest.x != b->dest.x || a->dest.y != b->dest.y;
}

static i32 move_cmp_wrap(const void *a, const void *b)
{
	return move_cmp(a, b);
}

inline void ml_sort()
{
	// qsort function is not guaranteed to have a time complexity of O(n log n), it is a temporary solution.
	qsort(ml->moves, ml->size, sizeof(Move), move_cmp_wrap);
}
