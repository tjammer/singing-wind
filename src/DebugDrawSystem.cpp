//
// Created by jammer on 11/04/17.
//

#include "DebugDrawSystem.h"
#include "GameWorld.h"

DebugDrawSystem::DebugDrawSystem() : components({ (1 << CPosition) | (1 << CDebugDraw) }) {
}

void DebugDrawSystem::update(GameWorld &world, sf::RenderWindow &window) {
    sf::VertexArray lines_va(sf::Lines);
    WTransform zero_tf;
    for (const auto &tri : world.m_grid.get_objects()) {
        tri->add_gfx_lines(lines_va, zero_tf);
    }

    for (unsigned int entity = 0; entity < world.m_entities.size(); ++entity) {
        if (!has_component(world.m_entities[entity], components)) {
            continue;
        }
        auto &shape = world.m_debug_c[entity].shape;
        auto &transform = world.m_pos_c[entity].global_transform;
        shape->add_gfx_lines(lines_va, transform);
    }

    window.draw(lines_va);
}
