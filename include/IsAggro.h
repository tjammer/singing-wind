#ifndef ISAGGRO_H
#define ISAGGRO_H

#include "WBehaviourTree.h"

class GameWorld;

class IsAggro : public behaviour_tree::Behaviour
{
public:
  IsAggro(GameWorld& world, unsigned int entity)
    : m_world(world)
    , m_entity(entity)
  {
  }
  behaviour_tree::Status update() override;

private:
  GameWorld& m_world;
  const unsigned int m_entity;
};

#endif /* ISAGGRO_H */
