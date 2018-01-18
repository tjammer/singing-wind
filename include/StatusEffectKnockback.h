#ifndef STATUSEFFECTKNOCKBACK_H
#define STATUSEFFECTKNOCKBACK_H

#include "MoveSystems.h"
#include "StatusEffectComponent.h"

class KnockbackMove : public TimedMoveState
{
public:
  void enter(GameWorld&, unsigned int) override;
  void accel(GameWorld&, unsigned int) override;
  void leave(GameWorld&, unsigned int) override;
  std::unique_ptr<TimedMoveState> next() override { return nullptr; }
  KnockbackMove(float timer)
    : TimedMoveState(TimedMoveStateName::Knockback, timer)
  {
  }
};

class Knockback : public StatusEffect
{
public:
  void enter(GameWorld&, unsigned int) override;
  void tick(GameWorld&, unsigned int) override;
  void leave(GameWorld&, unsigned int) override;
  Knockback(float timer)
    : StatusEffect(StatusEffectName::Knockback, timer)
  {
  }
};

#endif /* STATUSEFFECTKNOCKBACK_H */
