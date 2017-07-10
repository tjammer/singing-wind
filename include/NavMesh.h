//
// Created by jammer on 22/05/17.
//

#ifndef SINGING_WIND_NAVMESH_H
#define SINGING_WIND_NAVMESH_H

#include "Island.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>
#include <nanoflann.hpp>
#include <unordered_map>

class StaticGrid;

enum class LinkType : int {
    Walk,
    JumpUp,
    Drop,
    JumpAlong
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
    ~NavTree() = default;

    void rebuild(const std::vector<NavNode>& nodes);
    std::vector<NavNode> get_nearest(const WVec &pos, unsigned int n=1);
private:
    struct NavCloud {
        std::vector<NavNode> nodes;

        // Must return the number of data points
        inline size_t kdtree_get_point_count() const { return nodes.size(); }

        // Returns the distance between the vector "p1[0:size-1]" and the data point with index "idx_p2" stored in the class:
        inline float kdtree_distance(const float *p1, const size_t idx_p2, size_t /*size*/) const
        {
            const float d0=p1[0]-nodes[idx_p2].x;
            const float d1=p1[1]-nodes[idx_p2].y;
            return abs(d0)+abs(d1);
        }

        // Returns the dim'th component of the idx'th point in the class:
        // Since this is inlined and the "dim" argument is typically an immediate value, the
        //  "if/else's" are actually solved at compile time.
        inline float kdtree_get_pt(const size_t idx, int dim) const
        {
            if (dim==0) return nodes[idx].x;
            else return nodes[idx].y;
        }

        // Optional bounding-box computation: return false to default to a standard bbox computation loop.
        //   Return true if the BBOX was already computed by the class and returned in "bb" so it can be avoided to redo it again.
        //   Look at bb.size() to find out the expected dimensionality (e.g. 2 or 3 for point clouds)
        template <class BBOX>
        bool kdtree_get_bbox(BBOX& /*bb*/) const { return false; }
    };
    typedef nanoflann::KDTreeSingleIndexAdaptor<nanoflann::L1_Adaptor<float, NavCloud> , NavCloud, 2> KDTree;
    NavCloud m_cloud;
    KDTree m_kd_tree;
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

    NavNode get_nearest(const WVec &pos);
    NavNode get_nearest_visible(const WVec &pos, StaticGrid &grid);

    // for reusage
    std::unordered_map<NavNode, NavNode> m_path;
    std::unordered_map<NavNode, float> m_cost;

    NavMesh() = default;
    NavMesh(const NavMesh &other);
    NavMesh& operator=(const NavMesh &);
private:
    NavTree m_tree;
};

NavMesh build_navmesh(const std::vector<Island> &m_islands, StaticGrid &gridmesh);

inline float heuristic(const NavNode &from, const NavNode &to) {
    return abs(from.x - to.x) + abs(from.y - to.y);
}

void build_levels_connections(NavMesh &mesh, StaticGrid &grid);
void build_node_space(NavMesh &mesh, StaticGrid &grid);

#endif //SINGING_WIND_NAVMESH_H
