#include "draw.h"
#include "comps.h"
#include "renderer.h"
#include "static_grid.h"

void
draw_update(const Position& pc, StaticGrid<StaticTriangle>& grid)
{
  WRenderer::set_mode(PLines);
  auto zero = WTransform();
  for (auto& obj : grid.get_objects()) {
    // obj.shape.add_gfx_lines(zero);
  }

  // auto circle = ColCapsule{ 10, 20 };
  // circle.add_gfx_lines(pc.global_transform);
}
