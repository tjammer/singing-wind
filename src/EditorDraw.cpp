#include "EditorDraw.h"
#include "Components.h"
#include "GameWorld.h"
#include "Island.h"
#include "Pathfinding.h"
#include "PosComponent.h"
#include "WRenderer.h"

#include <imgui.h>

const float point_size = 10;

void
drawables::draw(const EditorDrawSet& drawset, GameWorld& world)
{
  if (drawset.none()) {
    return;
  }

  for (unsigned int entity = 1; entity < world.entities().size(); ++entity) {
    const auto& entity_set = world.entities()[entity];
    if (entity_set.none()) {
      continue;
    }
    if (drawset.test((int)EditorDrawables::EntityPos)) {
      WRenderer::set_mode(PQuads);
      const auto& pos =
        WVec(world.pos_c(entity).global_transform * WVec3(0.f, 0.f, 1.f));
      for (const auto& v : make_quad(pos, point_size)) {
        WRenderer::add_primitive_vertex({ { v.x, v.y }, { 0.7, 0.07, 0.7 } });
      }
    }

    if (entity_set.test(static_cast<int>(CPathing)) and
        drawset.test(static_cast<int>(EditorDrawables::Pathfinding))) {
      WRenderer::set_mode(PLines);
      const auto& path = world.path_c(entity).path;
      if (path.size() > 0) {
        for (size_t i = 0; i < path.size() - 1; ++i) {
          WRenderer::add_primitive_vertex(
            { { path[i].x, path[i].y }, { 1, 0, 0 } });
          WRenderer::add_primitive_vertex(
            { { path[i + 1].x, path[i + 1].y }, { 1, 0, 0 } });
        }
      }
    }
  }
}

const std::map<EditorDrawables, const char*> drawables_string = {
  { EditorDrawables::EntityPos, "EntityPos" },
  { EditorDrawables::Pathfinding, "Pathfinding" },
  { EditorDrawables::AttentionZone, "AttentionZone" }
};

void
drawables::menu(EditorDrawSet& drawset)
{
  auto int_set = drawset.to_ulong();
  for (auto& pair : drawables_string) {
    ImGui::CheckboxFlags(
      pair.second, &int_set, 1 << static_cast<int>(pair.first));
  }
  drawset = EditorDrawSet(int_set);
}
