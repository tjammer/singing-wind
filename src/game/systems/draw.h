#ifndef DRAW_H
#define DRAW_H

#include <vector>

struct Transform;
class StaticGrid;
struct Appearance;

void
debug_draw_update(const Transform& pc, const StaticGrid& grid);

void
draw_update(const Appearance& ac, const Transform& t);

#endif /* DRAW_H */
