#ifndef HURTBOXCOMPONENT_H
#define HURTBOXCOMPONENT_H

#include <functional>
#include <vector>

class GameWorld;

// funcs return true if valid collision
struct HurtBoxComponent
{
  unsigned int owner = 0;
  std::vector<unsigned int> hit_entities;
  // for enemies
  std::function<bool(GameWorld&,
                     unsigned int victim,
                     unsigned int attacker,
                     unsigned int entity)>
    hurt_function = nullptr;
  // for owner
  std::function<bool(GameWorld&,
                     unsigned int attacker,
                     unsigned int victim,
                     unsigned int entity)>
    on_hit = nullptr;
};

namespace hurtbox {
void
on_dynamic_collision(GameWorld& world, unsigned int entity);
};

#endif /* HURTBOXCOMPONENT_H */
