#ifndef CANSEE_H
#define CANSEE_H

#include "WBehaviourTree.h"

class GameWorld;

class CanSee : public behaviour_tree::Behaviour
{
public:
  CanSee(GameWorld& world, unsigned int entity)
    : m_world(world)
    , m_entity(entity)
  {
  }
  behaviour_tree::Status update() override;

private:
  GameWorld& m_world;
  const unsigned int m_entity;
};

#endif /* CANSEE_H */
