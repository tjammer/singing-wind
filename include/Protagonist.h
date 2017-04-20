//
// Created by jammer on 11/04/17.
//

#ifndef SINGING_WIND_PROTAGONIST_H
#define SINGING_WIND_PROTAGONIST_H

#include "WindDefs.h"
#include "Components.h"
#include "Collision.h"

class GameWorld;

const float c_walk_accel = 1500;
const float c_jump_tolerance = 0.1;
const float c_stop_friction = 8;
const float c_turn_mod = 4;
const float c_jump_speed = 800;

struct OnGround : BaseMovementState {
    void accel(InputComponent &ic, MoveComponent &mc) override;

    OnGround() {m_move_state = MoveStateName ::OnGround;}
};

struct InJump : BaseMovementState {
    void accel(InputComponent &ic, MoveComponent &mc) override;

    InJump(InputComponent &ic, MoveComponent &mc);
};

struct InFalling : BaseMovementState {
    void accel(InputComponent &ic, MoveComponent &mc) override;

    InFalling() {m_move_state = MoveStateName::Falling;}
};

void on_static_collision(const ColResult &result, GameWorld &world, unsigned int entity);

void handle_inputs(InputComponent &ic, const WVec &mouse);

unsigned int create_player(GameWorld &world, const WVec &pos, unsigned int parent);

template<typename T>
void clear_arr(std::deque<T> &arr, const T &val) {
    for (uint i = 0 ; i < arr.size() ; ++i) {
        arr[i] = val;
    }
}

#endif //SINGING_WIND_PROTAGONIST_H
