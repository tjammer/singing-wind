#ifndef CSTATICCOL_H
#define CSTATICCOL_H

#include "Collision.h"
#include <map>
#include <functional>
#include <memory>

class GameWorld;

enum class StaticColResponse : int {
    Actor,
    SimpleFlyer,
    state_count
};

enum class DynColResponse : int {
    Actor,
    Projectile,
    HurtBox,
    state_count
};

const std::map<StaticColResponse, const char*> staticcolresponse_string = {
    {StaticColResponse::Actor, "Actor"},
    {StaticColResponse::SimpleFlyer, "SimpleFlyer"}
};

struct ColShapeComponent {
    std::shared_ptr<ColShape> shape;
};

struct StaticColComponent {
    ColResult col_result;
    StaticColResponse col_response;
};

struct DynamicColComponent {
    ColResult col_result;
    unsigned int collided;
    DynColResponse col_response;
};

std::function<void(GameWorld &, unsigned int)> get_static_col_response(const StaticColResponse&);
std::function<void(GameWorld &, unsigned int)> get_dynamic_col_response(const DynColResponse&);

void entity_edit_dyn_cols(GameWorld &world, unsigned int entity);
#endif /* CSTATICCOL_H */
