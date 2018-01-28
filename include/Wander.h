#ifndef WANDER_H
#define WANDER_H

#include "WBehaviourTree.h"

class GameWorld;

class Wander : public behaviour_tree::Behaviour
{
public:
  Wander(GameWorld& world, unsigned int entity);
  behaviour_tree::Status update() override;

private:
  GameWorld& m_world;
  const unsigned int m_entity;
};

#endif /* WANDER_H */
