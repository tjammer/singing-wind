#ifndef AICOMPONENT_H
#define AICOMPONENT_H

#include <vector>
#include <memory>
#include "WBehaviourTree.h"

class GameWorld;

enum class AIStateName : int {
    Idle,
    Pursuit,
    Attack,
    Flee,
    Return,
    NotInit,
    state_count
};

enum class AITypeName : int {
    TestEnemy
};

class AIState {
public:
    virtual void enter(GameWorld &, unsigned int) = 0;
    virtual void tick(GameWorld &, unsigned int, float dt) = 0;
    virtual void leave(GameWorld &, unsigned int) = 0;
    virtual void do_input(GameWorld&, unsigned int) = 0;
    virtual AIStateName name() = 0;
};

class AIType {
public:
    virtual std::unique_ptr<AIState> transition(GameWorld &, unsigned int) = 0;
    virtual void init(GameWorld &, unsigned int) = 0;
    virtual AIStateName name() = 0;
};

struct AIComponent {
    std::unique_ptr<AIType> type;
    std::unique_ptr<AIState> state;
    behaviour_tree::BehaviourTree btree;
};

namespace ai {

    const float c_pathfinding_time = 1.f;
    const float c_flee_time = 10.f;

    void entity_edit(GameWorld &, unsigned int);
    void init_ai_type(GameWorld &, unsigned int, AITypeName);
}

#endif /* AICOMPONENT_H */
