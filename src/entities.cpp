//
// Created by jammer on 11/04/17.
//

#include "entities.h"
#include <assert.h>
#include "GameWorld.h"
#include "Components.h"
#include "PosComponent.h"

unsigned int Entities::create_root(GameWorld &game_world) {
    // create root
    auto root = game_world.create_entity();
    assert(root == 0);
    bset comps;
    for (auto i : {CPosition}) {
        comps.set(i);
    }
    game_world.entities()[root] = comps;
    game_world.name_c(root) = "root";
    game_world.pos_c(root).position = {0, 0};
    game_world.pos_c(root).parent = 0;
    game_world.pos_c(root).global_transform = WTransform();

    return root;
}
