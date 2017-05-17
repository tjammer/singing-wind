//
// Created by jammer on 28/03/17.
//

#ifndef SINGING_WIND_WINDDEFS_H
#define SINGING_WIND_WINDDEFS_H

#include <glm/glm.hpp>
#include <ostream>
#include <bitset>

using WVec = glm::tvec2<float>;
using WVec3 = glm::tvec3<float>;
using WTransform = glm::mat3;
using bset = std::bitset<12>;

inline std::ostream& operator <<(std::ostream& os, const WVec &v) {
    os << "(" << v.x << ", " << v.y << ")";
    return os;
}

#endif //SINGING_WIND_WINDDEFS_H
