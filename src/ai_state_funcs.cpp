#include "ai_state_funcs.h"
#include "ai_state_transitions.h"
#include "GameWorld.h"
#include "Components.h"
#include "Pathfinding.h"
#include "Collision.h"
#include "PosComponent.h"
#include "AIComponent.h"

void ai_funcs::pursuit_func(GameWorld &world, unsigned int entity) {
    assert(world.entities()[entity].test(CPathing));
    auto &pc = world.path_c(entity);
    auto &pos = world.pos_c(entity).position;
    assert(pc.following > 0);
    auto &follow = world.pos_c(pc.following).position;

    while (pc.index > 0) {
        auto result = cast_ray_vs_static_grid(world.grid(), pos, pc.path[pc.index - 1]);
        if (!result.hits) {
            pc.index--;
            pc.path.pop_back();
        } else {
            break;
        }
    }
    if (pc.index == 0) {
        auto result = cast_ray_vs_static_grid(world.grid(), pos, follow);
        if (!result.hits) {
            pc.path[pc.index] = follow;
        }
    }

    if (world.ai_c(entity).timer >= ai::c_pathfinding_time) {
        ai_to_funcs::to_pursuit(world, entity);
    }

}

void ai_funcs::idle_func(GameWorld &world, unsigned int entity) {
    auto &ac = world.ai_c(entity);
    auto &path_c = world.path_c(entity);
    auto &pc = world.pos_c(entity);

    // reset msg_data to prolong alert bubble
    ac.msg_data.clear();
    if (path_c.path.size() == 0) {
        path_c.path.push_back(pc.position + WVec(0, -10));
    } else {
        path_c.path[0] = pc.position + WVec(0, -10);
    }
    path_c.index = 0;
}
