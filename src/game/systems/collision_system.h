#ifndef COLLISION_SYSTEM_H
#define COLLISION_SYSTEM_H

#include <vector>

struct Collision;
struct Position;
struct Movement;
template<typename T>
class StaticGrid;
struct StaticTriangle;

namespace ecs {
class World;
template<typename T>
struct tag;
} // ecs

using HasCollided = ecs::tag<struct has_collided>;

void
collision_update(Collision& cc,
                 Position& pc,
                 ecs::World& world,
                 std::size_t id,
                 StaticGrid<StaticTriangle>& grid);

void
on_collision(HasCollided,
             Movement& mc,
             Collision& cc,
             ecs::World& world,
             std::size_t id);

#endif /* COLLISION_SYSTEM_H */
