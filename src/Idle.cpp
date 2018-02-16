#include "Idle.h"
#include "GameWorld.h"
#include "PosComponent.h"
#include "MoveSystems.h"
#include "PatrolComponent.h"
#include "InputComponent.h"
#include "WVecMath.h"
#include "CollisionComponent.h"
#include "ColShape.h"

Idle::Idle(GameWorld& world, unsigned int entity)
  : m_world(world)
  , m_entity(entity)
{
}

behaviour_tree::Status
Idle::update()
{
  using namespace behaviour_tree;

  auto& pos = m_world.get<PosComponent>(m_entity).global_position;
  WVec p = m_world.get<PatrolComponent>(m_entity).patrol_point;
  float radius = m_world.get<ColShapeComponent>(m_entity).shape->get_radius();

  if (w_magnitude(p - pos) > radius * 0.5f) {
    m_world.get<InputComponent>(m_entity).mouse.push(p);
  } else {
    m_world.get<InputComponent>(m_entity).mouse.push(pos);
  }
  m_world.get<InputComponent>(m_entity).jump.push(false);
  m_world.get<InputComponent>(m_entity).wings.push(false);

  return behaviour_tree::Status::Success;
}
