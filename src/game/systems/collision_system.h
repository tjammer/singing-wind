#ifndef COLLISION_SYSTEM_H
#define COLLISION_SYSTEM_H

#include <vector>

struct Collision;
struct Position;
template<typename T>
class StaticGrid;
struct StaticTriangle;

void
collision_update(Collision& cc, Position& pc, StaticGrid<StaticTriangle>& grid);

#endif /* COLLISION_SYSTEM_H */
