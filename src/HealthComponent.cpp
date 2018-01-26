#include "HealthComponent.h"
#include "GameWorld.h"
#include "Components.h"
#include "imgui.h"

void
health::entity_edit(GameWorld& world, unsigned int entity)
{
  using namespace ImGui;
  if (world.entities()[entity].test(CHealth) && CollapsingHeader("health")) {
    auto& hc = world.get<HealthComponent>(entity);
    InputInt("health", &hc.health);
  }
}
