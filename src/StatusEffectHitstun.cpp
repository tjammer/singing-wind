#include "StatusEffectHitstun.h"
#include "GameWorld.h"
#include "MoveSystems.h"

void
Hitstun::enter(GameWorld& world, unsigned int entity)
{
  world.move_c(entity).time_fac *= 0.1f;
}

void
Hitstun::tick(GameWorld& world, unsigned int entity)
{
  world.move_c(entity).time_fac *= 0.1f;
}

void
Hitstun::leave(GameWorld&, unsigned int)
{
}
