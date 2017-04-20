//
// Created by jammer on 20/04/17.
//

#ifndef SINGING_WIND_FRAMETIMER_H
#define SINGING_WIND_FRAMETIMER_H

#include "WindDefs.h"

const float c_fixed_timestep = 1.f / 60.f;

class FrameTimer {
public:
    void update();
    void reset();
    bool pop_fixed();
    sf::Time get_elapsed() const {return m_elapsed;}
    float get_remaining() const {return m_fixed_counter;}

private:
    sf::Clock m_timer;
    sf::Time m_elapsed;
    float m_fixed_counter = 0;
};


#endif //SINGING_WIND_FRAMETIMER_H
