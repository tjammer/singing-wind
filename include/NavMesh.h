//
// Created by jammer on 22/05/17.
//

#ifndef SINGING_WIND_NAVMESH_H
#define SINGING_WIND_NAVMESH_H

#include "Island.h"
#include "ColGrid.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>
#include <nanoflann.hpp>

enum class LinkType : int {
    Walk,
    Jump,
    Fall
};

using NavNode = glm::tvec2<int>;

struct NavLink {
    NavNode first;
    NavNode second;
    float weight;
    LinkType link_type = LinkType::Walk;
    NavNode& get_next(const NavNode& n);

    NavLink(const NavNode& first, const NavNode& second);
};

// spatial partitioing to find nearest node


struct NavTree {
    NavTree();
    ~NavTree() = default;

    void rebuild(const std::vector<NavNode>& nodes);
    NavNode & get_nearest(const WVec &pos);
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
            return d0*d0+d1*d1;
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
    using KDTree = nanoflann::KDTreeSingleIndexAdaptor<nanoflann::L1_Adaptor<float, NavCloud> , NavCloud, 2>;
    NavCloud m_cloud;
    KDTree m_kd_tree;
};

struct NavMesh {
    std::unordered_map<NavNode, std::vector<NavLink>> graph;
    void build_tree();

    NavNode get_nearest(const WVec &pos);
private:
    NavTree m_tree;
};

void build_navmesh(const std::vector<Island> &m_islands, const StaticGrid &grid, const WVec &mouse);


#endif //SINGING_WIND_NAVMESH_H
