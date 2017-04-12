//
// Created by tobi on 4/11/17.
//

#include "systems.h"
#include "GameWorld.h"
#include <iostream>

void col_test_update(GameWorld &world, const WVec &mouse) {
    for (unsigned int entity = 0; entity < world.m_entities.size(); ++entity) {
        if (!has_component(world.m_entities[entity], c_col_test_components)) {
            continue;
        }
        auto &transform = world.m_pos_c[entity].global_transform;
        auto &pos = world.m_pos_c[entity].position;
        auto &rot = world.m_pos_c[entity].rotation;
        auto &shape = world.m_debug_c[entity].shape;
        auto parent = world.m_pos_c[entity].parent;

        WVec vel = (mouse - transform.transformPoint(0, 0)) * 0.1f;
        pos += vel;

        for (auto &tri : world.m_grid.get_objects()) {
            tri->m_highlight = false;
        }

        //circle to world
        transform = WTransform::Identity;
        transform.combine(world.m_pos_c[parent].global_transform).translate(pos).rotate(rot);
        shape->transform(transform);

        ColResult result;

        auto colliders = world.m_grid.find_colliders(shape);
        for (const auto &tri : colliders) {
            auto cr = shape->collides(*tri);
            if (cr.collides) {
                tri->m_highlight = true;
                if (cr.depth > result.depth) {
                    result = cr;
                }
            }
        }
        // in this example it's okay to reset the transformation here.
        // this deals only with the environmental collision.
        // (what to do about moving platforms)
        shape->transform(transform.getInverse());

        if (result.collides) {
            auto move_back = find_directed_overlap(result, vel);
            pos += move_back;

            // slide movement and collide again
            vel = slide(-move_back, result.normal);
            pos += vel;
            //circle to world
            transform = WTransform::Identity;
            transform.combine(world.m_pos_c[parent].global_transform).translate(pos).rotate(rot);
            shape->transform(transform);

            result = ColResult();

            for (const auto &tri : colliders) {
                auto cr = shape->collides(*tri);
                if (cr.collides) {
                    tri->m_highlight = true;
                    if (cr.depth> result.depth) {
                        result = cr;
                    }
                }
            }
            shape->transform(transform.getInverse());

            if (result.collides) {
                move_back = find_directed_overlap(result, vel);
                pos += move_back;

                transform = WTransform::Identity;
                transform.combine(world.m_pos_c[parent].global_transform).translate(pos).rotate(rot);
            }
        }
    }
}

void debug_draw_update(GameWorld &world, sf::RenderWindow &window) {
    sf::VertexArray lines_va(sf::Lines);
    WTransform zero_tf;
    for (const auto &tri : world.m_grid.get_objects()) {
        tri->add_gfx_lines(lines_va, zero_tf);
    }

    for (unsigned int entity = 0; entity < world.m_entities.size(); ++entity) {
        if (!has_component(world.m_entities[entity], c_debug_draw_components)) {
            continue;
        }
        auto &shape = world.m_debug_c[entity].shape;
        auto &transform = world.m_pos_c[entity].global_transform;
        shape->add_gfx_lines(lines_va, transform);
    }

    window.draw(lines_va);
}

void static_col_update(GameWorld &world) {

    for (unsigned int entity = 0; entity < world.m_entities.size(); ++entity) {
        if (!has_component(world.m_entities[entity], c_static_col_components)) {
            continue;
        }
        // position
        auto &transform = world.m_pos_c[entity].global_transform;
        auto &pos = world.m_pos_c[entity].position;
        auto &rot = world.m_pos_c[entity].rotation;
        auto parent = world.m_pos_c[entity].parent;

        // collision
        auto &result = world.m_static_col_c[entity].col_result;
        auto &shape = world.m_static_col_c[entity].shape;

        // movement
        auto &vel = world.m_move_c[entity].velocity;

        //circle to world
        transform = WTransform::Identity;
        transform.combine(world.m_pos_c[parent].global_transform).translate(pos).rotate(rot);
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
        shape->transform(transform.getInverse());

        if (result.collides) {
            auto move_back = result.normal * -result.depth;
            pos += move_back;

            WVec correction;
            if (dot(WVec(0, 1), result.normal) > c_max_floor_angle) {
                correction = slide(WVec(move_back.x, 0), result.normal);
                correction *= move_back.x / correction.x;
            }
            else {
                correction = slide(WVec(0, move_back.y), result.normal);
                correction *= move_back.y / correction.y;
            }

            // slide movement and collide again
            pos -= correction;
            //circle to world
            transform = WTransform::Identity;
            transform.combine(world.m_pos_c[parent].global_transform).translate(pos).rotate(rot);
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
            shape->transform(transform.getInverse());

            if (second_result.collides) {
                move_back = result.normal * -result.depth;
                pos += move_back;

                transform = WTransform::Identity;
                transform.combine(world.m_pos_c[parent].global_transform).translate(pos).rotate(rot);
            }

            // call back
            world.m_static_col_c[entity].on_static_col_cb(result, world, entity);
        }
    }
}

void input_update(GameWorld &world, const WVec &mouse) {
    for (unsigned int entity = 0; entity < world.m_entities.size(); ++entity) {
        if (!has_component(world.m_entities[entity], c_input_components)) {
            continue;
        }
        auto &ic = world.m_input_c[entity];
        world.m_input_c[entity].input_func(ic, mouse);
    }
}

void move_update(GameWorld &world, float dt) {
    for (unsigned int entity = 0; entity < world.m_entities.size(); ++entity) {
        if (!has_component(world.m_entities[entity], c_move_components)) {
            continue;
        }

        auto &mc = world.m_move_c[entity];
        auto &ic = world.m_input_c[entity];

        auto old_accel = mc.accel;

        mc.accel = WVec(0, c_gravity);
        mc.accel += mc.additional_accel;

        mc.velocity += old_accel * dt;

        mc.accel_func(ic, mc);

        mc.velocity += dt * (mc.accel - old_accel) / 2.0f;
        auto motion = dt * (mc.velocity + mc.accel * dt / 2.0f);
        world.m_pos_c[entity].position += motion;

        mc.air_time += dt;
    }

}
