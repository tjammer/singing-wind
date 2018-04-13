#include "move.h"
#include "comps.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_transform_2d.hpp>

void
move_update(Movement& mc, Position& pc, float dt)
{
  // leap frog
  mc.velocity += mc.accel * dt / 2.0f;
  pc.position += mc.velocity * dt;
  mc.velocity += mc.next_accel * dt / 2.0f;

  mc.accel = mc.next_accel;
  pc.global_transform = glm::translate(WTransform(), pc.position);
}
