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
  { AITreeType::TestEnemy, "TestEnemy" },
  { AITreeType::WanderCharge, "WanderCharge" },
  { AITreeType::StillFirePattern, "StillFirePattern" }
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
  auto& ac = world.get<AIComponent>(entity);
  switch (a_type) {
    case AITreeType::None: {
      ac.btree = testenemy::get_tree(world, entity);
      break;
    }
    case AITreeType::TestEnemy: {
      ac.btree = testenemy::get_tree(world, entity);
      break;
    }
    case AITreeType::WanderCharge: {
      ac.btree = wandercharge::get_tree(world, entity);
      break;
    }
    case AITreeType::StillFirePattern: {
      ac.btree = stillfirepattern::get_tree(world, entity);
      break;
    }
    default: {
      assert(false);
    }
  }
}

void
make_aggro(AIComponent& ac)
{
  ac.is_aggro = true;
  ac.aggro_time = c_aggro_time;
}
}
