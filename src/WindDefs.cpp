//
// Created by jammer on 28/03/17.
//

#include "WindDefs.h"

float w_magnitude(const WVec &v) {
    return sqrtf(v.x * v.x + v.y * v.y);
}

WVec w_normalize(WVec v) {
    v /= w_magnitude(v);
    return v;
}

float dot(const WVec &a, const WVec &b) {
    return a.x * b.x + a.y * b.y;
}

float cross(const WVec &a, const WVec &b) {
    return a.x * b.y - a.y * b.x;
}

WVec triple_prod(const WVec &a, const WVec &b, const WVec &c) {
    return b * dot(a, c) - c * dot(a, b);
}

WVec slide(const WVec &vec, const WVec &normal) {
    return vec - normal * dot(vec, normal);
}
