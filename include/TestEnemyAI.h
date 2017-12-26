#ifndef TESTENEMYAI_H
#define TESTENEMYAI_H

#include "WBehaviourTree.h"

class GameWorld;

namespace testenemy {
    behaviour_tree::BehaviourTree get_tree(GameWorld&, unsigned int);
};


#endif /* TESTENEMYAI_H */
