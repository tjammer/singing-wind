#include "flying.h"
#include "comps.h"
#include "ecs.hpp"
#include "vec_math.h"
#include "imgui.h"

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
    // finite stall at 0
    return expf(-powf(angle - HALF_PI, 2.0f) * 2.f) * 0.25f + 0.75f;
  }
  return 1;
}

float
lift(float angle, float stall)
{
  assert(angle <= (float)M_PI);
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
dummy_flying(const Transform& t, Movement& mc, const Input& ic)
{
  // TODO: sanitize values

  // quadratic
  // v_max = sqrt(a/f)
  constexpr float GRAVITY = 196;
  constexpr float AIR_DRAG = 0.003;
  constexpr float MAX_AIR_VEL = 650;
  constexpr float LIFT_TO_DRAG_PEAK = 0.7;
  const float LIFT = LIFT_TO_DRAG_PEAK * AIR_DRAG;
  const float STALL_ANGLE = 0.26;
  constexpr float AIR_ACCEL = MAX_AIR_VEL * MAX_AIR_VEL * AIR_DRAG;

  WVec force{ 0, -GRAVITY };
  float mouse_angle = angle_to_mouse(ic.mouse, t);
  if (abs(mouse_angle) > 0.1f) {
    mc.angular_accel += copysignf(1, mouse_angle);
  }

  auto dir = w_rotated({ 0, 1 }, t.direction * t.rotation);
  if (ic.left_click.is(KeyState::Press)) {
    force += dir * AIR_ACCEL;
    mc.angular_accel -= mc.angular_velocity * 14.2f;
  } else {
    force.y -= GRAVITY;
    mc.angular_accel -= mc.angular_velocity * 12.5f;
  }
  auto angle = w_angle_to_vec(mc.velocity, dir);
  float vel = w_magnitude(mc.velocity);
  force -= drag(angle) * AIR_DRAG * mc.velocity * vel;
  force += w_tangent(w_normalize(mc.velocity)) * vel * vel *
           lift(angle, STALL_ANGLE) * LIFT;

  mc.next_accel += force;
}
