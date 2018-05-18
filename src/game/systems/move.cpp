#include "move.h"
#include "comps.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_transform_2d.hpp>

void
rotate_angle(float angle, float max_change_angle, Transform& pc)
{
  pc.rotation += copysignf(fmin(max_change_angle, abs(angle)), angle);

  if (pc.rotation < 0) {
    pc.rotation *= -1;
    pc.direction *= -1;
  }
  if (pc.rotation > (float)M_PI) {
    pc.rotation = 2 * (float)M_PI - pc.rotation;
    pc.direction *= -1;
  }
}

void
move_update(Movement& mc, Transform& pc, float dt)
{
  using namespace glm;
  // leap frog
  // mc.velocity += mc.accel * dt / 2.0f;
  // pc.position += mc.velocity * dt;
  // mc.velocity += mc.next_accel * dt / 2.0f;
  // velocity verlet
  mc.velocity += dt * (mc.next_accel - mc.accel) * 0.5f;
  pc.position += dt * (mc.velocity + dt * 0.5f * mc.next_accel);
  mc.velocity += dt * mc.next_accel;
  // alt leap frog
  // pc.position += dt * mc.velocity + 0.5f * mc.accel * dt * dt;
  // mc.velocity += 0.5f * (mc.accel + mc.next_accel) * dt;

  mc.accel = mc.next_accel;
  mc.next_accel = { 0, 0 };
  rotate_angle(mc.change_angle, mc.max_change_angle, pc);
}
