//
// Created by jammer on 11/04/17.
//

#ifndef SINGING_WIND_DEBUGDRAWSYSTEM_H
#define SINGING_WIND_DEBUGDRAWSYSTEM_H

#include "WindDefs.h"

class GameWorld;

struct DebugDrawSystem {
    const bset components;
    void update(GameWorld &world, sf::RenderWindow &window);

    DebugDrawSystem();
    ~DebugDrawSystem() = default;
};


#endif //SINGING_WIND_DEBUGDRAWSYSTEM_H
