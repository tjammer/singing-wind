#include "Actor.h"
#include "GameWorld.h"
#include "WVecMath.h"
#include "MoveSystems.h"
#include "CollisionComponent.h"
#include "TagComponent.h"
#include "PosComponent.h"

void actor::on_static_collision(GameWorld &world, unsigned int entity) {
    auto &mc = world.move_c(entity);
    auto &gc = world.ground_move_c(entity);
    auto result = world.static_col_c(entity).col_result;

    if (w_dot(WVec(0, 1), result.normal) > c_max_floor_angle) {
        gc.air_time = 0;
        // movestate to ground
        auto trans_fc = get_trans_func(MoveState::OnGround, MoveSet::Protagonist);
        if (trans_fc) {
            trans_fc(world, entity);
        }
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

    if (world.tag_c(collider).test(static_cast<int>(Tags::Actor))) {
        pc.position -=  0.5f * result.normal * result.depth; 
        // dont slide for now
        world.move_c(collider).additional_force += 200.f * result.normal;
        mc.velocity = 0.5f * (mc.velocity + w_slide(mc.velocity, result.normal));
    } else {
        assert(false);
    }
}

