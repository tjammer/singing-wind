#include "InputComponent.h"
#include "Protagonist.h"
#include "TestEnemy.h"

#include <unordered_map>

const std::unordered_map<InputFunc, std::function<void(GameWorld &world, unsigned int entity)>> c_input_funcs = {
        {InputFunc::Protagonist, protagonist::handle_inputs},
        {InputFunc::AI, TestEnemy::handle_inputs}
};

std::function<void(GameWorld &world, unsigned int entity)> get_input_func(const InputFunc &ip) {
    return c_input_funcs.at(ip);
}
