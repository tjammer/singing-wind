#ifndef ENEMYINRANGE_H
#define ENEMYINRANGE_H

#include "WBehaviourTree.h"

class GameWorld;

class EnemyInRange : public behaviour_tree::Behaviour
{
public:
  EnemyInRange(GameWorld& world, unsigned int entity, float radius);
  behaviour_tree::Status update() override;

private:
  GameWorld& m_world;
  const unsigned int m_entity;
  const float m_radius;
};

#endif /* ENEMYINRANGE_H */
