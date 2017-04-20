//
// Created by jammer on 11/04/17.
//

#include "Protagonist.h"
#include "GameWorld.h"
#include <iostream>
#include <assert.h>

void drag(MoveComponent &mc) {
    mc.accel.x -= copysignf(mc.velocity.x * mc.velocity.x * c_drag, mc.velocity.x);
    mc.accel.y -= copysignf(mc.velocity.y * mc.velocity.y * c_drag, mc.velocity.y);
}

void on_static_collision(const ColResult &result, GameWorld &world, unsigned int entity) {
    auto &mc = world.m_move_c[entity];

    if (dot(WVec(0, 1), result.normal) > c_max_floor_angle) {
        mc.air_time = 0;
        if (!(mc.move_state->get_state() == MoveStateName::OnGround)) {
            mc.move_state = MoveState(new OnGround);
        }
        mc.velocity.y = slide(mc.velocity, result.normal).y * 0.5f;
    }
    else {
        mc.velocity = slide(mc.velocity, result.normal);
    }

}

void falling(InputComponent &ic, MoveComponent &mc) {
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

    auto shape = std::shared_ptr<ColShape>(new ColCapsule(25, 50));
    world.m_debug_c[player].shape = shape;

    world.m_move_c[player].move_state = MoveState(new InFalling);

    world.m_input_c[player].input_func = handle_inputs;

    world.m_static_col_c[player].shape = shape;
    world.m_static_col_c[player].on_static_col_cb = on_static_collision;

    return player;
}

void OnGround::accel(InputComponent &ic, MoveComponent &mc) {
    if (ic.direction[0] and ic.direction[0] != ic.last_dir) {
        ic.last_dir = ic.direction[0];
    }

    if (mc.air_time > c_jump_tolerance) {
        mc.move_state = MoveState(new InFalling);
    }

    float mod = 1;
    if (ic.direction[0] * mc.velocity.x < 0) {
        mod *= c_turn_mod;
    }
    mc.accel.x += ic.direction[0] * mod * c_walk_accel;

    if (ic.direction[0] == 0) {
        mc.accel.x -= c_stop_friction * mc.velocity.x;
    }

    drag(mc);

    // jumping
    if (ic.jump[0] and std::find(ic.jump.begin(), ic.jump.end(), false) != ic.jump.end()
        && mc.air_time < c_jump_tolerance) {
        // transistion to jump
        mc.move_state = MoveState(new InJump(ic, mc));

    }
}

void InJump::accel(InputComponent &ic, MoveComponent &mc) {
    if (mc.velocity.y < 0) {
        mc.move_state = MoveState(new InFalling);
    }

    float mod = 1;
    if (ic.direction[0] * mc.velocity.x < 0) {
        mod *= c_turn_mod;
    }
    mc.accel.x += ic.direction[0] * mod * c_walk_accel;

    drag(mc);
}

InJump::InJump(InputComponent &ic, MoveComponent &mc) {
    clear_arr(ic.jump, true);
    m_move_state = MoveStateName ::Jumping;
    mc.velocity.y = -c_jump_speed;

}

void InFalling::accel(InputComponent &ic, MoveComponent &mc) {
    float mod = 1;
    if (ic.direction[0] * mc.velocity.x < 0) {
        mod *= c_turn_mod;
    }
    mc.accel.x += ic.direction[0] * mod * c_walk_accel;

    drag(mc);
}
