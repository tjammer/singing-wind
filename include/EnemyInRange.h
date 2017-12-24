#ifndef ENEMYINRANGE_H
#define ENEMYINRANGE_H

#include "WBehaviourTree.h"

class GameWorld;
class PathingComponent;

class EnemyInRange : public behaviour_tree::Behaviour {
    public:
        EnemyInRange(const GameWorld& world, unsigned int entity, float radius, PathingComponent& path_c) :
            m_world(world), m_entity(entity), m_radius(radius), m_path_c(path_c) {}
        behaviour_tree::Status update() override;

    private:
        const GameWorld& m_world;
        const unsigned int m_entity;
        const float m_radius;
        PathingComponent& m_path_c;
};


#endif /* ENEMYINRANGE_H */
