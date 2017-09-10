//
// Created by tobi on 4/27/17.
//

#include "MoveSystems.h"
#include "Protagonist.h"
#include <unordered_map>
#include "SimpleFlyer.h"
#include "Melee.h"
#include "Lounge.h"
#include "GameWorld.h"
#include "StatusEffectComponent.h"

using accel_func = std::function<void(GameWorld &world, unsigned int entity)>;
using trans_func = std::function<bool(GameWorld &world, unsigned int entity)>;

const accel_func accel_funcs[static_cast<size_t>(MoveState::state_count)] = {
    protagonist::on_ground,
    protagonist::falling,
    protagonist::flying,
    protagonist::flying_accel,
    simpleflyer::simple_flying,
    simpleflyer::hover
};

const accel_func to_funcs[static_cast<size_t>(MoveState::state_count)] = {
    protagonist::to_ground,
    protagonist::to_falling,
    protagonist::to_flying,
    protagonist::to_flying_accel,
    simpleflyer::to_simple_flying,
    simpleflyer::to_hover
};

const trans_func transition_test_funcs[static_cast<size_t>(MoveState::state_count)] = {
    protagonist::transition_ground,
    protagonist::transistion_falling,
    protagonist::transition_flying,
    protagonist::transition_flying_accel,
    simpleflyer::transition_simple_flying,
    simpleflyer::transition_hover
};

void still_func(GameWorld &world, unsigned int entity) {
    auto &mc = world.move_c(entity);
    mc.velocity = {0, 0};
    mc.accel = {0, 0};
}

void void_func(GameWorld &, unsigned int ) {

}

const accel_func special_funcs[static_cast<size_t>(SpecialMoveState::state_count)] = {
    nullptr,
    still_func,
    statuseffects::knockback_move,
    melee_skill::move_buildup,
    melee_skill::move_channel,
    lounge_skill::move_buildup,
    lounge_skill::move_channel,
    void_func // lounge_recover
};

std::unordered_map<MoveState, std::vector<MoveState>> protagonist_trans = {
    {MoveState::OnGround, {MoveState::Falling, MoveState::Flying}},
    {MoveState::Falling, {MoveState::OnGround, MoveState::Flying}},
    {MoveState::Flying, {MoveState::FlyingAccel, MoveState::OnGround, MoveState::Falling}},
    {MoveState::FlyingAccel, {MoveState::OnGround}}
};

std::unordered_map<MoveState, std::vector<MoveState>> testenemy_trans = {
    {MoveState::SimpleFlying, {MoveState::Hover}},
    {MoveState::Hover, {MoveState::SimpleFlying}}
};

std::unordered_map<MoveState, std::vector<MoveState>> empty_trans = {};

accel_func get_accel_func(const MoveState &state) {
    return accel_funcs[static_cast<size_t>(state)];
}

trans_func get_trans_func(const MoveState &trans) {
    return transition_test_funcs[static_cast<size_t>(trans)];
}

accel_func get_to_func(const MoveState &state) {
    return to_funcs[static_cast<size_t>(state)];
}

accel_func get_special_func(const SpecialMoveState &state) {
    return special_funcs[static_cast<size_t>(state)];
}

const std::vector<MoveState> & get_trans_funcs (const MoveSet &set, const MoveState &state) {
    switch (set) {
        case MoveSet::Protagonist : return protagonist_trans[state]; break;
        case MoveSet::TestEnemy : return testenemy_trans[state]; break;
        default : return empty_trans[state];
    }
}

void reset_special(GameWorld &world, unsigned int entity, SpecialMoveState movestate) {
    if (world.move_c(entity).special == movestate) {
        world.move_c(entity).special = SpecialMoveState::None;
    }
}
