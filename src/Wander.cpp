#include "Wander.h"
#include "GameWorld.h"
#include "Components.h"
#include "PosComponent.h"
#include "InputComponent.h"
#include "PatrolComponent.h" // using patrol_c for force vec
#include "MoveSystems.h"
#include "steering.h"

#include <iostream>
Wander::Wander(GameWorld& world,
               unsigned int entity,
               float max_mag,
               float wander_rate)
  : m_world(world)
  , m_entity(entity)
  , m_max_mag(max_mag)
  , m_rate(wander_rate)
{
}

void
Wander::enter()
{
  std::random_device r;
  std::default_random_engine gen{ r() };
  std::uniform_real_distribution<float> dist{ -1, 1 };
  WVec force = { dist(gen), dist(gen) };
  force = w_normalize(force) * m_max_mag;
  m_world.get<PatrolComponent>(m_entity).patrol_point = force;
}

behaviour_tree::Status
Wander::update()
{
  using namespace behaviour_tree;
  auto& ic = m_world.get<InputComponent>(m_entity);
  auto& force = m_world.get<PatrolComponent>(m_entity).patrol_point;

  auto steering =
    SteeringBuilder(m_world.get<PosComponent>(m_entity).global_position,
                    m_world.get<MoveComponent>(m_entity).velocity,
                    m_world.get<SimpleFlyComponent>(m_entity).c_max_vel,
                    1)
      .wander(force, m_max_mag, m_rate)
      .end(m_world.get<SimpleFlyComponent>(m_entity).c_accel);
  std::cout << w_magnitude(steering) << std::endl;

  ic.jump.push(true);
  // straight ahead
  ic.mouse.push(m_world.get<PosComponent>(m_entity).global_position + steering);

  return Status::Running;
}

void Wander::leave(behaviour_tree::Status)
{
  m_world.get<InputComponent>(m_entity).jump.push(false);
}
