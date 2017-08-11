#include "Pathfinding.h"
#include "NavMesh.h"
#include "Collision.h"
#include "GameWorld.h"
#include "PosComponent.h"
#include "WVecMath.h"
#include "WRenderer.h"
#include "Components.h"
#include <iostream>
#include <imgui.h>

template <class pair>
struct PQCompare {
    inline bool operator()( const pair& lhs, const pair& rhs ) {
        return lhs.first > rhs.first;
    };
};

void construct_path(const std::unordered_map<NavNode, NavNode>& mesh_path, PathingComponent &pc, const NavNode &min_heur_node) {
    pc.path.clear();
    NavNode current = min_heur_node;
    NavNode next = mesh_path.at(min_heur_node);
    pc.path.push_back(WVec{current.x, current.y});
    while (next != current) {
        pc.path.push_back(WVec{next.x, next.y});
        current = next;
        next = mesh_path.at(current);
    }
}

int a_star_search(NavMesh &mesh, const NavNode &start, const NavNode &goal, PathingComponent &pc) {
    using namespace std;
    // todo: put in max_dist cutoff and construct path before returning

    mesh.m_path.clear();
    mesh.m_cost.clear();

    typedef pair<float, NavNode> PQElement;
    typedef priority_queue<PQElement, vector<PQElement>, PQCompare<PQElement>> PQueue;
    PQueue frontier;
    frontier.emplace(0, start);

    mesh.m_path[start] = start;
    mesh.m_cost[start] = 0;

    float min_heuristic = std::numeric_limits<float>::max();
    NavNode min_heur_node = start;

    while (!frontier.empty()) {
        auto current = frontier.top().second;
        frontier.pop();

        if (current == goal) {
            construct_path(mesh.m_path, pc, goal);
            return 1;
        }

        for (const auto &link : mesh.m_graph.at(current)) {
            float new_cost = mesh.m_cost[current] + link.cost;
            if (!mesh.m_cost.count(link.to) || new_cost < mesh.m_cost[link.to]) {
                mesh.m_cost[link.to] = new_cost;
                auto heur = heuristic(link.to, goal);

                if (heur >= pc.c_max_mh_dist) {
                    continue;
                }

                if (heur < min_heuristic) {
                    min_heuristic = heur;
                    min_heur_node = link.to;
                }

                float priority = new_cost + heur;
                frontier.emplace(priority, link.to);
                mesh.m_path[link.to] = current;
            }
        }
    }
    construct_path(mesh.m_path, pc, min_heur_node);
    return 0;
}

void get_path_fly(const WVec &from, const WVec &to, GameWorld &world, PathingComponent &pc) {
    // first of all check if there is direct visiblity
    auto result = cast_ray_vs_static_grid(world.grid(), from, to);
    if (!result.hits) {
        pc.path.clear();
        pc.path.push_back(to);
        return;
    }

    // need to find real path
    auto &mesh = world.navmesh();
    auto node_from = mesh.get_nearest_visible(from, world.grid());
    auto node_to = mesh.get_nearest_visible(to, world.grid());
    a_star_search(mesh, node_from, node_to, pc);

    // smooth path
    auto &node = pc.path[0];
    auto &spaces = mesh.m_space.at(node);

    node.y -= fmin(pc.c_padding, spaces.up / 2);
    if (spaces.right < pc.c_padding) {
    node.x -= fmin(pc.c_padding, spaces.right / 2);
    }
    if (spaces.left < pc.c_padding) {
    node.x += fmin(pc.c_padding, spaces.left / 2);
    }

    for (size_t i = 1 ; i < pc.path.size() ; ++i) {
        auto &node = pc.path[i];
        auto &spaces = mesh.m_space.at(node);

        node.y -= fmin(pc.c_padding, spaces.up / 2);
        if (spaces.right < pc.c_padding) {
        node.x -= fmin(pc.c_padding, spaces.right / 2);
        }
        if (spaces.left < pc.c_padding) {
        node.x += fmin(pc.c_padding, spaces.left / 2);
        }
        auto diff = pc.path[i-1] - node;
        if (diff.y > 0 and abs(diff.y) > abs(diff.x)) {
            if (diff.x > 0.f) {
                node.x += fmin(pc.c_padding, spaces.right / 2);
            } else if (diff.x < 0.f) {
                node.x -= fmin(pc.c_padding, spaces.left / 2);
            }
        }
    }

    WRenderer::set_mode(PLines);
    for (size_t i = 0 ; i < pc.path.size() - 1 ; ++i) {
        WRenderer::add_primitive_vertex({{pc.path[i].x, pc.path[i].y}, {1, 0, 0}});
        WRenderer::add_primitive_vertex({{pc.path[i+1].x, pc.path[i+1].y}, {1, 0, 0}});
    }
}

void get_path_platform(const WVec &, const WVec &, NavMesh &, PathingComponent &) {}
void get_path_jump(const WVec &, const WVec &, NavMesh &, PathingComponent &) {}

void get_path(GameWorld &world, unsigned int entity) {
    auto &pc = world.path_c(entity);
    switch (pc.p_type) {
        case PathingType::Platform : {
            assert(false); break;
        }
        case PathingType::Jump : {
            assert(false); break;
        }
        case PathingType::Fly : {
            if (pc.following != 0) {
                auto &pos = world.pos_c(entity).position;
                auto &follow = world.pos_c(pc.following).position;
                get_path_fly(pos, follow, world, pc);
                pc.index = pc.path.size() - 1;
            } else {
                assert(false); // patrol not implemented
            }
            break;
        }
    }
}

void entity_edit_pathfind(GameWorld &world, unsigned int entity) {
    using namespace ImGui;
    if (world.entities()[entity].test(CPathing) and CollapsingHeader("pathing")) {
        auto &pc = world.path_c(entity);

        if (DragFloat("max mh dist", &pc.c_max_mh_dist)) {}
        if (DragFloat("padding", &pc.c_padding)) {}
    }
}
