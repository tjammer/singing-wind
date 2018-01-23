//
// Created by jammer on 07/04/17.
//

#ifndef SINGING_WIND_GAMEWORLD_H
#define SINGING_WIND_GAMEWORLD_H

#include <string>
#include <vector>
#include <memory>
#include "WindDefs.h"
#include "ColGrid.h"
#include "NavMesh.h"
#include "CPruneSweep.h"

struct GLFWwindow;
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
struct NavMesh;
class Island;
using NameComponent = std::string;
using TagComponent = bset;

// physics constants
const float c_drag = 0.0026;
const float c_max_floor_angle = 0.7;
const float c_gravity = 980;
const float c_jump_tolerance = 0.1f;

struct EntityCreator
{
  std::function<void(GameWorld&, unsigned int new_ent, unsigned int parent)>
    func;
  unsigned int parent;
};

// implements the ecs
class GameWorld
{
public:
  GameWorld();
  ~GameWorld();

  // maybe a timer?
  void step_fixed(float dt);
  void pre_draw(float dt);
  void draw();

  std::vector<bset>& entities();
  std::vector<bset> const& entities() const;

  // components
  PosComponent& pos_c(unsigned int entity);
  PosComponent const& pos_c(unsigned int entity) const;
  DebugComponent& debug_c(unsigned int entity);
  DebugComponent const& debug_c(unsigned int entity) const;
  InputComponent& input_c(unsigned int entity);
  InputComponent const& input_c(unsigned int entity) const;
  MoveComponent& move_c(unsigned int entity);
  MoveComponent const& move_c(unsigned int entity) const;
  StaticColComponent& static_col_c(unsigned int entity);
  StaticColComponent const& static_col_c(unsigned int entity) const;
  GroundMoveComponent& ground_move_c(unsigned int entity);
  GroundMoveComponent const& ground_move_c(unsigned int entity) const;
  FallComponent& fall_c(unsigned int entity);
  FallComponent const& fall_c(unsigned int entity) const;
  FlyComponent& fly_c(unsigned int entity);
  FlyComponent const& fly_c(unsigned int entity) const;
  PathingComponent& path_c(unsigned int entity);
  PathingComponent const& path_c(unsigned int entity) const;
  NameComponent& name_c(unsigned int entity);
  NameComponent const& name_c(unsigned int entity) const;
  SimpleFlyComponent& simple_fly_c(unsigned int entity);
  SimpleFlyComponent const& simple_fly_c(unsigned int entity) const;
  SkillComponent& skill_c(unsigned int entity);
  SkillComponent const& skill_c(unsigned int entity) const;
  DynamicColComponent& dyn_col_c(unsigned int entity);
  DynamicColComponent const& dyn_col_c(unsigned int entity) const;
  TagComponent& tag_c(unsigned int entity);
  TagComponent const& tag_c(unsigned int entity) const;
  ColShapeComponent& cshape_c(unsigned int entity);
  ColShapeComponent const& cshape_c(unsigned int entity) const;
  LifeTimeComponent& lifetime_c(unsigned int entity);
  LifeTimeComponent const& lifetime_c(unsigned int entity) const;
  HurtBoxComponent& hurtbox_c(unsigned int entity);
  HurtBoxComponent const& hurtbox_c(unsigned int entity) const;
  StatusEffectComponent& statuseffect_c(unsigned int entity);
  StatusEffectComponent const& statuseffect_c(unsigned int entity) const;
  AIComponent& ai_c(unsigned int entity);
  AIComponent const& ai_c(unsigned int entity) const;
  PatrolComponent& patrol_c(unsigned int entity);
  PatrolComponent const& patrol_c(unsigned int entity) const;

  void reset_entities();
  void reset_islands();

  // delete entities
  void queue_delete(unsigned int entity);

  // communication with editor
  void update_world();

  unsigned int create_entity();
  unsigned int load_entity(const std::string& name);
  void create_root();
  void delete_entity_raw(unsigned int entity);

  // getters
  HashGrid<StaticTriangle>& grid();
  HashGrid<StaticTriangle> const& grid() const;
  PruneSweeper& prune_sweep();
  NavMesh& navmesh();
  NavMesh const& navmesh() const;
  std::vector<Island>& islands();
  std::vector<Island> const& islands() const;

private:
  class impl;
  std::unique_ptr<impl> pimpl;

  void find_entities_fixed();
  void find_entities_draw();
};

namespace for_gameworld {
inline bool
has_component(bset entity, bset component_mask)
{
  return (entity & component_mask) == component_mask;
}
}

#endif // SINGING_WIND_GAMEWORLD_H
