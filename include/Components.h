//
// Created by jammer on 10/04/17.
//

#ifndef SINGING_WIND_COMPONENTS_H
#define SINGING_WIND_COMPONENTS_H

#include "WindDefs.h"

#include <memory>
#include <deque>
#include <map>

class GameWorld;
class ColShape;
struct NavLink;

enum Components : int {
    CPosition,
    CMove,
    CStaticCol,
    CAppearance,
    CInput,
    CDebugDraw,
    CGroundMove,
    CJump,
    CFly,
    CPathing,
    CSimpleFly
};
const std::map<Components, const char*> components_string = {
    {CPosition, "CPosition"},
    {CMove, "CMove"},
    {CStaticCol, "CStaticCol"},
    {CAppearance, "CAppearance"},
    {CInput, "CInput"},
    {CDebugDraw, "CDebugDraw"},
    {CGroundMove, "CGroundMove"},
    {CJump, "CJump"},
    {CFly, "CFly"},
    {CPathing, "CPathing"},
    {CSimpleFly, "CSimpleFly"}
};

struct DebugComponent {
    std::shared_ptr<ColShape> shape;
};

// for serializing and identification
using NameComponent = std::string;

using TagComponent = bset;

enum class Tags : int {
    Actor,
    Protagonist,
    Enemy
};

#endif //SINGING_WIND_COMPONENTS_H
