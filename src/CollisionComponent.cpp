#include "CollisionComponent.h"
#include "Actor.h"
#include "SimpleFlyer.h"
#include "imgui.h"
#include "Components.h"
#include "GameWorld.h"
#include "EntityEditor.h"
#include "HurtBoxComponent.h"
#include "AlertBubble.h"
#include <unordered_map>

using response_func = std::function<void(GameWorld &, const unsigned int)>;

const response_func c_static_col_responses[static_cast<size_t>(StaticColResponse::state_count)] = {
    actor::on_static_collision,
    simpleflyer::on_static_collision
};

const response_func c_dyn_col_responses[static_cast<size_t>(DynColResponse::state_count)] = {
    actor::on_dynamic_collision,
    nullptr,
    hurtbox::on_dynamic_collision,
    alert_bubble::on_dynamic_collision
};

std::function<void(GameWorld &, unsigned int)> get_static_col_response(const StaticColResponse &scr) {
    return c_static_col_responses[static_cast<size_t>(scr)];
}

std::function<void(GameWorld & world, unsigned int entity)> get_dynamic_col_response(const DynColResponse &scr) {
    return c_dyn_col_responses[static_cast<size_t>(scr)];
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
