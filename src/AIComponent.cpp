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

    std::unordered_map<AIStateName, std::vector<AIStateName>> testenemy_ai_trans = {
        {AIStateName::Idle, {AIStateName::Pursuit}},
        {AIStateName::Pursuit, {AIStateName::Attack, AIStateName::Return}},
        {AIStateName::Attack, {AIStateName::Pursuit}},
        {AIStateName::Return, {AIStateName::Idle, AIStateName::Pursuit}},
        {AIStateName::NotInit, {AIStateName::Idle}}
    };

    const std::vector<AIStateName> & get_trans_states(const AITypeName &type, const AIStateName &state) {
        switch (type) {
            case AITypeName::TestEnemy : return testenemy_ai_trans[state];
        }
        assert(false);
        return testenemy_ai_trans[state];
    }

//    void do_input(GameWorld &world, unsigned int entity) {
//        if (world.move_c(entity).movestate->name() == MoveStateName::SimpleFlying) {
//            ai_input::simple_flying(world, entity);
//        } else if (world.move_c(entity).movestate->name() == MoveStateName::Hover) {
//            ai_input::hover(world, entity);
//        } else if (world.ai_c(entity).state == AIStateName::Attack) {
//            ai_input::attack(world, entity);
//        }
//    }

    const std::map<AITypeName, const char*> c_ai_types = {
        {AITypeName::TestEnemy, "TestEnemy"}
    };

    const std::map<AIStateName, const char*> c_ai_states = {
        {AIStateName::Idle, "Idle"},
        {AIStateName::Pursuit, "Pursuit"},
        {AIStateName::Attack, "Attack"},
        {AIStateName::Flee, "Flee"},
        {AIStateName::Return, "Return"},
        {AIStateName::NotInit, "NotInit"}
    };

    const auto ai_types = get_enum_string_array(c_ai_types);
    const auto ai_states = get_enum_string_array(c_ai_states);

    void entity_edit(GameWorld &world, unsigned int entity) {
        using namespace ImGui;
        if (world.entities()[entity].test(CAI) and CollapsingHeader("ai")) {
            auto &ac = world.ai_c(entity);
            int type = static_cast<int>(ac.state->name());
            if (Combo("type", &type, ai_types.data(), ai_types.size())) {
                ai::init_ai_type(world, entity, static_cast<AITypeName>(type));
            }
           // int state = static_cast<int>(ac.state);
           // if (Combo("state", &state, ai_states.data(), ai_states.size())) {
           //     ac.state = static_cast<AIStateName>(state);
           // }
        }
    }

    void init_ai_type(GameWorld &, unsigned int, AITypeName) {
        // TODO: bring AIComp into sane state and ensure no pointer are dropped
        assert(false);
    }
}

