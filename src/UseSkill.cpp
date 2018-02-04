#include "UseSkill.h"
#include "GameWorld.h"
#include "Components.h"
#include "SkillComponent.h"
#include "InputComponent.h"
#include "Pathfinding.h"
#include "PosComponent.h"

UseSkill::UseSkill(GameWorld& world, unsigned int entity)
  : m_world(world)
  , m_entity(entity)
{
}

void
UseSkill::enter()
{
  const auto& sc = m_world.get<SkillComponent>(m_entity);
  if (sc.skills.size() == 0) {
    return; // will fail
  }
  auto id = sc.skills[0]->get_id();
  if (!skill::can_cast(m_world, m_entity, id)) {
    return; // will fail
  }
  m_can_cast = true;
}

behaviour_tree::Status
UseSkill::update()
{
  using namespace behaviour_tree;
  if (!m_can_cast) {
    return Status::Failure;
  }
  auto victim = m_world.get<PathingComponent>(m_entity).following;
  m_world.get<InputComponent>(m_entity).mouse.push(
    m_world.get<PosComponent>(victim).global_position);

  if (m_world.get<SkillComponent>(m_entity).skills[0]->state ==
      SkillState::Cooldown) {
    return Status::Success;
  }
  return Status::Running;
}

void
UseSkill::leave(behaviour_tree::Status status)
{
  m_world.get<InputComponent>(m_entity).attacks[0].push(false);
}
