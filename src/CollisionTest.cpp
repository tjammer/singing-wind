//
// Created by tobi on 4/3/17.
//

#include "CollisionTest.h"
#include "Collision.h"
#include "GameWorld.h"

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

void ColTestSystem::update(GameWorld &world, const WVec &mouse) {
    for (unsigned int entity = 0; entity < world.m_entities.size(); ++entity) {
        if (!has_component(world.m_entities[entity], components)) {
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
                if (cr.e > result.e) {
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
                    if (cr.e> result.e) {
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

ColTestSystem::ColTestSystem() : components({ (1 << CPosition) | (1 << CDebugDraw) }){
}
