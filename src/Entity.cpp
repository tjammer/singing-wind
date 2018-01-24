#include "Entity.h"
#include "GameWorld.h"

Entity::Entity(unsigned int entity, std::shared_ptr<GameWorld> world)
  : m_entity(entity)
  , m_world(world)
{
}

Entity::Entity(const Entity& other)
{
  m_bset = other.m_bset;
  m_entity = other.m_entity;
  m_world = other.m_world;
}
PosComponent&
Entity::pos_c()
{
  return m_world->pos_c(m_entity);
}

DebugComponent&
Entity::debug_c()
{
  return m_world->debug_c(m_entity);
}

InputComponent&
Entity::input_c()
{
  return m_world->input_c(m_entity);
}

MoveComponent&
Entity::move_c()
{
  return m_world->move_c(m_entity);
}

StaticColComponent&
Entity::static_col_c()
{
  return m_world->static_col_c(m_entity);
}

GroundMoveComponent&
Entity::ground_move_c()
{
  return m_world->ground_move_c(m_entity);
}

FallComponent&
Entity::fall_c()
{
  return m_world->fall_c(m_entity);
}

FlyComponent&
Entity::fly_c()
{
  return m_world->fly_c(m_entity);
}

PathingComponent&
Entity::path_c()
{
  return m_world->path_c(m_entity);
}

NameComponent&
Entity::name_c()
{
  return m_world->name_c(m_entity);
}

SimpleFlyComponent&
Entity::simple_fly_c()
{
  return m_world->simple_fly_c(m_entity);
}

SkillComponent&
Entity::skill_c()
{
  return m_world->skill_c(m_entity);
}

DynamicColComponent&
Entity::dyn_col_c()
{
  return m_world->dyn_col_c(m_entity);
}

TagComponent&
Entity::tag_c()
{
  return m_world->tag_c(m_entity);
}

ColShapeComponent&
Entity::cshape_c()
{
  return m_world->cshape_c(m_entity);
}

LifeTimeComponent&
Entity::lifetime_c()
{
  return m_world->lifetime_c(m_entity);
}

HurtBoxComponent&
Entity::hurtbox_c()
{
  return m_world->hurtbox_c(m_entity);
}

StatusEffectComponent&
Entity::statuseffect_c()
{
  return m_world->statuseffect_c(m_entity);
}

AIComponent&
Entity::ai_c()
{
  return m_world->ai_c(m_entity);
}

PatrolComponent&
Entity::patrol_c()
{
  return m_world->patrol_c(m_entity);
}
