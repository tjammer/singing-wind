#ifndef DRAW_H
#define DRAW_H

#include <vector>

struct Position;
class ColTriangle;

void
draw_update(const Position& pc, std::vector<ColTriangle>& tris);

#endif /* DRAW_H */
