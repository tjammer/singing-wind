#include "flying.h"
#include "comps.h"
#include "ecs.hpp"
#include "vec_math.h"

const float HALF_PI = (float)(M_PI / 2.0);

float
angle_to_mouse(const WVec& mouse, const Transform& t)
{

  auto local_to = inversed(t, mouse - t.position);
  return -t.direction * atan2f(-local_to.x, local_to.y);
}

float
drag(float angle)
{
  if (angle < 0) {
    return drag(-angle);
  }
  if (angle < HALF_PI) {
    return expf(-powf(angle - HALF_PI, 2.0f) * 1.4f);
  }
  return expf(-powf(angle - HALF_PI, 2.0f) * 0.1f);
}

float
lift(float angle, float stall)
{
  assert(angle <= M_PI);
  if (angle < 0) {
    return -lift(-angle, stall);
  }
  if (angle <= stall) {
    return sin(angle * HALF_PI / stall);
  } else if (angle <= HALF_PI) {
    return cos((angle - stall) * HALF_PI / (HALF_PI - stall));
  }
  return -0.1f * lift((float)M_PI - angle, stall);
}

void
flying(CanFly,
       const Flying& fc,
       Movement& mc,
       const Transform& pc,
       const Input& ic)
{}

void
hover(const Transform& pc, Movement& mc, const Input& ic)
{
  mc.next_accel.y = copysignf(1, ic.mouse.y - pc.position.y) * 80.0f;
  if (abs(ic.mouse.y - pc.position.y) > 10) {
    mc.next_accel -= mc.velocity * 2.0f;
  } else {
    mc.next_accel.x -= mc.velocity.x * 4.0f;
  }
  mc.next_accel.y -= 5.0;
}

void
dummy_flying(const Transform& pc, Movement& mc, const Input& ic)
{
  float terminal_vel = 700;
  float a = 327;
  mc.next_accel.y -= a;

  WVec air_dir = w_normalize(mc.velocity);

  float vel_squ = w_dot(mc.velocity, mc.velocity);
  auto glide_dir = w_rotated({ 0, 1 }, pc.rotation * pc.direction);
  auto angle = w_angle_to_vec(mc.velocity, glide_dir);

  float min_drag = drag(0);
  float c_lift = 0.0095;

  if (ic.wings != KeyState::Release) {
    if (ic.wings == KeyState::JustPressed) {
      // mc.velocity = fmaxf(vel, 300) * glide_dir;
      mc.next_accel += 10000.f * glide_dir;
    }
    float ac = 600;
    // slightly changed direction
    auto dir = w_rotated(glide_dir, pc.direction * -0.46);
    mc.next_accel += ac * dir;
    a += ac;
  }
  float c_drag = 1.f / pow(terminal_vel, 2) * a / min_drag;

  mc.next_accel -= air_dir * vel_squ * drag(angle) * c_drag;
  mc.next_accel += w_tangent(air_dir) * vel_squ * lift(angle, 0.26) * c_lift;

  WVec dir{ 0, 0 };
  if (ic.left != KeyState::Release) {
    dir.x -= 1;
  }
  if (ic.right != KeyState::Release) {
    dir.x += 1;
  }
  if (ic.down != KeyState::Release) {
    dir.y -= 1;
  }
  if (ic.up != KeyState::Release) {
    dir.y += 1;
  }
  auto mag = fminf(w_magnitude(dir), 1);
  dir = w_normalize(dir);

  // rotations
  // mc.change_angle = angle_to_mouse(ic.mouse, next);
  auto local_to = inversed(pc, mag * dir);
  mc.change_angle = -pc.direction * fmod(atan2f(-local_to.x, local_to.y), M_PI);
  if (abs(mc.change_angle) == M_PI) {
    mc.change_angle = 0;
  }
}
