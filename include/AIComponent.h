#ifndef AICOMPONENT_H
#define AICOMPONENT_H

#include <functional>
#include <vector>

class GameWorld;

enum class AIState : int {
    Idle,
    Pursuit,
    Attack,
    Flee,
    Return,
    state_count
};

enum class AIType : int {
    TestEnemy
};

struct AIComponent {
    AIType type = AIType::TestEnemy;
    AIState state = AIState::Pursuit;
    std::vector<int> msg_data;
    float timer = 0;
};

namespace ai {

    using func = std::function<void(GameWorld&, unsigned int)>;
    using trans_func = std::function<bool(GameWorld &, unsigned int)>;

    func get_func(AIState);
    trans_func get_trans_func(AIState);
    func get_to_func(AIState);
    const std::vector<AIState> & get_trans_states(const AIType &, const AIState &);

    void do_input(GameWorld &, unsigned int);

    const float c_pathfinding_time = 1.f;
    const float c_flee_time = 10.f;
}

#endif /* AICOMPONENT_H */
