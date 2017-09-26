#include "steering.h"
#include "WVecMath.h"
#include "GameWorld.h"
#include "MoveSystems.h"

WVec seek(const WVec& direction, GameWorld &world, unsigned int entity) {
    assert(w_magnitude(direction) == 1);
    auto &mc = world.move_c(entity);
    auto velocity = w_normalize(mc.velocity);
    return direction - velocity;
}

WVec flee(const WVec& direction, GameWorld &world, unsigned int entity) {
    return -seek(direction, world, entity);
}

WVec arrive_velocity(const WVec& unnormed_direction, const WVec& unnormed_velocity, float arrive_radius) {
    auto direction = w_normalize(unnormed_direction);
    auto distance = w_magnitude(unnormed_direction);

    auto desired = direction * w_magnitude(unnormed_velocity) * distance / arrive_radius;
    return desired - unnormed_velocity;
}
