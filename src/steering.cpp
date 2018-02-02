#include "steering.h"
#include "WVecMath.h"
#include "StaticGrid.h"
#include "CollisionComponent.h"
#include "NavMesh.h"
#include <random>

std::default_random_engine gen{ 110 };

SteeringBuilder::SteeringBuilder(const WVec& self_pos,
                                 const WVec& self_vel,
                                 float max_vel,
                                 float cohesion_length)
  : m_pos(self_pos)
  , m_vel(self_vel)
  , m_max_vel(max_vel)
  , m_cohesion_length(cohesion_length)
{
}

SteeringBuilder&
SteeringBuilder::seek(const WVec& pos)
{
  add_seek(pos);
  return *this;
}

SteeringBuilder&
SteeringBuilder::flee(const WVec& pos)
{
  add_flee(pos);
  return *this;
}

SteeringBuilder&
SteeringBuilder::flock(const WVec& pos)
{
  auto dir = w_normalize(pos - m_pos);
  m_separate -= dir;
  m_flock_count++;

  return *this;
}

SteeringBuilder&
SteeringBuilder::wander(WVec& steering_force,
                        float max_steering,
                        float max_displacement)
{
  add_wander(steering_force, max_steering, max_displacement);
  return *this;
}

void
SteeringBuilder::add_seek(const WVec& position)
{
  auto desired_velocity = w_normalize(position - m_pos) * m_max_vel;
  m_seek += desired_velocity - m_vel;
}

void
SteeringBuilder::add_flee(const WVec& position)
{
  auto desired_velocity = w_normalize(position - m_pos) * m_max_vel;
  m_seek -= desired_velocity - m_vel;
}

void
SteeringBuilder::add_flock(const WVec& position)
{
  auto offset = m_pos - position;
  m_flock_count++;
  m_separate +=
    w_normalize(offset) / fmax(w_magnitude(offset), 1.f) * m_cohesion_length;
}

void
SteeringBuilder::add_cohesion(const WVec& position)
{
  auto offset = position - m_pos;
  m_cohesion = offset / m_cohesion_length * 0.5f;
}

void
SteeringBuilder::add_wander(WVec& steering_force,
                            float max_steering,
                            float max_displacement)
{
  std::uniform_real_distribution<float> dist{ -max_displacement,
                                              max_displacement };
  WVec displacement{ dist(gen), dist(gen) };

  steering_force += displacement;
  steering_force = w_normalize(steering_force) * max_steering;
  add_seek(m_pos + w_normalize(m_vel) * sqrtf(2) + steering_force);
}

void
SteeringBuilder::add_avoid_collision(const StaticGrid<StaticTriangle>& grid,
                                     const NavMesh& mesh,
                                     WVec& wander)
{
  RayCastResult result;
  // first ray
  auto dir = w_normalize(m_vel);
  auto tangent = w_tangent(dir);

  auto p1 = m_pos + tangent * m_cohesion_length;
  auto res1 =
    grid.raycast_against_grid(p1, p1 + dir * m_cohesion_length * 4.0f);
  if (res1.hits) {
    result = res1;
  }

  auto p2 = m_pos - tangent * m_cohesion_length;
  res1 = grid.raycast_against_grid(p2, p2 + dir * m_cohesion_length * 4.0f);
  if (res1.hits && res1.hit_parameter < result.hit_parameter) {
    result = res1;
  }

  if (!result.hits) {
    return;
  }

  float vel = w_magnitude(m_vel);
  m_seek -= w_normalize(result.hit_normal) * vel * 2.0f;
  m_seek += w_normalize(WVec{ mesh.get_nearest(m_pos) } - m_pos) * vel * 0.5f;
  auto dot = w_dot(w_normalize(wander), result.hit_normal);
  if (-0.3 < dot && dot < 0) {
    w_reflect(wander, result.hit_normal);
  }
}

WVec
SteeringBuilder::end(float force)
{
  if (m_flock_count > 0) {
    return m_seek + (m_separate + m_cohesion) * force / (float)m_flock_count;
  }
  return m_seek;
}
