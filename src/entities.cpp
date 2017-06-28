//
// Created by jammer on 11/04/17.
//

#include "entities.h"
#include <assert.h>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_transform_2d.hpp>
#include "GameWorld.h"
#include "Components.h"
#include "PosComponent.h"

unsigned int Entities::create_root(GameWorld &game_world, const WVec &position, unsigned int parent) {
    // create root
    auto root = game_world.create_entity();
    assert(root == 0);
    assert(parent == 0);
    bset comps;
    for (auto i : {CPosition}) {
        comps.set(i);
    }
    game_world.entities()[root] = comps;
    game_world.name_c(root) = "root";
    game_world.pos_c(root).position = position;
    game_world.pos_c(root).parent = parent;
    game_world.pos_c(root).global_transform = glm::translate(WTransform(), position);

    return root;
}
