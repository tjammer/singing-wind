#ifndef COLLISION_SYSTEM_H
#define COLLISION_SYSTEM_H

#include <vector>

struct Collision;
struct Transform;
struct Movement;
class StaticGrid;

namespace ecs {
class World;
template<typename T>
struct tag;
} // ecs

using HasCollided = ecs::tag<struct has_collided>;

void
collision_update(Collision& cc,
                 Transform& pc,
                 ecs::World& world,
                 std::size_t id,
                 StaticGrid& grid);

void
on_collision(HasCollided,
             Movement& mc,
             Collision& cc,
             ecs::World& world,
             std::size_t id);

#endif /* COLLISION_SYSTEM_H */
