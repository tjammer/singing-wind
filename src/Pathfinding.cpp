#include "Pathfinding.h"
#include "NavMesh.h"

template <class pair>
struct PQCompare {
    inline bool operator()( const pair& lhs, const pair& rhs ) {
        return lhs.first > rhs.first;
    };
};

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

    while (!frontier.empty()) {
        auto current = frontier.top().second;
        frontier.pop();

        if (current == goal) {
            return 1;
        }

        for (const auto &link : mesh.m_graph.at(current)) {
            float new_cost = mesh.m_cost[current] + link.cost;
            if (!mesh.m_cost.count(link.to) || new_cost < mesh.m_cost[link.to]) {
                mesh.m_cost[link.to] = new_cost;
                auto heur = heuristic(link.to, goal);
                float priority = new_cost + heur;
                frontier.emplace(priority, link.to);
                mesh.m_path[link.to] = current;
            }
        }
    }
    return 0;
}

