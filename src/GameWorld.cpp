//
// Created by jammer on 07/04/17.
//

#include "GameWorld.h"
#include "triangulate.h"
#include "entities.h"

void GameWorld::update_triangles() {
    std::vector<WVec> triangles;
    triangulate_island(m_island, triangles);
    m_grid.clear();
    for (uint i = 0 ; i < triangles.size() / 3 ; ++i) {
        auto p1 = m_pos_c[0].global_transform.transformPoint(triangles[i*3]);
        auto p2 = m_pos_c[0].global_transform.transformPoint(triangles[i*3+1]);
        auto p3 = m_pos_c[0].global_transform.transformPoint(triangles[i*3+2]);
        m_grid.add_object(std::shared_ptr<ColShape>(new ColTriangle(p1, p2, p3)));
    }

}

GameWorld::GameWorld() {
    // create root
    auto root = create_root(*this, {0, 0}, 0);
    create_coll_test(*this, {0, 0}, root);
}

void GameWorld::draw(sf::RenderWindow &window) {
    m_debug_draw_system.update(*this, window);
}

void GameWorld::step_fixed(const WVec &mouse) {
    m_test_system.update(*this, mouse);
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
    m_pos_c.push_back(PosComponent());
    m_debug_c.push_back(DebugComponent());
    m_id_c.push_back("");

    return entity;
}
