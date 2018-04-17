#ifndef COMPS_H
#define COMPS_H

#include "wind_defs.h"

namespace ecs {
template<typename T>
struct tag;
}

struct Position
{
  WVec position{ 0, 0 };
  float rotation{ 0.f };
  WVec global_position{ 0, 0 };
  WTransform global_transform{ WTransform{} };
  int direction{ 1 };
};

enum class MoveState
{
  Flying,
};

struct Movement
{
  WVec velocity{ 0, 0 };
  WVec accel{ 0, 0 };
  WVec next_accel{ 0, 0 };
  float transition_time{ 5 };
  float change_angle{ 0 };

  MoveState active_state{ MoveState::Flying };

  float mass{ 1 };
  float time_fac = { 1 };
  float timer{ 0 };
  float max_change_angle{ 0.065 };
};

enum class KeyState
{
  JustPressed,
  Hold,
  Release,
};

struct Input
{
  WVec mouse{ 0, 0 };
  KeyState wings{ KeyState::Release };
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

#endif /* COMPS_H */
