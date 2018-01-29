#include "TestEnemyAI.h"
#include "Behaviours.h"
#include "GameWorld.h"
#include "AIComponent.h"

behaviour_tree::BehaviourTree
testenemy::get_tree(GameWorld& world, unsigned int entity)
{
  using namespace behaviour_tree;
  auto tree = WBehaviourTreeBuilder<BehaviourTree>(AITreeType::TestEnemy)
                .decorator<Inverter>()
                .node<EnemyInRange>(world, entity, 700)
                .end()
                // TODO: maybe do nothing?
                .end()
                //.node<GoToEnemy>(world, entity)
                .node<Wander>(world, entity, 1.3, .6)
                .end()
                .end();

  return tree;
};
