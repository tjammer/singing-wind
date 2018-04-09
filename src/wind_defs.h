//
// Created by jammer on 28/03/17.
//

#ifndef SINGING_WIND_WINDDEFS_H
#define SINGING_WIND_WINDDEFS_H

#include <glm/glm.hpp>
#include <ostream>
#include <bitset>

typedef glm::tvec2<float> WVec;
typedef glm::tvec3<float> WVec3;
typedef glm::mat3 WTransform;

inline std::ostream&
operator<<(std::ostream& os, const WVec& v)
{
  os << "(" << v.x << ", " << v.y << ")";
  return os;
}

#endif // SINGING_WIND_WINDDEFS_H
