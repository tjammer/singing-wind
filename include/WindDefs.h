//
// Created by jammer on 28/03/17.
//

#ifndef SINGING_WIND_WINDDEFS_H
#define SINGING_WIND_WINDDEFS_H

#include "SFML/Graphics.hpp"
#include <math.h>

using WVec = sf::Vector2f;
using WTransform = sf::Transform;

float w_magnitude(const WVec &v);

WVec w_normalize(WVec v);

float dot(const WVec &a, const WVec &b);

float cross(const WVec &a, const WVec &b);

WVec triple_prod(const WVec &a, const WVec &b, const WVec &c);

WVec slide(const WVec &vec, const WVec &normal);

#endif //SINGING_WIND_WINDDEFS_H
