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
  WVec p = m_world.navmesh().get_nearest_visible(pos, m_world.grid());
  float radius = m_world.get<ColShapeComponent>(m_entity).shape->get_radius();

  if (w_magnitude(p - pos) > radius * 0.25f) {
    m_world.get<InputComponent>(m_entity).mouse.push(p);
  } else {
    m_world.get<InputComponent>(m_entity).mouse.push(pos);
  }

  return behaviour_tree::Status::Success;
}
