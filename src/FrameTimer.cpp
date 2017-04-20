//
// Created by jammer on 20/04/17.
//

#include "FrameTimer.h"

void FrameTimer::reset() {
    m_fixed_counter = 0;
    m_elapsed = m_timer.restart();
}

void FrameTimer::update() {
    m_elapsed = m_timer.restart();
    m_fixed_counter += m_elapsed.asSeconds();
}

bool FrameTimer::pop_fixed() {
    if (m_fixed_counter >= c_fixed_timestep) {
        m_fixed_counter -= c_fixed_timestep;
        return true;
    }
    return false;
}
