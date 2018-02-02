#include "Wander.h"
#include "GameWorld.h"
#include "Components.h"
#include "PosComponent.h"
#include "InputComponent.h"
#include "PatrolComponent.h" // using patrol_c for force vec
#include "MoveSystems.h"
#include "TagComponent.h"
#include "Pathfinding.h"
#include "CollisionComponent.h"
#include "ColShape.h"
#include "WVecMath.h"
#include "steering.h"

Wander::Wander(GameWorld& world,
               unsigned int entity,
               float max_mag,
               float wander_rate)
  : m_world(world)
  , m_entity(entity)
  , m_max_mag(max_mag)
  , m_rate(wander_rate)
{
  m_radius = world.get<ColShapeComponent>(entity).shape->get_radius();
}

behaviour_tree::Status
Wander::update()
{
  using namespace behaviour_tree;
  auto& ic = m_world.get<InputComponent>(m_entity);
  auto& force = m_world.get<PatrolComponent>(m_entity).patrol_point;
  auto& pos = m_world.get<PosComponent>(m_entity).global_position;
  auto& pc = m_world.get<PathingComponent>(m_entity);

  auto builder =
    SteeringBuilder(pos,
                    m_world.get<MoveComponent>(m_entity).velocity,
                    m_world.get<SimpleFlyComponent>(m_entity).c_max_vel,
                    m_radius)
      .wander(force, m_max_mag, m_rate);

  // flock
  auto colliders =
    m_world.prune_sweep().find_in_radius(pos, m_radius * 4.0f, m_entity);
  pc.flock.clear();

  for (auto& col : colliders) {
    if (col.entity == m_entity) {
      continue;
    }
    if (m_world.get<TagComponent>(col.entity)
          .tags.test(static_cast<int>(Tags::Enemy))) {
      auto center = (col.maxs + col.mins) / 2.0f;
      float radius = w_magnitude(center - col.mins);
      builder.add_flock(pos + nearest_dist_with_radii(pos, 0, center, radius));
    }
  }
  builder.add_avoid_collision(m_world.grid(), m_world.navmesh(), force);

  ic.jump.push(true);
  auto steering =
    builder.end(m_world.get<SimpleFlyComponent>(m_entity).c_accel);
  ic.mouse.push(m_world.get<PosComponent>(m_entity).global_position + steering);

  return Status::Success;
}

void Wander::leave(behaviour_tree::Status)
{
  // m_world.get<InputComponent>(m_entity).jump.push(false);
}
