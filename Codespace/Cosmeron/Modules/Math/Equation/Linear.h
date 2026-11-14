#pragma once

#include <stddef.h>

#include "../../../Core/Condition/Status.h"
#include "../Impl/Common.inc"

/* ========================================================================== */
/* Linear Equation Solver: a*x + b = 0                                        */
/* ========================================================================== */

#define COSMERON_MATH_EQUATION_LINEAR_IMPL(TYPE, SUFFIX)                       \
  static inline OPSTATUS MODULE_FUNC(Math, Equation, Linear, SUFFIX)(          \
      TYPE a, TYPE b, TYPE *outResult,                                         \
      MODULE_ENUM(Math, Solution) *outSolution) {                              \
    if (!outResult || !outSolution) {                                          \
      return STATUS_CONST(INVALID_ARGUMENT);                                   \
    }                                                                          \
    if (a == (TYPE)0) {                                                        \
      if (b == (TYPE)0) {                                                      \
        *outSolution = CNS(Math, INFINITE);                                    \
        *outResult = (TYPE)0;                                                  \
        return STATUS_CONST(SUCCESS);                                          \
      }                                                                        \
      *outSolution = CNS(Math, NONE);                                          \
      *outResult = (TYPE)0;                                                    \
      return STATUS_CONST(SUCCESS);                                            \
    }                                                                          \
    *outSolution = CNS(Math, ONE);                                             \
    *outResult = -b / a;                                                       \
    return STATUS_CONST(SUCCESS);                                              \
  }

COSMERON_MATH_EQUATION_LINEAR_IMPL(float, Float32)
COSMERON_MATH_EQUATION_LINEAR_IMPL(double, Float64)
COSMERON_MATH_EQUATION_LINEAR_IMPL(long double, Float128)

/* EOF */
