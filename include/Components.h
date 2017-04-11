//
// Created by jammer on 10/04/17.
//

#ifndef SINGING_WIND_COMPONENTS_H
#define SINGING_WIND_COMPONENTS_H

#include <WindDefs.h>
#include "ColShape.h"
#include <memory>
#include "Collision.h"

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
    std::vector<int> direction = std::vector<int>(c_input_buffer_length);
    int last_dir;
    std::vector<bool> jump = std::vector<bool>(c_input_buffer_length);
    std::vector<WVec> mouse = std::vector<WVec>(c_input_buffer_length);
    std::function<void()> handle_inputs;
};

enum MoveState {
    OnGround,
    Jumping,
    Falling,
    Flying
};

struct MoveComponent {
    WVec velocity = {0, 0};
    WVec accel = {0, 0};
    WVec additional_accel = {0, 0};
    float air_time = 0;
    MoveState move_state = OnGround;
    // different function depending on player state
    std::function<void(InputComponent &ic, MoveComponent &mc)> accel_func = nullptr;
};

using MovementFct = std::function<void(InputComponent &ic, MoveComponent &mc)>;


struct StaticColComponent {
    ColResult col_result;
    std::shared_ptr<ColShape> shape;
    std::function<void(const ColResult &, GameWorld &, unsigned int)> on_static_col_cb = nullptr;
};


#endif //SINGING_WIND_COMPONENTS_H
