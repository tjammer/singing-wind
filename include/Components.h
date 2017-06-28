//
// Created by jammer on 10/04/17.
//

#ifndef SINGING_WIND_COMPONENTS_H
#define SINGING_WIND_COMPONENTS_H

#include "Collision.h"
#include <memory>
#include <deque>
#include <queue>
#include <map>

class GameWorld;
class ColShape;
struct NavLink;

enum Components : int {
    CPosition,
    CMove,
    CStaticCol,
    CAppearance,
    CInput,
    CDebugDraw,
    CGroundMove,
    CJump,
    CFly,
    CPathing,
    CSimpleFly
};
const std::map<Components, const char*> components_string = {
    {CPosition, "CPosition"},
    {CMove, "CMove"},
    {CStaticCol, "CStaticCol"},
    {CAppearance, "CAppearance"},
    {CInput, "CInput"},
    {CDebugDraw, "CDebugDraw"},
    {CGroundMove, "CGroundMove"},
    {CJump, "CJump"},
    {CFly, "CFly"},
    {CPathing, "CPathing"},
    {CSimpleFly, "CSimpleFly"}
};

struct PosComponent {
    WVec position = {0, 0};
    float rotation = 0.f;
    WTransform global_transform = WTransform();
    unsigned int parent = 0;
};

struct DebugComponent {
    std::shared_ptr<ColShape> shape;
};

// for serializing and identification
using NameComponent = std::string;

using TagComponent = bset;

enum class Tags : int {
    Actor,
    Protagonist,
    Enemy
};

const int c_input_buffer_length = 15;

enum class InputFunc : int {
    Protagonist,
    TestEnemy
};
const std::map<InputFunc, const char*> inputfunc_string = {
    {InputFunc::Protagonist, "Protagonist"},
    {InputFunc::TestEnemy, "TestEnemy"}
};

struct InputComponent {
    std::deque<int> direction = std::deque<int>(c_input_buffer_length);
    int last_dir;
    std::deque<bool> jump = std::deque<bool>(c_input_buffer_length);
    std::deque<WVec> mouse = std::deque<WVec>(c_input_buffer_length);
    std::deque<bool> wings = std::deque<bool>(c_input_buffer_length);
    InputFunc input_func;
};

enum class MoveState : int {
    OnGround,
    Jumping,
    Falling,
    Flying,
    FlyingAccel,
};
const std::map<MoveState, const char*> movestate_string = {
    {MoveState::OnGround, "OnGround"},
    {MoveState::Jumping, "Jumping"},
    {MoveState::Falling, "Falling"},
    {MoveState::Flying, "Flying"},
    {MoveState::FlyingAccel, "FlyingAccel"}
};

enum class MoveTransition : int {
    ToGround
};

enum class MoveSet : int {
    Protagonist,
    TestEnemy
};
const std::map<MoveSet, const char*> moveset_string = {
    {MoveSet::Protagonist, "Protagonist"},
    {MoveSet::TestEnemy, "TestEnemy"}
};


struct MoveComponent {
    WVec velocity = {0, 0};
    WVec accel = {0, 0};
    WVec additional_force = {0, 0};
    MoveState movestate;
    MoveSet moveset;
    float mass = 1;
};

enum class StaticColResponse : int {
    Actor,
    SimpleFlyer
};
const std::map<StaticColResponse, const char*> staticcolresponse_string = {
    {StaticColResponse::Actor, "Actor"},
    {StaticColResponse::SimpleFlyer, "SimpleFlyer"}
};

struct StaticColComponent {
    ColResult col_result;
    std::shared_ptr<ColShape> shape;
    StaticColResponse col_response;
};

struct GroundMoveComponent {
    float c_accel = 1000;
    float c_stop_friction = 8;
    float c_turn_mod = 4;
    float air_time = 0;
    float c_max_vel = 0;
};

struct JumpComponent {
    float c_accel = 500;
    float c_turn_mod = 4;
    float c_jump_height = 1000;
    float c_max_vel = 0;
};

struct FlyComponent {
    float c_lift = 0.0055;
    float c_stall_angle = 0.26;
    float c_max_change_angle = 4;
    float c_accel_force = 1000;
    float c_accel_time = 0.7f;
    float c_drag = 0.0026;
    float c_push_vel = 500.f;
    float timer = 0;

    // bcurve for acceling
    WVec from = {0.000, 0.000};
    WVec ctrl_from = {0.819312513, -0.31962499};
    WVec ctrl_to = {-0.0288125277, 1.40112495};
    WVec to = {1.000, 0.870875001};
};

enum class PathingType : int {
    Platform,
    Jump,
    Fly
};
const std::map<PathingType, const char*> pathingtype_string = {
    {PathingType::Platform, "Platform"},
    {PathingType::Jump, "Jump"},
    {PathingType::Fly, "Fly"}
};

struct PathingComponent {
    PathingType p_type;
    std::deque<NavLink> path;
};

struct SimpleFlyComponent {
    float c_max_vel = 500;
    float c_accel = 1000;
    float c_near_threshold = 10;
    float c_stop_coef = 0.04;
};

#endif //SINGING_WIND_COMPONENTS_H
