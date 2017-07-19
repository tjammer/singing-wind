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

    // start state funcs
    void to_ground(GameWorld &world, unsigned int entity);
    void to_falling(GameWorld &world, unsigned int entity_name);
    void to_flying(GameWorld &world, unsigned int entity);
    void to_flying_accel(GameWorld &world, unsigned int entity);
    // accel funcs
    void on_ground(GameWorld &world, unsigned int entity);
    void falling(GameWorld &world, unsigned int entity);
    void flying(GameWorld &world, unsigned int entity);
    void flying_accel(GameWorld &world, unsigned int entity);
    
    // transistion funcs
    bool transition_ground(GameWorld &world, unsigned int entity);
    bool transistion_falling(GameWorld &world, unsigned int entity);
    bool transition_flying(GameWorld &world, unsigned int entity);
    bool transition_flying_accel(GameWorld &world, unsigned int entity);
}

#endif //SINGING_WIND_PROTAGONIST_H
