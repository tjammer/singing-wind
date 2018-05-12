#include "draw.h"
#include "comps.h"
#include "renderer.h"
#include "static_grid.h"

void
verts_to_renderer(const std::vector<WVec>& verts)
{
  for (unsigned int i = 0; i < verts.size(); ++i) {
    auto& a = verts[i];
    auto& b = verts[(i + 1) % verts.size()];
    WRenderer::add_primitive_vertex({ { a.x, a.y }, { 1, 1, 1 } });
    WRenderer::add_primitive_vertex({ { b.x, b.y }, { 1, 1, 1 } });
  }
}

void
draw_update(const Transform& pc, const StaticGrid& grid)
{
  WRenderer::set_mode(PLines);
  Transform zerotf;
  for (auto& obj : grid.get_objects()) {
    auto verts = obj.shape.draw_vertices(zerotf);
    verts_to_renderer(verts);
  }

  auto circle = Capsule{ 20, 10 };
  verts_to_renderer(circle.draw_vertices(pc));
}
