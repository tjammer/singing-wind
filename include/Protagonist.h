    //
    // Created by jammer on 11/04/17.
    //

#ifndef SINGING_WIND_PROTAGONIST_H
#define SINGING_WIND_PROTAGONIST_H

#include "WindDefs.h"
#include "Collision.h"
#include <memory>

class GameWorld;

namespace protagonist {
    void handle_inputs(GameWorld &world, unsigned int entity);

    const WVec c_capsule_size = {15, 30};

    void to_ground(GameWorld &world, unsigned int entity);
    void on_ground(GameWorld &world, unsigned int entity);
    void to_falling(GameWorld &world, unsigned int entity_name);
    void falling(GameWorld &world, unsigned int entity);
    void to_flying(GameWorld &world, unsigned int entity);
    void flying(GameWorld &world, unsigned int entity);
    void to_flying_accel(GameWorld &world, unsigned int entity);
    void flying_accel(GameWorld &world, unsigned int entity);
}

#endif //SINGING_WIND_PROTAGONIST_H
