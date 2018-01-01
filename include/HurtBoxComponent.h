#ifndef HURTBOXCOMPONENT_H
#define HURTBOXCOMPONENT_H

#include <functional>
#include <vector>

class GameWorld;

struct HurtBoxComponent
{
  unsigned int owner = 0;
  std::vector<unsigned int> hit_entities;
  // for enemies
  std::function<
    void(GameWorld&, const unsigned int victim, const unsigned int attacker)>
    hurt_function = nullptr;
  // for owner
  std::function<
    void(GameWorld&, const unsigned int attacker, const unsigned int victim)>
    on_hit = nullptr;
};

namespace hurtbox {
void
on_dynamic_collision(GameWorld& world, const unsigned int entity);
};

#endif /* HURTBOXCOMPONENT_H */
