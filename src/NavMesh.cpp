//
// Created by jammer on 22/05/17.
//

#include "NavMesh.h"
#include "WRenderer.h"
#include "triangulate.h"
#include "WVecMath.h"
#include "GameWorld.h"
#include <ColShape.h>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_transform_2d.hpp>

std::unordered_map<NavNode, std::vector<NavLink>> walkable_from_tri(std::array<WVec, 3> tri, const StaticGrid &grid) {
    std::unordered_map<NavNode, std::vector<NavLink>> graph;
    std::vector<NavNode> nodes;
    for (auto i = 0 ; i < 3 ; ++i) {
        auto norm_tang = w_normalize(w_tangent(tri[i] - tri[(i+1)%3]));
        if (w_dot(WVec(0, 1), norm_tang) > c_max_floor_angle) {
            auto point = (tri[i] + tri[(i+1)%3]) / 2.f - norm_tang * 10.f;
            auto result = ColResult();

            auto col_p = ColPoint();
            col_p.transform(glm::translate(WTransform(), point));
            for (const auto &shape : grid.get_objects()) {
                auto res = shape->collides(col_p);
                if (res.collides) {
                    result.collides = true;
                }
            }
            if (!result.collides) {
                NavNode n1 = {tri[i].x, tri[i].y - 15};
                NavNode n2 = {tri[(i+1)%3].x, tri[(i+1)%3].y - 15};
                NavLink nl = {n1, n2};
                graph[n1].push_back(nl);
                graph[n2].push_back(nl);
            }
        }
    }
    return graph;
}

void build_navmesh(const std::vector<Island> &m_islands, const StaticGrid &grid, const WVec &mouse) {
    NavMesh mesh;
    std::vector<WVec> triangles;
    for (const auto &island : m_islands) {
        triangulate_island(island, triangles);
        for (unsigned int i = 0 ; i < triangles.size() / 3 ; ++i) {
            auto p1 = triangles[i*3];
            auto p2 = triangles[i*3+1];
            auto p3 = triangles[i*3+2];
            std::unordered_map<NavNode, std::vector<NavLink>> links = walkable_from_tri({p1, p2, p3}, grid);
            for (const auto &pr : links) {
                auto node = pr.first;
                for (const auto &link : pr.second) {
                    mesh.graph[node].push_back(link);
                }
            }
        }
    }
    mesh.build_tree();
    // render
    for (const auto &pr : mesh.graph) {
        auto & node = pr.first;
        auto & links = pr.second;
        WRenderer::set_mode(GL_QUADS);
        for (const auto &q : make_quad({node.x, node.y}, 5)) {
            WRenderer::add_primitive_vertex({{q.x, q.y}, {0, 1, 1}});
        }
        WRenderer::set_mode(GL_LINES);
        // links
        for (const auto & link : links) {
            WRenderer::add_primitive_vertex({{(float)link.first.x, (float)link.first.y}, {1, 1, 0}});
            WRenderer::add_primitive_vertex({{(float)link.second.x, (float)link.second.y}, {1, 1, 0}});
        }
    }
    const auto & node = mesh.get_nearest(mouse);
    WRenderer::set_mode(GL_QUADS);
    for (const auto & q : make_quad({node.x, node.y}, 6)) {
        WRenderer::add_primitive_vertex({{q.x, q.y}, {1, 0, 1}});
    }
}

NavLink::NavLink(const NavNode &first, const NavNode &second)
    : first(first), second(second) {
    weight = w_magnitude(first - second);
}

NavNode &NavLink::get_next(const NavNode &n) {
    if (n == first) {
        return second;
    }
    else if (n == second) {
        return first;
    }
    assert(false);
    return first;
}


NavTree::NavTree() : m_cloud(), m_kd_tree(2, m_cloud, nanoflann::KDTreeSingleIndexAdaptorParams(5)) {
}

void NavTree::rebuild(const std::vector<NavNode> &nodes) {
    m_cloud.nodes = nodes;
    m_kd_tree.buildIndex();
}

NavNode & NavTree::get_nearest(const WVec &pos) {
    float query_pt[2] = {pos.x, pos.y};

    std::vector<size_t>   ret_index(1);
    std::vector<float> out_dist_sqr(1);

    m_kd_tree.knnSearch(&query_pt[0], 1, &ret_index[0], &out_dist_sqr[0]);
    return m_cloud.nodes[ret_index[0]];
}

void NavMesh::build_tree() {
    std::vector<NavNode> nodes;
    for (auto &pr : graph) {
        auto & node = pr.first;
        nodes.push_back(node);
    }
    m_tree.rebuild(nodes);
}

NavNode NavMesh::get_nearest(const WVec &pos) {
    return m_tree.get_nearest(pos);
}
