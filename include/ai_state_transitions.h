#ifndef AI_STATE_TRANSITIONS_H
#define AI_STATE_TRANSITIONS_H

#include <functional>

class GameWorld;

namespace ai_to_funcs {
    void to_idle(GameWorld &, unsigned int);
    void to_pursuit(GameWorld &, unsigned int);
    void to_attack(GameWorld &, unsigned int);
    void to_flee(GameWorld &, unsigned int);
    void to_return(GameWorld &, unsigned int);
};

namespace ai_transitions {
    bool trans_idle(GameWorld &, unsigned int); // after pursuit
    bool trans_pursuit(GameWorld &, unsigned int); // needs to recover from attack
    bool trans_attack(GameWorld &, unsigned int);
    bool trans_flee(GameWorld &, unsigned int);
    bool trans_return(GameWorld &, unsigned int);
    bool trans_init(GameWorld &, unsigned int);
}

#endif /* AI_STATE_TRANSITIONS_H */
