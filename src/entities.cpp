//
// Created by jammer on 11/04/17.
//

#include "entities.h"
#include <assert.h>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_transform_2d.hpp>

unsigned int Entities::create_root(GameWorld &game_world, const WVec &position, unsigned int parent) {
    // create root
    auto root = game_world.create_entity();
    assert(root == 0);
    assert(parent == 0);
    bset comps;
    for (auto i : {CPosition}) {
        comps.set(i);
    }
    game_world.m_entities[root] = comps;
    game_world.m_id_c[root] = "root";
    game_world.m_pos_c[root].position = position;
    game_world.m_pos_c[root].parent = parent;
    game_world.m_pos_c[root].global_transform = glm::translate(WTransform(), position);

    return root;
}