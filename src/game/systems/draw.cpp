#include "draw.h"
#include "comps.h"
#include "renderer.h"
#include "col_shape.h"

void
draw_update(const Position& pc)
{
  WRenderer::set_mode(PLines);
  auto circle = ColCapsule{ 10, 20 };
  circle.add_gfx_lines(pc.global_transform);
}
