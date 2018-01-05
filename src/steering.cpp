#include "steering.h"
#include "WVecMath.h"

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
SteeringBuilder::arrive(const WVec& pos, float radius)
{
  add_arrive(pos, radius);
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
SteeringBuilder::add_arrive(const WVec& position, float radius)
{
  auto offset = position - m_pos;
  auto dist = w_magnitude(offset);
  auto ramped_vel = m_max_vel * (dist / radius);
  ramped_vel = fmin(ramped_vel, m_max_vel);
  auto desired_velocity = ramped_vel / dist * offset;
  m_seek += desired_velocity - m_vel;
}

void
SteeringBuilder::add_flock(const WVec& position)
{
  auto offset = m_pos - position;
  m_flock_count++;
  m_separate += w_normalize(offset) / w_magnitude(offset) * m_cohesion_length;
}

void
SteeringBuilder::add_cohesion(const WVec& position)
{
  auto offset = position - m_pos;
  m_cohesion =
    w_normalize(offset) * w_magnitude(offset) / m_cohesion_length * 0.5f;
}

WVec
SteeringBuilder::end(float force)
{
  if (m_flock_count > 0) {
    return m_seek + (m_separate + m_cohesion) * force / (float)m_flock_count;
  }
  return m_seek;
}
