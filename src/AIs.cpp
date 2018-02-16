#include "AIs.h"
#include "Behaviours.h"
#include "GameWorld.h"
#include "AIComponent.h"

behaviour_tree::BehaviourTree
testenemy::get_tree(GameWorld& world, unsigned int entity)
{
  using namespace behaviour_tree;
  auto tree = WBehaviourTreeBuilder<BehaviourTree>(AITreeType::TestEnemy)
                .composite<Sequence>()
                .node<IsAggro>(world, entity)
                .end()
                .decorator<RunForFrames>(1200)
                .node<HoldDistance>(world, entity, 500, 200)
                .end()
                .end()
                .node<UseSkill>(world, entity)
                .end()
                .end()
                .decorator<MakeAggroIf>(world, entity)
                .node<EnemyInRange>(world, entity, 400)
                .end()
                .end()

                .node<Idle>(world, entity)
                .end()
                .end();

  return tree;
};

behaviour_tree::BehaviourTree
wandercharge::get_tree(GameWorld& world, unsigned int entity)
{
  using namespace behaviour_tree;
  auto tree = WBehaviourTreeBuilder<BehaviourTree>(AITreeType::WanderCharge)
                .composite<Sequence>()
                .node<EnemyInRange>(world, entity, 400)
                .end()
                .node<UseSkill>(world, entity)
                .end()
                .end()
                .node<Wander>(world, entity, 1.3, .4)
                .end()
                .end();
  return tree;
}

behaviour_tree::BehaviourTree
stillfirepattern::get_tree(GameWorld& world, unsigned int entity)
{
  using namespace behaviour_tree;
  auto tree = WBehaviourTreeBuilder<BehaviourTree>(AITreeType::StillFirePattern)
                .composite<Sequence>()
                .node<EnemyInRange>(world, entity, 400)
                .end()
                .node<UseSkill>(world, entity)
                .end()
                .end()
                .node<Idle>(world, entity)
                .end()
                .end();
  return tree;
}
