//
// Created by jammer on 07/04/17.
//

#ifndef SINGING_WIND_GAMEWORLD_H
#define SINGING_WIND_GAMEWORLD_H

#include "WindDefs.h"
#include "Island.h"
#include "ColGrid.h"
#include "Components.h"
#include "CollisionTest.h"
#include "DebugDrawSystem.h"

// implements the ecs
class GameWorld {
public:
    GameWorld();
    ~GameWorld() = default;

    // maybe a timer?
    void step_fixed(const WVec &mouse);
    void pre_draw(float dt);
    void draw(sf::RenderWindow& window);

    // ecs stuff
    std::vector<bset> m_entities;

        // components
        std::vector<PosComponent> m_pos_c;
        std::vector<DebugComponent> m_debug_c;
        std::vector<IdComponent> m_id_c;

        // systems
        ColTestSystem m_test_system;
        DebugDrawSystem m_debug_draw_system;

    // communication with editor
    Island &get_island_ref() {return m_island;};
    void update_triangles();

    unsigned int create_entity();

    // members
    StaticGrid m_grid;
    Island m_island;
private:
};

enum Components {
    CPosition,
    CMovement,
    CCollision,
    CAppearance,
    CInput,
    CDebugDraw
};

template<typename bs1, typename bs2>
bool has_component(bs1 entity, bs2 component_mask)
{
    return (entity & component_mask) == component_mask;
}


#endif //SINGING_WIND_GAMEWORLD_H
