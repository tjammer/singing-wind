#include "AIComponent.h"
#include "GameWorld.h"
#include "ai_state_transitions.h"
#include "ai_state_funcs.h"
#include "ai_input_funcs.h"
#include <unordered_map>

namespace ai {

    // maybe not needed for now
    const func c_ai_funcs[static_cast<size_t>(AIState::state_count)] = {
        nullptr,
        ai_funcs::pursuit_func,
        nullptr,
        nullptr,
        nullptr
    };

    trans_func c_ai_transitions[static_cast<size_t>(AIState::state_count)] = {
        ai_transitions::trans_idle,
        ai_transitions::trans_pursuit,
        ai_transitions::trans_attack,
        ai_transitions::trans_flee,
        ai_transitions::trans_pursuit
    };

    func c_ai_to_funcs[static_cast<size_t>(AIState::state_count)] = {
        ai_to_funcs::to_idle,
        ai_to_funcs::to_pursuit,
        ai_to_funcs::to_attack,
        ai_to_funcs::to_flee,
        ai_to_funcs::to_return
    };

    std::unordered_map<AIState, std::vector<AIState>> testenemy_ai_trans = {
        {AIState::Idle, {AIState::Pursuit}},
        {AIState::Pursuit, {AIState::Attack, AIState::Return}},
        {AIState::Attack, {AIState::Pursuit}},
        {AIState::Return, {AIState::Idle, AIState::Pursuit}}
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
        assert(world.ai_c(entity).state == AIState::Pursuit);
        ai_input::simple_flying(world, entity);
    } 
}

