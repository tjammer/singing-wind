#ifndef COMPS_H
#define COMPS_H

#include "w_vec.h"
#include "transform.h"
#include "gjk.h"
#include "input_buffer.h"
#include <memory>

namespace ecs {
template<typename T>
struct tag;
}

enum class MoveState
{
  Fall,
  Run,
  Jump
};

using IsFalling = ecs::tag<struct is_falling>;
using IsWalking = ecs::tag<struct is_walking>;

struct Movement
{
  WVec velocity{ 0, 0 };
  WVec accel{ 0, 0 };
  WVec next_accel{ 0, 0 };
  float timer{ 0 };
  float change_angle{ 0 };

  MoveState active_state{ MoveState::Fall };
  float max_change_angle{ 0.065 };
};

enum class KeyState
{
  Release,
  Press
};

struct Input
{
  WVec mouse{ 0, 0 };
  InputBuffer<KeyState> left_click{};
  InputBuffer<KeyState> left{};
  InputBuffer<KeyState> right{};
  InputBuffer<KeyState> jump{};
};

using CanFly = ecs::tag<struct can_fly>;

struct Flying
{
  float c_lift{ 0.0055 };
  float c_stall_angle{ 0.26 };
  float c_accel_force{ 1000 };
  float c_accel_time{ 0.7f };
  float c_drag{ 0.0026 };
  float c_push_vel{ 500.f };
};

struct JumpRun
{
  float accel{ 0 };
};

struct Collision
{
  ColResult result;
  std::unique_ptr<ConvexShape> shape;
};

using HasCollided = ecs::tag<struct has_collided>;

#endif /* COMPS_H */
