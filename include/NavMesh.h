//
// Created by jammer on 22/05/17.
//

#ifndef SINGING_WIND_NAVMESH_H
#define SINGING_WIND_NAVMESH_H

#include "WindDefs.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>
#include <vector>
#include <memory>
#include <unordered_map>

// for the rewrite:
// will have different navmeshs, will be build from criteria:
// only on floor?
// walls?
// ceilings?
// how to treat level-bridges
// function for level-bridges, boolstruct for rest?
// float for spacing of nodes
// pathfinding struct has enum to call correct node
// sufficient for now; path could store link-type
// node-type

class Island;
struct StaticTriangle;
template<typename grid_object>
class HashGrid;
class GameWorld;

enum class LinkType : int {
    Walk,
    Fly,
    JumpAlong
};

enum class NodeType : int {
    Ground,
    Wall,
    Ceiling
};

typedef glm::tvec2<int> NavNodeBase;
struct NavNode : public NavNodeBase {
    using NavNodeBase::vec;
};

namespace std {
    template <>
    struct hash<NavNode> : public hash<NavNodeBase>{};
}

inline bool operator<(const NavNode &lhs, const NavNode& rhs) {
    return std::hash<NavNode>{}(lhs) < std::hash<NavNode>{}(rhs);
}

struct NavLink {
    NavNode to;
    NavNode from;
    float cost;
    LinkType link_type = LinkType::Walk;

    NavLink(const NavNode& to, const NavNode& from);
};

inline bool operator==(const NavLink &lhs, const NavLink &rhs) {
    return lhs.to == rhs.to && lhs.from == rhs.from;
}

typedef std::unordered_map<NavNode, std::vector<NavLink>> NavGraph;

struct NavTree {
    NavTree();
    ~NavTree();

    void rebuild(const std::vector<NavNode>& nodes);
    std::vector<size_t> get_nearest_indices(const WVec &pos) const;
    const std::vector<NavNode> &get_nodes() const;
private:
    class impl;
    std::unique_ptr<impl> pimpl;
};

struct NodeSpace {
    // no down, bc nodes are built on top of islands
    float up = std::numeric_limits<float>::max();
    float left = std::numeric_limits<float>::max();
    float right = std::numeric_limits<float>::max();
};

struct NavMesh {
    NavGraph m_graph;
    std::unordered_map<NavNode, unsigned int> m_levels;
    std::unordered_map<NavNode, NodeSpace> m_space;
    void build_tree();

    NavNode get_nearest(const WVec &pos) const;
    NavNode get_nearest_visible(const WVec &pos, const HashGrid<StaticTriangle> &grid) const;

    // for reusage
    // std::unordered_map<NavNode, NavNode> m_path;
    // std::unordered_map<NavNode, float> m_cost;

    NavMesh() = default;
    NavMesh(const NavMesh &other);
    NavMesh& operator=(const NavMesh &);
private:
    NavTree m_tree;
};

NavMesh build_navmesh(const std::vector<Island> &m_islands, const HashGrid<StaticTriangle> &gridmesh);

inline float heuristic(const NavNode &from, const NavNode &to) {
    return abs(from.x - to.x) + abs(from.y - to.y);
}

void build_levels_connections(NavMesh &mesh, const HashGrid<StaticTriangle> &grid);
void build_node_space(NavMesh &mesh, const HashGrid<StaticTriangle> &grid);

struct NavMeshCreationConfig {
    std::bitset<3> walkable_nodes;
    std::bitset<3> walkable_links;
};

const auto c_fly_nodes = std::bitset<3>{}.set();
const auto c_fly_links = std::bitset<3>{}.set();
const auto c_platform_nodes = std::bitset<3>{}.set(static_cast<int>(NodeType::Ground));
const auto c_platform_links = std::bitset<3>{}.set(static_cast<int>(LinkType::Walk));

// world cannot be const due to HashGrid
NavMesh create_navmesh(GameWorld &world, const NavMeshCreationConfig &config);

#endif //SINGING_WIND_NAVMESH_H
