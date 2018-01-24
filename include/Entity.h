#ifndef ENTITY_H
#define ENTITY_H

#include "WindDefs.h"
#include <memory>

class GameWorld;
struct PosComponent;
struct DebugComponent;
struct InputComponent;
struct MoveComponent;
struct StaticColComponent;
struct GroundMoveComponent;
struct FallComponent;
struct FlyComponent;
struct PathingComponent;
struct SimpleFlyComponent;
struct SkillComponent;
struct DynamicColComponent;
struct ColShapeComponent;
struct LifeTimeComponent;
struct HurtBoxComponent;
struct StatusEffectComponent;
struct AIComponent;
struct PatrolComponent;
struct NameComponent;
struct TagComponent;

class Entity
{
public:
  Entity();
  Entity(unsigned int entity, std::shared_ptr<GameWorld> world);
  Entity(const Entity& other);
  ~Entity();

  bset& get_bset() { return m_bset; }
  bset const& get_bset() const { return m_bset; }

  void reset() { m_bset.reset(); }
  bool none() const { return m_bset.none(); }

  // components getters
  PosComponent& pos_c();
  DebugComponent& debug_c();
  InputComponent& input_c();
  MoveComponent& move_c();
  StaticColComponent& static_col_c();
  GroundMoveComponent& ground_move_c();
  FallComponent& fall_c();
  FlyComponent& fly_c();
  PathingComponent& path_c();
  NameComponent& name_c();
  SimpleFlyComponent& simple_fly_c();
  SkillComponent& skill_c();
  DynamicColComponent& dyn_col_c();
  TagComponent& tag_c();
  ColShapeComponent& cshape_c();
  LifeTimeComponent& lifetime_c();
  HurtBoxComponent& hurtbox_c();
  StatusEffectComponent& statuseffect_c();
  AIComponent& ai_c();
  PatrolComponent& patrol_c();

private:
  bset m_bset{ 0 };
  unsigned int m_entity = 0;
  std::shared_ptr<GameWorld> m_world = nullptr;
};

#endif /* ENTITY_H */
