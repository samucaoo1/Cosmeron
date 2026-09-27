#pragma once

#include "../../../Core/Error/Status.h"
#include "../../../Core/Preprocessor/Compiling.inc"
#include <stdbool.h>
#include <stddef.h>
#include "../Math.space"

#define MATH_EQUATION_LINEAR_PROTOTYPE(TYPE, SUFFIX)                  \
  inline static OPSTATUS EQUATION_TYPED_FUNC(Linear, SUFFIX)(       \
      TYPE a, TYPE b, TYPE *result, TMathSolution *solution)

#include "Impl/Linear.impl"

MATH_EQUATION_LINEAR_IMPLEMENT(float, F32)
MATH_EQUATION_LINEAR_IMPLEMENT(double, F64)
MATH_EQUATION_LINEAR_IMPLEMENT(long double, F128)

/* EOF */
