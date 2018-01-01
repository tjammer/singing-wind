#ifndef GOTOENEMY_H
#define GOTOENEMY_H

#include "WBehaviourTree.h"

enum class PathfindingStatus;
class GameWorld;
struct PathingComponent;

class GoToEnemy : public behaviour_tree::Behaviour {
    public:
        GoToEnemy(GameWorld &, unsigned int);
        void enter() override;
        behaviour_tree::Status update() override;
        void leave(behaviour_tree::Status) override;

    private:
        PathfindingStatus m_status;
        GameWorld &m_world;
        unsigned int m_entity;
        float m_radius;
};


#endif /* GOTOENEMY_H */
