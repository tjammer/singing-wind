#ifndef LOUNGE_H
#define LOUNGE_H

#include "SkillComponent.h"
#include "MoveSystems.h"

class LoungeAfterCastMove : public TimedMoveState
{
public:
  void enter(GameWorld&, unsigned int) override {}
  void accel(GameWorld&, unsigned int) override;
  void leave(GameWorld&, unsigned int) override {}
  std::unique_ptr<TimedMoveState> next() override { return nullptr; }
  LoungeAfterCastMove()
    : TimedMoveState(TimedMoveStateName::LoungeAfterCast, 1)
  {
  }
};

class LoungeAttackMove : public TimedMoveState
{
public:
  void enter(GameWorld&, unsigned int) override;
  void accel(GameWorld&, unsigned int) override;
  void leave(GameWorld&, unsigned int) override {}
  std::unique_ptr<TimedMoveState> next() override
  {
    return std::make_unique<LoungeAfterCastMove>();
  }
  LoungeAttackMove()
    : TimedMoveState(TimedMoveStateName::LoungeAttack, 1.6)
  {
  }
};

class LoungeCastMove : public TimedMoveState
{
public:
  void enter(GameWorld&, unsigned int) override {}
  void accel(GameWorld&, unsigned int) override;
  void leave(GameWorld&, unsigned int) override {}
  std::unique_ptr<TimedMoveState> next() override
  {
    return std::make_unique<LoungeAttackMove>();
  }
  LoungeCastMove()
    : TimedMoveState(TimedMoveStateName::LoungeCast, 1.2)
  {
  }
};

class LoungeSkill : public BaseSkill
{
public:
  void set_special(MoveComponent& move_c) override;
  LoungeSkill()
    : BaseSkill(SkillID::Lounge, 3.8, 6)
  {
  }
};
#endif /* LOUNGE_H */
