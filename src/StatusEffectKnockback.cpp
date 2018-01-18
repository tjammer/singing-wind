#include "StatusEffectKnockback.h"
#include "GameWorld.h"
#include "MoveSystems.h"
#include "Components.h"

void
KnockbackMove::enter(GameWorld& world, unsigned int entity)
{
}

void
KnockbackMove::accel(GameWorld& world, unsigned int entity)
{
  auto& mc = world.move_c(entity);
  mc.accel.y -= c_gravity * 0.5f;
}

void
KnockbackMove::leave(GameWorld& world, unsigned int entity)
{
}

void
Knockback::enter(GameWorld& world, unsigned int entity)
{
  auto& mc = world.move_c(entity);
  mc.special_movestate = std::make_unique<KnockbackMove>(timer);

  auto& comps = world.entities()[entity];
  comps.set(Components::CInput, false);
}

void
Knockback::leave(GameWorld& world, unsigned int entity)
{
  auto& comps = world.entities()[entity];
  comps.set(Components::CInput);
}

void
Knockback::tick(GameWorld& world, unsigned int entity)
{
  auto& comps = world.entities()[entity];
  comps.set(Components::CInput, false);
}
