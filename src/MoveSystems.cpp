//
// Created by tobi on 4/27/17.
//

#include "MoveSystems.h"
#include "Protagonist.h"
#include <unordered_map>
#include "TestEnemy.h"

using namespace std;
using accel_func = std::function<void(GameWorld &world, unsigned int entity)>;
using trans_func = std::function<bool(GameWorld &world, unsigned int entity)>;

const accel_func accel_funcs[static_cast<size_t>(MoveState::state_count)] = {
    protagonist::on_ground,
    protagonist::falling,
    protagonist::flying,
    protagonist::flying_accel,
    TestEnemy::simple_flying
};

const accel_func to_funcs[static_cast<size_t>(MoveState::state_count)] = {
    protagonist::to_ground,
    protagonist::to_falling,
    protagonist::to_flying,
    protagonist::to_flying_accel,
    TestEnemy::to_simple_flying
};

const trans_func transition_test_funcs[static_cast<size_t>(MoveState::state_count)] = {
    protagonist::transition_ground,
    protagonist::transistion_falling,
    protagonist::transition_flying,
    protagonist::transition_flying_accel,
    TestEnemy::transition_simple_flying
};

std::unordered_map<MoveState, std::vector<MoveState>> protagonist_trans = {
    {MoveState::OnGround, {MoveState::Falling, MoveState::Flying}},
    {MoveState::Falling, {MoveState::OnGround, MoveState::Flying}},
    {MoveState::Flying, {MoveState::FlyingAccel, MoveState::OnGround, MoveState::Falling}},
    {MoveState::FlyingAccel, {MoveState::OnGround}}
};

std::unordered_map<MoveState, std::vector<MoveState>> testenemy_trans = {
    {MoveState::SimpleFlying, {}},
    {MoveState::Falling, {MoveState::SimpleFlying}}
};

std::unordered_map<MoveState, std::vector<MoveState>> special_trans = {};

accel_func get_accel_func(const MoveState &state) {
    return accel_funcs[static_cast<size_t>(state)];
}

trans_func get_trans_func(const MoveState &trans) {
    return transition_test_funcs[static_cast<size_t>(trans)];
}

accel_func get_to_func(const MoveState &state) {
    return to_funcs[static_cast<size_t>(state)];
}

const std::vector<MoveState> & get_trans_funcs (const MoveSet &set, const MoveState &state) {
    switch (set) {
        case MoveSet::Protagonist : return protagonist_trans[state]; break;
        case MoveSet::TestEnemy : return testenemy_trans[state]; break;
        case MoveSet::Special : return special_trans[state];
    }
}
