#ifndef MAKEAGGROIF_H
#define MAKEAGGROIF_H

#include "WBehaviourTree.h"

class GameWorld;

class MakeAggroIf : public behaviour_tree::Decorator
{
public:
  MakeAggroIf(GameWorld& world, unsigned int entity)
    : m_world(world)
    , m_entity(entity)
  {
  }
  behaviour_tree::Status update() override;

private:
  GameWorld& m_world;
  const unsigned int m_entity;
};

#endif /* MAKEAGGROIF_H */
