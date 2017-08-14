#include "ai_state_transitions.h"
#include "GameWorld.h"
#include "AIComponent.h"
#include "Pathfinding.h"
#include "SkillComponent.h"
#include "InputComponent.h"
#include "PosComponent.h"
#include "Collision.h"
#include "WVecMath.h"
#include "Components.h"
#include "PatrolComponent.h"
#include <iostream>

void ai_to_funcs::to_idle(GameWorld &world, unsigned int entity) {
    auto &ac = world.ai_c(entity);
    ac.timer = 0;
    ac.state = AIState::Idle;

    // spawn alert bubble
    alert_bubble::spawn(world, entity);
    ac.msg_data.clear();
    std::cout << "to idle" << std::endl;
}

void ai_to_funcs::to_pursuit(GameWorld &world, unsigned int entity) {
    auto &ac = world.ai_c(entity);
    ac.timer = 0;
    ac.msg_data.clear();
    ac.msg_data.push_back(0);

    // set new path
    // this needs to have the following field in pathfinding comp set
    get_path(world, entity);
    ac.state = AIState::Pursuit;
    std::cout << "to pursuit" << std::endl;
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

void ai_to_funcs::to_return(GameWorld &world, unsigned int entity) {
    auto &ic = world.input_c(entity);
    auto &ac = world.ai_c(entity);
    ac.timer = 0;
    ac.msg_data.clear();
    world.path_c(entity).following = 0;

    // set new path
    push_value(ic.mouse, WVec(200, 200));
    get_path(world, entity);
    ac.state = AIState::Return;
    std::cout << "to return" << std::endl;
}

void ai_to_funcs::to_flee(GameWorld &, unsigned int ) {
    // TODO: pick semirandom location (maybe with allies)? and set path to there
    assert(false);
}

bool ai_transitions::trans_idle(GameWorld & world, unsigned int entity) {
    if (world.ai_c(entity).state == AIState::NotInit) {
        return true;
    }
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
    auto &pc = world.pos_c(entity);

    // check for near entities to trans out of idle
    for (auto e = 0 ; e < (int)ac.msg_data.size() - 1; ++e) {
        auto other = static_cast<unsigned int>(ac.msg_data[e]);
        // check for visibility
        auto result = cast_ray_vs_static_grid(world.grid(), pc.position, world.pos_c(other).position);
        if (!result.hits) {
            // send other to pursuit
            auto &path_c = world.path_c(entity);
            path_c.following = other;
            world.queue_delete(static_cast<unsigned int>(ac.msg_data[ac.msg_data.size()-1]));
            return true;
        }
    }

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

bool ai_transitions::trans_return(GameWorld &world, unsigned int entity) {
    auto &ac = world.ai_c(entity);
    if (ac.msg_data[0] > 10) {
        return true;
    }
    return false;
}
