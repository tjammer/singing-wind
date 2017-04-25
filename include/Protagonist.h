//
// Created by jammer on 11/04/17.
//

#ifndef SINGING_WIND_PROTAGONIST_H
#define SINGING_WIND_PROTAGONIST_H

#include "WindDefs.h"
#include "Components.h"
#include "Collision.h"

class GameWorld;

const float c_walk_accel = 800;
const float c_air_accel_mod = 0.5;
const float c_jump_tolerance = 0.1;
const float c_stop_friction = 8;
const float c_turn_mod = 4;
const float c_jump_speed = 600;
const float c_lift = 0.0055;
const float c_stall_angle = 0.26;
const float c_max_change_angle = 6;
const float c_fly_accel_force = 3000;
const float c_fly_accel_time = 0.5f;
const float c_fly_accel_cd = 1.f;

struct MOnGround : BaseMovementState {
    void accel(GameWorld &world, unsigned int entity) override;

    MOnGround() {m_move_state = MoveStateName ::OnGround;}
};

struct MJumping : BaseMovementState {
    void accel(GameWorld &world, unsigned int entity) override;

    MJumping(InputComponent &ic, MoveComponent &mc);
};

struct MFalling : BaseMovementState {
    void accel(GameWorld &world, unsigned int entity) override;

    MFalling() {m_move_state = MoveStateName::Falling;}
};

struct MFlying : BaseMovementState {
    void accel(GameWorld &world, unsigned int entity) override;

    MFlying(GameWorld &world, unsigned int entity, bool set_rotation=false);
};

// for now, this needs to be a skill later
struct MFlyingAccel : BaseMovementState {
    void accel(GameWorld &world, unsigned int entity) override;

    // bezier curve
    WVec from =       {0.000, 0.000};
    WVec ctrl_from =  {0.249, 2.071};
    WVec ctrl_to =    {0.729, 0.323};
    WVec to =         {1.000, 0.074};

    float m_timer = 0;

    MFlyingAccel() {m_move_state = MoveStateName ::FlyingAccel;};
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
