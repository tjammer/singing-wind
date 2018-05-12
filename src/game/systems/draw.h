#ifndef DRAW_H
#define DRAW_H

#include <vector>

struct Transform;
class StaticGrid;

void
draw_update(const Transform& pc, const StaticGrid& grid);

#endif /* DRAW_H */
