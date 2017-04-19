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

    void clear() {m_points.clear(); m_ctrl_points.clear();}
    std::vector<WVec> m_points;
    std::vector<WVec> m_ctrl_points;

    std::vector<sf::Vertex> get_points(float spread) const;
    std::vector<sf::Vertex> get_ctrl_points(float spread) const;
    std::vector<sf::Vertex> get_curves(float distance);
};

inline bool operator==(const Island& lhs, const Island& rhs) {
    if (lhs.m_points.size() != rhs.m_points.size()) {
        return false;
    }
    for (uint i = 0 ; i < lhs.m_points.size() ; ++i) {
        if (lhs.m_points[i] != rhs.m_points[i]) {
            return false;
        }
    }
    return true;
}

std::vector<WVec> make_quad(const WVec& pos, float spread);
#endif //SINGING_WIND_ISLAND_H
