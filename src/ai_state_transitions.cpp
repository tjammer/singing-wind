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
}

void ai_to_funcs::to_attack(GameWorld &world, unsigned int entity) {
    if (!world.entities()[entity].test(CSkill)) {
        std::cout << "entity " << entity << " has no skill component, cannot attack" << std::endl;
    }

    // input is simulated instead of starting attack right here
    push_value(world.input_c(entity).att_melee, true);
}

void ai_to_funcs::to_return(GameWorld &world, unsigned int entity) {
    auto &ic = world.input_c(entity);
    auto &ac = world.ai_c(entity);
    auto &pc = world.patrol_c(entity);
    ac.timer = 0;
    ac.msg_data.clear();
    world.path_c(entity).following = 0;

    // set new path
    push_value(ic.mouse, pc.patrol_point);
    get_path(world, entity);
    ac.state = AIState::Return;
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
    if (pc.path.size() > 0 and w_magnitude(pc.path[0] - world.pos_c(entity).global_position) < pc.c_padding) {
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
        auto result = cast_ray_vs_static_grid(world.grid(), pc.global_position, world.pos_c(other).global_position);
        if (!result.hits) {
            // send other to pursuit
            auto &path_c = world.path_c(entity);
            path_c.following = other;
            world.queue_delete(static_cast<unsigned int>(ac.msg_data[ac.msg_data.size()-1]));
            return true;
        }
    }

    // transition back from attack
    if (ac.state == AIState::Attack and world.skill_c(entity).active == nullptr) {
        std::cout << "finished with skill" << std::endl;
        return true;
    }
    return false;
}

bool ai_transitions::trans_attack(GameWorld &world, unsigned int entity) {
    auto &sc = world.skill_c(entity);
    for (auto &skill : sc.skills) {
        if (ai_skill_attack_transitions::trans_attack(world, entity, static_cast<int>(skill->get_id()))) {
            return true;
        }
    }
    return false;
}

bool ai_transitions::trans_flee(GameWorld &, unsigned int ) {
    return false;
}

bool ai_transitions::trans_return(GameWorld &world, unsigned int entity) {
    auto &ac = world.ai_c(entity);
    if (ac.msg_data[0] > 30) {
        return true;
    }
    return false;
}

namespace ai_skill_attack_transitions {
    bool trans_lounge(GameWorld &world, unsigned int entity) {
        const auto &pathc = world.path_c(entity);
        const auto &pc = world.pos_c(entity);
        auto &ac = world.ai_c(entity);

        const float min_attack_distance = 400;
        const float min_attack_angle = 0.3;
        // can see
        if (pathc.following > 0 and pathc.index == 0) {
            auto diff = world.pos_c(pathc.following).global_position - pc.global_position;
            auto distance = w_magnitude(pc.global_position - world.pos_c(pathc.following).global_position);
            auto angle = w_angle_to_vec(diff, w_rotated(WVec(0, -1), pc.rotation * pc.direction));
            // could hit
            if (distance < min_attack_distance and angle < min_attack_angle) {
                if (skill::can_cast(world, entity, SkillID::Lounge)) {
                    ac.msg_data.clear();
                    ac.msg_data.push_back(static_cast<int>(SkillID::Lounge));
                    std::cout << "attacking" << std::endl;
                    return true;
                }
            }
        }
        return false;
    }

    bool trans_attack(GameWorld &world, unsigned int entity, int skill_id_) {
        auto skill_id = static_cast<SkillID>(skill_id_);
        assert(skill_id == SkillID::Lounge);
        return trans_lounge(world, entity);
    }
}
