#pragma once

#include "../../../Core/Error/Status.h"
#include "../../../Core/Preprocessor/Compiling.inc"
#include <stdbool.h>
#include <stddef.h>
#include "../Math.space"

#define MATH_EQUATION_LINEAR_PROTOTYPE(TYPE, SUFFIX)                  \
  static inline OPSTATUS EQUATION_TYPED_FUNC(Linear, SUFFIX)(       \
      TYPE a, TYPE b, TYPE *outResult, MATH_TYPE(Solution) *outSolution)

#include "Impl/Linear.impl"

MATH_EQUATION_LINEAR_IMPLEMENT(float, F32)
MATH_EQUATION_LINEAR_IMPLEMENT(double, F64)
MATH_EQUATION_LINEAR_IMPLEMENT(long double, F128)

/* EOF */
