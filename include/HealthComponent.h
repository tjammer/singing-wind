#ifndef ALIVECOMPONENT_H
#define ALIVECOMPONENT_H

class GameWorld;

struct HealthComponent
{
  int health = 10;
};

struct DamageType
{
  float base_damage;
};

namespace health {
void
entity_edit(GameWorld&, unsigned int);
void
damage(GameWorld&, unsigned int, DamageType);
}

#endif /* ALIVECOMPONENT_H */
