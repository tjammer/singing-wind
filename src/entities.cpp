//
// Created by jammer on 11/04/17.
//

#include <GameWorld.h>
#include <assert.h>

unsigned int create_root(GameWorld &game_world, const WVec &position, unsigned int parent) {
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
    WTransform trans;
    game_world.m_pos_c[root].global_transform = trans.translate(position);

    return root;
}