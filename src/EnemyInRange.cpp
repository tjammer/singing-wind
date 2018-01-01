#include "EnemyInRange.h"
#include "ColGrid.h"
#include "Collision.h"
#include "GameWorld.h"
#include "Pathfinding.h"
#include "PosComponent.h"
#include "TagComponent.h"

EnemyInRange::EnemyInRange(GameWorld& world, unsigned int entity, float radius)
  : m_world(world)
  , m_entity(entity)
  , m_radius(radius)
{
}

behaviour_tree::Status
EnemyInRange::update()
{
  using namespace behaviour_tree;
  const WVec& pos = m_world.pos_c(m_entity).global_position;
  auto colliders =
    m_world.dynamic_grid().find_colliders_in_radius(pos, m_radius);

  for (auto& col : colliders) {
    if (m_world.tag_c(col.entity).test(static_cast<int>(Tags::Protagonist))) {
      m_world.path_c(m_entity).following = col.entity;
      auto cast_result =
        cast_ray_vs_static_grid(m_world.grid(), pos, col.center);
      if (!cast_result.hits) {
        return Status::Success;
      }
    }
  }

  return Status::Failure;
}
