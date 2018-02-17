#include "CanSee.h"
#include "GameWorld.h"
#include "StaticGrid.h"
#include "CollisionComponent.h"
#include "Pathfinding.h"
#include "PosComponent.h"

behaviour_tree::Status
CanSee::update()
{
  const auto& target =
    m_world.get<PosComponent>(m_world.get<PathingComponent>(m_entity).following)
      .global_position;
  const auto& pos = m_world.get<PosComponent>(m_entity).global_position;
  auto result = m_world.grid().raycast_against_grid(pos, target);
  if (!result.hits) {
    return behaviour_tree::Status::Success;
  }
  return behaviour_tree::Status::Failure;
}
