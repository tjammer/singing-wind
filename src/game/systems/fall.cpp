#include "fall.h"
#include "comps.h"
#include "vec_math.h"
#include "ecs.hpp"

void
fall_update(IsFalling, Movement& mc, const Input& ic, const Transform& pc)
{
  mc.next_accel.y -= 750.;
  WVec air_dir = w_normalize(mc.velocity);
  float vel = w_magnitude(mc.velocity);

  mc.next_accel -= air_dir * vel * 5.f;

  if (ic.right != KeyState::Release) {
    mc.next_accel.x += 750;
  }
  if (ic.left != KeyState::Release) {
    mc.next_accel.x -= 750;
  }

  // rotations
  mc.change_angle = copysignf(fmin(abs(pc.rotation), 0.02), -pc.rotation);
}
