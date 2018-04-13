#include "flying.h"
#include "comps.h"
#include "ecs.hpp"
#include "vec_math.h"
#include "imgui.h"

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
  mc.next_accel = w_normalize(ic.mouse - pc.position) * 80.0f;
  if (abs(ic.mouse.y - pc.position.y) > 10) {
    mc.next_accel -= mc.velocity * 2.0f;
  } else {
    mc.next_accel.x -= mc.velocity.x * 4.0f;
  }
  mc.next_accel.y -= 5;
  ImGui::Text("%f", w_magnitude(mc.velocity));
  ImGui::Text("%f", abs(ic.mouse.y - pc.position.y));
}
