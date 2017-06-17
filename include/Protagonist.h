//
// Created by jammer on 11/04/17.
//

#ifndef SINGING_WIND_PROTAGONIST_H
#define SINGING_WIND_PROTAGONIST_H

#include "WindDefs.h"
#include "Collision.h"
#include <memory>

class GameWorld;

namespace Protagonist {

    void on_static_collision(const ColResult &result, GameWorld &world, unsigned int entity);

    void handle_inputs(GameWorld &world, unsigned int entity);

    const WVec c_capsule_size = {15, 30};
}

#endif //SINGING_WIND_PROTAGONIST_H
