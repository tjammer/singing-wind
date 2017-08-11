#include "InputComponent.h"
#include "Protagonist.h"
#include "AIComponent.h"
#include <unordered_map>

using input_func = std::function<void(GameWorld &world, unsigned int entity)>;

const input_func c_input_funcs[static_cast<size_t>(InputFunc::state_count)] = {
    protagonist::handle_inputs,
    ai::do_input
}; 

std::function<void(GameWorld &world, unsigned int entity)> get_input_func(const InputFunc &ip) {
    return c_input_funcs[static_cast<size_t>(ip)];
}
