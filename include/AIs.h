#ifndef AIS_H
#define AIS_H

#include "WBehaviourTree.h"

class GameWorld;

namespace testenemy {
behaviour_tree::BehaviourTree
get_tree(GameWorld&, unsigned int);
};

namespace wandercharge {
behaviour_tree::BehaviourTree
get_tree(GameWorld&, unsigned int);
};

namespace stillfirepattern {
behaviour_tree::BehaviourTree
get_tree(GameWorld&, unsigned int);
};

namespace stilldeploy {
behaviour_tree::BehaviourTree
get_tree(GameWorld&, unsigned int);
};

namespace swarm {
behaviour_tree::BehaviourTree
get_tree(GameWorld&, unsigned int);
};

#endif /* AIS_H */
