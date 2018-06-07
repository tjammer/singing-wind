#include "jumprun.h"
#include "comps.h"
#include "ecs.hpp"
#include "vec_math.h"
#include "imgui.h"

void
jump_run_update(Movement& mc, Transform& t, const Input& ic)
{
  // https://stackoverflow.com/questions/667034/simple-physics-based-movement
  // stokes
  // v_max = a / f
  // quadratic
  // v_max = sqrt(a/f)

  // from old godot project
  constexpr float GRAVITY = 2500;
  const float STOP_FRICTION = 8;
  constexpr float GROUND_DRAG = 0.02; // not actually godot
  constexpr float AIR_DRAG = 0.005;
  const float TURN_MUL = 2;

  // velocities
  const float JUMP_SPEED = 800;
  const float MAX_WALK_VEL = 224;
  const float MAX_RUN_VEL = 520;
  const float MAX_AIR_VEL = 320;
  constexpr float FALL_VEL = 720;
  constexpr float FAST_FALL_VEL = 960;
  const float WALK_ACCEL = MAX_WALK_VEL * MAX_WALK_VEL * GROUND_DRAG;
  const float RUN_ACCEL = MAX_RUN_VEL * MAX_RUN_VEL * GROUND_DRAG;
  const float AIR_ACCEL = MAX_AIR_VEL * MAX_AIR_VEL * AIR_DRAG;
  constexpr float FALL_DRAG = GRAVITY / (FALL_VEL * FALL_VEL);
  static_assert(FALL_DRAG > 0, "fall drag <= 0");

  WVec force{ 0, -GRAVITY };

  // inputs
  float dir;
  if (ic.left_click.is(KeyState::Press)) {
    dir = copysignf(1, ic.mouse.x - t.position.x);
  } else {
    dir = 0;
  }
  bool jump = ic.jump.just_added(KeyState::Press);

  // horizontal movement
  if (dir != 0) {
    float accel;
    if (mc.active_state == MoveState::Run) {
      accel = dir * WALK_ACCEL;
      if (dir * mc.velocity.x < 0) {
        accel *= TURN_MUL;
      }
    } else {
      accel = dir * AIR_ACCEL;
    }
    force.x += accel;
  } else if (mc.active_state == MoveState::Run) {
    force.x -= STOP_FRICTION * mc.velocity.x;
  }

  // drags
  if (mc.active_state == MoveState::Run) {
    force.x -= GROUND_DRAG * mc.velocity.x * fabsf(mc.velocity.x);
  } else {
    force.x -= AIR_DRAG * mc.velocity.x * fabsf(mc.velocity.x);
    if (mc.velocity.y < 0) {
      mc.active_state = MoveState::Fall;
    }
  }
  if (mc.active_state == MoveState::Fall) {
    force.y -= FALL_DRAG * mc.velocity.y * fabsf(mc.velocity.y);
  }

  // jump
  if (jump && mc.active_state == MoveState::Run) {
    mc.velocity.y = JUMP_SPEED;
    mc.active_state = MoveState::Jump;
  }

  mc.next_accel = force;

  // TODO: work on mouse jump

  ImGui::Text("%f, %f", mc.velocity.x, mc.velocity.y);
}
