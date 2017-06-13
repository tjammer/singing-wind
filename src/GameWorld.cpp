//
// Created by jammer on 07/04/17.
//

#include "GameWorld.h"
#include "triangulate.h"
#include "entities.h"
#include "systems.h"
#include "Editor.h"
#include "SceneIO.h"

#include "NavMesh.h"
#include "WRenderer.h"
#include <iostream>

void GameWorld::update_world() {
    std::vector<WVec> triangles;
    m_grid.clear();
    for (const auto &island : m_islands) {
        triangulate_island(island, triangles);
        for (unsigned int i = 0 ; i < triangles.size() / 3 ; ++i) {
            auto p1 = WVec(m_pos_c[0].global_transform * WVec3(triangles[i*3], 1));
            auto p2 = WVec(m_pos_c[0].global_transform * WVec3(triangles[i*3+1], 1));
            auto p3 = WVec(m_pos_c[0].global_transform * WVec3(triangles[i*3+2], 1));
            m_grid.add_object(std::shared_ptr<ColShape>(new ColTriangle(p1, p2, p3)));
        }
    }

    m_navmesh = build_navmesh(m_islands, m_grid);
}

GameWorld::GameWorld() {
}

void GameWorld::draw() {
    find_entities_draw();

    debug_draw_update(*this, m_debug_draw_ents);
}

void GameWorld::step_fixed(float dt, const WVec &mouse) {
    find_entities_fixed();

    input_update(*this, mouse, m_input_ents);
    move_update(*this, dt, m_move_ents);
    static_col_update(*this, m_static_col_ents);

    // house keeping systems
    ground_move_update(*this, dt, m_ground_move_ents);
    fly_update(*this, dt, m_fly_ents);


    auto result = cast_ray_vs_static_grid(m_grid, m_pos_c[1].position, m_pos_c[1].position + WVec{0, 1000});
    auto p = m_pos_c[1].position;
    if (result.hits) {
        p.y += result.hitParameter;
    }
    result = cast_ray_vs_static_grid(m_grid, mouse, mouse + WVec{0, 1000});
    auto m = mouse;
    if (result.hits) {
        m.y += result.hitParameter;
    }
    WRenderer::set_mode(GL_QUADS);
    for (auto q : make_quad(m, 10)) {
        WRenderer::add_primitive_vertex({{q.x, q.y}, {1.f, 1.f, 1.f}});
    }
}

unsigned int GameWorld::create_entity() {
    unsigned int entity;
    for (entity = 0 ; entity < m_entities.size() ; ++entity) {
        if (m_entities[entity].none()) {
            return entity;
        }
    }
    entity = static_cast<unsigned int>(m_entities.size());

    // each component needs to be resized
    m_entities.push_back(0);
    m_name_c.push_back(NameComponent());

    return entity;
}

void GameWorld::find_entities_fixed() {
    using namespace for_gameworld;

    m_input_ents.clear();
    m_move_ents.clear();
    m_ground_move_ents.clear();
    m_fly_ents.clear();
    m_static_col_ents.clear();

    for (unsigned int i = 0 ; i < m_entities.size() ; ++i) {
        auto ent = m_entities[i];

        if (has_component(ent, c_input_components)) {
            m_input_ents.push_back(i);
        }

        if (has_component(ent, c_move_components)) {
            m_move_ents.push_back(i);
        }

        if (has_component(ent, c_ground_move_components)) {
            m_ground_move_ents.push_back(i);
        }

        if (has_component(ent, c_fly_components)) {
            m_fly_ents.push_back(i);
        }

        if (has_component(ent, c_static_col_components)) {
            m_static_col_ents.push_back(i);
        }
    }
}

void GameWorld::find_entities_draw() {
    using namespace for_gameworld;
    m_debug_draw_ents.clear();

    for (unsigned int i = 0 ; i < m_entities.size() ; ++i) {
        if (has_component(m_entities[i], c_debug_draw_components)) {
            m_debug_draw_ents.push_back(i);
        }
    }
}

bool GameWorld::load_entity(const std::string &name) {
    std::string filename = "scenes/" + name + ".went";
    unsigned int ent = create_entity();

    return load_entity_from_filename(filename, *this, ent);
}

void GameWorld::reset_entities() {
    m_entities.clear();

    // components
    m_pos_c.clear();
    m_debug_c.clear();
    m_input_c.clear();
    m_move_c.clear();
    m_static_col_c.clear();
    m_ground_move_c.clear();
    m_jump_c.clear();
    m_fly_c.clear();
    m_name_c.clear();
}

void GameWorld::create_root() {
    Entities::create_root(*this, {0, 0}, 0);
}

void GameWorld::delete_entity_raw(unsigned int entity) {
    m_pos_c.erase(entity);
    m_debug_c.erase(entity);
    m_input_c.erase(entity);
    m_move_c.erase(entity);
    m_static_col_c.erase(entity);
    m_ground_move_c.erase(entity);
    m_jump_c.erase(entity);
    m_fly_c.erase(entity);
    m_entities[entity].reset();
    assert(m_entities[entity].none());
}
