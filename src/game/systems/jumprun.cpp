#include "jumprun.h"
#include "comps.h"
#include "ecs.hpp"
#include "vec_math.h"
#include "imgui.h"
#include <iostream>

void
jump_run_update(JumpRun& jr, Movement& mc, Transform& t, const Input& ic)
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
  const float JUMP_SPEED = 1200;
  const float MAX_WALK_VEL = 224;
  const float MAX_RUN_VEL = 520;
  const float MAX_AIR_VEL = 270;
  constexpr float WALL_VEL = 270;
  constexpr float FALL_VEL = 720;
  // constexpr float FAST_FALL_VEL = 960;
  const float WALK_ACCEL = MAX_WALK_VEL * MAX_WALK_VEL * GROUND_DRAG;
  const float RUN_ACCEL = MAX_RUN_VEL * MAX_RUN_VEL * GROUND_DRAG;
  const float AIR_ACCEL = MAX_AIR_VEL * MAX_AIR_VEL * AIR_DRAG;
  constexpr float FALL_DRAG = GRAVITY / (FALL_VEL * FALL_VEL);
  static_assert(FALL_DRAG > 0, "fall drag <= 0");
  constexpr float WALL_DRAG = GRAVITY / (WALL_VEL * WALL_VEL);
  static_assert(WALL_DRAG > 0, "wall drag <= 0");

  // times
  const float WALL_TIME = 1.2;
  const float AIR_TIME = 0.2; // jump time also

  const float JUMP_FRAC = 4.0f / 5.0f;

  // TODO: wall jump ,then onto fly again

  WVec force{ 0, -GRAVITY };

  // inputs
  int dir;
  if (ic.left_click.is(KeyState::Press)) {
    dir = copysignf(1, ic.mouse.x - t.position.x);
    t.direction = dir;
  } else {
    dir = 0;
  }
  bool jump = ic.jump.just_added(KeyState::Press);

  // states
  switch (mc.active_state) {
    case MoveState::Fall: {
      force.x += dir * AIR_ACCEL;
      force.x -= AIR_DRAG * mc.velocity.x * fabsf(mc.velocity.x);
      force.y -= FALL_DRAG * mc.velocity.y * fabsf(mc.velocity.y);
      break;
    }
    case MoveState::Run: {
      if (ic.left_click.double_tabbed(KeyState::Press)) {
        jr.running = true;
      } else if (dir == 0 || dir * mc.velocity.x < 0) {
        jr.running = false;
      }

      float accel = dir * (jr.running ? RUN_ACCEL : WALK_ACCEL);

      if (dir * mc.velocity.x < 0) {
        accel *= TURN_MUL;
      }

      force.x += accel;
      force.x -= GROUND_DRAG * mc.velocity.x * fabsf(mc.velocity.x);
      if (dir == 0) {
        force.x -= STOP_FRICTION * mc.velocity.x;
      }
      if (mc.timer == 0) {
        jr.can_wall_jump = true;
      }
      break;
    }
    case MoveState::Jump: {
      force.x += dir * AIR_ACCEL;
      force.x -= AIR_DRAG * mc.velocity.x * fabsf(mc.velocity.x);
      if (ic.jump.is(KeyState::Release)) {
        jr.boosting = false;
      } else {
        force.y += 500;
      }
      break;
    }
    case MoveState::Wall: {
      if (mc.velocity.y < 0) {
        force.y -= WALL_DRAG * mc.velocity.y * fabsf(mc.velocity.y);
      }
      if (mc.timer >= AIR_TIME) {
        force.x += dir * AIR_ACCEL;
        force.x -= AIR_DRAG * mc.velocity.x * fabsf(mc.velocity.x);
      }
      break;
    }
  }

  // transitions
  switch (mc.active_state) {
    case MoveState::Fall: {
      break;
    }
    case MoveState::Run: {
      if (jump && mc.timer < AIR_TIME) {
        // jump dir
        auto dir = w_normalize(inversed(t, ic.mouse - t.position));
        dir = JUMP_FRAC * WVec{ 0, 1 } + (1.f - JUMP_FRAC) * dir;
        mc.velocity += dir * JUMP_SPEED;
        mc.active_state = MoveState::Jump;

        // jump is boost
        jr.boosting = true;
      }
      break;
    }
    case MoveState::Jump: {
      break;
    }
    case MoveState::Wall: {
      if (jump && jr.can_wall_jump) {
        mc.velocity.y = JUMP_SPEED;
        mc.active_state = MoveState::Jump;
        jr.can_wall_jump = false;
      }
      break;
    }
  }
  if (mc.active_state != MoveState::Run) {
    jr.running = false;
  }
  if (mc.active_state != MoveState::Jump) {
    jr.boosting = false;
  }
  if (mc.velocity.y < 0 && mc.timer > AIR_TIME) {
    mc.active_state = MoveState::Fall;
    mc.timer = 0;
  }
  if (dir * mc.velocity.x < 0) {
    jr.running = false;
    jr.boosting = false;
    t.rotation = 0;
  }

  // rotations
  mc.change_angle = -t.rotation;

  auto direc = inversed(t, ic.mouse - t.position);

  ImGui::Text("%d", t.direction);
  ImGui::Text("%f, %f", direc.x, direc.y);
  mc.next_accel = force;
}
