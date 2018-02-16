#include "IsAggro.h"
#include "GameWorld.h"
#include "AIComponent.h"

behaviour_tree::Status
IsAggro::update()
{
  if (m_world.get<AIComponent>(m_entity).is_aggro) {
    return behaviour_tree::Status::Success;
  }
  return behaviour_tree::Status::Failure;
}
