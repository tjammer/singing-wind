#include "MakeAggroIf.h"
#include "GameWorld.h"
#include "AIComponent.h"

behaviour_tree::Status
MakeAggroIf::update()
{
  using namespace behaviour_tree;
  auto status = m_child->tick();

  if (status == Status::Success) {
    ai::make_aggro(m_world.get<AIComponent>(m_entity));
  }
  return status;
}
