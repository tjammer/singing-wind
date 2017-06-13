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

void Protagonist::handle_inputs(InputComponent &ic, const WVec &mouse) {
    ic.jump.pop_back();
    ic.jump.push_front(WInput::is_key_pressed(GLFW_KEY_SPACE));

    ic.direction.pop_back();
    ic.direction.push_front(WInput::is_key_pressed(GLFW_KEY_D) - WInput::is_key_pressed(GLFW_KEY_A));

    ic.mouse.pop_back();
    ic.mouse.push_front(mouse);

    ic.wings.pop_back();
    ic.wings.push_front(WInput::is_mouse_button_pressed(GLFW_MOUSE_BUTTON_RIGHT));
}

unsigned int Protagonist::create_player(GameWorld &world, const WVec &pos, unsigned int parent) {
    auto player = world.create_entity();
    bset comps;
    for (auto i : {CPosition, CDebugDraw, CMove, CInput, CStaticCol, CGroundMove, CJump, CFly}) {
        comps.set(i);
    }

    world.m_entities[player] = comps;
    world.m_name_c[player] = "player";
    world.m_pos_c[player].position = pos;
    world.m_pos_c[player].parent = parent;
    world.m_pos_c[player].global_transform = glm::rotate(glm::translate(WTransform(), pos),world.m_pos_c[player].rotation);

            //auto shape = std::shared_ptr<ColShape>(new ColCapsule(15, 30));
    world.m_debug_c[player].shape = std::shared_ptr<ColShape>(new ColCapsule(Protagonist::c_capsule_size.x,
                                                                             Protagonist::c_capsule_size.y));

    world.m_move_c[player].movestate = MoveState::OnGround;
    world.m_move_c[player].moveset = MoveSet ::Protagonist;

    world.m_input_c[player].input_func = InputFunc::Protagonist;

    world.m_static_col_c[player].shape = world.m_debug_c[player].shape;
    world.m_static_col_c[player].col_response = StaticColResponse::Actor;

    return player;
}

