#ifndef FALL_H
#define FALL_H

struct Movement;
struct Input;
struct Transform;

namespace ecs {
class World;
template<typename T>
struct tag;
} // ecs
using IsFalling = ecs::tag<struct is_falling>;

void
fall_update(IsFalling, Movement& mc, const Input& ic, const Transform& pc);

#endif /* FALL_H */
