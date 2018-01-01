#ifndef STEERING_H
#define STEERING_H

#include "WindDefs.h"

class SteeringBuilder {
public:
    SteeringBuilder(const WVec& self_pos, const WVec& self_vel, float max_vel, float cohesion_length=50);
    SteeringBuilder& seek(const WVec &position);
    SteeringBuilder& flee(const WVec &position);
    SteeringBuilder& arrive(const WVec &position, float radius);
    SteeringBuilder& flock(const WVec &position);
    void add_seek(const WVec &position);
    void add_flee(const WVec &position);
    void add_arrive(const WVec &position, float radius);
    void add_flock(const WVec &position);
    void add_cohesion(const WVec &position);
    WVec end(float force);

private:
    const WVec& m_pos;
    const WVec m_vel;
    const float m_max_vel;
    const float m_cohesion_length;

    WVec m_seek{0, 0};
    WVec m_separate{0, 0};
    WVec m_cohesion;

    int m_flock_count{0};
};

#endif /* STEERING_H */
