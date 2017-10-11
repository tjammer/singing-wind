//
// Created by jammer on 11/04/17.
//

#include "Protagonist.h"
#include "GameWorld.h"
#include "MoveSystems.h"
#include "PosComponent.h"
#include "Components.h"
#include "InputComponent.h"
#include "CollisionComponent.h"
#include <WInput.h>
#include <WVecMath.h>
#include "BCurve.h"
#include <algorithm>

#include <iostream>
using namespace protagonist;

void protagonist::handle_inputs(GameWorld &world, unsigned int entity) {
    auto &ic = world.input_c(entity);

    push_value(ic.jump, WInput::is_key_pressed(GLFW_KEY_SPACE));

    push_value(ic.direction, (WInput::is_key_pressed(GLFW_KEY_D) - WInput::is_key_pressed(GLFW_KEY_A)));

    push_value(ic.mouse, WInput::get_mouse_pos());

    push_value(ic.wings, WInput::is_mouse_button_pressed(GLFW_MOUSE_BUTTON_RIGHT));

    push_value(ic.att_melee, WInput::is_key_pressed(GLFW_KEY_Q));
}

template<class GC>
inline void walk(InputComponent &ic, MoveComponent &mc, GC &gc) {
    float mod = 1;
    float accel = gc.c_accel;
    if (ic.direction[0] * mc.velocity.x < 0) {
        mod *= gc.c_turn_mod;
    }
    else {
        // calc diminishing accel
        float vel = fmin(gc.c_max_vel, abs(mc.velocity.x));
        accel *= (1.f - exp(-pow(vel - gc.c_max_vel, 2.f) * 0.1f/gc.c_max_vel));
    }
    mc.accel.x += ic.direction[0] * mod * accel;
}

inline void drag_x(MoveComponent &mc) {
    mc.accel.x -= copysignf(mc.velocity.x * mc.velocity.x * c_drag * 0.01f, mc.velocity.x);
}

template<class GC>
inline void pseudo_friction(MoveComponent &mc, GC &gc) {
    if (abs(mc.velocity.x) > gc.c_max_vel) {
        mc.accel.x -= copysignf(gc.c_accel * (1.f - abs(mc.velocity.x) / gc.c_max_vel), mc.velocity.x);
    }
}

const float c_half_pi = (float)M_PI / 2.f;

const float c_velocity_scaling = 900;

inline float calc_drag(float angle, float vel_mag, float drag) {
    auto drag_coeff = drag * exp(vel_mag / c_velocity_scaling);
    return drag_coeff * expf(-powf(angle - c_half_pi, 2.f) * 1.4f);
}

inline float calc_lift(float angle, float vel_mag, const FlyComponent &fc) {
    if (angle < 0) {
        return -calc_lift(-angle, vel_mag, fc);
    }
    assert(angle >= 0);
    // damps lift for high angles to make going backwards difficult
    float lift =  fc.c_lift / expf(angle / (float)M_PI);
    return lift * cos((angle - fc.c_stall_angle) * c_half_pi / (c_half_pi - fc.c_stall_angle));
}

// get angle from mouse to player in degrees (for smfl), zero is up
inline float angle_up_from_local_mouse_deg(const WVec &mouse) {
    return atan2f(mouse.x, -mouse.y);
};

const WVec c_accel_angle_coords[4] = {{0, 1}, {0, 0.25}, {1, 1.25}, {1, 0}};

const BCurve c_accel_angle_curve = {
    c_accel_angle_coords[0],
    c_accel_angle_coords[1],
    c_accel_angle_coords[2],
    c_accel_angle_coords[3]};

inline WVec calc_accel_angle(const PosComponent &pc) {
    float fac = pc.rotation / (float)M_PI;
    float additional_rot = c_accel_angle_curve.eval(fac).y * (float)M_PI / 2.f;
    return w_rotated(WVec{0, -1}, (pc.rotation - additional_rot) * pc.direction);
}

inline void fly(MoveComponent &mc, float angle, const FlyComponent &fc, float strength=1.0f) {
    auto air_dir = w_normalize(mc.velocity);

    float vel_mag = w_magnitude(mc.velocity);
    float vel_squared = powf(vel_mag, 2);

    // use mass to determine static lift. for now dont take volume into
    // account, only flat mass
    mc.accel.y -= c_gravity * (1 - mc.mass);

    mc.accel -= air_dir * vel_squared * calc_drag(abs(angle), vel_mag, fc.c_drag) * strength;
    mc.accel -= w_tangent(air_dir) * vel_squared * calc_lift(angle, vel_mag, fc) * strength;
}

void ::protagonist::to_falling(GameWorld &world, unsigned int entity) {
    auto &mc = world.move_c(entity);
    mc.movestate = MoveState::Falling;
}

void protagonist::falling(GameWorld &world, unsigned int entity) {
    auto &ic = world.input_c(entity);
    auto &mc = world.move_c(entity);
    auto &fc = world.fall_c(entity);
    auto &pc = world.pos_c(entity);

    walk(ic, mc, fc);

    drag_x(mc);

    rotate_angle(-pc.rotation, mc.c_max_change_angle / 2.0f, pc);
}

void ::protagonist::to_ground(GameWorld &world, unsigned int entity) {
    auto &mc = world.move_c(entity);
    if (mc.movestate == MoveState::OnGround) {
        return;
    }
    if (!world.entities()[entity].test(CGroundMove)) {
        return;
    }
    auto &pc = world.pos_c(entity);

    mc.movestate = MoveState::OnGround;
    pc.rotation = 0;
}

void protagonist::on_ground(GameWorld &world, unsigned int entity) {
    auto &ic = world.input_c(entity);
    auto &mc = world.move_c(entity);
    auto &gc = world.ground_move_c(entity);
    auto &pc = world.pos_c(entity);

    /*if (bset.test(CInput) and ic.direction[0] and ic.direction[0] != ic.last_dir) {
        ic.last_dir = ic.direction[0];
    }*/

    walk(ic, mc, gc);
    pseudo_friction(mc, gc);

    if (ic.direction[0] == 0) {
        mc.accel.x -= gc.c_stop_friction * mc.velocity.x;
    } else {
        pc.direction = ic.direction[0];
    }

    drag_x(mc);
}

void ::protagonist::to_flying(GameWorld &world, unsigned int entity) {
    if (!world.entities()[entity].test(CFly)) {
        return;
    }
    auto &ic = world.input_c(entity);
    auto &mc = world.move_c(entity);
    auto &pc = world.pos_c(entity);

    clear_arr(ic.wings, true);
    // if coming from ground push
    if (mc.movestate == MoveState::OnGround) {
        mc.velocity.y = -world.fly_c(entity).c_push_vel;
        pc.rotation = (float)M_PI / 4.f;
    }

    mc.movestate = MoveState::Flying;
}

void ::protagonist::flying(GameWorld &world, unsigned int entity) {
    auto &pc = world.pos_c(entity);
    auto &ic = world.input_c(entity);
    auto &mc = world.move_c(entity);
    auto &fc = world.fly_c(entity);

    rotate_to(ic.mouse[0], mc.c_max_change_angle, pc);

    auto glide_dir = w_rotated(WVec(0, -1), pc.rotation * pc.direction);
    auto angle = w_angle_to_vec(mc.velocity, glide_dir);
    fly(mc, angle, fc);
}

void ::protagonist::to_flying_accel(GameWorld &world, unsigned int entity) {
    if (!world.entities()[entity].test(CFly)) {
        return;
    }
    auto &mc = world.move_c(entity);
    auto &pc = world.pos_c(entity);
    auto &ic = world.input_c(entity);

    clear_arr(ic.wings, true);
    mc.movestate = MoveState::FlyingAccel;
    // can get set after skill hit or so
    if (mc.timer < 0) {
        auto mouse = WVec(glm::inverse(pc.global_transform) * WVec3(ic.mouse[0], 1));
        float mouse_angle = angle_up_from_local_mouse_deg(mouse);
        pc.rotation += copysignf(fmin(mc.c_max_change_angle, abs(mouse_angle)), mouse_angle);
        pc.rotation = std::remainder(pc.rotation, (float)M_PI * 2.f);
        mc.velocity = world.fly_c(entity).c_push_vel * 1.0f * w_rotated(WVec(0, -1), pc.rotation);
        pc.rotation = w_angle_to_vec(WVec(0, -1), mc.velocity);
    }
    mc.timer = 0;
}

void ::protagonist::flying_accel(GameWorld &world, unsigned int entity) {
    auto &pc = world.pos_c(entity);
    auto &ic = world.input_c(entity);
    auto &mc = world.move_c(entity);
    auto &fc = world.fly_c(entity);

    if (!ic.wings[0] or mc.timer >= fc.c_accel_time) {
        mc.movestate = MoveState ::Flying;
        return;
    }
    BCurve curve = {fc.from, fc.ctrl_from, fc.ctrl_to, fc.to};
    float time_frac = curve.eval(mc.timer / fc.c_accel_time).y;

    rotate_to(ic.mouse[0], mc.c_max_change_angle, pc);

    auto glide_dir = w_rotated(WVec(0, -1), pc.rotation * pc.direction);
    auto angle = w_angle_to_vec(mc.velocity, glide_dir);
    fly(mc, angle, fc, 0.75f);

    mc.accel += calc_accel_angle(pc) * fc.c_accel_force * time_frac;
}

bool protagonist::transition_ground(GameWorld &world, unsigned int entity) {
    const auto &result = world.static_col_c(entity).col_result;

    if (w_dot(WVec(0, 1), result.normal) > c_max_floor_angle) {
        return true;
    }
    return false;
}

bool protagonist::transistion_falling(GameWorld &world, unsigned int entity) {
    const auto &mc = world.move_c(entity);
    auto &ic = world.input_c(entity);

    if (mc.movestate == MoveState::OnGround and mc.timer > c_jump_tolerance) {
        return true;
    } else if (mc.movestate == MoveState::Flying) {
        if (ic.jump[0] and std::find(ic.jump.begin(), ic.jump.end(), false) != ic.jump.end()) {
            clear_arr(ic.jump, true);
            to_falling(world, entity);
        }
    }
    return false;
}

bool protagonist::transition_flying(GameWorld &world, unsigned int entity) {
    const auto &ic = world.input_c(entity);
    if (ic.wings[0] and std::find(ic.wings.begin(), ic.wings.end(), false) != ic.wings.end()) {
        return true;
    }
    return false;
}

bool protagonist::transition_flying_accel(GameWorld &world, unsigned int entity) {
    const auto &ic = world.input_c(entity);
    if (ic.wings[0] and std::find(ic.wings.begin(), ic.wings.end(), false) != ic.wings.end()) {
        return true;
    }
    return false;
}
