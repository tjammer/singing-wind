#ifndef AICOMPONENT_H
#define AICOMPONENT_H

#include <vector>
#include <memory>
#include "WBehaviourTree.h"

class GameWorld;

enum class AIState : int {
    Idle,
    Pursuit,
    Attack,
    Flee,
    Return,
    NotInit,
    state_count
};

enum class AITreeType : int {
    None,
    TestEnemy
};

struct AIComponent {
    behaviour_tree::BehaviourTree btree;
};

namespace ai {

    const float c_pathfinding_time = 1.f;
    const float c_flee_time = 10.f;

    void entity_edit(GameWorld &, unsigned int);
    void init_ai_type(GameWorld &, unsigned int, AITreeType);
}

#endif /* AICOMPONENT_H */
