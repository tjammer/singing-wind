//
// Created by jammer on 11/04/17.
//

#include "Protagonist.h"
#include "GameWorld.h"

void drag(MoveComponent &mc) {
    mc.accel.x -= copysignf(mc.velocity.x * mc.velocity.x * c_drag, mc.velocity.x);
    mc.accel.y -= copysignf(mc.velocity.y * mc.velocity.y * c_drag, mc.velocity.y);
}

void on_ground(InputComponent &ic, MoveComponent &mc) {
    if (ic.direction[0] and ic.direction[0] != ic.last_dir) {
        ic.last_dir = ic.direction[0];
    }

    if (mc.air_time > c_jump_tolerance) {
        enter_falling(ic, mc);
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
        mc.accel_func = enter_jump(ic, mc);

    }
}
float jumping;

MovementFct enter_jump(InputComponent &ic, MoveComponent &mc) {
    jumping = 0.016;
    clear_arr(ic.jump, true);
    mc.move_state = Jumping;
    return jump;
}

MovementFct enter_falling(InputComponent &ic, MoveComponent &mc) {
    mc.move_state = Falling;
    return falling;
}

void jump(InputComponent &ic, MoveComponent &mc) {
    drag(mc);
    if (mc.velocity.y < 0) {
        mc.accel_func = enter_falling(ic, mc);
    }
}

void on_static_collision(const ColResult &result, GameWorld &world, unsigned int entity) {
    auto &mc = world.m_move_c[entity];
    auto &ic = world.m_input_c[entity];
    if (dot(WVec(0, -1), result.normal) > c_max_floor_angle) {
        mc.air_time = 0;
        if (!mc.move_state == OnGround) {
            mc.accel_func = enter_ground(ic, mc);
        }
        mc.velocity.y = slide(mc.velocity, result.normal).y * 0.5f;
    }
    else {
        mc.velocity = slide(mc.velocity, result.normal);
    }

}

MovementFct enter_ground(InputComponent &ic, MoveComponent &mc) {
    mc.move_state = OnGround;
    return on_ground;
}

void falling(InputComponent &ic, MoveComponent &mc) {
    drag(mc);
}
