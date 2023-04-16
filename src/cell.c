#include "include/cell.h"

Cell empty;
Cell sand;

void init_cells()
{
	empty = create_cell(EMPTY_ID);
	sand = create_cell(SAND_ID);
}

inline Cell create_cell(u16 id)
{
	Cell result;
	result.id = id;
	result.acc = (Vec2f) {{0.0f, 0.0f}};
	result.vel = (Vec2f) {{0.0f, 0.0f}};
	return result;
}
