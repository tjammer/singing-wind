//
// Created by tobi on 4/27/17.
//

#include "MoveSystems.h"
#include "Protagonist.h"
#include <unordered_map>
#include "TestEnemy.h"

using namespace std;
using accel_func = std::function<void(GameWorld &world, unsigned int entity)>;

// protagonist
const unordered_map<MoveState, accel_func> protagonist_ms = {
        {MoveState::OnGround, protagonist::on_ground},
        {MoveState::Falling, protagonist::falling},
        {MoveState::Flying, protagonist::flying},
        {MoveState::FlyingAccel, protagonist::flying_accel}
};

const unordered_map<MoveTransition, accel_func> protagonist_trans = {
        {MoveTransition::ToGround, protagonist::to_ground}
};

// testenemy
const unordered_map<MoveState, accel_func> testenemy_ms = {
    {MoveState::Flying, TestEnemy::simple_flying}
};

const unordered_map<MoveSet, unordered_map<MoveState, accel_func>> move_sets = {
        {MoveSet::Protagonist, protagonist_ms},
        {MoveSet::TestEnemy, testenemy_ms}
};

const unordered_map<MoveSet, unordered_map<MoveTransition, accel_func>> trans_sets = {
        {MoveSet::Protagonist, protagonist_trans}
};


accel_func get_accel_func(const MoveState &state, const MoveSet &set) {
    return move_sets.at(set).at(state);
}

accel_func get_trans_func(const MoveTransition &trans, const MoveSet &set) {
    return trans_sets.at(set).at(trans);
}
