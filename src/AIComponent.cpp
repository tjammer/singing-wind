#include "AIComponent.h"
#include "Components.h"
#include "EntityEditor.h"
#include "GameWorld.h"
#include "MoveSystems.h"
#include "AIs.h"
#include <imgui.h>

namespace ai {
const std::map<AITreeType, const char*> c_ai_types = {
  { AITreeType::None, "None" },
  { AITreeType::TestEnemy, "TestEnemy" }
};

const auto ai_types = get_enum_string_array(c_ai_types);

void
entity_edit(GameWorld& world, unsigned int entity)
{
  using namespace ImGui;
  if (world.entities()[entity].test(CAI) and CollapsingHeader("ai")) {
    auto& ac = world.get<AIComponent>(entity);
    int type = static_cast<int>(ac.btree.type());
    if (Combo("type", &type, ai_types.data(), ai_types.size())) {
      ai::init_ai_type(world, entity, static_cast<AITreeType>(type));
    }
  }
}

void
init_ai_type(GameWorld& world, unsigned int entity, AITreeType a_type)
{
  switch (a_type) {
    case AITreeType::None: {
      world.get<AIComponent>(entity).btree = testenemy::get_tree(world, entity);
      break;
    }
    case AITreeType::TestEnemy: {
      world.get<AIComponent>(entity).btree = testenemy::get_tree(world, entity);
      break;
    }
  }
}
}
