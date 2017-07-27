//
// Created by tobi on 4/27/17.
//

#ifndef SINGING_WIND_MOVESYSTEMS_H
#define SINGING_WIND_MOVESYSTEMS_H

#include "WindDefs.h"
#include <assert.h>
#include <functional>
#include <map>
#include <vector>

class GameWorld;

enum class MoveState : int {
    OnGround,
    Falling,
    Flying,
    FlyingAccel,
    SimpleFlying,
    state_count
};
const std::map<MoveState, const char*> movestate_string = {
    {MoveState::OnGround, "OnGround"},
    {MoveState::Falling, "Falling"},
    {MoveState::Flying, "Flying"},
    {MoveState::FlyingAccel, "FlyingAccel"},
    {MoveState::SimpleFlying, "SimpleFlying"}
};

enum class MoveSet : int {
    Protagonist,
    TestEnemy,
    Special
};
const std::map<MoveSet, const char*> moveset_string = {
    {MoveSet::Protagonist, "Protagonist"},
    {MoveSet::TestEnemy, "TestEnemy"},
    {MoveSet::Special, "Special"}
};


struct MoveComponent {
    WVec velocity = {0, 0};
    WVec accel = {0, 0};
    WVec additional_force = {0, 0};
    MoveState movestate;
    MoveSet moveset;
    float mass = 1;
};

struct GroundMoveComponent {
    float c_accel = 1000;
    float c_stop_friction = 8;
    float c_turn_mod = 4;
    float air_time = 0;
    float c_max_vel = 100;
};

struct FallComponent {
    float c_accel = 500;
    float c_turn_mod = 4;
    float c_jump_height = 1000;
    float c_max_vel = 100;
};

struct FlyComponent {
    float c_lift = 0.0055;
    float c_stall_angle = 0.26;
    float c_max_change_angle = 4;
    float c_accel_force = 1000;
    float c_accel_time = 0.7f;
    float c_drag = 0.0026;
    float c_push_vel = 500.f;
    float timer = 0;

    // bcurve for acceling
    WVec from = {0.000, 0.000};
    WVec ctrl_from = {0.819312513, -0.31962499};
    WVec ctrl_to = {-0.0288125277, 1.40112495};
    WVec to = {1.000, 0.870875001};
};

struct SimpleFlyComponent {
    float c_max_vel = 500;
    float c_accel = 1000;
    float c_near_threshold = 10;
    float c_stop_coef = 0.04;
    float c_max_change_angle = 0.06;
};

std::function<void(GameWorld &world, unsigned int entity)> get_accel_func(const MoveState &state);
std::function<bool(GameWorld &world, unsigned int entity)> get_trans_func(const MoveState &trans);
std::function<void(GameWorld &world, unsigned int entity)> get_to_func(const MoveState &state);
const std::vector<MoveState> & get_trans_funcs(const MoveSet &set, const MoveState &state); 
#endif //SINGING_WIND_MOVESYSTEMS_H
