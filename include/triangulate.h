//
// Created by tobi on 4/2/17.
//

#ifndef SINGING_WIND_TRIANGULATE_H
#define SINGING_WIND_TRIANGULATE_H

#ifdef ANSI_DECLARATORS
#define IGL_PREVIOUSLY_DEFINED_ANSI_DECLARATORS ANSI_DECLARATORS
#undef ANSI_DECLARATORS
#endif
#ifdef REAL
#define IGL_PREVIOUSLY_DEFINED_REAL REAL
#undef REAL
#endif
#ifdef VOID
#define IGL_PREVIOUSLY_DEFINED_VOID VOID
#undef VOID
#endif
#define ANSI_DECLARATORS
#define REAL float
#define VOID int

extern "C" {
#include <triangle.h>
}

#undef ANSI_DECLARATORS
#ifdef IGL_PREVIOUSLY_DEFINED_ANSI_DECLARATORS
#define ANSI_DECLARATORS IGL_PREVIOUSLY_DEFINED_ANSI_DECLARATORS
#endif

#undef REAL
#ifdef IGL_PREVIOUSLY_DEFINED_REAL
#define REAL IGL_PREVIOUSLY_DEFINED_REAL
#endif

#undef VOID
#ifdef IGL_PREVIOUSLY_DEFINED_VOID
#define VOID IGL_PREVIOUSLY_DEFINED_VOID
#endif

#include "Island.h"

struct QuickTri
{
  WVec a;
  WVec b;
  WVec c;
};

void
triangulate_island(const Island& island,
                   std::vector<WVec>& tri_vector,
                   float split_length);

void
triangulate_between_islands(const std::vector<Island>& islands,
                            std::vector<QuickTri>& points,
                            float split_length);

#endif // SINGING_WIND_TRIANGULATE_H
