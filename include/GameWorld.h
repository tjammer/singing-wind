//
// Created by jammer on 07/04/17.
//

#ifndef SINGING_WIND_GAMEWORLD_H
#define SINGING_WIND_GAMEWORLD_H

#include <string>
#include <vector>
#include <memory>
#include "WindDefs.h"

class GLFWwindow;
struct PosComponent;
struct DebugComponent;
struct InputComponent;
struct MoveComponent;
struct StaticColComponent;
struct GroundMoveComponent;
struct JumpComponent;
struct FlyComponent;
struct PathingComponent;
class StaticGrid;
class NavMesh;
class Island;
using NameComponent = std::string;

// physics constants
const float c_drag = 0.0026;
const float c_max_floor_angle = 0.7;
const float c_gravity = 1900;
const float c_jump_tolerance = 0.1f;

// implements the ecs
class GameWorld {
public:
    GameWorld();
    ~GameWorld();

    // maybe a timer?
    void step_fixed(float dt);
    void pre_draw(float dt);
    void draw();

    std::vector<bset>& entities();

        // components
        PosComponent& pos_c(unsigned int entity);
        DebugComponent &debug_c(unsigned int entity);
        InputComponent &input_c(unsigned int entity);
        MoveComponent &move_c(unsigned int entity);
        StaticColComponent &static_col_c(unsigned int entity);
        GroundMoveComponent &ground_move_c(unsigned int entity);
        JumpComponent &jump_c(unsigned int entity);
        FlyComponent &fly_c(unsigned int entity);
        PathingComponent &path_c(unsigned int entity);
        NameComponent &name_c(unsigned int entity);

    void reset_entities();
    void reset_islands();

    // communication with editor
    void update_world();

    unsigned int create_entity();
    bool load_entity(const std::string &name);
    void create_root();
    void delete_entity_raw(unsigned int entity);

    // getters
    StaticGrid &grid();
    NavMesh &navmesh();
    std::vector<Island> &islands();

private:
    class impl;
    std::unique_ptr<impl> pimpl;    

    void find_entities_fixed();
    void find_entities_draw();
};

namespace for_gameworld {
    inline bool has_component(bset entity, bset component_mask) {
        return (entity & component_mask) == component_mask;
    }
}


#endif //SINGING_WIND_GAMEWORLD_H
