#include "TestEnemyAI.h"
#include "GameWorld.h"
#include "Pathfinding.h"
#include "Behaviours.h"

behaviour_tree::BehaviourTree get_tree(GameWorld &world, unsigned int entity) {
    using namespace behaviour_tree;
    auto tree = BehaviourTreeBuilder()
        .node<EnemyInRange>(world, entity, 300, world.path_c(entity))
        .end()
        .decorator<RepeatUntilFail>()
        .end()
    .end();

    return tree;
};
