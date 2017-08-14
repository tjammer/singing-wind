#ifndef PATROLCOMPONENT_H
#define PATROLCOMPONENT_H

#include "WindDefs.h"

struct PatrolComponent {
    WVec patrol_point;
};

class GameWorld;

namespace alert_bubble {
    void spawn(GameWorld &, unsigned int);
    void on_dynamic_collision(GameWorld &, unsigned int);
}


#endif /* PATROLCOMPONENT_H */
