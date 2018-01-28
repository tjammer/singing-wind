#include "GoToEnemy.h"
#include "Collision.h"
#include "Components.h"
#include "GameWorld.h"
#include "InputComponent.h"
#include "MoveSystems.h"
#include "Pathfinding.h"
#include "PosComponent.h"
#include "TagComponent.h"
#include "Pathfinding.h"
#include "CollisionComponent.h"
#include "WVecMath.h"
#include "steering.h"

GoToEnemy::GoToEnemy(GameWorld& world, unsigned int entity)
  : m_world(world)
  , m_entity(entity)
{
}

void
GoToEnemy::enter()
{
  auto& pc = m_world.get<PathingComponent>(m_entity);
  assert(pc.following > 0);
  assert(m_world.entities()[m_entity].test(CPathing));
  const auto& pos = m_world.get<PosComponent>(m_entity).global_position;
  const auto& to = m_world.get<PosComponent>(pc.following).global_position;
  m_status = get_path(pos, to, m_world, pc);
  m_radius = m_world.get<ColShapeComponent>(m_entity).shape->get_radius();
  m_world.get<SimpleFlyComponent>(m_entity).c_arrive_radius = m_radius;
}

behaviour_tree::Status
GoToEnemy::update()
{
  if (m_status == PathfindingStatus::Failure) {
    return behaviour_tree::Status::Failure;
  }
  const auto& pos = m_world.get<PosComponent>(m_entity).global_position;
  auto& pc = m_world.get<PathingComponent>(m_entity);

  if (!can_follow_path_until_zero(pos, pc, m_world)) {
    enter();
  } else if (pc.index == 1 && // cannot see follow anymore
             w_magnitude(pos - pc.path[pc.index]) < m_radius) {
    enter();
  }

  if (pc.index == 0) { // directly following
    const auto& follow =
      m_world.get<PosComponent>(pc.following).global_position;
    float follow_radius =
      m_world.get<ColShapeComponent>(pc.following).shape->get_radius();
    auto result = cast_ray_vs_static_grid(m_world.grid(), pos, follow);
    if (!result.hits) {
      pc.path[pc.index] = follow;
    } else {
      enter();
    }
    if (w_magnitude(nearest_dist_with_radii(
          pos, m_radius, follow, follow_radius)) < m_radius) {
      return behaviour_tree::Status::Success;
    }
  }
  m_world.get<InputComponent>(m_entity).jump.push(true); // for seeking

  auto builder =
    SteeringBuilder(pos,
                    m_world.get<MoveComponent>(m_entity).velocity,
                    m_world.get<SimpleFlyComponent>(m_entity).c_max_vel,
                    m_radius)
      .seek(pc.path[pc.index]);

  // flock
  auto colliders =
    m_world.prune_sweep().find_in_radius(pos, m_radius * 4.0f, m_entity);
  pc.cohesion = pos;
  int i = 1;
  pc.flock.clear();

  for (auto& col : colliders) {
    if (col.entity == m_entity) {
      continue;
    }
    if (m_world.get<TagComponent>(col.entity)
          .tags.test(static_cast<int>(Tags::Enemy))) {
      auto center = (col.maxs + col.mins) / 2.0f;
      float radius = w_magnitude(center - col.mins);
      // pc.flock.push_back(pos + nearest_dist_with_radii(pos, 0, center,
      // radius));
      builder.add_flock(pos + nearest_dist_with_radii(pos, 0, center, radius));
      pc.cohesion += center;
      i++;
    }
  }
  pc.cohesion /= (float)i;
  builder.add_cohesion(pc.cohesion);

  m_world.get<InputComponent>(m_entity).mouse.push(
    pos + builder.end(m_world.get<SimpleFlyComponent>(m_entity).c_accel));

  return behaviour_tree::Status::Running;
}

void GoToEnemy::leave(behaviour_tree::Status)
{
  m_world.get<InputComponent>(m_entity).jump.push(false);
}
