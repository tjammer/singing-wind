#include "CollisionComponent.h"
#include "Actor.h"
#include "TestEnemy.h"
#include "imgui.h"
#include "Components.h"
#include "GameWorld.h"
#include "EntityEditor.h"
#include <unordered_map>

std::unordered_map<StaticColResponse, std::function<void(GameWorld &, unsigned int)>> c_static_col_responses = {
        {StaticColResponse::Actor, actor::on_static_collision},
        {StaticColResponse::SimpleFlyer, TestEnemy::on_static_collision}
};

std::unordered_map<DynColResponse, std::function<void(GameWorld &, unsigned int)>> c_dyn_col_responses = {
    {DynColResponse::Actor, actor::on_dynamic_collision}
};

std::function<void(GameWorld &, unsigned int)> get_static_col_response(const StaticColResponse &scr) {
    return c_static_col_responses[scr];
}

std::function<void(GameWorld &, unsigned int entity)> get_dynamic_col_response(const DynColResponse &scr) {
    return c_dyn_col_responses[scr];
}

const std::map<DynColResponse, const char*> c_dyn_col_response_string = {
    {DynColResponse::Actor, "Actor"},
    {DynColResponse::Projectile, "Projectile"}
};

auto dyn_col_responses = get_enum_string_array(c_dyn_col_response_string);

void entity_edit_dyn_cols(GameWorld &world, unsigned int entity) {
    using namespace ImGui;
    if (world.entities()[entity].test(CDynCol) and CollapsingHeader("DynCol")) {
        auto &dc = world.dyn_col_c(entity);
        int response = static_cast<int>(dc.col_response);
        if (Combo("response", &response, dyn_col_responses.data(), dyn_col_responses.size())) {
            dc.col_response = static_cast<DynColResponse>(response);
        }
    }
}
