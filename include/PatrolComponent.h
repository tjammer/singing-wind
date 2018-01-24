#ifndef PATROLCOMPONENT_H
#define PATROLCOMPONENT_H

#include "WindDefs.h"
#include "Components.h"

struct PatrolComponent
{
  WVec patrol_point;
  static const Components type = CPatrol;
};

class GameWorld;

namespace alert_bubble {
void
spawn(GameWorld&, unsigned int);
void
on_dynamic_collision(GameWorld&, unsigned int);
}

namespace patrol {
void
entity_edit(GameWorld&, unsigned int);
}

#endif /* PATROLCOMPONENT_H */
