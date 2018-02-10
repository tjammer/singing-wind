#ifndef CSTATICCOL_H
#define CSTATICCOL_H

#include "Collision.h"
#include "Components.h"
#include <map>
#include <functional>
#include <memory>

class GameWorld;

enum class StaticColResponse : int
{
  Actor,
  SimpleFlyer,
  DiskProjectile,
  DeleteAfterCol,
  state_count
};

enum class DynColResponse : int
{
  Actor,
  Projectile,
  HurtBox,
  AlertBubble,
  state_count
};

struct ColShapeComponent
{
  std::shared_ptr<ColShape> shape;
  static const Components type = CColShape;
};

struct StaticColComponent
{
  ColResult col_result;
  StaticColResponse col_response_name;
  std::function<void(GameWorld&, unsigned int)> col_response = nullptr;
  static const Components type = CStaticCol;
};

struct DynamicColComponent
{
  ColResult col_result;
  unsigned int collided;
  DynColResponse col_response_name;
  std::function<void(GameWorld&, unsigned int)> col_response = nullptr;
  static const Components type = CDynCol;
};

std::function<void(GameWorld&, unsigned int)>
get_static_col_response(const StaticColResponse&);
std::function<void(GameWorld&, unsigned int)>
get_dynamic_col_response(const DynColResponse&);

void
delete_after_col(GameWorld&, unsigned int);

void
set_static_col(StaticColComponent&, const StaticColResponse&);
void
set_dynamic_col(DynamicColComponent&, const DynColResponse&);

void
entity_edit_dyn_cols(GameWorld& world, unsigned int entity);
void
entity_edit_static_cols(GameWorld& world, unsigned int entity);
#endif /* CSTATICCOL_H */
