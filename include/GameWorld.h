//
// Created by jammer on 07/04/17.
//

#ifndef SINGING_WIND_GAMEWORLD_H
#define SINGING_WIND_GAMEWORLD_H

#include "Island.h"
#include "ColGrid.h"
#include "Components.h"

// physics constants
const float c_drag = 0.0026;
const float c_friction = 0.0026;
const float c_max_floor_angle = 0.7;
const float c_gravity = 1900;
const float c_jump_tolerance = 0.1f;

// implements the ecs
class GameWorld {
public:
    GameWorld();
    ~GameWorld() = default;

    // maybe a timer?
    void step_fixed(float dt, const WVec &mouse);
    void pre_draw(float dt);
    void draw(sf::RenderWindow& window);

    // ecs stuff
    std::vector<bset> m_entities;

        // components
        std::unordered_map<unsigned int, PosComponent> m_pos_c;
        std::unordered_map<unsigned int, DebugComponent> m_debug_c;
        std::unordered_map<unsigned int, InputComponent> m_input_c;
        std::unordered_map<unsigned int, MoveComponent> m_move_c;
        std::unordered_map<unsigned int, StaticColComponent> m_static_col_c;
        std::unordered_map<unsigned int, GroundMoveComponent> m_ground_move_c;
        std::unordered_map<unsigned int, JumpComponent> m_jump_c;
        std::unordered_map<unsigned int, FlyComponent> m_fly_c;
        std::vector<IdComponent> m_id_c;

    // communication with editor
    std::vector<Island> &get_islands_ref() {return m_islands;};
    void update_triangles();

    unsigned int create_entity();
    bool load_entity(const std::string &name);

    // members
    StaticGrid m_grid;
    std::vector<Island> m_islands;
private:
    std::vector<unsigned int> m_input_ents;
    std::vector<unsigned int> m_move_ents;
    std::vector<unsigned int> m_debug_draw_ents;
    std::vector<unsigned int> m_ground_move_ents;
    std::vector<unsigned int> m_fly_ents;
    std::vector<unsigned int> m_static_col_ents;

    void find_entities_fixed();
    void find_entities_draw();
};

namespace for_gameworld {
    inline bool has_component(bset entity, bset component_mask) {
        return (entity & component_mask) == component_mask;
    }
}


#endif //SINGING_WIND_GAMEWORLD_H
