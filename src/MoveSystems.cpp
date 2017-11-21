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
#include "SkillComponent.h"

std::unordered_map<MoveStateName, std::vector<MoveStateName>> protagonist_trans = {
    {MoveStateName::OnGround, {MoveStateName::Falling, MoveStateName::Flying}},
    {MoveStateName::Falling, {MoveStateName::OnGround, MoveStateName::Flying}},
    {MoveStateName::Flying, {MoveStateName::FlyingAccel, MoveStateName::OnGround, MoveStateName::Falling}},
    {MoveStateName::FlyingAccel, {MoveStateName::OnGround}}
};

std::unordered_map<MoveStateName, std::vector<MoveStateName>> testenemy_trans = {
    {MoveStateName::SimpleFlying, {MoveStateName::Hover}},
    {MoveStateName::Hover, {MoveStateName::SimpleFlying}},
    {MoveStateName::Falling, {MoveStateName::SimpleFlying}}
};

void reset_special(GameWorld &world, unsigned int entity, TimedMoveStateName movestate) {
    if (world.move_c(entity).special_movestate == nullptr) {
        return;
    }
    if (world.move_c(entity).special_movestate->name() == movestate) {
        world.move_c(entity).special_movestate = nullptr;
    }
}

void interrupt(GameWorld &world, unsigned int entity) {
    // TODO: reset skill, reset movement
    auto &mc = world.move_c(entity);
    mc.timer = 0;
    mc.moveset->init(world, entity);
    assert(mc.special_movestate == nullptr);
    skill::reset(world, entity);
}

void init_moveset(GameWorld &world, unsigned int entity, MoveSetName moveset_name) {
    auto &mc = world.move_c(entity);
    switch (moveset_name) {
        case MoveSetName::Protagonist : {
                                            mc.moveset = std::unique_ptr<MoveSet>(new ProtagonistMoveSet());
                                            break;
                                        }
        case MoveSetName::TestEnemy : {
                                          mc.moveset = std::unique_ptr<MoveSet>(new SimpleFlyerMoveSet());
                                          break;
                                      }
    }
    mc.moveset->init(world, entity);
}
