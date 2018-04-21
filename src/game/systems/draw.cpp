#include "draw.h"
#include "comps.h"
#include "renderer.h"
#include "col_shape.h"
#include "col_shape.h"

void
draw_update(const Position& pc, std::vector<ColTriangle>& tris)
{
  WRenderer::set_mode(PLines);
  auto zero = WTransform();
  for (auto& tri : tris) {
    tri.add_gfx_lines(zero);
  }

  auto circle = ColCapsule{ 10, 20 };
  circle.add_gfx_lines(pc.global_transform);
}
