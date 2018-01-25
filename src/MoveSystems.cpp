//
// Created by tobi on 4/27/17.
//

#include "MoveSystems.h"
#include "GameWorld.h"
#include "Protagonist.h"
#include "SimpleFlyer.h"
#include "SkillComponent.h"
#include "Components.h"
#include "EntityEditor.h"
#include "imgui.h"
#include <unordered_map>

void
movement::interrupt(GameWorld& world, unsigned int entity)
{
  // TODO: reset skill, reset movement
  auto& mc = world.get<MoveComponent>(entity);
  mc.timer = 0;
  mc.moveset->init(world, entity);
  assert(mc.special_movestate == nullptr);
  skill::reset(world, entity);
}

void
movement::init_moveset(GameWorld& world,
                       unsigned int entity,
                       MoveSetName moveset_name)
{
  auto& mc = world.get<MoveComponent>(entity);
  switch (moveset_name) {
    case MoveSetName::Protagonist: {
      mc.moveset = std::make_unique<ProtagonistMoveSet>();
      break;
    }
    case MoveSetName::TestEnemy: {
      mc.moveset = std::make_unique<SimpleFlyerMoveSet>();
      break;
    }
    case MoveSetName::TimedOnly: {
      mc.moveset = std::make_unique<TimedOnlyMoveSet>();
      break;
    }
  }
  mc.moveset->init(world, entity);
}

void
TimedOnlyMoveSet::init(GameWorld& world, unsigned int entity)
{
  // this should never happen
  // delete entity
  assert(false);
  world.queue_delete(entity);
}

const std::map<MoveSetName, const char*> moveset_string = {
  { MoveSetName::Protagonist, "Protagonist" },
  { MoveSetName::TestEnemy, "TestEnemy" },
  { MoveSetName::TimedOnly, "TimedOnly" },
};

const auto moveset_names = get_enum_string_array(moveset_string);
const auto movestate_names = get_enum_string_array(movestate_string);

void
movement::entity_edit(GameWorld& world, unsigned int entity)
{
  using namespace ImGui;
  if (world.entities()[entity].test(CMove) && CollapsingHeader("movement")) {
    auto& mc = world.get<MoveComponent>(entity);
    float data[2] = { mc.velocity.x, mc.velocity.y };
    if (DragFloat2("velocity", data)) {
      mc.velocity = { data[0], data[1] };
    }
    data[0] = mc.accel.x;
    data[1] = mc.accel.y;
    if (DragFloat2("accel", data)) {
      mc.accel = { data[0], data[1] };
    }
    if (mc.movestate) {
      int movestate = static_cast<int>(mc.movestate->name());
      if (Combo("MoveState",
                &movestate,
                movestate_names.data(),
                movestate_names.size())) {
        // mc.movestate = static_cast<MoveStateName>(movestate);
      }
    }
    int moveset = static_cast<int>(mc.moveset->name());
    if (Combo(
          "MoveSet", &moveset, moveset_names.data(), moveset_names.size())) {
      movement::init_moveset(world, entity, static_cast<MoveSetName>(moveset));
    }
    DragFloat(
      "max turn angle", &mc.c_max_change_angle, .0001f, 0.0f, 0.0f, "%.5f");
    if (DragFloat("mass", &mc.mass)) {
    }
  }
}
