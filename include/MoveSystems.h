//
// Created by tobi on 4/27/17.
//

#ifndef SINGING_WIND_MOVESYSTEMS_H
#define SINGING_WIND_MOVESYSTEMS_H

#include "GameWorld.h"
#include <assert.h>

std::function<void(GameWorld &world, unsigned int entity)> get_accel_func(const MoveState &state, const MoveSet &set);
std::function<void(GameWorld &world, unsigned int entity)> get_trans_func(const MoveTransition &trans, const MoveSet &set);


namespace protagonist {
    void to_ground(GameWorld &world, unsigned int entity);
    void on_ground(GameWorld &world, unsigned int entity);
    void to_jumping(GameWorld &world, unsigned int entity);
    void jumping(GameWorld &world, unsigned int entity);
    void to_falling(MoveComponent &mc);
    void to_flying(GameWorld &world, unsigned int entity);
    void flying(GameWorld &world, unsigned int entity);
    void to_flying_accel(GameWorld &world, unsigned int entity);
    void flying_accel(GameWorld &world, unsigned int entity);
}
#endif //SINGING_WIND_MOVESYSTEMS_H
