#include "SimpleFlyer.h"
#include "GameWorld.h"
#include "WVecMath.h"
#include "PosComponent.h"
#include "InputComponent.h"
#include "CollisionComponent.h"
#include "MoveSystems.h"
#include "AIComponent.h"

void simpleflyer::on_static_collision(GameWorld &world, unsigned int entity) {
    auto &mc = world.move_c(entity);
    auto result = world.static_col_c(entity).col_result;
    mc.velocity = w_slide(mc.velocity, result.normal);
}

void simpleflyer::simple_flying(GameWorld &world, unsigned int entity) {
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

void simpleflyer::to_simple_flying(GameWorld &world, unsigned int entity) {
    auto &mc = world.move_c(entity);
    mc.movestate = MoveState::SimpleFlying;
}

bool simpleflyer::transition_simple_flying(GameWorld &world, unsigned int entity) {
    if (world.ai_c(entity).state == AIState::Pursuit) {
        return true;
    }
    return false;
}

void simpleflyer::to_hover(GameWorld &world, unsigned int entity) {
    auto &mc = world.move_c(entity);
    mc.movestate = MoveState::Hover;
}

void simpleflyer::hover(GameWorld &world, unsigned int entity) {
    auto &mc = world.move_c(entity);
    auto &pc = world.pos_c(entity);
    auto &fc = world.simple_fly_c(entity);
    auto &ic = world.input_c(entity);

    // rotate
    pc.rotation += copysignf(fmin(fc.c_max_change_angle, abs(pc.rotation)), pc.rotation - (float)M_PI);
    //pc.rotation = std::remainder(pc.rotation, (float)M_PI * 2.f);
    
    // hover
    // mitigate gravity
    mc.accel.y *= 0.0f;
    mc.accel.y += c_gravity * .01f * (ic.mouse[0].y - pc.position.y);

    mc.accel.x -= fc.c_stop_coef * mc.velocity.x;
    if (abs(mc.velocity.y) > 130) {
        mc.accel.y -= fc.c_stop_coef * 0.01f * mc.velocity.y;
    }
}

bool simpleflyer::transition_hover(GameWorld &world, unsigned int entity) {
    if (world.ai_c(entity).state == AIState::Idle) {
        return true;
    }
    return false;
}
