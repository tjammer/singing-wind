//
// Created by jammer on 11/04/17.
//

#include "Protagonist.h"
#include "BCurve.h"
#include "CollisionComponent.h"
#include "Components.h"
#include "GameWorld.h"
#include "InputComponent.h"
#include "MoveSystems.h"
#include "PosComponent.h"
#include <WInput.h>
#include <WVecMath.h>
#include <algorithm>

#include <iostream>
using namespace protagonist;

void
protagonist::handle_inputs(GameWorld& world, unsigned int entity)
{
  auto& ic = world.get<InputComponent>(entity);

  ic.jump.push(WInput::is_key_pressed(GLFW_KEY_SPACE));
  ic.direction.push(WInput::is_key_pressed(GLFW_KEY_D) -
                    WInput::is_key_pressed(GLFW_KEY_A));
  ic.mouse.push(WInput::get_mouse_pos());
  ic.wings.push(WInput::is_mouse_button_pressed(GLFW_MOUSE_BUTTON_RIGHT));
  ic.attacks[0].push(WInput::is_key_pressed(GLFW_KEY_Q));
}

template<class GC>
inline void
walk(InputComponent& ic, MoveComponent& mc, GC& gc)
{
  float mod = 1;
  float accel = gc.c_accel;
  if (ic.direction.get() * mc.velocity.x < 0) {
    mod *= gc.c_turn_mod;
  } else {
    // calc diminishing accel
    float vel = fmin(gc.c_max_vel, abs(mc.velocity.x));
    accel *= (1.f - exp(-pow(vel - gc.c_max_vel, 2.f) * 0.1f / gc.c_max_vel));
  }
  mc.accel.x += ic.direction.get() * mod * accel;
}

inline void
drag_x(MoveComponent& mc)
{
  mc.accel.x -=
    copysignf(mc.velocity.x * mc.velocity.x * c_drag * 0.01f, mc.velocity.x);
}

template<class GC>
inline void
pseudo_friction(MoveComponent& mc, GC& gc)
{
  if (abs(mc.velocity.x) > gc.c_max_vel) {
    mc.accel.x -= copysignf(
      gc.c_accel * (1.f - abs(mc.velocity.x) / gc.c_max_vel), mc.velocity.x);
  }
}

const float c_half_pi = (float)M_PI / 2.f;

const float c_velocity_scaling = 900;

inline float
calc_drag(float angle, float vel_mag, float drag)
{
  auto drag_coeff = drag * exp(vel_mag / c_velocity_scaling);
  return drag_coeff * expf(-powf(angle - c_half_pi, 2.f) * 1.4f);
}

inline float
calc_lift(float angle, float vel_mag, const FlyComponent& fc)
{
  if (angle < 0) {
    return -calc_lift(-angle, vel_mag, fc);
  }
  assert(angle >= 0);
  // damps lift for high angles to make going backwards difficult
  float lift = fc.c_lift / expf(angle / (float)M_PI);
  return lift * cos((angle - fc.c_stall_angle) * c_half_pi /
                    (c_half_pi - fc.c_stall_angle));
}

// get angle from mouse to player in degrees (for smfl), zero is up
inline float
angle_up_from_local_mouse_deg(const WVec& mouse)
{
  return atan2f(mouse.x, -mouse.y);
};

const WVec c_accel_angle_coords[4] = { { 0, 1 }, { 0, 0 }, { 1, 1 }, { 1, 0 } };

const BCurve c_accel_angle_curve = { c_accel_angle_coords[0],
                                     c_accel_angle_coords[1],
                                     c_accel_angle_coords[2],
                                     c_accel_angle_coords[3] };

inline WVec
calc_accel_angle(const PosComponent& pc)
{
  float fac = pc.rotation / (float)M_PI;
  float additional_rot = c_accel_angle_curve.eval(fac).y * (float)M_PI / 2.f;
  return w_rotated(WVec{ 0, -1 },
                   (pc.rotation - additional_rot) * pc.direction);
}

inline void
fly(MoveComponent& mc,
    float angle,
    const FlyComponent& fc,
    float strength = 1.0f)
{
  auto air_dir = w_normalize(mc.velocity);

  float vel_mag = w_magnitude(mc.velocity);
  float vel_squared = powf(vel_mag, 2);

  // use mass to determine static lift. for now dont take volume into
  // account, only flat mass
  mc.accel.y -= c_gravity * (1 - mc.mass);

  mc.accel -= air_dir * vel_squared *
              calc_drag(abs(angle), vel_mag, fc.c_drag) * strength;
  mc.accel -=
    w_tangent(air_dir) * vel_squared * calc_lift(angle, vel_mag, fc) * strength;
}

void
FallingMove::enter(GameWorld& world, unsigned int entity)
{
}

void
FallingMove::accel(GameWorld& world, unsigned int entity)
{
  auto& ic = world.get<InputComponent>(entity);
  auto& mc = world.get<MoveComponent>(entity);
  auto& fc = world.get<FallComponent>(entity);
  auto& pc = world.get<PosComponent>(entity);

  walk(ic, mc, fc);

  drag_x(mc);

  rotate_angle(-pc.rotation, mc.c_max_change_angle / 2.0f, pc);
}

MoveStateName
FallingMove::name()
{
  return MoveStateName::Falling;
}
bool
FallingMove::transition(GameWorld& world, unsigned int entity)
{
  const auto& mc = world.get<MoveComponent>(entity);
  auto& ic = world.get<InputComponent>(entity);

  if (mc.movestate->name() == MoveStateName::OnGround and
      mc.timer > c_jump_tolerance) {
    return true;
  } else if (mc.movestate->name() == MoveStateName::Flying) {
    if (ic.jump.just_added(true)) {
      return true;
    }
  }
  return false;
}

void
GroundMove::enter(GameWorld& world, unsigned int entity)
{
  auto& pc = world.get<PosComponent>(entity);
  pc.rotation = 0;
}

void
GroundMove::accel(GameWorld& world, unsigned int entity)
{
  auto& ic = world.get<InputComponent>(entity);
  auto& mc = world.get<MoveComponent>(entity);
  auto& gc = world.get<GroundMoveComponent>(entity);
  auto& pc = world.get<PosComponent>(entity);

  walk(ic, mc, gc);
  pseudo_friction(mc, gc);

  if (ic.direction.get() == 0) {
    mc.accel.x -= gc.c_stop_friction * mc.velocity.x;
  } else {
    pc.direction = ic.direction.get();
  }

  drag_x(mc);
}

MoveStateName
GroundMove::name()
{
  return MoveStateName::OnGround;
}

bool
GroundMove::transition(GameWorld& world, unsigned int entity)
{
  if (!world.entities()[entity].test(CGroundMove)) {
    return false;
  }

  const auto& result = world.get<StaticColComponent>(entity).col_result;
  if (w_dot(WVec(0, 1), result.normal) > c_max_floor_angle) {
    return true;
  }
  return false;
}

void
FlyingMove::enter(GameWorld& world, unsigned int entity)
{
  auto& ic = world.get<InputComponent>(entity);
  auto& mc = world.get<MoveComponent>(entity);
  auto& pc = world.get<PosComponent>(entity);

  ic.wings.fill(true);
  // if coming from ground push
  if (mc.movestate->name() == MoveStateName::OnGround) {
    mc.velocity.y = -world.get<FlyComponent>(entity).c_push_vel;
    pc.rotation = (float)M_PI / 4.f;
  }
}

void
FlyingMove::accel(GameWorld& world, unsigned int entity)
{
  auto& pc = world.get<PosComponent>(entity);
  auto& ic = world.get<InputComponent>(entity);
  auto& mc = world.get<MoveComponent>(entity);
  auto& fc = world.get<FlyComponent>(entity);

  rotate_to(ic.mouse.get(), mc.c_max_change_angle, pc);

  auto glide_dir = w_rotated(WVec(0, -1), pc.rotation * pc.direction);
  auto angle = w_angle_to_vec(mc.velocity, glide_dir);
  fly(mc, angle, fc);
}

MoveStateName
FlyingMove::name()
{
  return MoveStateName::Flying;
}

bool
FlyingMove::transition(GameWorld& world, unsigned int entity)
{
  if (!world.entities()[entity].test(CFly)) {
    return false;
  }
  const auto& ic = world.get<InputComponent>(entity);
  if (ic.wings.just_added(true)) {
    return true;
  }
  return false;
}

void
FlyingAccelMove::enter(GameWorld& world, unsigned int entity)
{
  auto& mc = world.get<MoveComponent>(entity);
  auto& ic = world.get<InputComponent>(entity);

  ic.wings.fill(true);
  mc.timer = 0;
  mc.special_movestate->timer = world.get<FlyComponent>(entity).c_accel_time;
}

void
FlyingAccelMove::accel(GameWorld& world, unsigned int entity)
{
  auto& pc = world.get<PosComponent>(entity);
  auto& ic = world.get<InputComponent>(entity);
  auto& mc = world.get<MoveComponent>(entity);
  auto& fc = world.get<FlyComponent>(entity);

  BCurve curve = { fc.from, fc.ctrl_from, fc.ctrl_to, fc.to };
  float time_frac = curve.eval(mc.timer / fc.c_accel_time).y;

  rotate_to(ic.mouse.get(), mc.c_max_change_angle, pc);

  auto glide_dir = w_rotated(WVec(0, -1), pc.rotation * pc.direction);
  auto angle = w_angle_to_vec(mc.velocity, glide_dir);
  fly(mc, angle, fc, 0.75f);

  mc.accel += calc_accel_angle(pc) * fc.c_accel_force * time_frac;
}

bool
FlyingAccelMove::transition(GameWorld& world, unsigned int entity)
{
  if (!world.entities()[entity].test(CFly)) {
    return false;
  }
  const auto& ic = world.get<InputComponent>(entity);
  if (ic.wings.just_added(true)) {
    return true;
  }
  return false;
}

std::unique_ptr<MoveState>
ProtagonistMoveSet::transition(GameWorld& world, unsigned int entity)
{
  using namespace std;
  auto& mc = world.get<MoveComponent>(entity);

  if (mc.movestate == nullptr) {
    return from_undefined(world, entity);
  }

  switch (mc.movestate->name()) {
    case MoveStateName::OnGround: {
      if (FallingMove::transition(world, entity)) {
        return make_unique<FallingMove>();
      }
      if (FlyingMove::transition(world, entity)) {
        return make_unique<FlyingMove>();
      }
      break;
    }
    case MoveStateName::Falling: {
      if (GroundMove::transition(world, entity)) {
        return make_unique<GroundMove>();
      }
      if (FlyingMove::transition(world, entity)) {
        return make_unique<FlyingMove>();
      }
      break;
    }
    case MoveStateName::Flying: {
      if (FlyingAccelMove::transition(world, entity)) {
        mc.special_movestate = make_unique<FlyingAccelMove>();
        mc.special_movestate->enter(world, entity);
        return make_unique<FlyingMove>();
      }
      if (GroundMove::transition(world, entity)) {
        return make_unique<GroundMove>();
      }
      if (FallingMove::transition(world, entity)) {
        return make_unique<FallingMove>();
      }
      break;
    }
    default: {
      return from_undefined(world, entity);
      break;
    }
  }
  return nullptr;
}

void
ProtagonistMoveSet::init(GameWorld& world, unsigned int entity)
{
  auto& mc = world.get<MoveComponent>(entity);
  mc.movestate = from_undefined(world, entity);
  mc.special_movestate = nullptr;
}

std::unique_ptr<MoveState>
ProtagonistMoveSet::from_undefined(GameWorld&, unsigned int)
{
  return std::make_unique<FallingMove>();
}
