#include "CollisionComponent.h"
#include "Actor.h"
#include "Components.h"
#include "EntityEditor.h"
#include "GameWorld.h"
#include "HurtBoxComponent.h"
#include "PatrolComponent.h"
#include "SimpleFlyer.h"
#include "SkillDisk.h"
#include "imgui.h"
#include <unordered_map>

using response_func = std::function<void(GameWorld&, const unsigned int)>;

const response_func c_static_col_responses[static_cast<size_t>(
  StaticColResponse::state_count)] = { actor::on_static_collision,
                                       simpleflyer::on_static_collision,
                                       disk_projectile::on_static_collision };

const response_func c_dyn_col_responses[static_cast<size_t>(
  DynColResponse::state_count)] = { actor::on_dynamic_collision,
                                    nullptr,
                                    hurtbox::on_dynamic_collision,
                                    alert_bubble::on_dynamic_collision };

std::function<void(GameWorld&, unsigned int)>
get_static_col_response(const StaticColResponse& scr)
{
  return c_static_col_responses[static_cast<size_t>(scr)];
}

std::function<void(GameWorld& world, unsigned int entity)>
get_dynamic_col_response(const DynColResponse& scr)
{
  return c_dyn_col_responses[static_cast<size_t>(scr)];
}

const std::map<StaticColResponse, const char*> staticcolresponse_string = {
  { StaticColResponse::Actor, "Actor" },
  { StaticColResponse::SimpleFlyer, "SimpleFlyer" },
  { StaticColResponse::DiskProjectile, "DiskProjectile" }
};

const std::map<DynColResponse, const char*> c_dyn_col_response_string = {
  { DynColResponse::Actor, "Actor" },
  { DynColResponse::Projectile, "Projectile" },
  { DynColResponse::HurtBox, "HurtBox" },
  { DynColResponse::AlertBubble, "AlerBubble" }
};

auto dyn_col_responses = get_enum_string_array(c_dyn_col_response_string);

void
entity_edit_dyn_cols(GameWorld& world, unsigned int entity)
{
  using namespace ImGui;
  if (world.entities()[entity].test(CDynCol) && CollapsingHeader("DynCol")) {
    auto& dc = world.get<DynamicColComponent>(entity);
    int response = static_cast<int>(dc.col_response_name);
    if (Combo("response",
              &response,
              dyn_col_responses.data(),
              dyn_col_responses.size())) {
      set_dynamic_col(dc, static_cast<DynColResponse>(response));
    }
  }
}

void
set_static_col(StaticColComponent& sc, const StaticColResponse& res)
{
  sc.col_response_name = res;
  sc.col_response = get_static_col_response(res);
}

void
set_dynamic_col(DynamicColComponent& dc, const DynColResponse& res)
{
  dc.col_response_name = res;
  dc.col_response = get_dynamic_col_response(res);
}

const auto col_responses = get_enum_string_array(staticcolresponse_string);

void
entity_edit_static_cols(GameWorld& world, unsigned int entity)
{
  using namespace ImGui;
  if (world.entities()[entity].test(CStaticCol) &&
      CollapsingHeader("static collision")) {
    auto& sc = world.get<StaticColComponent>(entity);
    int response = static_cast<int>(sc.col_response_name);
    if (Combo(
          "response", &response, col_responses.data(), col_responses.size())) {
      set_static_col(sc, static_cast<StaticColResponse>(response));
    }
  }
}
