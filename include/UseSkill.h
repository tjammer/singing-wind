#ifndef USESKILL_H
#define USESKILL_H

#include "WBehaviourTree.h"

class GameWorld;

class UseSkill : public behaviour_tree::Behaviour
{
public:
  UseSkill(GameWorld&, unsigned int);
  behaviour_tree::Status update() override;
  void enter() override;
  void leave(behaviour_tree::Status) override;

private:
  GameWorld& m_world;
  const unsigned int m_entity;
  bool m_can_cast{ false };
};

#endif /* USESKILL_H */
