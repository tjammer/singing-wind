//
// Created by jammer on 10/04/17.
//

#ifndef SINGING_WIND_COMPONENTS_H
#define SINGING_WIND_COMPONENTS_H

#include <map>

enum Components : int {
    CPosition,
    CMove,
    CStaticCol,
    CAppearance,
    CInput,
    CDebugDraw,
    CGroundMove,
    CFall,
    CFly,
    CPathing,
    CSimpleFly,
    CSkill,
    CDynCol,
    CTag,
    CColShape,
    CLifeTime,
    CStatusEffect,
    CAI,
    CPatrol
};
const std::map<Components, const char*> components_string = {
    {CPosition, "CPosition"},
    {CMove, "CMove"},
    {CStaticCol, "CStaticCol"},
    {CAppearance, "CAppearance"},
    {CInput, "CInput"},
    {CDebugDraw, "CDebugDraw"},
    {CGroundMove, "CGroundMove"},
    {CFall, "CFall"},
    {CFly, "CFly"},
    {CPathing, "CPathing"},
    {CSimpleFly, "CSimpleFly"},
    {CSkill, "CSkill"},
    {CDynCol, "CDynCol"},
    {CTag, "CTag"},
    {CColShape, "ColShape"},
    {CStatusEffect, "StatusEffect"},
    {CAI, "AI"},
    {CPatrol, "Patrol"}
};

struct DebugComponent {
    int f;
};

// for serializing and identification
using NameComponent = std::string;

#endif //SINGING_WIND_COMPONENTS_H
