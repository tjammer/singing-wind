#ifndef CINPUT_H
#define CINPUT_H

#include "WindDefs.h"

#include <map>
#include <deque>
#include <functional>

class GameWorld;

const int c_input_buffer_length = 15;

enum class InputFunc : int {
    Protagonist,
    AI // needs AI component to work
};
const std::map<InputFunc, const char*> inputfunc_string = {
    {InputFunc::Protagonist, "Protagonist"},
    {InputFunc::AI, "AI"}
};

struct InputComponent {
    std::deque<int> direction = std::deque<int>(c_input_buffer_length);
    int last_dir;
    std::deque<bool> jump = std::deque<bool>(c_input_buffer_length);
    std::deque<WVec> mouse = std::deque<WVec>(c_input_buffer_length);
    std::deque<bool> wings = std::deque<bool>(c_input_buffer_length);
    InputFunc input_func;
};

std::function<void(GameWorld &world, unsigned int entity)> get_input_func(const InputFunc&);

#endif /* CINPUT_H */
