#ifndef AICOMPONENT_H
#define AICOMPONENT_H

#include "Components.h"
#include <vector>
#include <memory>
#include "WBehaviourTree.h"

class GameWorld;

enum class AIState : int
{
  Idle,
  Pursuit,
  Attack,
  Flee,
  Return,
  NotInit,
  state_count
};

enum class AITreeType : int
{
  None,
  TestEnemy,
  WanderCharge,
  StillFirePattern,
  StillDeploy,
  Swarmer
};

struct AIComponent
{
  behaviour_tree::BehaviourTree btree;
  static const Components type = CAI;
  float aggro_time = 0;
  bool is_aggro = false;
};

namespace ai {

const float c_pathfinding_time = 1.f;
const float c_flee_time = 10.f;
const float c_aggro_time = 15.f;

void
make_aggro(AIComponent&);

void
entity_edit(GameWorld&, unsigned int);
void
init_ai_type(GameWorld&, unsigned int, AITreeType);
}

#endif /* AICOMPONENT_H */
