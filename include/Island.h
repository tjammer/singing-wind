//
// Created by jammer on 28/03/17.
//

#ifndef SINGING_WIND_ISLAND_H
#define SINGING_WIND_ISLAND_H

#include "WindDefs.h"

class Island {
public:
    Island() {}
    Island(WVec pos, float spread);
    ~Island() {}

    std::vector<WVec> m_points;
    std::vector<WVec> m_ctrl_points;

    std::vector<sf::Vertex> get_points(float spread) const;
    std::vector<sf::Vertex> get_ctrl_points(float spread) const;
    std::vector<sf::Vertex> get_curves(float distance);
};

std::vector<WVec> make_quad(const WVec& pos, float spread);
#endif //SINGING_WIND_ISLAND_H
