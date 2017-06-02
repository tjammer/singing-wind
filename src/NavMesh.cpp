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
    std::unordered_map<NavNode, std::vector<NavLink>> links;
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
                NavLink nl;
                nl.nodes = {n1, n2};
                links[n1].push_back(nl);
                links[n2].push_back(nl);
            }
        }
    }
    return links;
}

void build_navmesh(const std::vector<Island> &m_islands, const StaticGrid &grid) {
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
                    mesh.links[node].push_back(link);
                }
            }
        }
    }
    // render
    for (const auto &node : mesh.links) {
        WRenderer::set_mode(GL_QUADS);
        auto quad = make_quad({node.first.x, node.first.y}, 5);
        for (const auto &q : quad) {
            WRenderer::add_primitive_vertex({{q.x, q.y}, {0, 1, 1}});
        }
        WRenderer::set_mode(GL_LINES);
        // links
        for (const auto & link : node.second) {
            WRenderer::add_primitive_vertex({{(float)link.nodes.first.x, (float)link.nodes.first.y}, {1, 1, 0}});
            WRenderer::add_primitive_vertex({{(float)link.nodes.second.x, (float)link.nodes.second.y}, {1, 1, 0}});
        }
    }
}
