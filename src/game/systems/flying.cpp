#include "flying.h"
#include "comps.h"
#include "ecs.hpp"
#include "vec_math.h"
#include "imgui.h"

float
angle_to_mouse(const WVec& mouse, const WTransform& global_transform)
{
  auto local_to = WVec(glm::inverse(global_transform) * WVec3(mouse, 1));
  return atan2f(-local_to.x, local_to.y);
}

void
flying(CanFly,
       const Flying& fc,
       Movement& mc,
       const Position& pc,
       const Input& ic)
{}

void
dummy_flying(const Position& pc, Movement& mc, const Input& ic)
{
  mc.next_accel = w_normalize(ic.mouse - pc.position) * 80.0f;
  if (abs(ic.mouse.y - pc.position.y) > 10) {
    mc.next_accel -= mc.velocity * 2.0f;
  } else {
    mc.next_accel.x -= mc.velocity.x * 4.0f;
  }
  mc.next_accel.y -= 5;

  // rotations
  mc.change_angle = angle_to_mouse(ic.mouse, pc.global_transform);

  ImGui::Text("%f", pc.rotation);
}
