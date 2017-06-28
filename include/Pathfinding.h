#ifndef PATHFINDING_H
#define PATHFINDING_H

#include "WindDefs.h"
#include <map>
#include <unordered_map>
#include <deque>
#include <vector>
#include <queue>

struct NavLink;
struct NavMesh;
struct StaticGrid;
struct NavNode;
typedef std::unordered_map<NavNode, std::vector<NavLink>> NavGraph;

enum class _PathingType : int {
    Platform,
    Jump,
    Fly
};

typedef std::bitset<3> PathingType;

const std::map<_PathingType, const char*> pathingtype_string = {
    {_PathingType::Platform, "Platform"},
    {_PathingType::Jump, "Jump"},
    {_PathingType::Fly, "Fly"}
};

struct PathingComponent {
    PathingType p_type;
    std::deque<NavLink> path;
    float c_max_mh_dist = 10000;
};

void get_path_platform(WVec &from, PathingComponent &pc, NavMesh &mesh, StaticGrid &grid);
void get_path_jump(const WVec &from, const WVec &to, NavMesh &mesh, StaticGrid &grid);
void get_path_fly(const WVec &from, const WVec &to, NavMesh &mesh);

int a_star_search(NavMesh &mesh, const NavNode &start, const NavNode &goal, PathingComponent &pc);
#endif /* PATHFINDING_H */
