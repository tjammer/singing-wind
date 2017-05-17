//
// Created by jammer on 11/04/17.
//

#ifndef SINGING_WIND_PROTAGONIST_H
#define SINGING_WIND_PROTAGONIST_H

#include "WindDefs.h"
#include "Collision.h"
#include <memory>

class GameWorld;
class InputComponent;

namespace Protagonist {

    void on_static_collision(const ColResult &result, GameWorld &world, unsigned int entity);

    void handle_inputs(InputComponent &ic, const WVec &mouse);

    unsigned int create_player(GameWorld &world, const WVec &pos, unsigned int parent);

    std::shared_ptr<ColShape> get_shape();
}

#endif //SINGING_WIND_PROTAGONIST_H
