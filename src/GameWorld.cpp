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
#include "MoveSystems.h"
#include "Pathfinding.h"


GameWorld::~GameWorld() = default;

class GameWorld::impl {
    public:
    // ecs stuff
    std::vector<bset> m_entities;

        // components
        std::unordered_map<unsigned int, PosComponent> m_pos_c;
        std::unordered_map<unsigned int, DebugComponent> m_debug_c;
        std::unordered_map<unsigned int, InputComponent> m_input_c;
        std::unordered_map<unsigned int, MoveComponent> m_move_c;
        std::unordered_map<unsigned int, StaticColComponent> m_static_col_c;
        std::unordered_map<unsigned int, GroundMoveComponent> m_ground_move_c;
        std::unordered_map<unsigned int, JumpComponent> m_jump_c;
        std::unordered_map<unsigned int, FlyComponent> m_fly_c;
        std::unordered_map<unsigned int, PathingComponent> m_path_c;
        std::unordered_map<unsigned int, SimpleFlyComponent> m_simple_fly_c;
        std::vector<NameComponent> m_name_c;


    StaticGrid m_grid;
    std::vector<Island> m_islands;
    NavMesh m_navmesh;

    std::vector<unsigned int> m_input_ents;
    std::vector<unsigned int> m_move_ents;
    std::vector<unsigned int> m_debug_draw_ents;
    std::vector<unsigned int> m_ground_move_ents;
    std::vector<unsigned int> m_fly_ents;
    std::vector<unsigned int> m_static_col_ents;
    std::vector<unsigned int> m_path_ents;
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

    input_update(*this, pimpl->m_input_ents);
    move_update(*this, dt, pimpl->m_move_ents);
    static_col_update(*this, pimpl->m_static_col_ents);

    // house keeping systems
    ground_move_update(*this, dt,pimpl-> m_ground_move_ents);
    fly_update(*this, dt, pimpl->m_fly_ents);
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
    pimpl->m_name_c.push_back(NameComponent());

    return entity;
}

void GameWorld::find_entities_fixed() {
    using namespace for_gameworld;

    pimpl->m_input_ents.clear();
    pimpl->m_move_ents.clear();
    pimpl->m_ground_move_ents.clear();
    pimpl->m_fly_ents.clear();
    pimpl->m_static_col_ents.clear();
    pimpl->m_path_ents.clear();

    for (unsigned int i = 0 ; i < pimpl->m_entities.size() ; ++i) {
        auto ent = pimpl->m_entities[i];

        if (has_component(ent, c_input_components)) {
            pimpl->m_input_ents.push_back(i);
        }

        if (has_component(ent, c_move_components)) {
            pimpl->m_move_ents.push_back(i);
        }

        if (has_component(ent, c_ground_move_components)) {
            pimpl->m_ground_move_ents.push_back(i);
        }

        if (has_component(ent, c_fly_components)) {
            pimpl->m_fly_ents.push_back(i);
        }

        if (has_component(ent, c_static_col_components)) {
            pimpl->m_static_col_ents.push_back(i);
        }

        if (has_component(ent, c_path_components)) {
            pimpl->m_path_ents.push_back(i);
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

    // components
    pimpl->m_pos_c.clear();
    pimpl->m_debug_c.clear();
    pimpl->m_input_c.clear();
    pimpl->m_move_c.clear();
    pimpl->m_static_col_c.clear();
    pimpl->m_ground_move_c.clear();
    pimpl->m_jump_c.clear();
    pimpl->m_fly_c.clear();
    pimpl->m_name_c.clear();
    pimpl->m_path_c.clear();
    pimpl->m_simple_fly_c.clear();
}

void GameWorld::reset_islands() {
    pimpl->m_islands.clear();
}

void GameWorld::create_root() {
    Entities::create_root(*this, {0, 0}, 0);
}

void GameWorld::delete_entity_raw(unsigned int entity) {
    pimpl->m_pos_c.erase(entity);
    pimpl->m_debug_c.erase(entity);
    pimpl->m_input_c.erase(entity);
    pimpl->m_move_c.erase(entity);
    pimpl->m_static_col_c.erase(entity);
    pimpl->m_ground_move_c.erase(entity);
    pimpl->m_jump_c.erase(entity);
    pimpl->m_fly_c.erase(entity);
    pimpl->m_entities[entity].reset();
    pimpl->m_path_c.erase(entity);
    pimpl->m_simple_fly_c.erase(entity);
    assert(pimpl->m_entities[entity].none());
}

StaticGrid& GameWorld::grid() {
    return pimpl->m_grid;
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

JumpComponent & GameWorld::jump_c(unsigned int entity) {
    return pimpl->m_jump_c[entity];
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
