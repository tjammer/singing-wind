//
// Created by jammer on 10/04/17.
//

#ifndef SINGING_WIND_COMPONENTS_H
#define SINGING_WIND_COMPONENTS_H

#include <WindDefs.h>
#include "ColShape.h"
#include <memory>

struct PosComponent {
    WVec position = {0, 0};
    float rotation = 0.f;
    WTransform global_transform = WTransform::Identity;
    unsigned int parent = 0;
};

struct DebugComponent {
    std::shared_ptr<ColShape> shape;
};

// for serializing and identification
using IdComponent = std::string;


#endif //SINGING_WIND_COMPONENTS_H
