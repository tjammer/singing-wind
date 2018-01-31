//
// Created by jammer on 22/05/17.
//

#include "NavMesh.h"
#include "StaticGrid.h"
#include "Collision.h"
#include "GameWorld.h"
#include "WVecMath.h"
#include "triangulate.h"
#include <ColShape.h>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_transform_2d.hpp>
#include <nanoflann.hpp>

class NavTree::impl
{
public:
  struct NavCloud
  {
    std::vector<NavNode> nodes;

    // Must return the number of data points
    inline size_t kdtree_get_point_count() const { return nodes.size(); }

    // Returns the distance between the vector "p1[0:size-1]" and the data point
    // with index "idx_p2" stored in the class:
    inline float kdtree_distance(const float* p1,
                                 const size_t idx_p2,
                                 size_t /*size*/) const
    {
      const float d0 = p1[0] - nodes[idx_p2].x;
      const float d1 = p1[1] - nodes[idx_p2].y;
      return abs(d0) + abs(d1);
    }

    // Returns the dim'th component of the idx'th point in the class:
    // Since this is inlined and the "dim" argument is typically an immediate
    // value, the
    //  "if/else's" are actually solved at compile time.
    inline float kdtree_get_pt(const size_t idx, int dim) const
    {
      if (dim == 0)
        return nodes[idx].x;
      else
        return nodes[idx].y;
    }

    // Optional bounding-box computation: return false to default to a standard
    // bbox computation loop.
    //   Return true if the BBOX was already computed by the class and returned
    //   in "bb" so it can be avoided to redo it again.
    //   Look at bb.size() to find out the expected dimensionality (e.g. 2 or 3
    //   for point clouds)
    template<class BBOX>
    bool kdtree_get_bbox(BBOX& /*bb*/) const
    {
      return false;
    }
  };
  typedef nanoflann::KDTreeSingleIndexAdaptor<
    nanoflann::L1_Adaptor<float, NavCloud>,
    NavCloud,
    2>
    KDTree;
  NavCloud m_cloud;
  KDTree m_kd_tree;

  impl();
};

NavTree::impl::impl()
  : m_cloud()
  , m_kd_tree(2, m_cloud, nanoflann::KDTreeSingleIndexAdaptorParams(5))
{
}

NavTree::NavTree()
  : pimpl(std::make_unique<impl>())
{
}

NavTree::~NavTree()
{
}

const std::vector<NavNode>&
NavTree::get_nodes() const
{
  return pimpl->m_cloud.nodes;
}

std::unordered_map<NavNode, std::vector<NavLink>> walkable_from_tri(
  std::array<WVec, 3> tri,
  const StaticGrid<StaticTriangle>& grid)
{
  std::unordered_map<NavNode, std::vector<NavLink>> graph;
  std::vector<NavNode> nodes;
  for (auto i = 0; i < 3; ++i) {
    // check if point is on top of island
    auto norm_tang = w_normalize(w_tangent(tri[i] - tri[(i + 1) % 3]));
    if (w_dot(WVec(0, 1), norm_tang) > c_max_floor_angle) {
      auto point = (tri[i] + tri[(i + 1) % 3]) / 2.f - norm_tang * 10.f;
      auto result = ColResult();

      auto col_p = ColPoint();
      col_p.transform(glm::translate(WTransform(), point));
      for (const auto& tri : grid.get_objects()) {
        auto res = tri.shape->collides(col_p);
        if (res.collides) {
          result.collides = true;
        }
      }
      if (!result.collides) {
        NavNode n1 = { tri[i].x, tri[i].y - 15 };
        NavNode n2 = { tri[(i + 1) % 3].x, tri[(i + 1) % 3].y - 15 };
        NavLink link_1 = { n2, n1 };
        NavLink link_2 = { n1, n2 };
        graph[n1].push_back(link_1);
        graph[n2].push_back(link_2);
      }
    }
  }
  return graph;
}

bool
triangle_is_adjacent(const QuickTri& tri1, const QuickTri& tri2)
{
  int samecount = 0;
  for (const auto& a : { tri1.a, tri1.b, tri1.c }) {
    for (const auto& b : { tri2.a, tri2.b, tri2.c }) {
      if (a == b) {
        samecount++;
      }
    }
  }
  return samecount == 2;
}

NavMesh
build_navmesh_walk(const std::vector<Island>& m_islands,
                   const StaticGrid<StaticTriangle>& grid)
{
  NavMesh mesh;
  std::vector<WVec> triangles;
  for (const auto& island : m_islands) {
    triangulate_island(island, triangles, c_line_triangulate_split);
    for (unsigned int i = 0; i < triangles.size() / 3; ++i) {
      auto p1 = triangles[i * 3];
      auto p2 = triangles[i * 3 + 1];
      auto p3 = triangles[i * 3 + 2];
      std::unordered_map<NavNode, std::vector<NavLink>> links =
        walkable_from_tri({ { p1, p2, p3 } }, grid);
      for (const auto& pr : links) {
        auto node = pr.first;
        for (const auto& link : pr.second) {
          mesh.m_graph[node].push_back(link);
        }
      }
    }
  }
  mesh.build_tree();
  return mesh;
}

NavMesh
build_navmesh_fly(const std::vector<Island>& islands,
                  const StaticGrid<StaticTriangle>& static_grid)

{
  NavMesh mesh;
  std::vector<QuickTri> tris;
  triangulate_between_islands(islands, tris, c_line_triangulate_split);
  for (const auto& tri : tris) {
    auto node = NavNode((tri.a + tri.b + tri.c) / 3.0f);

    int adjacent_ct = 0;
    for (const auto& other_tri : tris) {
      if (triangle_is_adjacent(tri, other_tri)) {
        adjacent_ct++;
        auto other_node =
          NavNode((other_tri.a + other_tri.b + other_tri.c) / 3.0f);

        mesh.m_graph[node].push_back(NavLink(other_node, node));
      }
      if (adjacent_ct == 3) {
        break;
      }
    }
  }
  mesh.build_tree();
  return mesh;
}

NavLink::NavLink(const NavNode& to, const NavNode& from)
  : to(to)
  , from(from)
{
  // manhattan distance
  cost = abs(to.x - from.x) + abs(to.y - from.y);
}

void
NavTree::rebuild(const std::vector<NavNode>& nodes)
{
  pimpl->m_cloud.nodes = nodes;
  pimpl->m_kd_tree.buildIndex();
}

std::vector<size_t>
NavTree::get_nearest_indices(const WVec& pos) const
{
  float query_pt[2] = { pos.x, pos.y };
  size_t n = pimpl->m_cloud.nodes.size();

  std::vector<size_t> ret_index(n);
  std::vector<float> out_dist_sqr(n);

  pimpl->m_kd_tree.knnSearch(
    &query_pt[0], n, ret_index.data(), out_dist_sqr.data());
  return ret_index;
}

void
NavMesh::build_tree()
{
  std::vector<NavNode> nodes;
  for (auto& pr : m_graph) {
    auto& node = pr.first;
    nodes.push_back(node);
  }
  m_tree.rebuild(nodes);
}

NavNode
NavMesh::get_nearest(const WVec& pos) const
{
  return m_tree.get_nodes()[m_tree.get_nearest_indices(pos)[0]];
}

NavNode
NavMesh::get_nearest_visible(const WVec& pos,
                             const StaticGrid<StaticTriangle>& grid) const
{
  // get four nearest
  auto inds = m_tree.get_nearest_indices(pos);
  const auto& nodes = m_tree.get_nodes();
  for (auto& i : inds) {
    const auto& node = nodes[i];
    if (!grid.raycast_against_grid(pos, WVec{ node.x, node.y }).hits) {
      return node;
    }
  }
  return nodes[0];
}

NavMesh::NavMesh(const NavMesh& other)
{
  m_graph = other.m_graph;

  build_tree();
}

NavMesh&
NavMesh::operator=(const NavMesh& other)
{
  m_graph = other.m_graph;

  build_tree();
  return *this;
}
