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

#include <queue>

std::unordered_map<NavNode, std::vector<NavLink>> walkable_from_tri(std::array<WVec, 3> tri, const StaticGrid &grid) {
    std::unordered_map<NavNode, std::vector<NavLink>> graph;
    std::vector<NavNode> nodes;
    for (auto i = 0 ; i < 3 ; ++i) {
        // check if point is on top of island
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
                NavLink link_1 = {n2, n1};
                NavLink link_2 = {n1, n2};
                graph[n1].push_back(link_1);
                graph[n2].push_back(link_2);
            }
        }
    }
    return graph;
}

void dummy_search(const WVec &from, const WVec &to, NavMesh &mesh, const bset &ent) {
    const NavNode & node = mesh.get_nearest(from);
    const NavNode & _to = mesh.get_nearest(to);
    if (a_star_search(mesh.m_graph, node, _to, mesh.m_path)) {
        // render path
        auto current = _to;
        while (true) {
            WRenderer::set_mode(GL_LINES);
            if (mesh.m_path.count(current) && !(mesh.m_path.at(current) == current)) {
                WRenderer::add_primitive_vertex({{(float)current.x, (float)current.y}, {1, 0, 0}});
                current = mesh.m_path.at(current);
                WRenderer::add_primitive_vertex({{(float)current.x, (float)current.y}, {1, 0, 0}});
                if (current == node) {
                    break;
                }
            }
            else {
                break;
            }
        }
    }

}

NavMesh build_navmesh(const std::vector<Island> &m_islands, StaticGrid &grid) {
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
                    mesh.m_graph[node].push_back(link);
                }
            }
        }
    }
    // todo sort into islands and check visibility to create links
    build_levels_connections(mesh, grid);
    mesh.build_tree();
    return mesh;
}

template <class pair>
struct PQCompare {
    inline bool operator()( const pair& lhs, const pair& rhs ) {
        return lhs.first > rhs.first;
    };
};

int a_star_search(const NavGraph &graph, const NavNode &start, const NavNode &goal,
                  std::unordered_map<NavNode, NavNode> &path) {
    using namespace std;

    unordered_map<NavNode, float> cost;

    path.clear();

    typedef pair<float, NavNode> PQElement;
    typedef priority_queue<PQElement, vector<PQElement>, PQCompare<PQElement>> PQueue;
    PQueue frontier;
    frontier.emplace(0, start);

    path[start] = start;
    cost[start] = 0;

    while (!frontier.empty()) {
        auto current = frontier.top().second;
        frontier.pop();

        if (current == goal) {
            return 1;
        }

        for (const auto &link : graph.at(current)) {
            float new_cost = cost[current] + link.cost;
            if (!cost.count(link.to) || new_cost < cost[link.to]) {
                cost[link.to] = new_cost;
                auto heur = heuristic(link.to, goal);
                float priority = new_cost + heur;
                frontier.emplace(priority, link.to);
                path[link.to] = current;
            }
        }
    }
    return 0;
}

void visit_node(NavNode &node, NavMesh &mesh, std::set<NavNode> &visited, unsigned int level) {
    if (visited.count(node)) {
        return;
    }
    mesh.m_levels[node] = level;
    visited.insert(node);
    for (auto &link : mesh.m_graph.at(node)) {
        visit_node(link.to, mesh, visited, level);
    }
}

LinkType find_link_type(const NavLink &link) {
    float dx = abs(link.to.x - link.from.x);
    float dy = link.to.y - link.from.y;

    if (abs(dy) > dx) {
        // fall or jump
        if (dy < 0) {
            return LinkType::JumpUp;
        }
        return LinkType::Drop;
    }
    // along
    return LinkType::JumpAlong;
}

void find_shortest_links(unsigned int l1, unsigned int l2, NavMesh &mesh, StaticGrid &grid) {
    struct Match {
        float dist = std::numeric_limits<float>::max();
        NavNode node;
        bool create= false;
    };
    std::unordered_map<NavNode, Match> outer_matches;
    std::unordered_map<NavNode, Match> inner_matches;

    for (auto &outer: mesh.m_graph) {
        auto &outer_node = outer.first;
        if (mesh.m_levels.at(outer_node) != l1) {
            continue;
        }
        for (auto &inner : mesh.m_graph) {
            auto &inner_node = inner.first;
            if (mesh.m_levels.at(inner_node) != l2) {
                continue;
            }
            auto result = cast_ray_vs_static_grid(grid, outer_node, inner_node);
            if (result.hitParameter < w_magnitude(outer_node - inner_node)) {
                continue;
            }
            NavLink link{outer_node, inner_node};
            if (link.cost < outer_matches[outer_node].dist && link.cost < inner_matches[inner_node].dist) {
                outer_matches[outer_node].node = inner_node;
                outer_matches[outer_node].dist = link.cost;
                outer_matches[outer_node].create = true;

                inner_matches[inner_node].node = outer_node;
                inner_matches[inner_node].dist = link.cost;
                inner_matches[inner_node].create = true;
            }
        }
    }

    for (auto &pair : outer_matches) {
        auto &n1 = pair.first;
        auto &match = pair.second;
        if (match.create && inner_matches.at(match.node).node == n1) {
            NavLink l1{match.node, n1};
            l1.link_type = find_link_type(l1);
            mesh.m_graph.at(n1).push_back(l1);

            NavLink l2{n1, match.node};
            l2.link_type = find_link_type(l2);
            mesh.m_graph.at(match.node).push_back(l2);
        }

    }
}

void build_levels_connections(NavMesh &mesh, StaticGrid &grid) {
    // first add level to nodes
    unsigned int level = 0;
    std::set<NavNode> visited;
    for (auto &pair : mesh.m_graph) {
        auto node = pair.first;
        if (!visited.count(node)) {
            level++;
        }

        visit_node(node, mesh, visited, level);
    }

    // find shortest link to each node on other level
    unsigned int max_level = 0;
    for (auto & pair : mesh.m_graph) {
        if (mesh.m_levels.at(pair.first) > max_level) {
            max_level = mesh.m_levels.at(pair.first);
        }
    }
    for (unsigned int i = 0 ; i < max_level ; ++i) {
        for (unsigned int j = i+1 ; j <= max_level ; ++j) {
            find_shortest_links(i, j, mesh, grid);
        }
    }
}

NavLink::NavLink(const NavNode &to, const NavNode &from)
    : to(to), from(from) {
    // manhattan distance
    cost = abs(to.x - from.x) + abs(to.y - from.y);
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

    m_kd_tree.knnSearch(&query_pt[0], 1, ret_index.data(), out_dist_sqr.data());
    return m_cloud.nodes[ret_index[0]];
}

void NavMesh::build_tree() {
    std::vector<NavNode> nodes;
    for (auto &pr : m_graph) {
        auto & node = pr.first;
        nodes.push_back(node);
    }
    m_tree.rebuild(nodes);
}

NavNode NavMesh::get_nearest(const WVec &pos) {
    return m_tree.get_nearest(pos);
}

NavMesh::NavMesh(const NavMesh &other) {
    m_graph = other.m_graph;
    m_levels = other.m_levels;

    build_tree();

}

NavMesh& NavMesh::operator=(const NavMesh &other) {
    m_graph = other.m_graph;
    m_levels = other.m_levels;

    build_tree();
    return *this;
}
