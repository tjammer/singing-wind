#ifndef AICOMPONENT_H
#define AICOMPONENT_H

#include <functional>
#include <vector>
#include <memory>

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

enum class AIType : int {
    TestEnemy
};

class AIState {
public:
    virtual void enter(GameWorld &, unsigned int) = 0;
    virtual void tick(GameWorld &, unsigned int) = 0;
    virtual void leave(GameWorld &, unsigned int) = 0;
    virtual AIStateName name() = 0;
    std::unique_ptr<AIState> transition(GameWorld &, unsigned int);
};

struct AIComponent {
    AIType type = AIType::TestEnemy;
    AIStateName state = AIStateName::NotInit;
    std::vector<int> msg_data;
    float timer = 0;
};

namespace ai {

    using func = std::function<void(GameWorld&, unsigned int)>;
    using trans_func = std::function<bool(GameWorld &, unsigned int)>;

    func get_func(AIStateName);
    trans_func get_trans_func(AIStateName);
    func get_to_func(AIStateName);
    const std::vector<AIStateName> & get_trans_states(const AIType &, const AIStateName &);

    void do_input(GameWorld &, unsigned int);

    const float c_pathfinding_time = 1.f;
    const float c_flee_time = 10.f;

    void entity_edit(GameWorld &, unsigned int);
}

#endif /* AICOMPONENT_H */
