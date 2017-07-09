#ifndef CSTATICCOL_H
#define CSTATICCOL_H

#include "Collision.h"
#include <map>
#include <functional>
#include <memory>

class GameWorld;

enum class StaticColResponse : int {
    Actor,
    SimpleFlyer
};

enum class DynColResponse : int {
    Actor,
    Projectile
};

const std::map<StaticColResponse, const char*> staticcolresponse_string = {
    {StaticColResponse::Actor, "Actor"},
    {StaticColResponse::SimpleFlyer, "SimpleFlyer"}
};

struct StaticColComponent {
    ColResult col_result;
    std::shared_ptr<ColShape> shape;
    StaticColResponse col_response;
};

struct DynamicColComponent {
    ColResult col_result;
    std::shared_ptr<ColShape> shape;
    DynColResponse col_response;
};

std::function<void(const ColResult &, GameWorld &, unsigned int)> get_static_col_response(const StaticColResponse&);
std::function<void(const ColResult &, GameWorld &, unsigned int)> get_dynamic_col_response(const DynColResponse&);

#endif /* CSTATICCOL_H */
