#ifndef CPRUNESWEEP_H
#define CPRUNESWEEP_H

#include "Collision.h"
#include <glm/glm.hpp>
#include <memory>

struct PSBox {
    WVec mins;
    WVec maxs;
    unsigned int entity;
};

using ColPair = std::pair<unsigned int,unsigned int>;

class PruneSweeper {
public:
    void prune_and_sweep();
    // for updating, adding and removing can be done by system
    std::vector<PSBox> &get_boxes() {return m_boxes;}
    std::vector<ColPair> &get_pairs() {return m_pairs;}
private:
    std::vector<PSBox> m_boxes;
    std::vector<ColPair> m_pairs;
    size_t sort_axis = 0;
};

#endif /* CPRUNESWEEP_H */
