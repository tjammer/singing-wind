#ifndef PROTAGONISTMELEE_H
#define PROTAGONISTMELEE_H

#include "SkillComponent.h"
#include "MoveSystems.h"

class MeleeAttackMove : public TimedMoveState
{
public:
  void enter(GameWorld&, unsigned int) override;
  void accel(GameWorld&, unsigned int) override;
  void leave(GameWorld&, unsigned int) override {}
  std::unique_ptr<TimedMoveState> next() override { return nullptr; }
  MeleeAttackMove()
    : TimedMoveState(TimedMoveStateName::MeleeAttack, 0.4)
  {
  }
};

class MeleeCastMove : public TimedMoveState
{
public:
  void enter(GameWorld&, unsigned int) override {}
  void accel(GameWorld&, unsigned int) override {}
  void leave(GameWorld&, unsigned int) override {}
  std::unique_ptr<TimedMoveState> next() override
  {
    return std::make_unique<MeleeAttackMove>();
  }
  MeleeCastMove()
    : TimedMoveState(TimedMoveStateName::MeleeCast, 0.2)
  {
  }
};

class MeleeSkill : public BaseSkill
{
public:
  void set_special(MoveComponent& move_c) override;
  MeleeSkill()
    : BaseSkill(SkillID::Melee, .6, 2)
  {
  }
};
#endif /* PROTAGONISTMELEE_H */
