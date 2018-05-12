#ifndef EPA_H
#define EPA_H

#include "w_vec.h"
#include <vector>

struct ColResult;
class MinkowskiDiff;

void
epa_solve(const std::vector<WVec>& simplex,
          const MinkowskiDiff& mdiff,
          ColResult& result);

#endif /* EPA_H */
