//
// Created by jammer on 07/04/17.
//

#include "GameWorld.h"
#include "triangulate.h"
#include "entities.h"
#include "systems.h"
#include "Editor.h"
#include "assert.h"

void GameWorld::update_triangles() {
    std::vector<WVec> triangles;
    m_grid.clear();
    for (const auto &island : m_islands) {
        triangulate_island(island, triangles);
        for (unsigned int i = 0 ; i < triangles.size() / 3 ; ++i) {
            auto p1 = m_pos_c[0].global_transform.transformPoint(triangles[i*3]);
            auto p2 = m_pos_c[0].global_transform.transformPoint(triangles[i*3+1]);
            auto p3 = m_pos_c[0].global_transform.transformPoint(triangles[i*3+2]);
            m_grid.add_object(std::shared_ptr<ColShape>(new ColTriangle(p1, p2, p3)));
        }
    }

}

GameWorld::GameWorld() {
    // create root
    auto root = create_root(*this, {0, 0}, 0);
    //create_coll_test(*this, {0, 0}, root);
    //Protagonist::create_player(*this, {0, 0}, root);
    assert(load_entity("player"));
    assert(m_entities.size() == 2);
}

void GameWorld::draw(sf::RenderWindow &window) {
    find_entities_draw();

    debug_draw_update(*this, window, m_debug_draw_ents);
}

void GameWorld::step_fixed(float dt, const WVec &mouse) {
    find_entities_fixed();

    input_update(*this, mouse, m_input_ents);
    move_update(*this, dt, m_move_ents);
    static_col_update(*this, m_static_col_ents);

    // house keeping systems
    ground_move_update(*this, dt, m_ground_move_ents);
    fly_update(*this, dt, m_fly_ents);
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
    m_id_c.push_back(IdComponent());

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
