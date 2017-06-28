//
// Created by jammer on 11/04/17.
//

#ifndef SINGING_WIND_ENTITIES_H
#define SINGING_WIND_ENTITIES_H

#include "WindDefs.h"

class GameWorld;

namespace Entities {
    unsigned int create_root(GameWorld &game_world, const WVec &position, unsigned int parent);
}

#endif //SINGING_WIND_ENTITIES_H
