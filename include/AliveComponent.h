#ifndef ALIVECOMPONENT_H
#define ALIVECOMPONENT_H

class GameWorld;

struct AliveComponent
{
  int health = 10;
};

struct DamageType
{
  float base_damage;
};

void
damage_entity(GameWorld&, unsigned int, DamageType);

#endif /* ALIVECOMPONENT_H */
