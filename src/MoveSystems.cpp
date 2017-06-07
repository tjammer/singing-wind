//
// Created by tobi on 4/27/17.
//

#include "MoveSystems.h"
#include "BCurve.h"
#include <iostream>
#include <algorithm>
#include <WVecMath.h>

using namespace std;
using accel_func = std::function<void(GameWorld &world, unsigned int entity)>;

const unordered_map<MoveState, accel_func> protagonist_ms = {
        {MoveState::OnGround, protagonist::on_ground},
        {MoveState::Falling, protagonist::jumping},
        {MoveState::Jumping, protagonist::jumping},
        {MoveState::Flying, protagonist::flying},
        {MoveState::FlyingAccel, protagonist::flying_accel}
};

const unordered_map<MoveTransition, accel_func> protagonist_trans = {
        {MoveTransition::ToGround, protagonist::to_ground}
};

const unordered_map<MoveSet, unordered_map<MoveState, accel_func>> move_sets = {
        {MoveSet::Protagonist, protagonist_ms}
};

const unordered_map<MoveSet, unordered_map<MoveTransition, accel_func>> trans_sets = {
        {MoveSet::Protagonist, protagonist_trans}
};


accel_func get_accel_func(const MoveState &state, const MoveSet &set) {
    return move_sets.at(set).at(state);
}

accel_func get_trans_func(const MoveTransition &trans, const MoveSet &set) {
    return trans_sets.at(set).at(trans);
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

inline void drag(MoveComponent &mc) {
    mc.accel.x -= copysignf(mc.velocity.x * mc.velocity.x * c_drag * 0.01f, mc.velocity.x);
}

template<class GC>
inline void pseudo_friction(MoveComponent &mc, GC &gc) {
    if (abs(mc.velocity.x) > gc.c_max_vel) {
        mc.accel.x -= copysignf(gc.c_accel * (1.f - abs(mc.velocity.x) / gc.c_max_vel), mc.velocity.x);
    }
}

const float c_half_pi = (float)M_PI / 2.f;

const float c_velocity_scaling = 1400;

inline float calc_drag(float angle, float vel_mag) {
    auto drag_coeff = c_drag * exp(vel_mag / c_velocity_scaling);
    return drag_coeff * expf(-powf(angle - c_half_pi, 2.f) * 1.4f);
}

inline float calc_lift(float angle, float vel_mag, FlyComponent &fc) {
    if (angle < 0) {
        return -calc_lift(-angle, vel_mag, fc);
    }
    assert(angle >= 0);
    if (angle < fc.c_stall_angle || abs(fmod(angle + (float)M_PI, (float)M_PI)) < fc.c_stall_angle) {
        return fc.c_lift * sin(angle * c_half_pi / fc.c_stall_angle);
    }
    return fc.c_lift * cos((angle - fc.c_stall_angle) * c_half_pi / (c_half_pi - fc.c_stall_angle));
}

// get angle from mouse to player in degrees (for smfl), zero is up
inline float angle_up_from_local_mouse_deg(const WVec &mouse) {
    return atan2f(mouse.x, -mouse.y);
};

inline void fly(GameWorld & world, unsigned int entity) {
    auto &pc = world.m_pos_c[entity];
    auto &mc = world.m_move_c[entity];
    auto &fc = world.m_fly_c[entity];

    mc.accel.y -= mc.mass * c_gravity * 0.5f;

    auto air_dir = w_normalize(mc.velocity);
    auto glide_dir = w_rotated_deg(WVec(0, -1), pc.rotation);
    auto angle = w_angle_to_vec(air_dir, glide_dir);

    float vel_mag = w_magnitude(mc.velocity);
    float vel_squared = powf(vel_mag, 2);

    mc.accel -= air_dir * vel_squared * calc_drag(abs(angle), vel_mag);
    mc.accel -= w_tangent(air_dir) * vel_squared * calc_lift(angle, vel_mag, fc);
}

void protagonist::on_ground(GameWorld &world, unsigned int entity) {
    auto &ic = world.m_input_c[entity];
    auto &mc = world.m_move_c[entity];
    auto &gc = world.m_ground_move_c[entity];
    auto &bset = world.m_entities[entity];

    if (bset.test(CInput) and ic.direction[0] and ic.direction[0] != ic.last_dir) {
        ic.last_dir = ic.direction[0];
    }

    if (gc.air_time > c_jump_tolerance) {
        to_falling(mc);
    }

    walk(ic, mc, gc);
    pseudo_friction(mc, gc);

    if (ic.direction[0] == 0) {
        mc.accel.x -= gc.c_stop_friction * mc.velocity.x;
    }

    drag(mc);

    // jumping
    if (ic.jump[0] and std::find(ic.jump.begin(), ic.jump.end(), false) != ic.jump.end()
        && gc.air_time < c_jump_tolerance) {
        // transistion to jump
        to_jumping(world, entity);
    }
}

void ::protagonist::to_flying(GameWorld &world, unsigned int entity) {
    if (!world.m_entities[entity].test(CFly)) {
        return;
    }
    auto &ic = world.m_input_c[entity];
    auto &mc = world.m_move_c[entity];
    auto &pc = world.m_pos_c[entity];

    mc.movestate = MoveState::Flying;
    clear_arr(ic.wings, true);
    clear_arr(ic.jump, true);
    // rotate player in direction of movement
    pc.rotation = w_angle_to_vec(WVec(0, -1), mc.velocity);
}

void ::protagonist::to_falling(MoveComponent &mc) {
    mc.movestate = MoveState::Falling;
}

void ::protagonist::jumping(GameWorld &world, unsigned int entity) {
    auto &ic = world.m_input_c[entity];
    auto &mc = world.m_move_c[entity];
    auto &jc = world.m_jump_c[entity];

    if (mc.movestate == MoveState::Jumping) {
        if (mc.velocity.y > 0) {
            to_falling(mc);
        }
        if (!ic.jump[0]) {
            mc.velocity.y *= 0;
            to_falling(mc);
        }
    }

    // to fly
    if (ic.wings[0] and std::find(ic.wings.begin(), ic.wings.end(), false) != ic.wings.end()) {
        to_flying(world, entity);
    }

    //mc.accel.x += ic.direction[0] * jc.c_accel;
    walk(ic, mc, jc);

    drag(mc);
}

void ::protagonist::to_jumping(GameWorld &world, unsigned int entity) {
    if (!world.m_entities[entity].test(CJump)) {
        return;
    }
    auto &mc = world.m_move_c[entity];
    auto &ic = world.m_input_c[entity];
    auto &jc = world.m_jump_c[entity];

    clear_arr(ic.jump, true);
    mc.movestate = MoveState::Jumping;
    // calc jump speed from jump height
    float jump_speed = sqrt(2.f * c_gravity * jc.c_jump_height);
    mc.velocity.y = -jump_speed;
}

void ::protagonist::to_ground(GameWorld &world, unsigned int entity) {
    if (!world.m_entities[entity].test(CGroundMove)) {
        return;
    }
    auto &mc = world.m_move_c[entity];
    auto &pc = world.m_pos_c[entity];

    mc.movestate = MoveState::OnGround;
    pc.rotation = 0;
}

void ::protagonist::flying(GameWorld &world, unsigned int entity) {
    auto &pc = world.m_pos_c[entity];
    auto &ic = world.m_input_c[entity];
    auto &mc = world.m_move_c[entity];
    auto &fc = world.m_fly_c[entity];

    auto mouse = WVec(glm::inverse(pc.global_transform) * WVec3(ic.mouse[0], 1));
    float mouse_angle = angle_up_from_local_mouse_deg(mouse);
    pc.rotation += copysignf(fmin(fc.c_max_change_angle, abs(mouse_angle)), mouse_angle);
    pc.rotation = std::remainder(pc.rotation, (float)M_PI * 2.f);

    fly(world, entity);

    // to falling
    if (ic.jump[0] and std::find(ic.jump.begin(), ic.jump.end(), false) != ic.jump.end()) {
        clear_arr(ic.jump, true);
        to_falling(mc);
        pc.rotation = 0;
    }
    // to wings
    if (ic.wings[0] and std::find(ic.wings.begin(), ic.wings.end(), false) != ic.wings.end()) {
        to_flying_accel(world, entity);
    }
}

void ::protagonist::to_flying_accel(GameWorld &world, unsigned int entity) {
    if (!world.m_entities[entity].test(CFly)) {
        return;
    }
    auto &mc = world.m_move_c[entity];
    auto &fc = world.m_fly_c[entity];

    mc.movestate = MoveState::FlyingAccel;
    fc.timer = 0;
}

void ::protagonist::flying_accel(GameWorld &world, unsigned int entity) {
    auto &pc = world.m_pos_c[entity];
    auto &ic = world.m_input_c[entity];
    auto &mc = world.m_move_c[entity];
    auto &fc = world.m_fly_c[entity];

    if (!ic.wings[0] or fc.timer >= fc.c_accel_time) {
        mc.movestate = MoveState ::Flying;
        return;
    }
    BCurve curve = {fc.from, fc.ctrl_from, fc.ctrl_to, fc.to};
    float time_frac = curve.eval(fc.timer / fc.c_accel_time).y;

    auto mouse = WVec(glm::inverse(pc.global_transform) * WVec3(ic.mouse[0], 1));
    float mouse_angle = angle_up_from_local_mouse_deg(mouse);
    pc.rotation += copysignf(fmin(fc.c_max_change_angle * time_frac, abs(mouse_angle)), mouse_angle);
    pc.rotation = std::remainder(pc.rotation, (float)M_PI * 2.f);

    fly(world, entity);

    auto glide_dir = w_rotated_deg(WVec(0, -1), pc.rotation);

    mc.accel += glide_dir * fc.c_accel_force * time_frac;
}
