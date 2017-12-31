#ifndef PATHFINDING_H
#define PATHFINDING_H

#include "WindDefs.h"
#include <map>
#include <unordered_map>
#include <vector>

struct NavLink;
struct NavMesh;
struct NavNode;
typedef std::unordered_map<NavNode, std::vector<NavLink>> NavGraph;
class GameWorld;

enum class SteerType : int {
    Seek,
    Arrive,
    Flee
};

struct PathingComponent {
    SteerType s_type = SteerType::Seek;
    unsigned int following = 1; // if 0, needs extra PatrolComp
    std::vector<WVec> path;
    std::vector<unsigned int> flock;
    size_t index = 0;
    float c_padding = 20;
    float c_max_mh_dist = 10000;
};

enum class PathfindingStatus {
    Success,
    Failure
};

PathfindingStatus get_path(const WVec &from, const WVec &to, const GameWorld &, PathingComponent&);

PathfindingStatus a_star_search(const NavMesh &mesh, const NavNode &start, const NavNode &goal, PathingComponent &pc);

void entity_edit_pathfind(GameWorld &world, unsigned int entity);

#endif /* PATHFINDING_H */
