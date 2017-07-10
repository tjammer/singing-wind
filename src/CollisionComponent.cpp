#include "CollisionComponent.h"
#include "Protagonist.h"
#include "TestEnemy.h"
#include <unordered_map>

std::unordered_map<StaticColResponse, std::function<void(GameWorld &, unsigned int)>> c_static_col_responses = {
        {StaticColResponse::Actor, protagonist::on_static_collision},
        {StaticColResponse::SimpleFlyer, TestEnemy::on_static_collision}
};

std::unordered_map<DynColResponse, std::function<void(GameWorld &, unsigned int)>> c_dyn_col_responses = {};

std::function<void(GameWorld &, unsigned int)> get_static_col_response(const StaticColResponse &scr) {
    return c_static_col_responses[scr];
}

std::function<void(GameWorld &, unsigned int entity)> get_dynamic_col_response(const DynColResponse &scr) {
    return c_dyn_col_responses[scr];
}
