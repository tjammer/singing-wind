//
// Created by tobi on 4/27/17.
//

#ifndef SINGING_WIND_MOVESYSTEMS_H
#define SINGING_WIND_MOVESYSTEMS_H

#include "Components.h"
#include <assert.h>
#include <functional>


class GameWorld;

std::function<void(GameWorld &world, unsigned int entity)> get_accel_func(const MoveState &state, const MoveSet &set);
std::function<void(GameWorld &world, unsigned int entity)> get_trans_func(const MoveTransition &trans, const MoveSet &set);
#endif //SINGING_WIND_MOVESYSTEMS_H
