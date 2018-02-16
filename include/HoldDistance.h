#ifndef HOLDDISTANCE_H
#define HOLDDISTANCE_H

#include "WBehaviourTree.h"
#include "WindDefs.h"

class GameWorld;

class HoldDistance : public behaviour_tree::Behaviour
{
public:
  HoldDistance(GameWorld&, unsigned int, float max_range, float min_range);
  behaviour_tree::Status update() override;
  void leave(behaviour_tree::Status) override;

private:
  GameWorld& m_world;
  const unsigned int m_entity;
  const float m_max_range;
  const float m_min_range;
};

#endif /* HOLDDISTANCE_H */
