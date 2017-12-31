#include "GoToEnemy.h"
#include "Components.h"
#include "GameWorld.h"
#include "Pathfinding.h"
#include "PosComponent.h"
#include "Collision.h"
#include "WVecMath.h"
#include "InputComponent.h"
#include "MoveSystems.h"

GoToEnemy::GoToEnemy(GameWorld &world, unsigned int entity, float radius) : m_world(world), m_entity(entity), m_radius(radius) {
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
            m_world.input_c(m_entity).direction.push(1); // arriving
        } else {
            enter();
            if (m_status == PathfindingStatus::Failure) {
                return behaviour_tree::Status::Failure;
            }
        }
        if (w_magnitude(pos - follow) < m_radius * 0.1f) {
            return behaviour_tree::Status::Success;
        }
    }
    m_world.input_c(m_entity).jump.push(true); // for seeking
    m_world.input_c(m_entity).mouse.push(pc.path[pc.index]);
    return behaviour_tree::Status::Running;
}

void GoToEnemy::leave(behaviour_tree::Status) {
    m_world.input_c(m_entity).jump.push(false);
    m_world.input_c(m_entity).direction.push(0); // cancels arriving and fleeing
}
