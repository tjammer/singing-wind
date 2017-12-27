#ifndef GOTOENEMY_H
#define GOTOENEMY_H

#include "WBehaviourTree.h"

enum class PathfindingStatus;
class GameWorld;
struct PathingComponent;

class GoToEnemy : public behaviour_tree::Behaviour {
    public:
        GoToEnemy(const GameWorld &world, unsigned int entity, float radius, PathingComponent &path_c) :
            m_world(world), m_entity(entity), m_radius(radius), m_path_c(path_c) {}
        void enter() override;
        behaviour_tree::Status update() override;

    private:
        PathfindingStatus m_status;
        const GameWorld &m_world;
        unsigned int m_entity;
        float m_radius;
        PathingComponent &m_path_c;
};


#endif /* GOTOENEMY_H */
