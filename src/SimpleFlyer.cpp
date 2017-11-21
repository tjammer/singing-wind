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

void SimpleFlyingMove::accel(GameWorld &world, unsigned int entity) {
    auto &ic = world.input_c(entity);
    auto &fc = world.simple_fly_c(entity);
    auto &pc = world.pos_c(entity);
    auto &mc = world.move_c(entity);

    auto dir = w_normalize(ic.mouse[0] - pc.global_position);
    auto vel = w_normalize(mc.velocity);
    auto steering = dir - vel;

    auto angle =  w_angle_to_vec(w_rotated(WVec(0, -1), pc.rotation * pc.direction), mc.velocity);
    rotate_angle(angle * pc.direction, mc.c_max_change_angle, pc);

    mc.velocity = w_magnitude(mc.velocity) * w_rotated(WVec(0, -1), pc.rotation * pc.direction);

    mc.accel = (vel + steering) * fc.c_accel;
    if (w_magnitude(mc.velocity) > fc.c_max_vel) {
        mc.velocity = vel * fc.c_max_vel;
    }
}

void SimpleFlyingMove::enter(GameWorld &world, unsigned int entity) {
}

MoveStateName SimpleFlyingMove::name() {
    return MoveStateName::SimpleFlying;
}

bool SimpleFlyingMove::transition(GameWorld &world, unsigned int entity) {
    if (world.ai_c(entity).state == AIState::Pursuit) {
        return true;
    }
    return false;
}

void HoverMove::enter(GameWorld &world, unsigned int entity) {
}

void HoverMove::accel(GameWorld &world, unsigned int entity) {
    auto &mc = world.move_c(entity);
    auto &pc = world.pos_c(entity);
    auto &fc = world.simple_fly_c(entity);
    auto &ic = world.input_c(entity);

    // rotate
    pc.rotation += copysignf(fmin(mc.c_max_change_angle, abs(pc.rotation)), pc.rotation - (float)M_PI);
    pc.rotation = std::remainder(pc.rotation, (float)M_PI * 2.f);

    // hover
    // mitigate gravity
    mc.accel.y *= 0.0f;
    mc.accel.y += c_gravity * .01f * (ic.mouse[0].y - pc.global_position.y);

    mc.accel.x -= fc.c_stop_coef * mc.velocity.x;
    if (abs(mc.velocity.y) > 130) {
        mc.accel.y -= fc.c_stop_coef * 0.01f * mc.velocity.y;
    }
}

MoveStateName HoverMove::name() {
    return MoveStateName::Hover;
}

bool HoverMove::transition(GameWorld &world, unsigned int entity) {
    if (world.ai_c(entity).state == AIState::Idle) {
        return true;
    }
    return false;
}

std::unique_ptr<MoveState> SimpleFlyerMoveSet::transition(GameWorld &world, unsigned int entity) {
    auto &mc = world.move_c(entity);

    if (!mc.movestate) {
        return from_undefined(world, entity);
    }

    switch(mc.movestate->name()) {
        case MoveStateName::SimpleFlying : {
                                               if (SimpleFlyingMove::transition(world, entity)) {
                                                   return std::make_unique<HoverMove>();
                                               }
                                               break;
                                           }
        case MoveStateName::Hover : {
                                        if (HoverMove::transition(world, entity)) {
                                            return std::make_unique<SimpleFlyingMove>();
                                            break;
                                        }
                                    }
        default : {
                      return from_undefined(world, entity);
                      break;
                  }
    }
    return nullptr;
}

void SimpleFlyerMoveSet::init(GameWorld &world, unsigned int entity) {
    auto &mc = world.move_c(entity);
    mc.movestate = from_undefined(world, entity);
    mc.special_movestate = nullptr;
}

std::unique_ptr<MoveState> SimpleFlyerMoveSet::from_undefined(GameWorld &, unsigned int) {
    return std::make_unique<HoverMove>();
}
