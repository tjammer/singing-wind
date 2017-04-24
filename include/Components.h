//
// Created by jammer on 10/04/17.
//

#ifndef SINGING_WIND_COMPONENTS_H
#define SINGING_WIND_COMPONENTS_H

#include <WindDefs.h>
#include "ColShape.h"
#include <memory>
#include "Collision.h"
#include <deque>

class GameWorld;

enum Components : int {
    CPosition,
    CMove,
    CStaticCol,
    CAppearance,
    CInput,
    CDebugDraw
};

struct PosComponent {
    WVec position = {0, 0};
    float rotation = 0.f;
    WTransform global_transform = WTransform::Identity;
    unsigned int parent = 0;
};

struct DebugComponent {
    std::shared_ptr<ColShape> shape;
};

// for serializing and identification
using IdComponent = std::string;

const int c_input_buffer_length = 15;

struct InputComponent {
    std::deque<int> direction = std::deque<int>(c_input_buffer_length);
    int last_dir;
    std::deque<bool> jump = std::deque<bool>(c_input_buffer_length);
    std::deque<WVec> mouse = std::deque<WVec>(c_input_buffer_length);
    std::function<void(InputComponent&, const WVec&)> input_func;
};

enum class MoveStateName {
    OnGround,
    Jumping,
    Falling,
    Flying
};

// the actual state
struct BaseMovementState;
using MoveState = std::unique_ptr<BaseMovementState>;

struct MoveComponent {
    WVec velocity = {0, 0};
    WVec accel = {0, 0};
    WVec additional_accel = {0, 0};
    float air_time = 0;
    std::unique_ptr<BaseMovementState> move_state = nullptr;
};

struct BaseMovementState {
    MoveStateName m_move_state = MoveStateName::OnGround;
    virtual void accel(GameWorld &world, unsigned int entity) = 0;
    virtual MoveStateName get_state() const {return m_move_state;}
};

struct StaticColComponent {
    ColResult col_result;
    std::shared_ptr<ColShape> shape;
    std::function<void(const ColResult &, GameWorld &, unsigned int)> on_static_col_cb = nullptr;
};


#endif //SINGING_WIND_COMPONENTS_H
