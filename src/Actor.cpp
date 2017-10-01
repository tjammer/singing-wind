#include "Actor.h"
#include "GameWorld.h"
#include "WVecMath.h"
#include "MoveSystems.h"
#include "CollisionComponent.h"
#include "TagComponent.h"
#include "PosComponent.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_transform_2d.hpp>

void actor::on_static_collision(GameWorld &world, unsigned int entity) {
    auto &mc = world.move_c(entity);
    const auto &result = world.static_col_c(entity).col_result;

    if (w_dot(WVec(0, 1), result.normal) > c_max_floor_angle) {
        mc.timer = 0;
        mc.velocity.y = w_slide(mc.velocity, result.normal).y * 0.5f;
    }
    else {
        mc.velocity = w_slide(mc.velocity, result.normal);
    }
}

void actor::on_dynamic_collision(GameWorld &world, unsigned int entity) {
    const auto &collider = world.dyn_col_c(entity).collided; 
    const auto &result = world.dyn_col_c(entity).col_result;
    auto &mc = world.move_c(entity);
    auto &pc = world.pos_c(entity);

    const auto &tag = world.tag_c(collider);
    if (tag.test(static_cast<int>(Tags::Actor))) {
        pc.position -=  0.5f * result.normal * result.depth; 
        //pc.global_transform = glm::rotate(glm::translate(WTransform(), pc.position), pc.rotation) * world.pos_c(pc.parent).global_transform;
        // dont slide for now
        world.move_c(collider).additional_force += 200.f * result.normal;
        mc.velocity =  0.98f * mc.velocity + .02f * w_slide(mc.velocity, result.normal);
    }
}

