//
// Created by jammer on 07/04/17.
//

#include "GameWorld.h"
#include "AIComponent.h"
#include "StaticGrid.h"
#include "ColShape.h"
#include "CollisionComponent.h"
#include "Editor.h"
#include "HurtBoxComponent.h"
#include "InputComponent.h"
#include "LifeTimeComponent.h"
#include "MoveSystems.h"
#include "NavMesh.h"
#include "Pathfinding.h"
#include "PatrolComponent.h"
#include "PosComponent.h"
#include "SceneIO.h"
#include "SkillComponent.h"
#include "StatusEffectComponent.h"
#include "TagComponent.h"
#include "HealthComponent.h"
#include "systems.h"
#include "triangulate.h"

GameWorld::~GameWorld() = default;

class GameWorld::impl
{
public:
  // ecs stuff
  std::vector<bset> m_entities;

  // components
  std::vector<PosComponent> m_pos_c;
  std::vector<DebugComponent> m_debug_c;
  std::vector<InputComponent> m_input_c;
  std::vector<MoveComponent> m_move_c;
  std::vector<StaticColComponent> m_static_col_c;
  std::vector<GroundMoveComponent> m_ground_move_c;
  std::vector<FallComponent> m_fall_c;
  std::vector<FlyComponent> m_fly_c;
  std::vector<PathingComponent> m_path_c;
  std::vector<SimpleFlyComponent> m_simple_fly_c;
  std::vector<SkillComponent> m_skill_c;
  std::vector<DynamicColComponent> m_dyn_c;
  std::vector<TagComponent> m_tag_c;
  std::vector<ColShapeComponent> m_cshape_c;
  std::vector<LifeTimeComponent> m_lifetime_c;
  std::vector<HurtBoxComponent> m_hurtbox_c;
  std::vector<StatusEffectComponent> m_statuseffect_c;
  std::vector<AIComponent> m_ai_c;
  std::vector<PatrolComponent> m_patrol_c;
  std::vector<NameComponent> m_name_c;
  std::vector<HealthComponent> m_health_c;

  StaticGrid<StaticTriangle> m_grid;
  PruneSweeper m_prune_sweep;
  std::vector<Island> m_islands;
  NavMesh m_navmesh;

  std::vector<unsigned int> m_input_ents;
  std::vector<unsigned int> m_debug_draw_ents;
  std::vector<unsigned int> m_ground_move_ents;
  std::vector<unsigned int> m_fly_ents;
  std::vector<unsigned int> m_static_col_ents;
  std::vector<unsigned int> m_path_ents;
  std::vector<unsigned int> m_skill_ents;
  std::vector<unsigned int> m_dyn_col_ents;
  std::vector<unsigned int> m_lifetime_ents;
  std::vector<unsigned int> m_statuseffect_ents;
  std::vector<unsigned int> m_ai_ents;
  std::vector<unsigned int> m_health_ents;

  std::vector<unsigned int> m_to_delete;
  std::vector<std::function<void(GameWorld&, unsigned int)>> m_to_create;
  void delete_entitites(GameWorld&);
  void create_entities(GameWorld&);
};

template<>
PosComponent&
GameWorld::get<PosComponent>(unsigned int entity)
{
  return pimpl->m_pos_c[entity];
}

template<>
DebugComponent&
GameWorld::get<DebugComponent>(unsigned int entity)
{
  return pimpl->m_debug_c[entity];
}

template<>
InputComponent&
GameWorld::get<InputComponent>(unsigned int entity)
{
  return pimpl->m_input_c[entity];
}

template<>
MoveComponent&
GameWorld::get<MoveComponent>(unsigned int entity)
{
  return pimpl->m_move_c[entity];
}

template<>
StaticColComponent&
GameWorld::get<StaticColComponent>(unsigned int entity)
{
  return pimpl->m_static_col_c[entity];
}

template<>
GroundMoveComponent&
GameWorld::get<GroundMoveComponent>(unsigned int entity)
{
  return pimpl->m_ground_move_c[entity];
}

template<>
FallComponent&
GameWorld::get<FallComponent>(unsigned int entity)
{
  return pimpl->m_fall_c[entity];
}

template<>
FlyComponent&
GameWorld::get<FlyComponent>(unsigned int entity)
{
  return pimpl->m_fly_c[entity];
}

template<>
PathingComponent&
GameWorld::get<PathingComponent>(unsigned int entity)
{
  return pimpl->m_path_c[entity];
}

template<>
NameComponent&
GameWorld::get<NameComponent>(unsigned int entity)
{
  return pimpl->m_name_c[entity];
}

template<>
SimpleFlyComponent&
GameWorld::get<SimpleFlyComponent>(unsigned int entity)
{
  return pimpl->m_simple_fly_c[entity];
}

template<>
SkillComponent&
GameWorld::get<SkillComponent>(unsigned int entity)
{
  return pimpl->m_skill_c[entity];
}

template<>
DynamicColComponent&
GameWorld::get<DynamicColComponent>(unsigned int entity)
{
  return pimpl->m_dyn_c[entity];
}

template<>
TagComponent&
GameWorld::get<TagComponent>(unsigned int entity)
{
  return pimpl->m_tag_c[entity];
}

template<>
ColShapeComponent&
GameWorld::get<ColShapeComponent>(unsigned int entity)
{
  return pimpl->m_cshape_c[entity];
}

template<>
LifeTimeComponent&
GameWorld::get<LifeTimeComponent>(unsigned int entity)
{
  return pimpl->m_lifetime_c[entity];
}

template<>
HurtBoxComponent&
GameWorld::get<HurtBoxComponent>(unsigned int entity)
{
  return pimpl->m_hurtbox_c[entity];
}

template<>
StatusEffectComponent&
GameWorld::get<StatusEffectComponent>(unsigned int entity)
{
  return pimpl->m_statuseffect_c[entity];
}

template<>
AIComponent&
GameWorld::get<AIComponent>(unsigned int entity)
{
  return pimpl->m_ai_c[entity];
}

template<>
PatrolComponent&
GameWorld::get<PatrolComponent>(unsigned int entity)
{
  return pimpl->m_patrol_c[entity];
}

template<>
HealthComponent&
GameWorld::get<HealthComponent>(unsigned int entity)
{
  return pimpl->m_health_c[entity];
}

void
GameWorld::update_world()
{
  std::vector<WVec> triangles;
  grid().clear();
  for (const auto& island : islands()) {
    triangulate_island(island, triangles, c_line_triangulate_split);
    for (unsigned int i = 0; i < triangles.size() / 3; ++i) {
      auto p1 = WVec(get<PosComponent>(0).global_transform *
                     WVec3(triangles[i * 3], 1));
      auto p2 = WVec(get<PosComponent>(0).global_transform *
                     WVec3(triangles[i * 3 + 1], 1));
      auto p3 = WVec(get<PosComponent>(0).global_transform *
                     WVec3(triangles[i * 3 + 2], 1));
      auto tri = std::make_shared<ColTriangle>(p1, p2, p3);
      grid().lazy_add(
        StaticTriangle{ tri->m_center, tri->get_radius(), tri, i });
    }
  }
  grid().finish();

  pimpl->m_navmesh = build_navmesh_fly(islands(), grid());

  pimpl->delete_entitites(*this);
  pimpl->create_entities(*this);
}

GameWorld::GameWorld()
  : pimpl(std::make_unique<impl>())
{
}

void
GameWorld::draw()
{
  find_entities_draw();

  debug_draw_update(*this, pimpl->m_debug_draw_ents);
}

void
GameWorld::step_fixed(float dt)
{
  find_entities_fixed();

  input_update(*this, pimpl->m_input_ents);
  move_update(*this, dt);
  static_col_update(*this, pimpl->m_static_col_ents);
  dyn_col_update(*this, pimpl->m_dyn_col_ents);

  // house keeping systems
  skill_update(*this, dt, pimpl->m_skill_ents);
  lifetime_update(*this, dt, pimpl->m_lifetime_ents);
  statuseffect_update(*this, dt, pimpl->m_statuseffect_ents);
  ai_update(*this, dt, pimpl->m_ai_ents);

  // kill killed entities
  health_update(*this, pimpl->m_health_ents);

  // delete entities then create
  pimpl->delete_entitites(*this);
  pimpl->create_entities(*this);
}

unsigned int
GameWorld::create_entity_raw()
{
  unsigned int entity;
  for (entity = 0; entity < entities().size(); ++entity) {
    if (entities()[entity].none()) {
      return entity;
    }
  }
  entity = static_cast<unsigned int>(entities().size());

  // each component needs to be resized
  pimpl->m_entities.push_back(0);
  pimpl->m_name_c.emplace_back(NameComponent());
  pimpl->m_pos_c.emplace_back(PosComponent());
  pimpl->m_debug_c.emplace_back(DebugComponent());
  pimpl->m_input_c.emplace_back(InputComponent());
  pimpl->m_move_c.emplace_back(MoveComponent());
  pimpl->m_static_col_c.emplace_back(StaticColComponent());
  pimpl->m_ground_move_c.emplace_back(GroundMoveComponent());
  pimpl->m_fall_c.emplace_back(FallComponent());
  pimpl->m_fly_c.emplace_back(FlyComponent());
  pimpl->m_path_c.emplace_back(PathingComponent());
  pimpl->m_simple_fly_c.emplace_back(SimpleFlyComponent());
  pimpl->m_skill_c.emplace_back(SkillComponent());
  pimpl->m_dyn_c.emplace_back(DynamicColComponent());
  pimpl->m_tag_c.emplace_back(TagComponent());
  pimpl->m_cshape_c.emplace_back(ColShapeComponent());
  pimpl->m_lifetime_c.emplace_back(LifeTimeComponent());
  pimpl->m_hurtbox_c.emplace_back(HurtBoxComponent());
  pimpl->m_statuseffect_c.emplace_back(StatusEffectComponent());
  pimpl->m_ai_c.emplace_back(AIComponent());
  pimpl->m_patrol_c.emplace_back(PatrolComponent());
  pimpl->m_health_c.emplace_back(HealthComponent());

  return entity;
}

void
GameWorld::find_entities_fixed()
{
  using namespace for_gameworld;

  pimpl->m_input_ents.clear();
  // pimpl->m_move_ents.clear();
  pimpl->m_static_col_ents.clear();
  pimpl->m_path_ents.clear();
  pimpl->m_skill_ents.clear();
  pimpl->m_dyn_col_ents.clear();
  pimpl->m_lifetime_ents.clear();
  pimpl->m_statuseffect_ents.clear();
  pimpl->m_ai_ents.clear();
  pimpl->m_health_ents.clear();

  for (unsigned int i = 0; i < pimpl->m_entities.size(); ++i) {
    auto ent = pimpl->m_entities[i];
    if (ent.none()) {
      continue;
    }

    if (has_component(ent, c_input_components)) {
      pimpl->m_input_ents.push_back(i);
    }

    if (has_component(ent, c_static_col_components)) {
      pimpl->m_static_col_ents.push_back(i);
    }

    if (has_component(ent, c_path_components)) {
      pimpl->m_path_ents.push_back(i);
    }

    if (has_component(ent, c_skill_components)) {
      pimpl->m_skill_ents.push_back(i);
    }

    if (has_component(ent, c_dyn_col_components)) {
      pimpl->m_dyn_col_ents.push_back(i);
    }

    if (has_component(ent, c_lifetime_components)) {
      pimpl->m_lifetime_ents.push_back(i);
    }

    if (has_component(ent, c_statuseffect_components)) {
      pimpl->m_statuseffect_ents.push_back(i);
    }

    if (has_component(ent, c_ai_components)) {
      pimpl->m_ai_ents.push_back(i);
    }

    if (has_component(ent, c_health_components)) {
      pimpl->m_health_ents.push_back(i);
    }
  }
}

void
GameWorld::find_entities_draw()
{
  using namespace for_gameworld;
  pimpl->m_debug_draw_ents.clear();

  for (unsigned int i = 0; i < pimpl->m_entities.size(); ++i) {
    if (has_component(pimpl->m_entities[i], c_debug_draw_components)) {
      pimpl->m_debug_draw_ents.push_back(i);
    }
  }
}

unsigned int
GameWorld::load_entity(const std::string& name)
{
  std::string filename = "scenes/" + name + ".went";
  unsigned int ent = create_entity_raw();

  load_entity_from_filename(filename, *this, ent);
  return ent;
}

void
GameWorld::reset_entities()
{
  pimpl->m_entities.clear();

  // not needed rlly
  // components
  pimpl->m_pos_c.clear();
  pimpl->m_debug_c.clear();
  pimpl->m_input_c.clear();
  pimpl->m_move_c.clear();
  pimpl->m_static_col_c.clear();
  pimpl->m_ground_move_c.clear();
  pimpl->m_fall_c.clear();
  pimpl->m_fly_c.clear();
  pimpl->m_name_c.clear();
  pimpl->m_path_c.clear();
  pimpl->m_simple_fly_c.clear();
  pimpl->m_skill_c.clear();
  pimpl->m_dyn_c.clear();
  pimpl->m_tag_c.clear();
  pimpl->m_cshape_c.clear();
  pimpl->m_lifetime_c.clear();
  pimpl->m_hurtbox_c.clear();
  pimpl->m_statuseffect_c.clear();
  pimpl->m_ai_c.clear();
  pimpl->m_patrol_c.clear();
  pimpl->m_health_c.clear();
}

void
GameWorld::reset_islands()
{
  pimpl->m_islands.clear();
}

unsigned int
GameWorld::create_root()
{
  auto root = create_entity_raw();
  assert(root == 0);
  bset comps;
  for (auto i : { CPosition }) {
    comps.set(i);
  }
  entities()[root] = comps;
  get<NameComponent>(root).name = "root";
  get<PosComponent>(root).position = { 0, 0 };
  get<PosComponent>(root).parent = 0;
  get<PosComponent>(root).global_transform = WTransform();

  return root;
}

void
GameWorld::delete_entity_raw(unsigned int entity)
{
  pimpl->m_entities[entity].reset();
  // reset every comp

  pimpl->m_name_c[entity] = NameComponent();
  pimpl->m_pos_c[entity] = PosComponent();
  pimpl->m_debug_c[entity] = DebugComponent();
  pimpl->m_input_c[entity] = InputComponent();
  pimpl->m_move_c[entity] = MoveComponent();
  pimpl->m_static_col_c[entity] = StaticColComponent();
  pimpl->m_ground_move_c[entity] = GroundMoveComponent();
  pimpl->m_fall_c[entity] = FallComponent();
  pimpl->m_fly_c[entity] = FlyComponent();
  pimpl->m_path_c[entity] = PathingComponent();
  pimpl->m_simple_fly_c[entity] = SimpleFlyComponent();
  pimpl->m_skill_c[entity] = SkillComponent();
  pimpl->m_dyn_c[entity] = DynamicColComponent();
  pimpl->m_tag_c[entity] = TagComponent();
  pimpl->m_cshape_c[entity] = ColShapeComponent();
  pimpl->m_lifetime_c[entity] = LifeTimeComponent();
  pimpl->m_hurtbox_c[entity] = HurtBoxComponent();
  pimpl->m_statuseffect_c[entity] = StatusEffectComponent();
  assert(pimpl->m_statuseffect_c[entity].effects.size() == 0);
  pimpl->m_ai_c[entity] = AIComponent();
  pimpl->m_patrol_c[entity] = PatrolComponent();
  pimpl->m_health_c[entity] = HealthComponent();
  assert(pimpl->m_entities[entity].none());
}

void
GameWorld::impl::delete_entitites(GameWorld& world)
{
  for (auto entity : m_to_delete) {
    world.delete_entity_raw(entity);
  }
  m_to_delete.clear();
}

void
GameWorld::impl::create_entities(GameWorld& world)
{
  for (auto& func : m_to_create) {
    func(world, world.create_entity_raw());
  }
  m_to_create.clear();
}

void
GameWorld::queue_delete(unsigned int entity)
{
  pimpl->m_to_delete.push_back(entity);
  queue_delete_children(entity);
}

void
GameWorld::queue_delete_children(unsigned int entity)
{
  for (auto it = pimpl->m_pos_c.begin(); it != pimpl->m_pos_c.end(); ++it) {
    if ((*it).parent == entity) {
      queue_delete(it - pimpl->m_pos_c.begin());
    }
  }
}

void
GameWorld::queue_create(std::function<void(GameWorld&, unsigned int)> func)
{
  pimpl->m_to_create.push_back(std::move(func));
}

StaticGrid<StaticTriangle>&
GameWorld::grid()
{
  return pimpl->m_grid;
}

StaticGrid<StaticTriangle> const&
GameWorld::grid() const
{
  return pimpl->m_grid;
}

PruneSweeper&
GameWorld::prune_sweep()
{
  return pimpl->m_prune_sweep;
}

NavMesh&
GameWorld::navmesh()
{
  return pimpl->m_navmesh;
}

NavMesh const&
GameWorld::navmesh() const
{
  return pimpl->m_navmesh;
}

std::vector<Island>&
GameWorld::islands()
{
  return pimpl->m_islands;
}

std::vector<bset>&
GameWorld::entities()
{
  return pimpl->m_entities;
}
