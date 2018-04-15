#include "flying.h"
#include "comps.h"
#include "ecs.hpp"
#include "vec_math.h"
#include "imgui.h"

const float HALF_PI = (float)(M_PI / 2.0);

float
angle_to_mouse(const WVec& mouse, const WTransform& global_transform)
{
  auto local_to = WVec(glm::inverse(global_transform) * WVec3(mouse, 1));
  return atan2f(-local_to.x, local_to.y);
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
  if (angle < stall) {
    return sin(angle * HALF_PI / stall);
  } else if (angle < HALF_PI) {
    return cos((angle - stall) * HALF_PI / (HALF_PI - stall));
  }
  return -0.3 * lift((float)M_PI - angle, stall);
}

void
flying(CanFly,
       const Flying& fc,
       Movement& mc,
       const Position& pc,
       const Input& ic)
{
}

void
dummy_flying(const Position& pc, Movement& mc, const Input& ic)
{
  mc.next_accel.y -= 98.0;

  WVec air_dir = w_normalize(mc.velocity);
  float vel_squ = w_dot(mc.velocity, mc.velocity);
  auto glide_dir = w_rotated({ 0, 1 }, pc.rotation * pc.direction);
  auto angle = w_angle_to_vec(mc.velocity, glide_dir);

  mc.next_accel -= air_dir * vel_squ * drag(angle) * 0.0067f;
  mc.next_accel -= w_tangent(air_dir) * vel_squ * lift(angle, 0.26) * 0.0061f;
  mc.next_accel += 25.0f * glide_dir;
  assert(!isnan(vel_squ));

  // rotations
  mc.change_angle = angle_to_mouse(ic.mouse, pc.global_transform);

  ImGui::Text("%f", w_magnitude(mc.velocity));
}

void
hover(const Position& pc, Movement& mc, const Input& ic)
{
  mc.next_accel = w_normalize(ic.mouse - pc.position) * 80.0f;
  if (abs(ic.mouse.y - pc.position.y) > 10) {
    mc.next_accel -= mc.velocity * 2.0f;
  } else {
    mc.next_accel.x -= mc.velocity.x * 4.0f;
  }
  mc.next_accel.y -= 5;
}
