#ifndef STEERING_H
#define STEERING_H

#include "WindDefs.h"

struct StaticTriangle;
template<typename t>
class StaticGrid;
class NavMesh;

class SteeringBuilder
{
public:
  SteeringBuilder(const WVec& self_pos,
                  const WVec& self_vel,
                  float max_vel,
                  float cohesion_length = 50);
  SteeringBuilder& seek(const WVec& position);
  SteeringBuilder& flee(const WVec& position);
  SteeringBuilder& flock(const WVec& position);
  SteeringBuilder& wander(WVec& steering_force,
                          float max_steering,
                          float max_displacement);
  void add_seek(const WVec& position);
  void add_flee(const WVec& position);
  void add_flock(const WVec& position);
  void add_cohesion(const WVec& position);
  void add_wander(WVec& steering_force,
                  float max_steering,
                  float max_displacement);
  void add_avoid_collision(StaticGrid<StaticTriangle>& grid,
                           const NavMesh& mesh);
  WVec end(float force);

private:
  const WVec& m_pos;
  const WVec m_vel;
  const float m_max_vel;
  const float m_cohesion_length;

  WVec m_seek{ 0, 0 };
  WVec m_separate{ 0, 0 };
  WVec m_cohesion;

  int m_flock_count{ 0 };
};

#endif /* STEERING_H */
