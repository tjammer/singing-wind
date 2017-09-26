#ifndef STEERING_H
#define STEERING_H

#include "WindDefs.h"
class GameWorld;

WVec seek(const WVec& direction, GameWorld &, unsigned int);
WVec flee(const WVec& direction, GameWorld &, unsigned int);
WVec arrive_velocity(const WVec& unnormed_direction, const WVec& velocity, float arrive_radius);

#endif /* STEERING_H */
