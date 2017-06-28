#include "CollisionComponent.h"
#include "Protagonist.h"
#include "TestEnemy.h"
#include <unordered_map>

const std::unordered_map<StaticColResponse, std::function<void(const ColResult &, GameWorld &, unsigned int)>> c_static_col_responses = {
        {StaticColResponse::Actor, protagonist::on_static_collision},
        {StaticColResponse::SimpleFlyer, TestEnemy::on_static_collision}
};

std::function<void(const ColResult &, GameWorld &, unsigned int)> get_static_col_response(const StaticColResponse &scr) {
    return c_static_col_responses.at(scr);
}
