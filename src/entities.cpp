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

unsigned int create_coll_test(GameWorld &world, const WVec &position, unsigned int parent) {
    auto entity = world.create_entity();
    bset comps;
    for (auto i : {CPosition, CDebugDraw}) {
        comps.set(i);
    }
    world.m_entities[entity] = comps;
    world.m_pos_c[entity].position = position;
    world.m_pos_c[entity].parent = parent;
    WTransform trans;
    world.m_pos_c[entity].global_transform = trans.combine(world.m_pos_c[parent].global_transform).translate(position);

    world.m_debug_c[entity].shape = std::shared_ptr<ColShape>(new ColCapsule(25, 50));

    world.m_id_c[entity] = "CollisionTest";

    return entity;
}