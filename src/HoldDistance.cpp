#include "HoldDistance.h"
#include "GameWorld.h"
#include "InputComponent.h"
#include "PosComponent.h"
#include "MoveSystems.h"
#include "steering.h"
#include "SimpleFlyer.h"
#include "CollisionComponent.h"
#include "ColShape.h"
#include "WVecMath.h"
#include "TagComponent.h"
#include "Pathfinding.h"

HoldDistance::HoldDistance(GameWorld& world,
                           unsigned int entity,
                           float max_range,
                           float min_range)
  : m_world(world)
  , m_entity(entity)
  , m_max_range(max_range)
  , m_min_range(min_range)
{
}

behaviour_tree::Status
HoldDistance::update()
{
  using namespace behaviour_tree;
  auto& ic = m_world.get<InputComponent>(m_entity);
  const auto& pos = m_world.get<PosComponent>(m_entity).global_position;
  const float radius =
    m_world.get<ColShapeComponent>(m_entity).shape->get_radius();
  auto& pc = m_world.get<PathingComponent>(m_entity);
  auto& mc = m_world.get<MoveComponent>(m_entity);
  const auto& target =
    m_world.get<PosComponent>(pc.following).wrapped_position(pos);

  auto builder =
    SteeringBuilder(pos,
                    mc.velocity,
                    m_world.get<SimpleFlyComponent>(m_entity).c_max_vel,
                    radius);

  // flock
  auto colliders =
    m_world.prune_sweep().find_in_radius(pos, radius * 4.0f, m_entity);
  pc.flock.clear();

  for (auto& col : colliders) {
    if (col.entity == m_entity) {
      continue;
    }
    if (m_world.get<TagComponent>(col.entity)
          .tags.test(static_cast<int>(Tags::Enemy))) {
      float radius = w_magnitude(col.maxs - col.mins) / 2.0f;
      assert(radius == w_magnitude(col.maxs - col.mins) / 2.0f);
      auto center = m_world.get<PosComponent>(col.entity).wrapped_position(pos);
      builder.add_flock(pos + nearest_dist_with_radii(pos, 0, center, radius));
    }
  }
  WVec v;
  builder.add_avoid_collision(m_world.grid(), m_world.navmesh(), v);

  ic.jump.push(true);

  float mag = w_magnitude(target - pos);
  if (mag > m_max_range) {
    ic.wings.push(false);
    builder.add_seek(target);
    ic.mouse.push(
      pos + builder.end(m_world.get<SimpleFlyComponent>(m_entity).c_accel));
    return Status::Running;
  } else if (mag > m_min_range) { // stop
    builder.add_seek(pos);
    // will cancel turning
    ic.wings.push(true);
    // turn to target
    rotate_to(
      target, mc.c_max_change_angle, m_world.get<PosComponent>(m_entity));
  } else { // turn around
    builder.add_flee(target);
    ic.wings.push(false);
  }
  ic.mouse.push(pos +
                builder.end(m_world.get<SimpleFlyComponent>(m_entity).c_accel));

  return Status::Success;
}
