#pragma once

#include "../../../Core/Module/Module.h"

/* ========================================================================== */
/* Math Domain Enums                                                          */
/* ========================================================================== */

#define MATH_RANGE_RESULT_TABLE(X)                                             \
  X((IN_RANGE, 0))                                                             \
  X((BELOW_RANGE, -1))                                                         \
  X((ABOVE_RANGE, 1))                                                          \
  X((INVALID_RANGE, 2))

#define MATH_SOLUTION_TABLE(X)                                                 \
  X((NONE, 0))                                                                 \
  X((ONE, 1))                                                                  \
  X((TWO, 2))                                                                  \
  X((INFINITE, 3))                                                             \
  X((COMPLEX, 4))

MODULE(Math,
    ENUM(RangeResult, MATH_RANGE_RESULT_TABLE),
    ENUM(Solution, MATH_SOLUTION_TABLE)
);

/* EOF */
