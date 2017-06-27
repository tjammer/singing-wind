//
// Created by jammer on 03/04/17.
//

#include "Collision.h"
#include "ColShape.h"
#include "ColGrid.h"
#include <WVecMath.h>
#include <assert.h>
#include <bits/shared_ptr.h>
#include <set>

const float c_epsilon = 0.01f;
const int c_max_it = 20;

ColResult static_collide(const ColShape &a, const ColShape &b) {
    ColResult result;
    WVec v = a.get_support(WVec(1, 0)) - b.get_support(WVec(-1, 0));
    Simplex s;
    WVec w = a.get_support(-v) - b.get_support(v);
    int it = 0;
    while ((w_dot(v, v) - w_dot(v, w)) > c_epsilon && it < c_max_it) {
        s.add(w);
        /*	if (dot(v, w) > 0)
                break;*/
        s.solve(WVec(0, 0));
        v = s.get_closest();
        if (w_dot(v, v) == 0.f)
            break;
        w = a.get_support(-v) - b.get_support(v);
        ++it;
    }
    result.collides = w_dot(v, v) == 0.f;
    if (result.collides) {
        WVec normal;
        int epa_it = 0;
        result.depth = find_normal_epa(a, b, s, normal, epa_it);
        result.normal = normal;
        result.gjk_it = it;
        result.epa_it = epa_it;
    }
    else
        result.depth = sqrtf(w_dot(v, v));
    return result;
}

RayCastResult cast_ray_vs_shape(const WVec &a, const ColShape &b, const WVec &dir) {
    RayCastResult result;
    result.innerits = 0;

    //initialize
    int it = 0;
    float t = 0;
    WVec s(0, 0);
    WVec n(0, 0);
    //pick point in A - B
    auto v = a - b.get_support(-dir);
    Simplex W;

    WVec p;
    while (w_dot(v, v) > c_epsilon && it < c_max_it) {
        p = a - b.get_support(v);

        float dvp = w_dot(v, p);
        float dvr = w_dot(v, dir);

        if (dvp > t*dvr) {
            if (dvr > 0) {
                result.innerits++;
                t = dvp / dvr;
                /*if (t > 1) {
                    return result;
                }*/
                s = t * dir;
                //simplex is reset
                W = Simplex();
                n = -v;
            }
            else {
                return result;
            }
        }
        W.add(p - s);
        W.solve(WVec(0, 0));
        v = W.get_closest();

        ++it;
        result.its++;
    }
    if (s == n) {
        n = v;
    }
    result.hit_parameter = t;
    result.hit_spot = s;
    result.hit_normal = n;
    result.hits = true;

    return result;
}


float find_normal_epa(const ColShape &a, const ColShape &b, Simplex &s, WVec &normal, int &epa_it) {
    float dist = std::numeric_limits<float>::max();
    int it = 0;
    while (true) {
        auto e = find_closest_edge(s);
        auto p = a.get_support(e.normal) - b.get_support(-e.normal);
        float d = w_dot(p, e.normal);
        float test = d - e.distance;
        if (test < c_epsilon || test == dist || it > c_max_it) {
            normal = e.normal;
            epa_it = it;
            return d;
        }
        s.add(p, e.index);
        dist = test < dist ? test : dist;
        ++it;
    }
}

Edge find_closest_edge(const Simplex &s) {
    Edge edge;
    edge.distance = std::numeric_limits<float>::max();

    for (unsigned int i = 0; i < s.verts.size(); ++i) {
        int j = i + 1 == s.verts.size() ? 0 : i + 1;
        auto a = s.verts[i];
        auto b = s.verts[j];

        auto e = b - a;
        auto n = w_triple_prod(e, a, e);
        //normalize
        //n /= sqrt(dot(n, n));
        n /= sqrtf(pow(n.x, 2.f) + powf(n.y, 2.f));

        auto d = w_dot(n, a);
        if (d < edge.distance) {
            edge.distance = d;
            edge.normal = n;
            edge.index = j;
        }
    }

    return edge;
}

void Simplex::solve(const WVec & x) {
    switch (count) {
        case 1:
            return;

        case 2: {
            solve2(x);
            return;
        }

        case 3: {
            solve3(x);
            return;
        }

        default:
            assert(false);
    }
}

bool Simplex::add(const WVec &v, int index) {
    switch (count) {
        case 0: {
            verts[0] = v;
            ++count;
            break;
        }

        case 1: {
            if (v == verts[0])
                return false;
            verts[1] = v;
            ++count;
            break;
        }

        case 2: {
            if (v == verts[0] || v == verts[1])
                return false;
            verts[2] = v;
            ++count;
            break;
        }

        default: {
            //assert(false);
            verts.insert(verts.begin() + index, v);
            ++count;
        }
    }
    return true;
}

WVec Simplex::get_closest() const {
    switch (count) {
        case 1:
            return verts[0];

        case 2: {
            float s = 1.0f / denom;
            return (s * barys[0]) * verts[0] + (s * barys[1]) * verts[1];
        }

        case 3:
            return WVec(0, 0);

        default:
            assert(false);
            return WVec(0, 0);
    }

}

void Simplex::solve2(const WVec & x) {

    // Compute barycentric coordinates (pre-division).
    float u = w_dot(x - verts[1], verts[0] - verts[1]);
    float v = w_dot(x - verts[0], verts[1] - verts[0]);

    // Region A
    if (v <= 0.0f) {
        // Simplex is reduced to just vertex A.
        barys[0] = 1.0f;
        denom = 1.0f;
        count = 1;
        return;
    }

    // Region B
    if (u <= 0.0f) {
        // Simplex is reduced to just vertex B.
        // We move vertex B into vertex A and reduce the count.
        verts[0] = verts[1];
        barys[0] = 1.0f;
        denom = 1.0f;
        count = 1;
        return;
    }

    // Region AB. Due to the conditions above, we are
    // guaranteed the the edge has non-zero length and division
    // is safe.
    barys[0] = u;
    barys[1] = v;
    auto e = verts[1] - verts[0];
    denom = w_dot(e, e);
    count = 2;
}

void Simplex::solve3(const WVec & x) {
    // Compute edge barycentric coordinates (pre-division).
    float uAB = w_dot(x - verts[1], verts[0] - verts[1]);
    float vAB = w_dot(x - verts[0], verts[1] - verts[0]);

    float uBC = w_dot(x - verts[2], verts[1] - verts[2]);
    float vBC = w_dot(x - verts[1], verts[2] - verts[1]);

    float uCA = w_dot(x - verts[0], verts[2] - verts[0]);
    float vCA = w_dot(x - verts[2], verts[0] - verts[2]);

    // Region A
    if (vAB <= 0.0f && uCA <= 0.0f) {
        barys[0] = 1.0f;
        denom = 1.0f;
        count = 1;
        return;
    }

    // Region B
    if (uAB <= 0.0f && vBC <= 0.0f) {
        verts[0] = verts[1];
        barys[0] = 1.0f;
        denom = 1.0f;
        count = 1;
        return;
    }

    // Region C
    if (uBC <= 0.0f && vCA <= 0.0f) {
        verts[0] = verts[2];
        barys[0] = 1.0f;
        denom = 1.0f;
        count = 1;
        return;
    }

    // Compute signed triangle area.
    float area = w_cross(verts[1] - verts[0], verts[2] - verts[0]);

    // Compute triangle barycentric coordinates (pre-division).
    float uABC = w_cross(verts[1] - x, verts[2] - x);
    float vABC = w_cross(verts[2] - x, verts[0] - x);
    float wABC = w_cross(verts[0] - x, verts[1] - x);

    // Region AB
    if (uAB > 0.0f && vAB > 0.0f && wABC * area <= 0.0f) {
        barys[0] = uAB;
        barys[1] = vAB;
        auto e = verts[1] - verts[0];
        denom = w_dot(e, e);
        count = 2;
        return;
    }

    // Region BC
    if (uBC > 0.0f && vBC > 0.0f && uABC * area <= 0.0f) {
        verts[0] = verts[1];
        verts[1] = verts[2];

        barys[0] = uBC;
        barys[1] = vBC;
        //auto e = verts[2] - verts[1];
        auto e = verts[1] - verts[0];
        denom = w_dot(e, e);
        count = 2;
        return;
    }

    // Region CA
    if (uCA > 0.0f && vCA > 0.0f && vABC * area <= 0.0f) {
        verts[1] = verts[0];
        verts[0] = verts[2];

        barys[0] = uCA;
        barys[1] = vCA;
        //auto e = verts[0] - verts[2];
        auto e = verts[1] - verts[0];
        denom = w_dot(e, e);
        count = 2;
        return;
    }

    // Region ABC
    // The triangle area is guaranteed to be non-zero.
    //assert(uABC > 0.0f && vABC > 0.0f && wABC > 0.0f);
    barys[0] = uABC;
    barys[1] = vABC;
    barys[2] = wABC;
    denom = area;
    count = 3;
}

WVec find_directed_overlap(const ColResult &result, const WVec &direction) {
    auto dir = w_normalize(direction);
    auto projection = w_dot(-dir, result.normal);
    /*if (abs(projection) < c_epsilon * 0.1f) {
        return result.normal * result.depth;
    }*/
    return dir * (result.depth / projection - .1f); // - result.normal * .001f;
}

std::ostream &operator<<(std::ostream &os, const ColResult &result) {
        return os << "collides: " << result.collides << ", normal: x: " <<
                  result.normal.x << " y: " << result.normal.y << ", depth: " << result.depth << ", gjk it: " <<
                  result.gjk_it << ", epa it: " << result.epa_it;
}

inline void swap(float& p1, float& p2) {
    auto tmp = p1;
    p1 = p2;
    p2 = tmp;
}
inline float pymod(float a, float b) {
    return fmod(b + fmod(a, b), b);
}

RayCastResult cast_ray_vs_static_grid(StaticGrid &grid, const WVec &from, const WVec &to) {
    //http://www.cs.yorku.ca/~amana/research/grid.pdf
    std::set<std::shared_ptr<ColShape>> tested;

    auto dir = w_normalize(to - from);

    RayCastResult result;

    auto pos = from;

    float cell_size = static_cast<float>(grid.get_size());

    float x_dt = 0;
    float y_dt = 0;

    while ((to.x - pos.x ) * dir.x >= 0 && (to.y - pos.y) * dir.y >= 0) {
        float t = 0;
        if (x_dt < y_dt) {
            t = x_dt;
        }
        else {
            t = y_dt;
        }
        pos += dir * t;

        auto colliders = grid.get_colliders_of_point(pos);
        // check collision
        for (const auto &shape: colliders) {
            if (tested.count(shape)) {
                continue;
            }
            auto res = cast_ray_vs_shape(from, *shape, -dir);
            if (res.hit_parameter < result.hit_parameter) {
                assert(res.hits);
                result = res;
            }
            tested.insert(shape);
        }
        if (result.hits) {
            break;
        }

        // todo: find xdt and ydt
        if (dir.x != 0) {
            x_dt = pymod(copysignf(pos.x, dir.x), cell_size) / abs(dir.x) + 0.1f;
        }
        else {
            x_dt = std::numeric_limits<float>::max();
        }

        if (dir.y != 0) {
            y_dt = pymod(copysignf(pos.y, dir.y), cell_size) / abs(dir.y) + 0.1f;
        }
        else {
            y_dt = std::numeric_limits<float>::max();
        }
    }
    return result;
}
