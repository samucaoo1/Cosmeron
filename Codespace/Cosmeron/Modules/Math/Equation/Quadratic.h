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
  inline static TYPE EQUATION_TYPED_FUNC(QuadraticDiscriminant, SUFFIX)(       \
      TYPE a, TYPE b, TYPE c)

#define MATH_EQUATION_QUADRATIC_PROTOTYPE(TYPE, SUFFIX)                       \
  inline static OPSTATUS EQUATION_TYPED_FUNC(Quadratic, SUFFIX)(               \
      TYPE a, TYPE b, TYPE c, TYPE *result1, TYPE *result2,                    \
      TMathSolution *solution)

#define MATH_EQUATION_QUADRATIC_COMPLEX_PROTOTYPE(TYPE, COMPLEX_TYPE, SUFFIX) \
  inline static OPSTATUS EQUATION_TYPED_FUNC(QuadraticComplex, SUFFIX)(        \
      TYPE a, TYPE b, TYPE c, COMPLEX_TYPE *result1, COMPLEX_TYPE *result2,    \
      TMathSolution *solution)

#include "Impl/Quadratic.impl"

#if COMPILER_MSVC
#define MATH_EQUATION_QUADRATIC_INSTANTIATE(TYPE, SUFFIX)                      \
  MATH_EQUATION_SQUARE_ROOT_IMPLEMENT(TYPE, SUFFIX)                            \
  MATH_EQUATION_QUADRATIC_DISCRIMINANT_IMPLEMENT(TYPE, SUFFIX)                 \
  MATH_EQUATION_QUADRATIC_IMPLEMENT(                                           \
      TYPE, SUFFIX, EQUATION_TYPED_FUNC(_SquareRoot, SUFFIX))
#else
#define MATH_EQUATION_QUADRATIC_INSTANTIATE(TYPE, SUFFIX)                      \
  MATH_EQUATION_SQUARE_ROOT_IMPLEMENT(TYPE, SUFFIX)                            \
  MATH_EQUATION_QUADRATIC_DISCRIMINANT_IMPLEMENT(TYPE, SUFFIX)                 \
  MATH_EQUATION_QUADRATIC_IMPLEMENT(                                           \
      TYPE, SUFFIX, EQUATION_TYPED_FUNC(_SquareRoot, SUFFIX))                  \
  MATH_EQUATION_QUADRATIC_COMPLEX_IMPLEMENT(                                   \
      TYPE, TYPE _Complex, SUFFIX, EQUATION_TYPED_FUNC(_SquareRoot, SUFFIX))
#endif

MATH_EQUATION_QUADRATIC_INSTANTIATE(float, F32)
MATH_EQUATION_QUADRATIC_INSTANTIATE(double, F64)
MATH_EQUATION_QUADRATIC_INSTANTIATE(long double, F128)

/* EOF */
