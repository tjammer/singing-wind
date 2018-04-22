#ifndef DRAW_H
#define DRAW_H

#include <vector>

struct Position;
template<typename T>
class StaticGrid;
struct StaticTriangle;

void
draw_update(const Position& pc, StaticGrid<StaticTriangle>& grid);

#endif /* DRAW_H */
