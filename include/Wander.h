#ifndef WANDER_H
#define WANDER_H

#include "WBehaviourTree.h"
#include "WindDefs.h"

class GameWorld;

class Wander : public behaviour_tree::Behaviour
{
public:
  Wander(GameWorld& world,
         unsigned int entity,
         float max_mag,
         float wander_rate);
  behaviour_tree::Status update() override;
  void leave(behaviour_tree::Status) override;

private:
  GameWorld& m_world;
  const unsigned int m_entity;
  const float m_max_mag;
  const float m_rate;
  float m_radius;
};

#endif /* WANDER_H */
