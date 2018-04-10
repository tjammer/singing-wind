#include "flying.h"
#include "comps.h"
#include "ecs.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_transform_2d.hpp>

// void
// flying(CanFly,
//       const Flying& fc,
//       Movement& mc,
//       const Position& pc,
//       const Input& ic)
//{}

void
dummy_flying(Position& pc, const Input& ic)
{
  pc.position = ic.mouse;
  pc.global_transform = glm::translate(WTransform(), pc.position);
}
