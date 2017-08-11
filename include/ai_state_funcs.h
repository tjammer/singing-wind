#ifndef AI_STATE_FUNCS_H
#define AI_STATE_FUNCS_H

class GameWorld;

namespace ai_funcs {
    void idle_func(GameWorld &, unsigned int);
    void pursuit_func(GameWorld &, unsigned int); // pursue actor
    void attack_func(GameWorld &, unsigned int);
    void flee_func(GameWorld &, unsigned int); // flee to place
    void return_func(GameWorld &, unsigned int); // return to place
}

#endif /* AI_STATE_FUNCS_H */
