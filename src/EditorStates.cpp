//
// Created by tobi on 3/31/17.
//

#include "EditorStates.h"
#include "Components.h"
#include "EntityEditor.h"
#include "GameWorld.h"
#include "Island.h"
#include "NavMesh.h"
#include "PosComponent.h"
#include "WRenderer.h"
#include <WVecMath.h>
#include <algorithm>
#include <imgui.h>
#include <iomanip>
#include <iostream>

std::unique_ptr<BaseEditorSubState>
IslandIdle::confirm(GameWorld&)
{
  // find curve nearest to cursor
  auto dist = std::numeric_limits<float>::max();
  auto size = m_island.m_points.size();
  unsigned int curve_index = 0;
  for (unsigned int i = 0; i < size; ++i) {
    BCurve curve = BCurve{ m_island.m_points[i],
                           m_island.m_ctrl_points[i * 2],
                           m_island.m_ctrl_points[i * 2 + 1],
                           m_island.m_points[(i + 1) % size] };
    auto vecs = curve.points_along_curve(1.0f);
    for (auto v : vecs) {
      if (w_magnitude(v - m_mpos) < dist) {
        dist = w_magnitude(v - m_mpos);
        curve_index = i;
      }
    }
  }
  if (dist > 50) {
    return std::unique_ptr<IslandIdle>(new IslandIdle(m_island));
  }
  auto curve = BCurve{ m_island.m_points[curve_index],
                       m_island.m_ctrl_points[curve_index * 2],
                       m_island.m_ctrl_points[curve_index * 2 + 1],
                       m_island.m_points[(curve_index + 1) % size] };
  return std::unique_ptr<CurveIdle>(new CurveIdle(curve, m_island));
}

EditorSubState
IslandIdle::menu(GameWorld& world)
{
  ImGui::Begin("island idle");
  if (ImGui::Button("select curve nearest to cursor")) {
    ImGui::End();
    return confirm(world);
  }

  if (ImGui::Button("move island")) {
    ImGui::End();
    return move(world);
  }
  if (ImGui::Button("delete island")) {
    ImGui::End();
    return delete_island(world);
  }
  if (ImGui::Button("cancel")) {
    ImGui::End();
    return cancel();
  }
  ImGui::End();
  return nullptr;
}

EditorSubState
IslandIdle::cancel()
{
  return EditorSubState(new EditorIdle);
}

void
IslandIdle::draw(GameWorld&)
{
  get_island_vertex_array(m_island);
}

EditorSubState
IslandIdle::delete_island(GameWorld& world)
{
  auto& islands = world.islands();
  islands.erase(std::remove(islands.begin(), islands.end(), m_island),
                islands.end());
  return EditorSubState(new EditorIdle);
}

EditorSubState
IslandIdle::move(GameWorld&)
{
  return EditorSubState(new IslandMove(m_island, m_mpos));
}

CurveIdle::CurveIdle(const BCurve& curve, Island& active)
  : m_curve(curve)
  , m_island(active)
{
}

void
CurveIdle::draw(GameWorld&)
{
  get_island_vertex_array(m_island);
  WRenderer::set_mode(PQuads);
  for (auto v : m_curve.line_along_curve(c_line_draw_distance)) {
    WRenderer::add_primitive_vertex({ { v.x, v.y }, { 0.5, 0.05, 0.5 } });
  }

  for (const auto& point :
       { m_curve.from, m_curve.ctrl_from, m_curve.ctrl_to, m_curve.to }) {
    for (const auto& v : make_quad(point, c_ctrl_point_size)) {
      WRenderer::add_primitive_vertex({ { v.x, v.y }, { 1, 1, 1 } });
    }
  }

  WRenderer::set_mode(PLines);
  WRenderer::add_primitive_vertex(
    { { m_curve.from.x, m_curve.from.y }, { 1, 1, 1 } });
  WRenderer::add_primitive_vertex(
    { { m_curve.ctrl_from.x, m_curve.ctrl_from.y }, { 1, 1, 1 } });
  WRenderer::add_primitive_vertex(
    { { m_curve.ctrl_to.x, m_curve.ctrl_to.y }, { 1, 1, 1 } });
  WRenderer::add_primitive_vertex(
    { { m_curve.to.x, m_curve.to.y }, { 1, 1, 1 } });
}

EditorSubState
CurveIdle::confirm(GameWorld&)
{
  auto dist = std::numeric_limits<float>::max();
  unsigned int point_index = 0;
  for (unsigned int i = 0; i < 4; ++i) {
    if (w_magnitude(m_curve[i] - m_mpos) < dist) {
      dist = w_magnitude(m_curve[i] - m_mpos);
      point_index = i;
    }
  }

  if (dist > 70) {
    return std::unique_ptr<IslandIdle>(new IslandIdle(m_island));
  }

  auto it = std::find(
    m_island.m_points.begin(), m_island.m_points.end(), m_curve[point_index]);
  if (it == m_island.m_points.end()) {
    it = std::find(m_island.m_ctrl_points.begin(),
                   m_island.m_ctrl_points.end(),
                   m_curve[point_index]);
  }

  auto at = std::find(m_island.m_points.begin(), m_island.m_points.end(), *it);
  if (at != m_island.m_points.end()) {
    auto i = it - m_island.m_points.begin();
    auto& c1 = m_island.m_ctrl_points[i * 2];
    auto& c2 =
      m_island.m_ctrl_points[(i * 2 - 1 + m_island.m_ctrl_points.size()) %
                             m_island.m_ctrl_points.size()];
    return EditorSubState(new PointEdit(*it, m_mpos, c1, c2, m_island));
  }
  return EditorSubState(new PointEdit(*it, m_mpos, m_island));
}

EditorSubState
CurveIdle::insert_item(GameWorld&)
{
  return EditorSubState(new CurveInsert(m_curve, m_island));
}

EditorSubState
CurveIdle::menu(GameWorld& world)
{
  ImGui::Begin("curve idle");
  if (ImGui::Button("add point")) {
    ImGui::End();
    return insert_item(world);
  }
  if (ImGui::Button("print bezier curve")) {
    ImGui::End();
    print_formatted_bezier();
    return nullptr;
  }
  if (ImGui::Button("cancel")) {
    ImGui::End();
    return cancel();
  }
  ImGui::End();
  return nullptr;
}

void
CurveIdle::print_formatted_bezier()
{
  using namespace std;
  // y flipped in sfml
  float high_y = std::numeric_limits<float>::max();
  float low_y = -std::numeric_limits<float>::max();
  float high_x = -std::numeric_limits<float>::max();
  float low_x = std::numeric_limits<float>::max();
  for (const auto& p : m_curve.points_along_curve(0.01f)) {
    if (p.y < high_y) {
      high_y = p.y;
    }
    if (p.y > low_y) {
      low_y = p.y;
    }
    if (p.x > high_x) {
      high_x = p.x;
    }
    if (p.x < low_x) {
      low_x = p.x;
    }
  }
  float df_y = 1.f / (high_y - low_y);
  float df_x = 1.f / (high_x - low_x);
  cout << fixed << setprecision(3) << "WVec from =       {"
       << (m_curve.to.x - low_x) * df_x << ", " << (m_curve.to.y - low_y) * df_y
       << "};" << endl;
  cout << fixed << setprecision(3) << "WVec ctrl_from =  {"
       << (m_curve.ctrl_to.x - low_x) * df_x << ", "
       << (m_curve.ctrl_to.y - low_y) * df_y << "};" << endl;
  cout << fixed << setprecision(3) << "WVec ctrl_to =    {"
       << (m_curve.ctrl_from.x - low_x) * df_x << ", "
       << (m_curve.ctrl_from.y - low_y) * df_y << "};" << endl;
  cout << fixed << setprecision(3) << "WVec to =         {"
       << (m_curve.from.x - low_x) * df_x << ", "
       << (m_curve.from.y - low_y) * df_y << "};" << endl;
}

EditorSubState
PointEdit::update(const WVec& mpos)
{
  auto diff = m_mpos - mpos;
  BaseEditorSubState::update(mpos);
  m_point -= diff;
  if (!m_ctrl) {
    m_c1 -= diff;
    m_c2 -= diff;
  }
  return nullptr;
}

EditorSubState
PointEdit::cancel()
{
  auto diff = m_point - m_orig_point;
  m_point = m_orig_point;
  if (!m_ctrl) {
    m_c1 -= diff;
    m_c2 -= diff;
  }
  return EditorSubState(new IslandIdle(m_island));
}

void
PointEdit::draw(GameWorld&)
{
  get_island_vertex_array(m_island);
  WRenderer::set_mode(PQuads);
  for (const auto& v : make_quad(m_point, c_point_size)) {
    WRenderer::add_primitive_vertex({ { v.x, v.y }, { 0.5, 0.05, 0.5 } });
  }
}

PointEdit::PointEdit(WVec& point,
                     const WVec& mouse,
                     WVec& c1,
                     WVec& c2,
                     Island& active)
  : m_point(point)
  , m_c1(c1)
  , m_c2(c2)
  , m_island(active)
{
  m_orig_point = m_point;
  m_mpos = mouse;
}

PointEdit::PointEdit(WVec& point, const WVec& mouse, Island& active)
  : m_point(point)
  , m_c1(point)
  , m_c2(point)
  , m_island(active)
{
  m_ctrl = true;
  m_orig_point = m_point;
  m_mpos = mouse;
}

EditorSubState
PointEdit::menu(GameWorld& world)
{
  ImGui::Begin("point move");
  if (ImGui::Button("place point here")) {
    ImGui::End();
    return confirm(world);
  }
  if (ImGui::Button("delete point")) {
    ImGui::End();
    return delete_item(world);
  }
  if (ImGui::Button("cancel")) {
    ImGui::End();
    return cancel();
  }
  ImGui::End();
  return nullptr;
}

EditorSubState
PointEdit::delete_item(GameWorld&)
{
  auto size = m_island.m_points.size();
  if (m_island.m_points.size() == 4) {
    return EditorSubState(new IslandIdle(m_island));
  }
  auto i = static_cast<size_t>(
    std::find(m_island.m_points.begin(), m_island.m_points.end(), m_point) -
    m_island.m_points.begin());
  // check if this is a control point
  if (i == size) {
    auto it = std::find(
      m_island.m_ctrl_points.begin(), m_island.m_ctrl_points.end(), m_point);
    if (it != m_island.m_ctrl_points.end()) {
      return EditorSubState(new IslandIdle(m_island));
    }
  }

  auto curve = BCurve{ m_island.m_points[(i - 1) % size],
                       m_island.m_ctrl_points[((i - 1) % size) * 2],
                       m_island.m_ctrl_points[((i - 1) % size) * 2 + 1],
                       m_island.m_points[i] };
  auto it =
    std::find(m_island.m_points.begin() + 1, m_island.m_points.end(), curve.to);
  m_island.m_points.erase(it);

  it = std::find(m_island.m_ctrl_points.begin(),
                 m_island.m_ctrl_points.end(),
                 curve.ctrl_to);
  m_island.m_ctrl_points.erase(it, it + 2);

  return EditorSubState(new IslandIdle(m_island));
}

void
CurveInsert::draw(GameWorld&)
{
  get_island_vertex_array(m_island);
  WRenderer::set_mode(PQuads);

  float t_low = fmax(m_new_point_t - 0.01f, 0.f);
  float t_high = fmin(m_new_point_t + 0.01f, 1.f);

  auto base = m_curve.eval(t_low);
  WRenderer::add_primitive_vertex(
    { { (base + m_curve.eval_perpendicular(t_low) * c_line_size).x,
        (base + m_curve.eval_perpendicular(t_low) * c_line_size).y },
      { 0.5, 0.05, 0.5 } });
  WRenderer::add_primitive_vertex(
    { { (base - m_curve.eval_perpendicular(t_low) * c_line_size).x,
        (base - m_curve.eval_perpendicular(t_low) * c_line_size).y },
      { 0.5, 0.05, 0.5 } });
  base = m_curve.eval(t_high);
  WRenderer::add_primitive_vertex(
    { { (base - m_curve.eval_perpendicular(t_high) * c_line_size).x,
        (base - m_curve.eval_perpendicular(t_high) * c_line_size).y },
      { 0.5, 0.05, 0.5 } });
  WRenderer::add_primitive_vertex(
    { { (base + m_curve.eval_perpendicular(t_high) * c_line_size).x,
        (base + m_curve.eval_perpendicular(t_high) * c_line_size).y },
      { 0.5, 0.05, 0.5 } });
}

CurveInsert::CurveInsert(BCurve curve, Island& active)
  : m_curve(curve)
  , m_island(active)
{
}

EditorSubState
CurveInsert::update(const WVec& mpos)
{
  BaseEditorSubState::update(mpos);
  auto to = (m_mpos - m_curve.to);
  auto from = (m_mpos - m_curve.from);

  if (to.x * from.x + to.y * from.y >= 0) {
    m_new_point_t = w_magnitude(to) > w_magnitude(from) ? 0 : 1;
  } else {
    m_new_point_t =
      1.f - w_magnitude(to) / (w_magnitude(from) + w_magnitude(to));
  }
  return nullptr;
}

EditorSubState
CurveInsert::confirm(GameWorld&)
{
  auto new_point = m_curve.eval(m_new_point_t);

  auto it = std::find(m_island.m_ctrl_points.begin(),
                      m_island.m_ctrl_points.end(),
                      m_curve.ctrl_to);
  it = m_island.m_ctrl_points.insert(
    it, new_point + (m_curve.from - new_point) / 4.f);
  m_island.m_ctrl_points.insert(it + 1,
                                new_point - (new_point - m_curve.to) / 4.f);
  *(it + 2) = new_point - (new_point - m_curve.to) * 0.75f;
  *(it - 1) = new_point + (m_curve.from - new_point) * 0.75f;

  it = std::find(
    m_island.m_points.begin() + 1, m_island.m_points.end(), m_curve.to);
  m_island.m_points.insert(it, new_point);

  return EditorSubState(new IslandIdle(m_island));
}

EditorSubState
CurveInsert::menu(GameWorld& world)
{
  ImGui::Begin("curve insert");
  if (ImGui::Button("insert curve here")) {
    ImGui::End();
    return confirm(world);
  }
  if (ImGui::Button("cancel")) {
    ImGui::End();
    return cancel();
  }
  ImGui::End();
  return nullptr;
}

EditorSubState
EditorIdle::confirm(GameWorld& world)
{
  auto& islands = world.islands();

  // find island point closest to cursor
  bool is_island = false;
  float dist = std::numeric_limits<float>::max();
  int index = -1;

  for (unsigned int i = 0; i < islands.size(); ++i) {
    const auto& island = islands[i];
    auto size = island.m_points.size();
    for (unsigned int j = 0; j < size; ++j) {
      BCurve curve = BCurve{ island.m_points[j],
                             island.m_ctrl_points[j * 2],
                             island.m_ctrl_points[j * 2 + 1],
                             island.m_points[(j + 1) % size] };
      auto vecs = curve.points_along_curve(1.0f);
      for (const auto& v : vecs) {
        float dist_to_point = w_magnitude(m_mpos - v);
        if (dist_to_point < dist) {
          dist = dist_to_point;
          index = i;
          is_island = true;
        }
      }
    }
  }

  for (unsigned int i = 0; i < world.entities().size(); ++i) {
    bset pos_set{ (1 << CPosition) | (1 << CDebugDraw) };
    if (for_gameworld::has_component(world.entities()[i], pos_set)) {
      float dist_to_point =
        w_magnitude(m_mpos - WVec(world.get<PosComponent>(i).global_transform *
                                  WVec3(0, 0, 1)));
      if (dist_to_point < dist) {
        dist = dist_to_point;
        index = i;
        is_island = false;
      }
    }
  }

  if (index == -1) {
    return EditorSubState(new EditorIdle);
  }
  if (is_island) {
    return EditorSubState(new IslandIdle(islands[index]));
  }
  return EditorSubState(
    new EntityIdle(world, static_cast<unsigned int>(index)));
}

void
EditorIdle::draw(GameWorld&)
{
}

EditorSubState
EditorIdle::insert_item(GameWorld& world)
{
  auto& islands = world.islands();
  auto index = static_cast<unsigned int>(islands.size());

  islands.push_back(Island(m_mpos, 50));
  return EditorSubState(new IslandMove(islands[index], m_mpos));
}

EditorSubState
EditorIdle::menu(GameWorld& world)
{
  ImGui::Begin("editor idle");
  if (ImGui::Button("insert island here")) {
    ImGui::End();
    return insert_item(world);
  }
  if (ImGui::Button("cancel")) {
    ImGui::End();
    return cancel();
  }
  ImGui::End();
  return nullptr;
}

void
get_island_vertex_array(const Island& island)
{
  WRenderer::set_mode(PQuads);
  auto curves = const_cast<Island&>(island).get_curves(c_line_draw_distance);
  for (auto& v : curves) {
    WRenderer::add_primitive_vertex(v);
  }

  auto verts = island.get_points(c_point_size);
  for (auto& v : verts) {
    WRenderer::add_primitive_vertex(v);
  }
}

void
IslandMove::draw(GameWorld&)
{
  get_island_vertex_array(m_island);
  WRenderer::set_mode(PQuads);
  for (const auto& point : m_island.m_points) {
    for (const auto& v : make_quad(point, c_point_size)) {
      WRenderer::add_primitive_vertex({ { v.x, v.y }, { 0.5, 0.05, 0.5 } });
    }
  }
}

EditorSubState
IslandMove::update(const WVec& mpos)
{
  auto diff = m_mpos - mpos;
  m_diff += diff;
  BaseEditorSubState::update(mpos);
  for (auto& point : m_island.m_points) {
    point -= diff;
  }
  for (auto& point : m_island.m_ctrl_points) {
    point -= diff;
  }
  return nullptr;
}

EditorSubState
IslandMove::cancel()
{
  for (auto& point : m_island.m_points) {
    point += m_diff;
  }
  for (auto& point : m_island.m_ctrl_points) {
    point += m_diff;
  }
  return EditorSubState(new IslandIdle(m_island));
}

EditorSubState
IslandMove::menu(GameWorld& world)
{
  ImGui::Begin("island move");
  if (ImGui::Button("confirm island position")) {
    ImGui::End();
    return confirm(world);
  }
  if (ImGui::Button("cancel")) {
    ImGui::End();
    return cancel();
  }
  ImGui::End();
  return nullptr;
}

IslandMove::IslandMove(Island& active, const WVec& mouse)
  : m_island(active)
{
  m_mpos = mouse;
}

void
NavMeshIdle::draw(GameWorld& world)
{
  const auto& mesh = world.navmesh();

  // render
  for (const auto& pr : mesh.m_graph) {
    auto& node = pr.first;
    auto& links = pr.second;
    WRenderer::set_mode(PQuads);
    for (const auto& q : make_quad({ node.x, node.y }, 5)) {
      WRenderer::add_primitive_vertex({ { q.x, q.y }, { 0, 1, 1 } });
    }
    WRenderer::set_mode(PLines);
    // links
    for (const auto& link : links) {
      WRenderer::add_primitive_vertex(
        { { (float)link.from.x, (float)link.from.y }, { 1, 1, 0 } });
      WRenderer::add_primitive_vertex(
        { { (float)link.to.x, (float)link.to.y }, { 1, 1, 0 } });
    }
  }
}

EditorSubState
NavMeshIdle::cancel()
{
  return EditorSubState(new EditorIdle());
}
