//
// Created by jammer on 07/04/17.
//

#include "GameWorld.h"
#include "triangulate.h"
#include "entities.h"
#include "systems.h"
#include "Protagonist.h"

void GameWorld::update_triangles() {
    std::vector<WVec> triangles;
    m_grid.clear();
    for (const auto &island : m_islands) {
        triangulate_island(island, triangles);
        for (uint i = 0 ; i < triangles.size() / 3 ; ++i) {
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
    Protagonist::create_player(*this, {0, 0}, root);
}

void GameWorld::draw(sf::RenderWindow &window) {
    debug_draw_update(*this, window);
}

void GameWorld::step_fixed(float dt, const WVec &mouse) {
    input_update(*this, mouse);
    move_update(*this, dt);
    static_col_update(*this);

    // house keeping systems
    ground_move_update(*this, dt);
    fly_update(*this, dt);
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
    m_input_c.push_back(InputComponent());
    m_move_c.push_back(MoveComponent());
    m_static_col_c.push_back(StaticColComponent());
    m_ground_move_c.push_back(GroundMoveComponent());
    m_jump_c.push_back(JumpComponent());
    m_fly_c.push_back(FlyComponent());

    return entity;
}
