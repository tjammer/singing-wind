//
// Created by jammer on 11/04/17.
//

#include "Protagonist.h"
#include "GameWorld.h"
#include "BCurve.h"
#include "FrameTimer.h"
#include <assert.h>

const float c_half_pi = (float)M_PI / 2.f;

inline void drag(MoveComponent &mc) {
    mc.accel.x -= copysignf(mc.velocity.x * mc.velocity.x * c_drag, mc.velocity.x);
    mc.accel.y -= copysignf(mc.velocity.y * mc.velocity.y * c_drag, mc.velocity.y);
}

inline void friction(MoveComponent &mc) {
    mc.accel.x -= copysignf(mc.velocity.x * mc.velocity.x * c_friction, mc.velocity.x);
}

inline void walk(InputComponent &ic, MoveComponent &mc) {
    float mod = 1;
    if (ic.direction[0] * mc.velocity.x < 0) {
        mod *= c_turn_mod;
    }
    mc.accel.x += ic.direction[0] * mod * c_walk_accel;
}

const float c_velocity_scaling = 1200;

inline float calc_drag(float angle, float vel_mag) {
    auto drag_coeff = c_drag * exp(vel_mag / c_velocity_scaling);
    return drag_coeff * expf(-powf(angle - c_half_pi, 2.f) * 1.5f);
}

inline float calc_lift(float angle, float vel_mag) {
    auto lift_coeff = c_lift * exp(-vel_mag / c_velocity_scaling / 10);
    if (angle < 0) {
        return -calc_lift(-angle, vel_mag);
    }
    assert(angle >= 0);
    if (angle < c_stall_angle || abs(fmod(angle + (float)M_PI, (float)M_PI)) < c_stall_angle) {
        return lift_coeff * sin(angle * c_half_pi / c_stall_angle);
    }
    return lift_coeff * cos((angle - c_stall_angle) * c_half_pi / (c_half_pi - c_stall_angle));
}

// get angle from mouse to player in degrees (for smfl), zero is up
inline float angle_up_from_local_mouse_deg(const WVec &mouse) {
    return atan2f(mouse.x, -mouse.y) * 180.f / (float)M_PI;
};

inline void fly(InputComponent &ic, MoveComponent &mc, PosComponent &pc) {
    auto mouse = pc.global_transform.getInverse().transformPoint(ic.mouse[0]);
    float mouse_angle = angle_up_from_local_mouse_deg(mouse);
    pc.rotation += copysignf(fmin(c_max_change_angle, abs(mouse_angle)), mouse_angle);
    pc.rotation = std::remainder(pc.rotation, 360.f);

    auto air_dir = w_normalize(mc.velocity);
    auto glide_dir = w_rotated_deg(WVec(0, -1), pc.rotation);
    auto angle = w_angle_to_vec(air_dir, glide_dir);

    float vel_mag = w_magnitude(mc.velocity);
    float vel_squared = powf(vel_mag, 2);

    mc.accel -= air_dir * vel_squared * calc_drag(abs(angle), vel_mag);
    mc.accel -= w_tangent(air_dir) * vel_squared * calc_lift(angle, vel_mag);
}

void on_static_collision(const ColResult &result, GameWorld &world, unsigned int entity) {
    auto &mc = world.m_move_c[entity];
    auto &pc = world.m_pos_c[entity];

    if (w_dot(WVec(0, 1), result.normal) > c_max_floor_angle) {
        mc.air_time = 0;
        if (!(mc.move_state->get_state() == MoveStateName::OnGround)) {
            mc.move_state = MoveState(new MOnGround);
            pc.rotation = 0;
        }
        mc.velocity.y = w_slide(mc.velocity, result.normal).y * 0.5f;
    }
    else {
        mc.velocity = w_slide(mc.velocity, result.normal);
    }

}

void handle_inputs(InputComponent &ic, const WVec &mouse) {
    using namespace sf;

    ic.jump.pop_back();
    ic.jump.push_front(Keyboard::isKeyPressed(Keyboard::Space));

    ic.direction.pop_back();
    ic.direction.push_front(Keyboard::isKeyPressed(Keyboard::D) - Keyboard::isKeyPressed(Keyboard::A));

    ic.mouse.pop_back();
    ic.mouse.push_front(mouse);
}

unsigned int create_player(GameWorld &world, const WVec &pos, unsigned int parent) {
    auto player = world.create_entity();
    bset comps;
    for (auto i : {CPosition, CDebugDraw, CMove, CInput, CStaticCol}) {
        comps.set(i);
    }

    world.m_entities[player] = comps;
    world.m_id_c[player] = "player";
    world.m_pos_c[player].position = pos;
    world.m_pos_c[player].parent = parent;
    WTransform trans;
    world.m_pos_c[player].global_transform = trans.translate(pos);

    auto shape = std::shared_ptr<ColShape>(new ColCapsule(15, 30));
    world.m_debug_c[player].shape = shape;

    world.m_move_c[player].move_state = MoveState(new MFalling);

    world.m_input_c[player].input_func = handle_inputs;

    world.m_static_col_c[player].shape = shape;
    world.m_static_col_c[player].on_static_col_cb = on_static_collision;

    return player;
}

void MOnGround::accel(GameWorld &world, unsigned int entity) {
    auto &ic = world.m_input_c[entity];
    auto &mc = world.m_move_c[entity];

    if (ic.direction[0] and ic.direction[0] != ic.last_dir) {
        ic.last_dir = ic.direction[0];
    }

    if (mc.air_time > c_jump_tolerance) {
        mc.move_state = MoveState(new MFalling);
    }

    walk(ic, mc);

    if (ic.direction[0] == 0) {
        mc.accel.x -= c_stop_friction * mc.velocity.x;
    }

    drag(mc);
    friction(mc);

    // jumping
    if (ic.jump[0] and std::find(ic.jump.begin(), ic.jump.end(), false) != ic.jump.end()
        && mc.air_time < c_jump_tolerance) {
        // transistion to jump
        mc.move_state = MoveState(new MJumping(ic, mc));
    }
}

void MJumping::accel(GameWorld &world, unsigned int entity) {
    auto &ic = world.m_input_c[entity];
    auto &mc = world.m_move_c[entity];

    if (mc.velocity.y < 0) {
        mc.move_state = MoveState(new MFalling);
    }

    // to fly
    if (ic.jump[0] and std::find(ic.jump.begin(), ic.jump.end(), false) != ic.jump.end()) {
        mc.move_state = MoveState(new MFlying(world, entity, true));
    }

    mc.accel.x += ic.direction[0] * c_air_accel_mod * c_walk_accel;

    drag(mc);
}

MJumping::MJumping(InputComponent &ic, MoveComponent &mc) {
    clear_arr(ic.jump, true);
    m_move_state = MoveStateName ::Jumping;
    mc.velocity.y = -c_jump_speed;

}

void MFalling::accel(GameWorld &world, unsigned int entity) {
    auto &ic = world.m_input_c[entity];
    auto &mc = world.m_move_c[entity];

    // to fly
    if (ic.jump[0] and std::find(ic.jump.begin(), ic.jump.end(), false) != ic.jump.end()) {
        mc.move_state = MoveState(new MFlying(world, entity, true));
    }

    mc.accel.x += ic.direction[0] * c_air_accel_mod * c_walk_accel;

    drag(mc);
}

void MFlying::accel(GameWorld &world, unsigned int entity) {
    auto &pc = world.m_pos_c[entity];
    auto &ic = world.m_input_c[entity];
    auto &mc = world.m_move_c[entity];

    fly(ic, mc, pc);

    // to falling
    if (ic.jump[0] and std::find(ic.jump.begin(), ic.jump.end(), false) != ic.jump.end()) {
        clear_arr(ic.jump, true);
        mc.move_state = MoveState(new MFlyingAccel);
        //pc.rotation = 0;
    }
}

MFlying::MFlying(GameWorld &world, unsigned int entity, bool set_rotation) {
    auto &ic = world.m_input_c[entity];
    auto &mc = world.m_move_c[entity];
    auto &pc = world.m_pos_c[entity];

    m_move_state = MoveStateName ::Flying;
    clear_arr(ic.jump, true);
    // rotate player in direction of movement
    if (set_rotation) {
        pc.rotation = w_angle_to_vec(WVec(0, -1), mc.velocity) * 180.f / (float)M_PI;
    }
}

void MFlyingAccel::accel(GameWorld &world, unsigned int entity) {
    auto &pc = world.m_pos_c[entity];
    auto &ic = world.m_input_c[entity];
    auto &mc = world.m_move_c[entity];

    if (!ic.jump[0]) {
        mc.move_state = MoveState(new MFlying(world, entity, false));
    }

    fly(ic, mc, pc);

    BCurve curve = {from, ctrl_from, ctrl_to, to};
    auto glide_dir = w_rotated_deg(WVec(0, -1), pc.rotation);
    auto accel_dir = (4.f * copysignf(1, pc.rotation) * w_tangent(glide_dir) + glide_dir) / 5.f;
    mc.accel += accel_dir * c_fly_accel_force * curve.eval(m_timer / c_fly_accel_time).y;

    m_timer += c_fixed_timestep;
    if (m_timer >= c_fly_accel_time) {
        mc.move_state = MoveState(new MFlying(world, entity, false));
    }
}
