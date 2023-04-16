#include "include/move_list.h"
#include <stdlib.h>

MoveList *ml_create()
{
	MoveList *result = malloc(sizeof(MoveList));
	result->moves = malloc(sizeof(Move)*MOVE_LIST_INITIAL_SIZE);
	result->allocated_size = MOVE_LIST_INITIAL_SIZE;
	result->size = 0;

	return result;
}
	
void ml_free(MoveList *ml)
{
	free(ml->moves);
	free(ml);
}

inline void ml_clear(MoveList *ml)
{
	ml->size = 0;
}

void ml_add(MoveList *ml, Move move)
{
	ml->moves[ml->size++] = move;

	if(ml->size >= ml->allocated_size) ml->moves = realloc(ml->moves, sizeof(Move)*(ml->allocated_size *= 2));
}

inline Move ml_get(MoveList *ml, u32 index)
{
	return ml->moves[index];
}

i32 move_cmp(Move a, Move b)
{
	return a.to.x != b.to.x || a.to.y != b.to.y;
}

static i32 move_cmp_wrap(const void *a, const void *b)
{
	return move_cmp(*(Move*)a, *(Move*)b);
}

inline void ml_sort(MoveList *ml)
{
	//qsort function is not guaranteed to have a time complexity of O(n log n), it is a temporary solution.
	qsort(ml->moves, ml->size, sizeof(Move), move_cmp_wrap);
}
