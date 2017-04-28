//
// Created by jammer on 10/04/17.
//

#ifndef SINGING_WIND_COMPONENTS_H
#define SINGING_WIND_COMPONENTS_H

#include <memory>
#include "Collision.h"
#include <deque>
#include <unordered_map>

class GameWorld;

enum Components : int {
    CPosition,
    CMove,
    CStaticCol,
    CAppearance,
    CInput,
    CDebugDraw,
    CGroundMove,
    CJump,
    CFly
};

const std::unordered_map<Components, std::string> component_names = {
    {CPosition, "Position"},
    {CMove, "Move"},
    {CStaticCol, "StaticCollision"},
    {CAppearance, "Appearance"},
    {CInput, "Input"},
    {CDebugDraw, "DebugDraw"},
    {CGroundMove, "GroundMove"},
    {CJump, "Jump"},
    {CFly, "Fly"}
};

struct PosComponent {
    WVec position = {0, 0};
    float rotation = 0.f;
    WTransform global_transform = WTransform::Identity;
    unsigned int parent = 0;
};

enum class SpecificShape : int {
    ProtagonistCapsule
};

struct DebugComponent {
    std::shared_ptr<ColShape> shape;
    SpecificShape shape_type;
};

// for serializing and identification
using IdComponent = std::string;

const int c_input_buffer_length = 15;

struct InputComponent {
    std::deque<int> direction = std::deque<int>(c_input_buffer_length);
    int last_dir;
    std::deque<bool> jump = std::deque<bool>(c_input_buffer_length);
    std::deque<WVec> mouse = std::deque<WVec>(c_input_buffer_length);
    std::deque<bool> wings = std::deque<bool>(c_input_buffer_length);
    std::function<void(InputComponent&, const WVec&)> input_func;
};

enum class MoveState : int {
    OnGround,
    Jumping,
    Falling,
    Flying,
    FlyingAccel,
};

enum class MoveTransition : int {
    ToGround
};

const std::unordered_map<MoveState, std::string> movestate_names {
        {MoveState::OnGround, "OnGround"},
        {MoveState::Jumping, "Jumping"},
        {MoveState::Falling, "Falling"},
        {MoveState::Flying, "Flying"},
        {MoveState::FlyingAccel, "FlyingAccel"}
};

enum class MoveSet : int {
    Protagonist
};


const std::unordered_map<MoveSet, std::string> moveset_names {
        {MoveSet::Protagonist, "Protagonist"}
};


struct MoveComponent {
    WVec velocity = {0, 0};
    WVec accel = {0, 0};
    WVec additional_accel = {0, 0};
    MoveState movestate;
    MoveSet moveset;
};

enum class StaticColResponse : int {
    Actor
};

const std::unordered_map<StaticColResponse , std::string> static_col_response_names {
        {StaticColResponse::Actor, "Actor"}
};

struct StaticColComponent {
    ColResult col_result;
    std::shared_ptr<ColShape> shape;
    StaticColResponse col_response;
    SpecificShape shape_type;
};

struct GroundMoveComponent {
    float c_accel = 800;
    float c_stop_friction = 8;
    float c_turn_mod = 4;
    float air_time = 0;
};

struct JumpComponent {
    float c_accel = 400;
    float c_jump_tolerance = 0.1;
    float c_turn_mod = 4;
    float c_jump_speed = 800;
};

struct FlyComponent {
    float c_lift = 0.0055;
    float c_stall_angle = 0.26;
    float c_max_change_angle = 6;
    float c_fly_accel_force = 3000;
    float c_fly_accel_time = 0.5f;

    WVec from = {0.000, 0.000};
    WVec ctrl_from = {0.249, 2.071};
    WVec ctrl_to = {0.729, 0.323};
    WVec to = {1.000, 0.074};

    float timer = 0;
};


#endif //SINGING_WIND_COMPONENTS_H
