//
// Created by jammer on 28/03/17.
//

#ifndef SINGING_WIND_WINDDEFS_H
#define SINGING_WIND_WINDDEFS_H

#include <glm/vec2.hpp>
#include <ostream>
#include <bitset>

using WVec = glm::tvec2<float>;

inline std::ostream&
operator<<(std::ostream& os, const WVec& v)
{
  os << "(" << v.x << ", " << v.y << ")";
  return os;
}

#endif // SINGING_WIND_WINDDEFS_H
