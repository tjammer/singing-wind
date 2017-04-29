//
// Created by jammer on 11/04/17.
//

#ifndef SINGING_WIND_ENTITIES_H
#define SINGING_WIND_ENTITIES_H

#include <unordered_map>
#include "GameWorld.h"
#include <functional>
#include "Protagonist.h"
#include "ColShape.h"

unsigned int create_root(GameWorld &game_world, const WVec &position, unsigned int parent);

const std::unordered_map<SpecificShape, std::shared_ptr<ColShape>> specific_shapes = {
        {SpecificShape::ProtagonistCapsule, std::shared_ptr<ColShape>(new ColCapsule(15, 30))}
};

const std::unordered_map<StaticColResponse, std::function<void(const ColResult &, GameWorld &, unsigned int)>> static_col_responses = {
        {StaticColResponse::Actor, Protagonist::on_static_collision}
};

#endif //SINGING_WIND_ENTITIES_H
