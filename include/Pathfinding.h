#ifndef PATHFINDING_H
#define PATHFINDING_H

#include "WindDefs.h"
#include <map>
#include <unordered_map>
#include <vector>
#include <queue>

struct NavLink;
struct NavMesh;
class StaticGrid;
struct NavNode;
typedef std::unordered_map<NavNode, std::vector<NavLink>> NavGraph;
class GameWorld;

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
    std::vector<WVec> path;
    size_t index = 0;
    float c_padding = 20;
    float c_max_mh_dist = 10000;
};

void get_path_platform(const WVec &from, const WVec &to, NavMesh &mesh, PathingComponent &pc);
void get_path_jump(const WVec &from, const WVec &to, NavMesh &mesh, PathingComponent &pc);
void get_path_fly(const WVec &from, const WVec &to, GameWorld &world, PathingComponent &pc);

int a_star_search(NavMesh &mesh, const NavNode &start, const NavNode &goal, PathingComponent &pc);
#endif /* PATHFINDING_H */
