//
// Created by jammer on 07/04/17.
//

#include "GameWorld.h"
#include "PosComponent.h"
#include "triangulate.h"
#include "entities.h"
#include "systems.h"
#include "Editor.h"
#include "SceneIO.h"
#include "NavMesh.h"
#include "InputComponent.h"
#include "CollisionComponent.h"
#include "ColShape.h"
#include "ColGrid.h"
#include "CPruneSweep.h"
#include "MoveSystems.h"
#include "Pathfinding.h"
#include "SkillComponent.h"
#include "TagComponent.h"
#include "HurtBoxComponent.h"
#include "LifeTimeComponent.h"
#include "StatusEffectComponent.h"
#include "AIComponent.h"
#include "PatrolComponent.h"

GameWorld::~GameWorld() = default;

class GameWorld::impl {
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


    StaticGrid m_grid;
    PruneSweeper m_prune_sweep;
    std::vector<Island> m_islands;
    NavMesh m_navmesh;

    std::vector<unsigned int> m_input_ents;
    //std::vector<unsigned int> m_move_ents;
    std::vector<unsigned int> m_debug_draw_ents;
    std::vector<unsigned int> m_ground_move_ents;
    std::vector<unsigned int> m_fly_ents;
    std::vector<unsigned int> m_static_col_ents;
    std::vector<unsigned int> m_path_ents;
    std::vector<unsigned int> m_skill_ents;
    std::unordered_map<unsigned int, bool> m_dyn_col_ents;
    std::vector<unsigned int> m_lifetime_ents;
    std::vector<unsigned int> m_statuseffect_ents;
    std::vector<unsigned int> m_ai_ents;

    std::vector<unsigned int> m_to_delete;
    void delete_entitites(GameWorld &);
};


void GameWorld::update_world() {
    std::vector<WVec> triangles;
    grid().clear();
    for (const auto &island : islands()) {
        triangulate_island(island, triangles);
        for (unsigned int i = 0 ; i < triangles.size() / 3 ; ++i) {
            auto p1 = WVec(pos_c(0).global_transform * WVec3(triangles[i*3], 1));
            auto p2 = WVec(pos_c(0).global_transform * WVec3(triangles[i*3+1], 1));
            auto p3 = WVec(pos_c(0).global_transform * WVec3(triangles[i*3+2], 1));
            grid().add_object(std::shared_ptr<ColShape>(new ColTriangle(p1, p2, p3)));
        }
    }

    pimpl->m_navmesh = build_navmesh(islands(), grid());
}

GameWorld::GameWorld() : pimpl(std::make_unique<impl>()) {
}

void GameWorld::draw() {
    find_entities_draw();

    debug_draw_update(*this, pimpl->m_debug_draw_ents);
}

void GameWorld::step_fixed(float dt) {
    find_entities_fixed();
    pimpl->m_to_delete.clear();

    input_update(*this, pimpl->m_input_ents);
    move_update(*this, dt);
    static_col_update(*this, pimpl->m_static_col_ents);
    dyn_col_update(*this, pimpl->m_dyn_col_ents);

    // house keeping systems
    skill_update(*this, dt, pimpl->m_skill_ents);
    lifetime_update(*this, dt, pimpl->m_lifetime_ents);
    statuseffect_update(*this, dt, pimpl->m_statuseffect_ents);
    ai_update(*this, dt, pimpl->m_ai_ents);

    // delete entities
    pimpl->delete_entitites(*this);
}

unsigned int GameWorld::create_entity() {
    unsigned int entity;
    for (entity = 0 ; entity < entities().size() ; ++entity) {
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

    return entity;
}

void GameWorld::find_entities_fixed() {
    using namespace for_gameworld;

    pimpl->m_input_ents.clear();
    //pimpl->m_move_ents.clear();
    pimpl->m_static_col_ents.clear();
    pimpl->m_path_ents.clear();
    pimpl->m_skill_ents.clear();
    pimpl->m_dyn_col_ents.clear();
    pimpl->m_lifetime_ents.clear();
    pimpl->m_statuseffect_ents.clear();
    pimpl->m_ai_ents.clear();

    for (unsigned int i = 0 ; i < pimpl->m_entities.size() ; ++i) {
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
            pimpl->m_dyn_col_ents[i] = false;
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
    }
}

void GameWorld::find_entities_draw() {
    using namespace for_gameworld;
    pimpl->m_debug_draw_ents.clear();

    for (unsigned int i = 0 ; i < pimpl->m_entities.size() ; ++i) {
        if (has_component(pimpl->m_entities[i], c_debug_draw_components)) {
            pimpl->m_debug_draw_ents.push_back(i);
        }
    }
}

unsigned int GameWorld::load_entity(const std::string &name) {
    std::string filename = "scenes/" + name + ".went";
    unsigned int ent = create_entity();

    load_entity_from_filename(filename, *this, ent);
    return ent;
}

void GameWorld::reset_entities() {
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
}

void GameWorld::reset_islands() {
    pimpl->m_islands.clear();
}

void GameWorld::create_root() {
    Entities::create_root(*this, {0, 0}, 0);
}

void GameWorld::delete_entity_raw(unsigned int entity) {
    pimpl->m_entities[entity].reset();
    assert(pimpl->m_entities[entity].none());
}

void GameWorld::impl::delete_entitites(GameWorld &world) {
    for (auto entity : m_to_delete) {
        world.delete_entity_raw(entity);
    }
}

void GameWorld::queue_delete(unsigned int entity) {
    pimpl->m_to_delete.push_back(entity);
}

StaticGrid& GameWorld::grid() {
    return pimpl->m_grid;
}

PruneSweeper& GameWorld::prune_sweep() {
    return pimpl->m_prune_sweep;
}

NavMesh& GameWorld::navmesh() {
    return pimpl->m_navmesh;
}

std::vector<Island>& GameWorld::islands() {
    return pimpl->m_islands;
}


std::vector<bset>& GameWorld::entities() {
    return pimpl->m_entities;
}

PosComponent& GameWorld::pos_c(unsigned int entity) {
    return pimpl->m_pos_c[entity];
}

DebugComponent& GameWorld::debug_c(unsigned int entity) {
    return pimpl->m_debug_c[entity];
}

InputComponent & GameWorld::input_c(unsigned int entity) {
    return pimpl->m_input_c[entity];
}

MoveComponent & GameWorld::move_c(unsigned int entity) {
    return pimpl->m_move_c[entity];
}

StaticColComponent & GameWorld::static_col_c(unsigned int entity) {
    return pimpl->m_static_col_c[entity];
}

GroundMoveComponent & GameWorld::ground_move_c(unsigned int entity) {
    return pimpl->m_ground_move_c[entity];
}

FallComponent & GameWorld::fall_c(unsigned int entity) {
    return pimpl->m_fall_c[entity];
}

FlyComponent & GameWorld::fly_c(unsigned int entity) {
    return pimpl->m_fly_c[entity];
}

PathingComponent & GameWorld::path_c(unsigned int entity) {
    return pimpl->m_path_c[entity];
}

NameComponent & GameWorld::name_c(unsigned int entity) {
    return pimpl->m_name_c[entity];
}

SimpleFlyComponent & GameWorld::simple_fly_c(unsigned int entity) {
    return pimpl->m_simple_fly_c[entity];
}

SkillComponent & GameWorld::skill_c(unsigned int entity) {
    return pimpl->m_skill_c[entity];
}

DynamicColComponent & GameWorld::dyn_col_c(unsigned int entity) {
    return pimpl->m_dyn_c[entity];
}

TagComponent & GameWorld::tag_c(unsigned int entity) {
    return pimpl->m_tag_c[entity];
}

ColShapeComponent & GameWorld::cshape_c(unsigned int entity) {
    return pimpl->m_cshape_c[entity];
}

LifeTimeComponent & GameWorld::lifetime_c(unsigned int entity) {
    return pimpl->m_lifetime_c[entity];
}

HurtBoxComponent & GameWorld::hurtbox_c(unsigned int entity) {
    return pimpl->m_hurtbox_c[entity];
}

StatusEffectComponent & GameWorld::statuseffect_c(unsigned int entity) {
    return pimpl->m_statuseffect_c[entity];
}

AIComponent & GameWorld::ai_c(unsigned int entity) {
    return pimpl->m_ai_c[entity];
}

PatrolComponent & GameWorld::patrol_c(unsigned int entity) {
    return pimpl->m_patrol_c[entity];
}
