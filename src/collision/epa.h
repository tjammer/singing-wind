#ifndef EPA_H
#define EPA_H

#include "w_vec.h"
#include <vector>

struct ColResult;
class TransformedMinkowskiDiff;

void
epa_solve(const std::vector<WVec>& simplex,
          const TransformedMinkowskiDiff& mdiff,
          ColResult& result);

#endif /* EPA_H */
