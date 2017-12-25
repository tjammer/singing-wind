#include "steering.h"
#include "WVecMath.h"

SteeringBuilder::SteeringBuilder(const WVec& self_pos, const WVec& self_vel) :
            m_pos(self_pos), m_vel(w_normalize(self_vel)) {}

SteeringBuilder& SteeringBuilder::seek(const WVec &pos) {
    auto dir = w_normalize(pos - m_pos);
    m_seek += dir - m_vel;
    return *this;
}

SteeringBuilder& SteeringBuilder::flee(const WVec &pos) {
    auto dir = w_normalize(pos - m_pos);
    m_seek -= dir - m_vel;
    return *this;
}

SteeringBuilder& SteeringBuilder::arrive(const WVec &pos, float radius) {
    auto dir = w_normalize(pos - m_pos);
    auto distance = w_magnitude(pos - m_pos);
    auto desired = dir * w_magnitude(m_vel) * distance / radius;
    m_seek += desired - m_vel;
    return *this;
}

SteeringBuilder& SteeringBuilder::flock(const WVec &pos) {
    auto dir = w_normalize(pos - m_pos);
    m_separate -= dir;
    m_flock_count++;

    return *this;
}

WVec SteeringBuilder::end(float force) {
    return w_normalize(m_seek) + w_normalize(m_separate / (float)m_flock_count) * force;
}
