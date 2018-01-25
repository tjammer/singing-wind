#include "SceneIO.h"
#include "AIComponent.h"
#include "CollisionComponent.h"
#include "Components.cpp"
#include "Editor.h"
#include "GameWorld.h"
#include "InputComponent.h"
#include "Island.h"
#include "MoveSystems.h"
#include "PatrolComponent.h"
#include "PosComponent.h"
#include "SkillComponent.h"
#include "TagComponent.h"
#include <ColShape.h>
#include <fstream>
#include <iostream>

#include <flatbuffers/flatbuffers.h>
#include <scene_generated.h>

std::unique_ptr<EntityFBS::EntityT>
get_fb_entity(GameWorld& world, unsigned int entity)
{
  using namespace std;
  unique_ptr<EntityFBS::EntityT> fbs_ent_ptr =
    unique_ptr<EntityFBS::EntityT>(new EntityFBS::EntityT);
  auto& fbs_ent = *fbs_ent_ptr;
  const auto& bset = world.entities().at(entity);

  // bitset
  fbs_ent.bitset = bset.to_ulong();

  // poc_c
  if (bset.test(CPosition)) {
    const auto& pos_c = world.get<PosComponent>(entity);
    fbs_ent.pos_c = make_unique<EntityFBS::PosComponentT>();
    fbs_ent.pos_c->rotation = pos_c.rotation;
    fbs_ent.pos_c->parent = pos_c.parent;
    fbs_ent.pos_c->position = make_unique<EntityFBS::Point>();
    fbs_ent.pos_c->position->mutate_x(pos_c.position.x);
    fbs_ent.pos_c->position->mutate_y(pos_c.position.y);
    fbs_ent.pos_c->direction = pos_c.direction;
  }

  // input_c
  if (bset.test(CInput)) {
    fbs_ent.input_c =
      static_cast<int>(world.get<InputComponent>(entity).input_func);
  }

  // move_c
  if (bset.test(CMove)) {
    fbs_ent.move_c = make_unique<EntityFBS::MoveComponentT>();
    const auto& mc = world.get<MoveComponent>(entity);
    fbs_ent.move_c->moveset = static_cast<int>(mc.moveset->name());
    fbs_ent.move_c->movestate = static_cast<int>(mc.movestate->name());
    fbs_ent.move_c->mass = mc.mass;
    fbs_ent.move_c->max_change_angle = mc.c_max_change_angle;
  }

  // shape_c
  if (bset.test(CColShape)) {
    const auto& sc = world.get<ColShapeComponent>(entity);
    auto& type = sc.shape->m_type;
    fbs_ent.shape_c = make_unique<EntityFBS::ShapeComponentT>();

    if (type == ColShapeName::ColCapsule) {
      const auto& capsule = dynamic_cast<ColCapsule*>(sc.shape.get());
      fbs_ent.shape_c->length = capsule->m_length;
      fbs_ent.shape_c->radius = capsule->get_capsule_radius();
    } else if (type == ColShapeName::ColCircle) {
      fbs_ent.shape_c->radius = sc.shape->get_radius();
    }
    fbs_ent.shape_c->shape = static_cast<int>(type);
  }

  // static col
  if (bset.test(CStaticCol)) {
    fbs_ent.static_col_c =
      static_cast<int>(world.get<StaticColComponent>(entity).col_response_name);
  }

  // name
  fbs_ent.name = world.get<NameComponent>(entity).name;

  // gound_move_c
  if (bset.test(CGroundMove)) {
    const auto& gc = world.get<GroundMoveComponent>(entity);
    fbs_ent.ground_move_c = make_unique<EntityFBS::GroundMoveComponentT>();
    fbs_ent.ground_move_c->accel = gc.c_accel;
    fbs_ent.ground_move_c->turn_mod = gc.c_turn_mod;
    fbs_ent.ground_move_c->stop_friction = gc.c_stop_friction;
    fbs_ent.ground_move_c->max_vel = gc.c_max_vel;
  }

  // fall_c
  if (bset.test(CFall)) {
    const auto& fc = world.get<FallComponent>(entity);
    fbs_ent.fall_c = make_unique<EntityFBS::FallComponentT>();
    fbs_ent.fall_c->accel = fc.c_accel;
    fbs_ent.fall_c->turn_mod = fc.c_turn_mod;
    fbs_ent.fall_c->jump_height = fc.c_jump_height;
    fbs_ent.fall_c->max_vel = fc.c_max_vel;
  }

  // fly_c
  if (bset.test(CFly)) {
    const auto& fc = world.get<FlyComponent>(entity);
    fbs_ent.fly_c = make_unique<EntityFBS::FlyComponentT>();
    auto& fbs_fc = fbs_ent.fly_c;
    fbs_fc->lift = fc.c_lift;
    fbs_fc->stall_angle = fc.c_stall_angle;
    fbs_fc->accel_force = fc.c_accel_force;
    fbs_fc->accel_time = fc.c_accel_time;
    fbs_fc->push_vel = fc.c_push_vel;
    fbs_fc->drag = fc.c_drag;

    fbs_fc->to = make_unique<EntityFBS::Point>();
    fbs_fc->ctrl_to = make_unique<EntityFBS::Point>();
    fbs_fc->ctrl_from = make_unique<EntityFBS::Point>();
    fbs_fc->from = make_unique<EntityFBS::Point>();
    fbs_fc->to->mutate_x(fc.to.x);
    fbs_fc->to->mutate_y(fc.to.y);
    fbs_fc->ctrl_to->mutate_x(fc.ctrl_to.x);
    fbs_fc->ctrl_to->mutate_y(fc.ctrl_to.y);
    fbs_fc->ctrl_from->mutate_x(fc.ctrl_from.x);
    fbs_fc->ctrl_from->mutate_y(fc.ctrl_from.y);
    fbs_fc->from->mutate_x(fc.from.x);
    fbs_fc->from->mutate_y(fc.from.y);
  }

  // simple fly
  if (bset.test(CSimpleFly)) {
    const auto& fc = world.get<SimpleFlyComponent>(entity);
    fbs_ent.simple_fly_c = make_unique<EntityFBS::SimpleFlyComponentT>();
    auto& fbc_fc = fbs_ent.simple_fly_c;

    fbc_fc->max_vel = fc.c_max_vel;
    fbc_fc->accel = fc.c_accel;
    fbc_fc->near_threshold = fc.c_arrive_radius;
    fbc_fc->stop_coef = fc.c_stop_coef;
  }

  // dyn_col_c
  if (bset.test(CDynCol)) {
    fbs_ent.dyn_col_c = static_cast<int>(
      world.get<DynamicColComponent>(entity).col_response_name);
  }

  // tag
  if (bset.test(CTag)) {
    fbs_ent.tag_c = world.get<TagComponent>(entity).tags.to_ulong();
  }

  // skill_c
  if (bset.test(CSkill)) {
    auto& skill_vec = fbs_ent.skill_c;
    skill_vec.clear();

    for (const auto& skill : world.get<SkillComponent>(entity).skills) {
      skill_vec.push_back(static_cast<int>(skill->get_id()));
    }
  }

  // patrol_c
  if (bset.test(CPatrol)) {
    fbs_ent.patrol_c = make_unique<EntityFBS::PatrolComponentT>();
    fbs_ent.patrol_c->pp = make_unique<EntityFBS::Point>();
    fbs_ent.patrol_c->pp->mutate_x(
      world.get<PatrolComponent>(entity).patrol_point.x);
    fbs_ent.patrol_c->pp->mutate_y(
      world.get<PatrolComponent>(entity).patrol_point.y);
  }

  // ai_c
  if (bset.test(CAI)) {
    fbs_ent.ai_c = make_unique<EntityFBS::AIComponentT>();
    fbs_ent.ai_c->type =
      static_cast<int>(world.get<AIComponent>(entity).btree.type());
  }

  return fbs_ent_ptr;
}

bool
save_entity_standalone(GameWorld& world, unsigned int entity)
{
  using namespace std;

  auto fbs_ent = get_fb_entity(world, entity);
  flatbuffers::FlatBufferBuilder fbb;
  fbb.Finish(EntityFBS::Entity::Pack(fbb, &*fbs_ent));

  string entity_name = world.get<NameComponent>(entity).name;
  string filename = "scenes/" + entity_name + ".went";
  ofstream entity_file(filename, ios_base::binary);
  entity_file.write(reinterpret_cast<char*>(fbb.GetBufferPointer()),
                    fbb.GetSize());
  entity_file.close();

  return true;
}

// saves only pos, name and bitset, rest is loaded from hdd
void
save_entity_scene(GameWorld& world, unsigned int entity)
{
  using namespace std;
  unique_ptr<EntityFBS::EntityT> fbs_ent_ptr =
    unique_ptr<EntityFBS::EntityT>(new EntityFBS::EntityT);
  auto& fbs_ent = *fbs_ent_ptr;
  const auto& bset = world.entities().at(entity);

  // bitset
  fbs_ent.bitset = bset.to_ulong();

  // poc_c
  if (bset.test(CPosition)) {
    const auto& pos_c = world.get<PosComponent>(entity);
    fbs_ent.pos_c = make_unique<EntityFBS::PosComponentT>();
    fbs_ent.pos_c->rotation = pos_c.rotation;
    fbs_ent.pos_c->parent = pos_c.parent;
    fbs_ent.pos_c->position = make_unique<EntityFBS::Point>();
    fbs_ent.pos_c->position->mutate_x(pos_c.position.x);
    fbs_ent.pos_c->position->mutate_y(pos_c.position.y);
    fbs_ent.pos_c->direction = pos_c.direction;
  }
  fbs_ent.name = world.get<NameComponent>(entity).name;
}

void
entity_to_world(const EntityFBS::EntityT& fb_ent,
                GameWorld& world,
                unsigned int entity)
{

  // bitset
  world.entities()[entity] = bset(fb_ent.bitset);
  const auto& bs = world.entities()[entity];

  // pos_c
  if (bs.test(CPosition)) {
    auto& pos_c = fb_ent.pos_c;
    auto& pc = world.get<PosComponent>(entity);

    pc.position = WVec(pos_c->position->x(), pos_c->position->y());
    pc.rotation = pos_c->rotation;
    pc.parent = pos_c->parent;
    pc.direction = pos_c->direction;
    build_global_transform(world, entity);
  }

  // move_c
  if (bs.test(CMove)) {
    auto& move_c = fb_ent.move_c;
    auto& mc = world.get<MoveComponent>(entity);

    movement::init_moveset(
      world, entity, static_cast<MoveSetName>(move_c->moveset));
    mc.mass = move_c->mass;
    mc.c_max_change_angle = move_c->max_change_angle;
  }

  // input_c
  if (bs.test(CInput)) {
    world.get<InputComponent>(entity).input_func =
      static_cast<InputFunc>(fb_ent.input_c);
  }

  // shape_c
  if (bs.test(CColShape)) {
    auto& shape_c = fb_ent.shape_c;
    auto& sc = world.get<ColShapeComponent>(entity);
    auto type = static_cast<ColShapeName>(shape_c->shape);

    if (type == ColShapeName::ColCapsule) {
      sc.shape = std::make_shared<ColCapsule>(
        ColCapsule{ shape_c->radius, shape_c->length });
    } else if (type == ColShapeName::ColCircle) {
      sc.shape = std::make_shared<ColCircle>(ColCircle{ shape_c->radius });
    } else
      assert(false);
  }

  // static col
  if (bs.test(CStaticCol)) {
    auto& sc = world.get<StaticColComponent>(entity);
    set_static_col(sc, static_cast<StaticColResponse>(fb_ent.static_col_c));
  }

  // name
  world.get<NameComponent>(entity).name = fb_ent.name;

  // ground move
  if (bs.test(CGroundMove)) {
    auto& ground_c = fb_ent.ground_move_c;
    auto& gc = world.get<GroundMoveComponent>(entity);

    gc.c_accel = ground_c->accel;
    gc.c_stop_friction = ground_c->stop_friction;
    gc.c_turn_mod = ground_c->turn_mod;
    gc.c_max_vel = ground_c->max_vel;
  }

  // fall_c
  if (bs.test(CFall)) {
    auto& fall_c = fb_ent.fall_c;
    auto& fc = world.get<FallComponent>(entity);

    fc.c_accel = fall_c->accel;
    fc.c_turn_mod = fall_c->turn_mod;
    fc.c_jump_height = fall_c->jump_height;
    fc.c_max_vel = fall_c->max_vel;
  }

  // fly_c
  if (bs.test(CFly)) {
    auto& fly_c = fb_ent.fly_c;
    auto& fc = world.get<FlyComponent>(entity);

    fc.c_lift = fly_c->lift;
    fc.c_stall_angle = fly_c->stall_angle;
    fc.c_accel_force = fly_c->accel_force;
    fc.c_accel_time = fly_c->accel_time;
    fc.c_push_vel = fly_c->push_vel;
    fc.c_drag = fly_c->drag;

    fc.from = WVec(fly_c->from->x(), fly_c->from->y());
    fc.ctrl_from = WVec(fly_c->ctrl_from->x(), fly_c->ctrl_from->y());
    fc.ctrl_to = WVec(fly_c->ctrl_to->x(), fly_c->ctrl_to->y());
    fc.to = WVec(fly_c->to->x(), fly_c->to->y());
  }

  // simple gly
  if (bs.test(CSimpleFly)) {
    auto& fly_c = fb_ent.simple_fly_c;
    auto& fc = world.get<SimpleFlyComponent>(entity);

    fc.c_max_vel = fly_c->max_vel;
    fc.c_accel = fly_c->accel;
    fc.c_arrive_radius = fly_c->near_threshold;
    fc.c_stop_coef = fly_c->stop_coef;
  }

  // dyn col
  if (bs.test(CDynCol)) {
    auto& dc = world.get<DynamicColComponent>(entity);
    set_dynamic_col(dc, static_cast<DynColResponse>(fb_ent.dyn_col_c));
  }

  // tag_c
  if (bs.test(CTag)) {
    world.get<TagComponent>(entity).tags = bset(fb_ent.tag_c);
  }

  // skill_c
  if (bs.test(CSkill)) {
    auto& sc = world.get<SkillComponent>(entity);
    sc.active = nullptr;
    sc.skills.clear();
    for (auto id : fb_ent.skill_c) {
      sc.skills.push_back(skill::get_new_skill(static_cast<SkillID>(id)));
    }
  }

  // patrol_c
  if (bs.test(CPatrol)) {
    world.get<PatrolComponent>(entity).patrol_point =
      WVec(fb_ent.patrol_c->pp->x(), fb_ent.patrol_c->pp->y());
  }

  // ai_c
  if (bs.test(CAI)) {
    // auto &ac = world.ai_c(entity);
    auto& ai_c = fb_ent.ai_c;

    ai::init_ai_type(world, entity, static_cast<AITreeType>(ai_c->type));
  }
}

bool
load_entity_from_filename(const std::string& name,
                          GameWorld& world,
                          unsigned int entity)
{
  using namespace std;

  fstream entity_file(name, ios_base::in | ios_base::binary);

  if (!entity_file) {
    cout << "Entity file not found: " << name << endl;
    return false;
  }

  // parse file
  entity_file.seekg(0, ios::end);
  auto size = entity_file.tellg();
  vector<char> flatbuffer;
  flatbuffer.resize(size);
  entity_file.seekg(0, ios::beg);
  entity_file.read(flatbuffer.data(), size);

  auto verifier =
    flatbuffers::Verifier(reinterpret_cast<uint8_t*>(flatbuffer.data()), size);
  if (!EntityFBS::VerifyEntityBuffer(verifier)) {
    return false;
  }
  auto fb_ent = EntityFBS::UnPackEntity(flatbuffer.data());
  entity_to_world(*fb_ent, world, entity);
  return true;
}

void
scene_entity_to_world_fbs(const EntityFBS::EntityT& fb_ent,
                          GameWorld& world,
                          unsigned int entity)
{
  std::string filename = "scenes/" + fb_ent.name + ".went";
  // TODO: this needs to be done with attributes maybe
  // first loading from filename and then adding
  // position component and setting patrol to pos if necessary
  if (fb_ent.name != "root" and
      load_entity_from_filename(filename, world, entity)) {
    if (world.entities()[entity].test(CPosition)) {
      auto& pos_c = fb_ent.pos_c;
      auto& pc = world.get<PosComponent>(entity);
      pc.rotation = pos_c->rotation;
      pc.position = WVec(pos_c->position->x(), pos_c->position->y());
      pc.parent = pos_c->parent;
      // pc.direction = pos_c->direction;
      build_global_transform(world, entity);
    }
    if (world.entities()[entity].test(CPatrol)) {
      world.get<PatrolComponent>(entity).patrol_point =
        WVec(fb_ent.patrol_c->pp->x(), fb_ent.patrol_c->pp->y());
    }
  } else if (fb_ent.name == "root") {
    entity_to_world(fb_ent, world, entity);
  }
}

bool
load_scene_from_fb(const std::string& name, GameWorld& world, float& zoom)
{
  using namespace std;

  string filename = "scenes/" + name + ".wscn";
  fstream scene_file(filename, ios::in | ios::binary);

  if (!scene_file) {
    cout << "Scene file not found: " << filename << endl;
    return false;
  }

  // parse file
  scene_file.seekg(0, ios::end);
  auto size = scene_file.tellg();
  vector<char> flatbuffer;
  flatbuffer.resize(size);
  scene_file.seekg(0, ios::beg);
  scene_file.read(flatbuffer.data(), size);

  auto verifier =
    flatbuffers::Verifier(reinterpret_cast<uint8_t*>(flatbuffer.data()), size);
  if (!SceneFBS::VerifySceneBuffer(verifier)) {
    return false;
  }
  auto fb_scene = SceneFBS::UnPackScene(flatbuffer.data());

  auto& islands = world.islands();
  islands.clear();
  for (auto& fb_island : fb_scene->islands) {
    Island island;
    for (auto& fb_point : fb_island->points) {
      island.m_points.push_back(WVec(fb_point.x(), fb_point.y()));
    }
    for (auto& fb_point : fb_island->ctrl_points) {
      island.m_ctrl_points.push_back(WVec(fb_point.x(), fb_point.y()));
    }
    islands.push_back(island);
  }

  world.reset_entities();
  for (auto& fb_ent : fb_scene->entities) {
    scene_entity_to_world_fbs(*fb_ent, world, world.create_entity_raw());
  }

  zoom = fb_scene->zoom;

  return true;
}

void
save_scene_to_fb(const std::string& name, GameWorld& world, float zoom)
{
  using namespace std;

  const auto& islands = world.islands();
  const auto& entities = world.entities();

  SceneFBS::SceneT scene;
  for (const auto& island : islands) {
    scene.islands.push_back(
      unique_ptr<SceneFBS::IslandT>(new SceneFBS::IslandT));
    auto& fb_island = scene.islands[scene.islands.size() - 1];
    for (const auto& point : island.m_points) {
      fb_island->points.push_back(SceneFBS::Point(point.x, point.y));
    }
    for (const auto& point : island.m_ctrl_points) {
      fb_island->ctrl_points.push_back(SceneFBS::Point(point.x, point.y));
    }
  }
  scene.zoom = zoom;

  for (unsigned int entity = 0; entity < entities.size(); ++entity) {
    if (world.get<PosComponent>(entity).parent == 0 && entities[entity].any()) {
      scene.entities.push_back(get_fb_entity(world, entity));
    }
  }

  // save file
  flatbuffers::FlatBufferBuilder fbb;
  fbb.Finish(SceneFBS::Scene::Pack(fbb, &scene));

  string filename = "scenes/" + name + ".wscn";
  ofstream scene_file(filename, ios_base::binary);
  scene_file.write(reinterpret_cast<char*>(fbb.GetBufferPointer()),
                   fbb.GetSize());
  scene_file.close();

  cout << "wrote to file: " << filename << endl;
}
