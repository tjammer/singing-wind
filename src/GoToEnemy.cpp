#include "GoToEnemy.h"
#include "Components.h"
#include "GameWorld.h"
#include "Pathfinding.h"
#include "PosComponent.h"
#include "Collision.h"
#include "WVecMath.h"
#include "InputComponent.h"
#include "MoveSystems.h"
#include "TagComponent.h"

GoToEnemy::GoToEnemy(GameWorld &world, unsigned int entity) : m_world(world), m_entity(entity) {
}

void GoToEnemy::enter() {
    auto &pc = m_world.path_c(m_entity);
    assert(pc.following > 0);
    assert(m_world.entities()[m_entity].test(CPathing));
    const auto & pos = m_world.pos_c(m_entity).global_position;
    const auto &to = m_world.pos_c(pc.following).global_position;
    m_status = get_path(pos, to, m_world, pc);
    m_world.simple_fly_c(m_entity).c_arrive_radius = m_radius;
}

behaviour_tree::Status GoToEnemy::update() {
    if (m_status == PathfindingStatus::Failure) {
        return behaviour_tree::Status::Failure;
    }
    const auto &pos = m_world.pos_c(m_entity).global_position;
    auto &pc = m_world.path_c(m_entity);

    while (pc.index > 0) {
        auto result = cast_ray_vs_static_grid(m_world.grid(), pos, pc.path[pc.index - 1]);
        if (!result.hits) {
            pc.index--;
            pc.path.pop_back();
        } else {
            break;
        }
    }

    if (pc.index == 0) {
        const auto &follow = m_world.pos_c(pc.following).global_position;
        auto result = cast_ray_vs_static_grid(m_world.grid(), pos, follow);
        if (!result.hits) {
            pc.path[pc.index] = follow;
        } else {
            enter();
            if (m_status == PathfindingStatus::Failure) {
                return behaviour_tree::Status::Failure;
            }
        }
        if (w_magnitude(pos - follow) < 100) {
            return behaviour_tree::Status::Success;
        }
    }
    m_world.input_c(m_entity).jump.push(true); // for seeking
    m_world.input_c(m_entity).mouse.push(pc.path[pc.index]);

    // flock
    auto colliders = m_world.dynamic_grid().find_colliders_in_radius(pos, 150);
    pc.cohesion = pos;
    int i = 1;
    pc.flock.clear();

    for (auto &col : colliders) {
        if (col.entity == m_entity) {
            continue;
        }
        if (m_world.tag_c(col.entity).test(static_cast<int>(Tags::Enemy))) {
            pc.flock.push_back(col.entity);
            pc.cohesion += m_world.pos_c(col.entity).global_position;
            i++;
        }
    }
    pc.cohesion /= (float)i;

    return behaviour_tree::Status::Running;
}

void GoToEnemy::leave(behaviour_tree::Status) {
    m_world.input_c(m_entity).jump.push(false);
}
