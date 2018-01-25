#include "PatrolComponent.h"
#include "AIComponent.h"
#include "ColShape.h"
#include "CollisionComponent.h"
#include "Components.h"
#include "GameWorld.h"
#include "LifeTimeComponent.h"
#include "PosComponent.h"
#include "TagComponent.h"
#include "WindDefs.h"
#include "TagComponent.h"
#include <imgui.h>

namespace alert_bubble {

void
create(GameWorld& world, unsigned int alert, unsigned int parent)
{
  bset comps;
  for (auto i : { CPosition, CColShape, CDynCol, CDebugDraw, CTag }) {
    comps.set(i);
  }
  world.entities()[alert] = comps;
  world.get<NameComponent>(alert).name = "alert bubble";

  // pos
  auto& pc = world.get<PosComponent>(alert);
  pc.parent = parent;
  pc.position = { 0, 0 };
  build_global_transform(world, alert);

  // col shape
  auto& csc = world.get<ColShapeComponent>(alert);
  csc.shape = std::shared_ptr<ColShape>(new ColCircle(400));

  // tags
  auto& tc = world.get<TagComponent>(alert);
  tc.tags.set(static_cast<int>(Tags::AlertBubble));

  // dyn col
  set_dynamic_col(world.get<DynamicColComponent>(alert),
                  DynColResponse::AlertBubble);
}

void
spawn(GameWorld& world, unsigned int entity)
{
  world.queue_create({ create, entity });
}

void
on_dynamic_collision(GameWorld& world, unsigned int entity)
{
  // test tag comp of collider
  auto& dc = world.get<DynamicColComponent>(entity);
  auto& tc = world.get<TagComponent>(dc.collided);
  if (!tc.tags.test(static_cast<int>(Tags::Protagonist))) {
    return;
  }
  // test if was updated -> still in idle
  // auto &pc = world.get<PosComponent>(entity);
  // auto &parent_ac = world.ai_c(pc.parent);
  // TODO: rewrite this
  assert(false);
  // if (world.ai_c(pc.parent).msg_data.size() > 0) {
  //    // last entry is alert bubble entity
  //    parent_ac.msg_data.pop_back();
  //}
  // parent_ac.msg_data.push_back(static_cast<unsigned int>(dc.collided));
  // parent_ac.msg_data.push_back(static_cast<unsigned int>(entity));
}
}

namespace patrol {
void
entity_edit(GameWorld& world, unsigned int entity)
{
  using namespace ImGui;
  if (world.entities()[entity].test(CPatrol) and CollapsingHeader("patrol")) {
    auto& pc = world.get<PosComponent>(entity);
    // TODO: draw line to point
    auto& patrol_c = world.get<PatrolComponent>(entity);
    float pp[2] = { patrol_c.patrol_point.x, patrol_c.patrol_point.y };
    if (DragFloat2("patrol point", pp)) {
      patrol_c.patrol_point = { pp[0], pp[1] };
    }
    if (Button("set point here")) {
      patrol_c.patrol_point = pc.global_position;
    }
  }
}
}
