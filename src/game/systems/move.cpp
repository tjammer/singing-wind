#include "move.h"
#include "comps.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_transform_2d.hpp>

void
build_global_transform(Position& pc)
{
  pc.global_transform =
    rotate(scale(translate(WTransform(), pc.position), WVec(pc.direction, 1)),
           pc.rotation);
}

void
rotate_angle(float angle, float max_change_angle, Position& pc)
{
  pc.rotation += copysignf(fmin(max_change_angle, abs(angle)), angle);

  if (pc.rotation < 0) {
    pc.rotation *= -1;
    pc.direction *= -1;
  } else if (pc.rotation > (float)M_PI) {
    pc.rotation = 2 * (float)M_PI - pc.rotation;
    pc.direction *= -1;
  }
}

void
move_update(Movement& mc, Position& pc, float dt)
{
  using namespace glm;
  // leap frog
  mc.velocity += mc.accel * dt / 2.0f;
  pc.position += mc.velocity * dt;
  mc.velocity += mc.next_accel * dt / 2.0f;

  mc.accel = mc.next_accel;
  mc.next_accel = { 0, 0 };
  rotate_angle(mc.change_angle, mc.max_change_angle, pc);
  build_global_transform(pc);
}
