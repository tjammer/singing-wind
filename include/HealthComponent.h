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

void
damage_entity(GameWorld&, unsigned int, DamageType);

namespace health {
void
entity_edit(GameWorld&, unsigned int);
}

#endif /* ALIVECOMPONENT_H */
