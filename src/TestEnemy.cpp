#include "TestEnemy.h"
#include "GameWorld.h"
#include "WVecMath.h"
#include "PosComponent.h"
#include "InputComponent.h"
#include "CollisionComponent.h"
#include "Collision.h"
#include "MoveSystems.h"
#include "Pathfinding.h"

using namespace TestEnemy;

void TestEnemy::on_static_collision(GameWorld &world, unsigned int entity) {
    auto &mc = world.move_c(entity);
    auto result = world.static_col_c(entity).col_result;
    mc.velocity = w_slide(mc.velocity, result.normal);
}

void TestEnemy::handle_inputs(GameWorld &world, unsigned int entity) {
    auto &ic = world.input_c(entity);
    auto &pc = world.path_c(entity);

    unsigned int following = 1; //should be player
    auto &ent_pos = world.pos_c(entity).position;
    auto &follow_pos = world.pos_c(following).position;
    get_path(world, entity);
    pc.index = pc.path.size() -1;
    while (pc.index > 0) {
        auto result = cast_ray_vs_static_grid(world.grid(), ent_pos, pc.path[pc.index - 1]);
        if (!result.hits) {
            pc.index--;
            pc.path.pop_back();
        } else {
            break;
        } 
    }
    if (pc.index > 0) {
        push_value(ic.mouse, pc.path[pc.index]);
    } else {
        auto result = cast_ray_vs_static_grid(world.grid(), ent_pos, follow_pos);
        if (!result.hits) {
            push_value(ic.mouse, follow_pos);
        } else {
            push_value(ic.mouse, pc.path[pc.index]);
        }
    }
}

void TestEnemy::simple_flying(GameWorld &world, unsigned int entity) {
    auto &ic = world.input_c(entity);
    auto &fc = world.simple_fly_c(entity);
    auto &pc = world.pos_c(entity);
    auto &mc = world.move_c(entity);
    
    auto dir = w_normalize(ic.mouse[0] - pc.position);
    // rotate
    auto mouse = WVec(glm::inverse(pc.global_transform) * WVec3(ic.mouse[0], 1));
    // see src/Protagonist angle_up
    float mouse_angle = atan2f(mouse.x, -mouse.y);
    pc.rotation += copysignf(fmin(fc.c_max_change_angle, abs(mouse_angle)), mouse_angle);
    pc.rotation = std::remainder(pc.rotation, (float)M_PI * 2.f);
    // cancel gravity
    mc.accel.y = 0;
    float distance = w_magnitude(ic.mouse[0] - pc.position);
    if (distance > fc.c_near_threshold) {
        // accel if not already max vel
        if (w_magnitude(mc.velocity) >= fc.c_max_vel) {
            mc.velocity = w_normalize(mc.velocity) * fc.c_max_vel * 0.5f + dir * fc.c_max_vel * 0.5f;
        }
        mc.accel += fc.c_accel * dir;
    } else {
        mc.accel -= fc.c_stop_coef * mc.velocity;
    }
}

void TestEnemy::to_simple_flying(GameWorld &world, unsigned int entity) {
    auto &mc = world.move_c(entity);
    mc.movestate = MoveState::SimpleFlying;
}

bool TestEnemy::transition_simple_flying(GameWorld &, unsigned int) {
    return true;
}
