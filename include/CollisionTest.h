//
// Created by tobi on 4/3/17.
//

#ifndef SINGING_WIND_COLLISIONTEST_H
#define SINGING_WIND_COLLISIONTEST_H

#include "WindDefs.h"

class GameWorld;

unsigned int create_coll_test(GameWorld &world, const WVec &position, unsigned int parent);

struct ColTestSystem {
    const bset components;
    void update(GameWorld &world, const WVec &mouse);

    ColTestSystem();
    ~ColTestSystem() = default;
};

#endif //SINGING_WIND_COLLISIONTEST_H
