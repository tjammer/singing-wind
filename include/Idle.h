#ifndef IDLE_H
#define IDLE_H

#include "WBehaviourTree.h"

class GameWorld;

class Idle : public behaviour_tree::Behaviour
{
public:
  Idle(GameWorld&, unsigned int);
  behaviour_tree::Status update() override;

private:
  GameWorld& m_world;
  const unsigned int m_entity;
};
#endif /* IDLE_H */
