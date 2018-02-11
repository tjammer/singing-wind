#ifndef SKILLSIXPATTERN_H
#define SKILLSIXPATTERN_H

#include "SkillComponent.h"
#include "MoveSystems.h"

class SixPatternAfterCastMove : public TimedMoveState
{
public:
  void enter(GameWorld&, unsigned int) override {}
  void accel(GameWorld&, unsigned int) override;
  void leave(GameWorld&, unsigned int) override {}
  std::unique_ptr<TimedMoveState> next() override { return nullptr; }
  SixPatternAfterCastMove()
    : TimedMoveState(TimedMoveStateName::SixPatternAfterCast, 1)
  {
  }
};

class SixPatternCastMove : public TimedMoveState
{
public:
  void enter(GameWorld&, unsigned int) override {}
  void accel(GameWorld&, unsigned int) override;
  void leave(GameWorld&, unsigned int) override;
  std::unique_ptr<TimedMoveState> next() override
  {
    return std::make_unique<SixPatternAfterCastMove>();
  }
  SixPatternCastMove()
    : TimedMoveState(TimedMoveStateName::SixPatternCast, 0.5)
  {
  }
};

class PatternProjectileMove : public TimedMoveState
{
public:
  void enter(GameWorld&, unsigned int) override;
  void accel(GameWorld&, unsigned int) override;
  void leave(GameWorld&, unsigned int) override;
  std::unique_ptr<TimedMoveState> next() override { return nullptr; }
  PatternProjectileMove(const WVec& a_dir, float a_speed)
    : TimedMoveState(TimedMoveStateName::PatternProjectile, 60)
    , dir(a_dir)
    , move_speed(a_speed)
  {
  }

private:
  const WVec dir;
  const float move_speed;
};

class SixPatternSkill : public BaseSkill
{
public:
  void set_special(MoveComponent&) override;
  SixPatternSkill()
    : BaseSkill(SkillID::SixPattern, 1.5, 7)
  {
  }
};

namespace sixpattern_projectile {
void
on_static_collision(GameWorld&, unsigned int);
}

#endif /* SKILLSIXPATTERN_H */
