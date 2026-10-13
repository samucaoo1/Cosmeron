#pragma once

#include "../../../Core/Preprocessor/Detect/Compiler.h"
#if !COMPILER_MSVC
#include <complex.h>
#endif
#include <stddef.h>

#include "../../../Core/Error/Status.h"
#include "../../../Core/Preprocessor/Compiling.inc"

#include "../Math.space"

#define MATH_EQUATION_QUADRATIC_DISCRIMINANT_PROTOTYPE(TYPE, SUFFIX)          \
  static inline TYPE EQUATION_TYPED_FUNC(QuadraticDiscriminant, SUFFIX)(       \
      TYPE a, TYPE b, TYPE c)

#define MATH_EQUATION_QUADRATIC_PROTOTYPE(TYPE, SUFFIX)                       \
  static inline OPSTATUS EQUATION_TYPED_FUNC(Quadratic, SUFFIX)(               \
      TYPE a, TYPE b, TYPE c, TYPE *outResult1, TYPE *outResult2,                    \
      MATH_TYPE(Solution) *outSolution)

#define MATH_EQUATION_QUADRATIC_COMPLEX_PROTOTYPE(TYPE, COMPLEX_TYPE, SUFFIX) \
  static inline OPSTATUS EQUATION_TYPED_FUNC(QuadraticComplex, SUFFIX)(        \
      TYPE a, TYPE b, TYPE c, COMPLEX_TYPE *outResult1, COMPLEX_TYPE *outResult2,    \
      MATH_TYPE(Solution) *outSolution)

#include "Impl/Quadratic.impl"

#if COMPILER_MSVC
#define MATH_EQUATION_QUADRATIC_IMPLEMENT_ALL(TYPE, SUFFIX)                      \
  MATH_EQUATION_SQUARE_ROOT_IMPLEMENT(TYPE, SUFFIX)                            \
  MATH_EQUATION_QUADRATIC_DISCRIMINANT_IMPLEMENT(TYPE, SUFFIX)                 \
  MATH_EQUATION_QUADRATIC_IMPLEMENT(                                           \
      TYPE, SUFFIX, EQUATION_TYPED_FUNC(_SquareRoot, SUFFIX))
#else
#define MATH_EQUATION_QUADRATIC_IMPLEMENT_ALL(TYPE, SUFFIX)                      \
  MATH_EQUATION_SQUARE_ROOT_IMPLEMENT(TYPE, SUFFIX)                            \
  MATH_EQUATION_QUADRATIC_DISCRIMINANT_IMPLEMENT(TYPE, SUFFIX)                 \
  MATH_EQUATION_QUADRATIC_IMPLEMENT(                                           \
      TYPE, SUFFIX, EQUATION_TYPED_FUNC(_SquareRoot, SUFFIX))                  \
  MATH_EQUATION_QUADRATIC_COMPLEX_IMPLEMENT(                                   \
      TYPE, TYPE _Complex, SUFFIX, EQUATION_TYPED_FUNC(_SquareRoot, SUFFIX))
#endif

MATH_EQUATION_QUADRATIC_IMPLEMENT_ALL(float, F32)
MATH_EQUATION_QUADRATIC_IMPLEMENT_ALL(double, F64)
MATH_EQUATION_QUADRATIC_IMPLEMENT_ALL(long double, F128)

/* EOF */
