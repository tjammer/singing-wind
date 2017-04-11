//
// Created by jammer on 11/04/17.
//

#ifndef SINGING_WIND_ENTITIES_H
#define SINGING_WIND_ENTITIES_H

#include <map>
#include "GameWorld.h"
#include <functional>

unsigned int create_root(GameWorld &game_world, const WVec &position, unsigned int parent);
unsigned int create_coll_test(GameWorld &world, const WVec &position, unsigned int parent);

const std::map<std::string, std::function<unsigned int(GameWorld&, const WVec&, unsigned int)>> all_entities = {
        {"root", create_root},
        {"CollisionTest", create_coll_test}
};


#endif //SINGING_WIND_ENTITIES_H
