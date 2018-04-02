#ifndef SKILLDISK_H
#define SKILLDISK_H

#include "SkillComponent.h"
#include "MoveSystems.h"

class DiskCastMove : public TimedMoveState
{
public:
  void enter(GameWorld&, unsigned int) override {}
  void accel(GameWorld&, unsigned int) override;
  void leave(GameWorld&, unsigned int) override;
  std::unique_ptr<TimedMoveState> next() override { return nullptr; }
  DiskCastMove()
    : TimedMoveState(TimedMoveStateName::DiskCast, 0.2)
  {
  }
};

class DiskProjectileMove : public TimedMoveState
{
public:
  void enter(GameWorld&, unsigned int) override;
  void accel(GameWorld&, unsigned int) override;
  void leave(GameWorld&, unsigned int) override;
  std::unique_ptr<TimedMoveState> next() override { return nullptr; }
  DiskProjectileMove(const WVec& a_dir, float a_speed)
    : TimedMoveState(TimedMoveStateName::DiskProjectile, 60)
    , dir(a_dir)
    , move_speed(fmin(a_speed + 700, 1200)) // base speed
  {
  }

  void set_dir(const WVec& a_dir) { dir = a_dir; }
  bool collided = false;

private:
  WVec dir;
  const float move_speed;
};

class DiskSkill : public BaseSkill
{
public:
  void set_special(MoveComponent&) override;
  DiskSkill()
    : BaseSkill(SkillID::Disk, .6, 1)
  {
  }
};

namespace disk_projectile {
void
on_static_collision(GameWorld&, unsigned int);
}

#endif /* SKILLDISK_H */
