//
// Created by jammer on 11/04/17.
//

#include "Protagonist.h"
#include "GameWorld.h"
#include "MoveSystems.h"
#include "entities.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_transform_2d.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <WInput.h>
#include <WVecMath.h>

using namespace Protagonist;

void Protagonist::on_static_collision(const ColResult &result, GameWorld &world, unsigned int entity) {
    auto &mc = world.m_move_c[entity];
    auto &gc = world.m_ground_move_c[entity];

    if (w_dot(WVec(0, 1), result.normal) > c_max_floor_angle) {
        gc.air_time = 0;
        if (!(mc.movestate == MoveState::OnGround)) {
            get_trans_func(MoveTransition::ToGround, MoveSet::Protagonist)(world, entity);
        }
        mc.velocity.y = w_slide(mc.velocity, result.normal).y * 0.5f;
    }
    else {
        mc.velocity = w_slide(mc.velocity, result.normal);
    }

}

void Protagonist::handle_inputs(GameWorld &world, unsigned int entity) {
    auto &ic = world.m_input_c.at(entity);

    push_value(ic.jump, WInput::is_key_pressed(GLFW_KEY_SPACE));

    push_value(ic.direction, (WInput::is_key_pressed(GLFW_KEY_D) - WInput::is_key_pressed(GLFW_KEY_A)));

    push_value(ic.mouse, WInput::get_mouse_pos());

    push_value(ic.wings, WInput::is_mouse_button_pressed(GLFW_MOUSE_BUTTON_RIGHT));
}
