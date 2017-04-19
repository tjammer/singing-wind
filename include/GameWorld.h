//
// Created by jammer on 07/04/17.
//

#ifndef SINGING_WIND_GAMEWORLD_H
#define SINGING_WIND_GAMEWORLD_H

#include "WindDefs.h"
#include "Island.h"
#include "ColGrid.h"
#include "Components.h"

// physics constants
const float c_drag = 0.0046;
const float c_max_floor_angle = 0.7;
const float c_gravity = 1800;

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
        std::vector<PosComponent> m_pos_c;
        std::vector<DebugComponent> m_debug_c;
        std::vector<IdComponent> m_id_c;
        std::vector<InputComponent> m_input_c;
        std::vector<MoveComponent> m_move_c;
        std::vector<StaticColComponent> m_static_col_c;

    // communication with editor
    std::vector<Island> &get_islands_ref() {return m_islands;};
    void update_triangles();

    unsigned int create_entity();

    // members
    StaticGrid m_grid;
    std::vector<Island> m_islands;
private:
};

template<typename bs1, typename bs2>
bool has_component(bs1 entity, bs2 component_mask) {
    return (entity & component_mask) == component_mask;
}


#endif //SINGING_WIND_GAMEWORLD_H
