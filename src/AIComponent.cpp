#include "AIComponent.h"
#include "GameWorld.h"
#include "Components.h"
#include "MoveSystems.h"
#include "ai_state_transitions.h"
#include "ai_state_funcs.h"
#include "ai_input_funcs.h"
#include "EntityEditor.h"
#include <imgui.h>
#include <unordered_map>

namespace ai {

    // maybe not needed for now
    const func c_ai_funcs[static_cast<size_t>(AIState::state_count)] = {
        ai_funcs::idle_func,
        ai_funcs::pursuit_func,
        nullptr,
        nullptr,
        ai_funcs::return_func,
        nullptr
    };

    trans_func c_ai_transitions[static_cast<size_t>(AIState::state_count)] = {
        ai_transitions::trans_idle,
        ai_transitions::trans_pursuit,
        ai_transitions::trans_attack,
        ai_transitions::trans_flee,
        ai_transitions::trans_return,
        ai_transitions::trans_idle, // notinit
    };

    func c_ai_to_funcs[static_cast<size_t>(AIState::state_count)] = {
        ai_to_funcs::to_idle,
        ai_to_funcs::to_pursuit,
        ai_to_funcs::to_attack,
        ai_to_funcs::to_flee,
        ai_to_funcs::to_return,
        ai_to_funcs::to_idle
    };

    std::unordered_map<AIState, std::vector<AIState>> testenemy_ai_trans = {
        {AIState::Idle, {AIState::Pursuit}},
        {AIState::Pursuit, {AIState::Attack, AIState::Return}},
        {AIState::Attack, {AIState::Pursuit}},
        {AIState::Return, {AIState::Idle, AIState::Pursuit}},
        {AIState::NotInit, {AIState::Idle}}
    };

    func get_func(AIState state) {
        return c_ai_funcs[static_cast<size_t>(state)];
    }

    trans_func get_trans_func(AIState state) {
        return c_ai_transitions[static_cast<size_t>(state)];
    }

    func get_to_func(AIState state) {
        return c_ai_to_funcs[static_cast<size_t>(state)];
    }

    const std::vector<AIState> & get_trans_states(const AIType &type, const AIState &state) {
        switch (type) {
            case AIType::TestEnemy : return testenemy_ai_trans[state];
        }
        assert(false);
        return testenemy_ai_trans[state];
    }

    void do_input(GameWorld &world, unsigned int entity) {
        if (world.move_c(entity).movestate == MoveState::SimpleFlying) {
            ai_input::simple_flying(world, entity);
        } else if (world.move_c(entity).movestate == MoveState::Hover) {
            ai_input::hover(world, entity);
        }
    } 

    const std::map<AIType, const char*> c_ai_types = {
        {AIType::TestEnemy, "TestEnemy"}
    };

    auto ai_types = get_enum_string_array(c_ai_types);

    void entity_edit(GameWorld &world, unsigned int entity) {
        using namespace ImGui;
        if (world.entities()[entity].test(CAI) and CollapsingHeader("ai")) {
            auto &ac = world.ai_c(entity);
            int type = static_cast<int>(ac.type);
            if (Combo("type", &type, ai_types.data(), ai_types.size())) {
                ac.type = static_cast<AIType>(type);
            }
        }
    }
}

