#include "AIComponent.h"
#include "GameWorld.h"
#include "Components.h"
#include "MoveSystems.h"
#include "EntityEditor.h"
#include <imgui.h>
#include "TestEnemyAI.h"

namespace ai {
    const std::map<AITreeType, const char*> c_ai_types = {
        {AITreeType::None, "None"},
        {AITreeType::TestEnemy, "TestEnemy"}
    };

    const auto ai_types = get_enum_string_array(c_ai_types);

    void entity_edit(GameWorld &world, unsigned int entity) {
        using namespace ImGui;
        if (world.entities()[entity].test(CAI) and CollapsingHeader("ai")) {
            auto &ac = world.ai_c(entity);
            int type = static_cast<int>(ac.btree.type());
            if (Combo("type", &type, ai_types.data(), ai_types.size())) {
                ai::init_ai_type(world, entity, static_cast<AITreeType>(type));
            }
           // int state = static_cast<int>(ac.state);
           // if (Combo("state", &state, ai_states.data(), ai_states.size())) {
           //     ac.state = static_cast<AIStateName>(state);
           // }
        }
    }

    void init_ai_type(GameWorld &world, unsigned int entity, AITreeType) {
        // TODO: bring AIComp into sane state and ensure no pointer are dropped
        world.ai_c(entity).btree = testenemy::get_tree(world, entity);
    }
}

