//
// Created by tobi on 4/11/17.
//

#include "systems.h"
#include "GameWorld.h"
#include "MoveSystems.h"
#include "entities.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_transform_2d.hpp>
#include <WRenderer.h>
#include <WVecMath.h>

void debug_draw_update(GameWorld &world, const std::vector<unsigned int> &entities) {
    WTransform zero_tf;
    WRenderer::set_mode(GL_LINES);
    for (const auto &tri : world.m_grid.get_objects()) {
        tri->add_gfx_lines(zero_tf);
    }

    for (const auto entity : entities) {
        auto &shape = world.m_debug_c[entity].shape;
        const auto &transform = world.m_pos_c[entity].global_transform;
        shape->add_gfx_lines(transform);
    }
}

void static_col_update(GameWorld &world, const std::vector<unsigned int> &entities) {

    for (const auto entity : entities) {
        // position
        auto &transform = world.m_pos_c[entity].global_transform;
        auto &pos = world.m_pos_c[entity].position;
        auto &rot = world.m_pos_c[entity].rotation;
        auto parent = world.m_pos_c[entity].parent;

        // collision
        auto &result = world.m_static_col_c[entity].col_result;
        auto &shape = world.m_static_col_c[entity].shape;

        //circle to world
        transform = glm::rotate(glm::translate(WTransform(), pos), rot) * world.m_pos_c[parent].global_transform;
        shape->transform(transform);

        // overwrite result
        result = ColResult();

        auto colliders = world.m_grid.find_colliders(shape);
        for (const auto &tri : colliders) {
            auto cr = shape->collides(*tri);
            if (cr.collides) {
                if (cr.depth > result.depth) {
                    result = cr;
                }
            }
        }
        // in this example it's okay to reset the transformation here.
        // this deals only with the environmental collision.
        // (what to do about moving platforms)
        shape->transform(glm::inverse(transform));

        if (result.collides && result.epa_it < 21) {
            auto move_back = result.normal * -result.depth;
            pos += move_back;

            // player can stand on slopes
            if (w_dot(WVec(0, 1), result.normal) > c_max_floor_angle) {
                WVec correction = w_slide(WVec(move_back.x, 0), result.normal);
                if (correction.x != 0) {
                    correction *= move_back.x / correction.x;
                }
                pos -= correction;
            }

            // slide movement and collide again
            //circle to world
            transform = glm::rotate(glm::translate(WTransform(), pos), rot) * world.m_pos_c[parent].global_transform;
            shape->transform(transform);

            ColResult second_result;

            for (const auto &tri : colliders) {
                auto cr = shape->collides(*tri);
                if (cr.collides) {
                    if (cr.depth> second_result.depth) {
                        second_result = cr;
                    }
                }
            }
            shape->transform(glm::inverse(transform));

            if (second_result.collides) {
                WVec correction = find_directed_overlap(second_result, WVec(-move_back.y, move_back.x));
                pos += correction;

                transform = glm::rotate(glm::translate(WTransform(), pos), rot) * world.m_pos_c[parent].global_transform;
            }

            // call back
            static_col_responses.at(world.m_static_col_c[entity].col_response)(result, world, entity);
        }
    }
}

void input_update(GameWorld &world, const WVec &mouse, const std::vector<unsigned int> &entities) {
    for (const auto entity : entities) {
        auto &ic = world.m_input_c[entity];
        input_funcs.at(ic.input_func)(ic, mouse);

    }
}

void move_update(GameWorld &world, float dt, const std::vector<unsigned int> &entities) {
    for (const auto entity : entities) {

        auto &mc = world.m_move_c[entity];
        auto &pc = world.m_pos_c[entity];

        auto old_accel = mc.accel;

        mc.accel = WVec(0, c_gravity);
        mc.accel += mc.additional_accel;

        mc.velocity += old_accel * dt;

        get_accel_func(mc.movestate, mc.moveset)(world, entity);

        mc.velocity += dt * (mc.accel - old_accel) / 2.0f;
        auto motion = dt * (mc.velocity + mc.accel * dt / 2.0f);
        pc.position += motion;

        pc.global_transform = glm::rotate(glm::translate(WTransform(), pc.position), pc.rotation) * world.m_pos_c[pc.parent].global_transform;
    }

}


void ground_move_update(GameWorld &world, float dt, const std::vector<unsigned int> &entities) {
    for (const auto entity : entities) {

        auto &gc = world.m_ground_move_c[entity];
        gc.air_time += dt;
    }
}

void fly_update(GameWorld &world, float dt, const std::vector<unsigned int> &entities) {
    for (const auto entity : entities) {

        auto &fc = world.m_fly_c[entity];
        fc.timer += dt;
        if (fc.timer > fc.c_accel_time) {
            fc.timer = fc.c_accel_time;
        }
    }
}
