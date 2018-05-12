#ifndef COMPS_H
#define COMPS_H

#include "w_vec.h"
#include "transform.h"
#include "gjk.h"
#include <memory>

namespace ecs {
template<typename T>
struct tag;
}

enum class MoveState
{
  Fall,
  Walk
};

struct MoveStateChange
{
  MoveState next;
};

using IsFalling = ecs::tag<struct is_falling>;
using IsWalking = ecs::tag<struct is_walking>;

struct Movement
{
  WVec velocity{ 0, 0 };
  WVec accel{ 0, 0 };
  WVec next_accel{ 0, 0 };
  float transition_time{ 5 };
  float change_angle{ 0 };

  MoveState active_state{ MoveState::Fall };

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
  KeyState left{ KeyState::Release };
  KeyState right{ KeyState::Release };
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

struct Collision
{
  ColResult result;
  std::unique_ptr<ConvexShape> shape;
};

using HasCollided = ecs::tag<struct has_collided>;

#endif /* COMPS_H */
