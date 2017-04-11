//
// Created by jammer on 11/04/17.
//

#ifndef SINGING_WIND_PROTAGONIST_H
#define SINGING_WIND_PROTAGONIST_H

#include "WindDefs.h"
#include "Components.h"
#include "Collision.h"

class GameWorld;

const float c_walk_accel = 1600;
const float c_jump_tolerance = 0.1;
const float c_stop_friction = 8;
const float c_turn_mod = 4;
const float c_drag = 0.0046;
const float c_max_floor_angle = 0.7;

MovementFct enter_ground(InputComponent &ic, MoveComponent &mc);
void on_ground(InputComponent &ic, MoveComponent &mc);
MovementFct enter_jump(InputComponent &ic, MoveComponent &mc);
void jump(InputComponent &ic, MoveComponent &mc);
MovementFct enter_falling(InputComponent &ic, MoveComponent &mc);
void falling(InputComponent &ic, MoveComponent &mc);

void on_static_collision(const ColResult &result, GameWorld &world, unsigned int entity);

template<typename T>
void clear_arr(std::vector<T> &arr, const T &val) {
    for (uint i = 0 ; i < arr.size() ; ++i) {
        arr[i] = val;
    }
}

#endif //SINGING_WIND_PROTAGONIST_H
