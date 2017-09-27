#include "ai_state_funcs.h"
#include "ai_state_transitions.h"
#include "GameWorld.h"
#include "Components.h"
#include "Pathfinding.h"
#include "Collision.h"
#include "PosComponent.h"
#include "AIComponent.h"
#include "PatrolComponent.h"

void ai_funcs::pursuit_func(GameWorld &world, unsigned int entity) {
    assert(world.entities()[entity].test(CPathing));
    auto &pc = world.path_c(entity);
    auto &pos = world.pos_c(entity).global_position;
    assert(pc.following > 0);
    auto &follow = world.pos_c(pc.following).global_position;

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
        auto &ac = world.ai_c(entity);
        auto times = ac.msg_data[0];
        ai_to_funcs::to_pursuit(world, entity);
        ac.msg_data[0] = times + 1;
    }
}

void ai_funcs::return_func(GameWorld &world, unsigned int entity) {
   assert(world.entities()[entity].test(CPathing));
    auto &pc = world.path_c(entity);
    auto &pos = world.pos_c(entity).global_position;
    assert(pc.following == 0);
    const auto &follow = world.patrol_c(entity).patrol_point;

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
}

void ai_funcs::idle_func(GameWorld &world, unsigned int entity) {
    auto &ac = world.ai_c(entity);
    // reset msg_data to prolong alert bubble
    ac.msg_data.clear();
}
