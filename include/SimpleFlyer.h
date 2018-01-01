#ifndef TESTENEMY_H
#define TESTENEMY_H

#include "MoveSystems.h"
class GameWorld;

namespace simpleflyer {
void
on_static_collision(GameWorld& world, unsigned int entity);
}

class SimpleFlyingMove : public MoveState
{
public:
  void enter(GameWorld&, unsigned int) override;
  void accel(GameWorld&, unsigned int) override;
  MoveStateName name() override;

  static bool transition(GameWorld&, unsigned int);
};

class HoverMove : public MoveState
{
public:
  void enter(GameWorld&, unsigned int) override;
  void accel(GameWorld&, unsigned int) override;
  MoveStateName name() override;

  static bool transition(GameWorld&, unsigned int);
};

class SimpleFlyerMoveSet : public MoveSet
{
public:
  std::unique_ptr<MoveState> transition(GameWorld&, unsigned int) override;
  void init(GameWorld&, unsigned int) override;
  MoveSetName name() override { return MoveSetName::TestEnemy; }

private:
  std::unique_ptr<MoveState> from_undefined(GameWorld&, unsigned int);
};

#endif /* TESTENEMY_H */
