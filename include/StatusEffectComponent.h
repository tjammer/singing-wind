#ifndef STATUSEFFECTCOMPONENT_H
#define STATUSEFFECTCOMPONENT_H

#include <functional>
#include <vector>

class GameWorld;

enum StatusEffects
{
  Knockback,
  HitStun
};

struct StatusEffect
{
  std::function<void(GameWorld&, unsigned int)> on_start = nullptr;
  std::function<void(GameWorld&, unsigned int)> on_tick = nullptr;
  std::function<void(GameWorld&, unsigned int)> on_stop = nullptr;
  float timer = 1;
  std::vector<int> saved_state;
  StatusEffects id;
};

struct StatusEffectComponent
{
  std::vector<StatusEffect> effects;
};

namespace statuseffects {
StatusEffect
knockback();
StatusEffect
hitstun();

void
add_effect(GameWorld&, unsigned int entity, StatusEffect& effect);
void
delete_effect(GameWorld&, unsigned int entity, StatusEffect& effect);

void
knockback_move(GameWorld&, unsigned int);
};

#endif /* STATUSEFFECTCOMPONENT_H */
