//
// Created by tobi on 4/19/17.
//

#ifndef SINGING_WIND_CAMERA_H
#define SINGING_WIND_CAMERA_H

#include "GameWorld.h"
#include "Engine.h"

struct Camera {
    unsigned int m_entity_to_follow;
    void update(GameWorld &game_world, Engine &engine);
};


#endif //SINGING_WIND_CAMERA_H
