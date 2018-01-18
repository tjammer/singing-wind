#include "StatusEffectComponent.h"
#include "CollisionComponent.h"
#include "Components.h"
#include "GameWorld.h"
#include "MoveSystems.h"
#include "WVecMath.h"
#include <algorithm>

#include <iostream>

namespace statuseffects {
void
add_effect(GameWorld& world,
           unsigned int entity,
           std::shared_ptr<StatusEffect> effect)
{
  if (!world.entities()[entity].test(CStatusEffect)) {
    return;
  }
  auto& effects = world.statuseffect_c(entity).effects;
  auto it = std::find_if(
    effects.begin(), effects.end(), [&effect](std::shared_ptr<StatusEffect> e) {
      return e->name() == effect->name();
    });
  if (it == effects.end()) {
    effects.push_back(effect);
  } else {
    // TODO: check for timing collisions
    *it = effect;
  }
  effect->enter(world, entity);
}

void
delete_effect(StatusEffectComponent& status_c,
              std::shared_ptr<StatusEffect> effect)
{
  auto& effects = status_c.effects;
  effects.erase(std::remove_if(
    effects.begin(), effects.end(), [&effect](std::shared_ptr<StatusEffect> e) {
      return e->name() == effect->name();
    }));
}
}
