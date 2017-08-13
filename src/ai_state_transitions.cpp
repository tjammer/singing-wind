#include "ai_state_transitions.h"
#include "GameWorld.h"
#include "AIComponent.h"
#include "Pathfinding.h"
#include "SkillComponent.h"
#include "PosComponent.h"
#include "WVecMath.h"
#include "Components.h"
#include <iostream>

void ai_to_funcs::to_idle(GameWorld &world, unsigned int entity) {
    auto &ac = world.ai_c(entity);
    ac.timer = 0;
    ac.state = AIState::Idle;
}

void ai_to_funcs::to_pursuit(GameWorld &world, unsigned int entity) {
    auto &ac = world.ai_c(entity);
    ac.timer = 0;

    // set new path
    // this needs to have the following field in pathfinding comp set
    get_path(world, entity);
    ac.state = AIState::Pursuit;
}

void ai_to_funcs::to_attack(GameWorld &world, unsigned int entity) {
    if (!world.entities()[entity].test(CSkill)) {
        std::cout << "entity " << entity << " has no skill component, cannot attack" << std::endl;
    }
    auto &ac = world.ai_c(entity);
    auto &sc = world.skill_c(entity);

    // transition has already set skillid in data vec
    SkillID sid = static_cast<SkillID>(ac.msg_data[0]);
    skill::cast(world, entity, sid);
    if (sc.active == sid) {
        ac.timer = 0;
        ac.state = AIState::Attack;
    } else {
        assert(false);
    }
    
}

void ai_to_funcs::to_return(GameWorld &, unsigned int ) {
    assert(false);
    // patrolling not implemented yet
}

void ai_to_funcs::to_flee(GameWorld &, unsigned int ) {
    // TODO: pick semirandom location (maybe with allies)? and set path to there
    assert(false);
}

bool ai_transitions::trans_idle(GameWorld & world, unsigned int entity) {
    if (!world.entities()[entity].test(CPathing)) {
        std::cout << "entity " << entity << " has no pathfinding comp, cannot test idle trans" << std::endl;
        return false;
    }
    const auto &pc = world.path_c(entity);
    if (pc.path.size() > 0 and w_magnitude(pc.path[0] - world.pos_c(entity).position) < pc.c_padding) {
        return true;
    }
    return false;
}

bool ai_transitions::trans_pursuit(GameWorld &world, unsigned int entity) {
    auto &ac= world.ai_c(entity);
    // TODO: spotting an enemy
    
    // transition back from attack
    if (ac.state == AIState::Attack and world.skill_c(entity).active == SkillID::None) {
        return true;
    }
    return false;
}

bool ai_transitions::trans_attack(GameWorld &, unsigned int ) {
    return false;
}

bool ai_transitions::trans_flee(GameWorld &, unsigned int ) {
    return false;
}

bool ai_transitions::trans_return(GameWorld &, unsigned int ) {
    return false;
}

