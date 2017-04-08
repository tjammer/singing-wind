//
// Created by jammer on 07/04/17.
//

#ifndef SINGING_WIND_GAMEWORLD_H
#define SINGING_WIND_GAMEWORLD_H

#include "WindDefs.h"
#include "Island.h"
#include "ColGrid.h"

// implements the ecs
class GameWorld {
public:
    GameWorld();
    ~GameWorld() = default;

    // maybe a timer?
    void step_fixed(const WVec &mouse);
    void pre_draw(float dt);
    void draw(sf::RenderWindow& window) const;

    std::vector<bset> m_entities;

    // components

    // systems

    void load_scene(std::string &name);

    // communication with editor
    Island &get_island_ref() {return m_island;};
    void update_triangles();
private:
    Island m_island;
    StaticGrid m_grid;
    std::shared_ptr<ColShape> m_circle;
    WTransform m_circle_tf;
};

enum Components {
    CPosition,
    CMovement,
    CCollision,
    CAppearance,
    CInput
};


#endif //SINGING_WIND_GAMEWORLD_H
