#ifndef AI_INPUT_FUNCS_H
#define AI_INPUT_FUNCS_H

#include <functional>

class GameWorld;
struct MoveComponent;

namespace ai_input {
    using func = std::function<void(GameWorld &, unsigned int)>;
    
    void simple_flying(GameWorld &, unsigned int);
}

#endif /* AI_INPUT_FUNCS_H */
