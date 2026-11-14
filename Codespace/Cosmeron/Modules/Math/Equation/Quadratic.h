#pragma once

#include <stddef.h>

#include "../../../Core/Condition/Status.h"
#include "../../../Core/Preprocessor/Detect/Detect.h"
#include "../Impl/Common.inc"
#include "Linear.h"

#if !COMPILER_MSVC
#include <complex.h>
#endif

/* ========================================================================== */
/* Quadratic Equation Solver: a*x^2 + b*x + c = 0                             */
/* ========================================================================== */

#define COSMERON_MATH_EQUATION_QUADRATIC_IMPL(TYPE, SUFFIX)                    \
  static inline TYPE MODULE_FUNC(Math, Equation, QuadraticSqrt, SUFFIX)(       \
      TYPE x) {                                                                \
    if (x <= (TYPE)0) {                                                        \
      return (TYPE)0;                                                          \
    }                                                                          \
    TYPE guess = x > (TYPE)1 ? x * (TYPE)0.5 : (TYPE)1;                        \
    for (int i = 0; i < 64; ++i) {                                             \
      TYPE next = (TYPE)0.5 * (guess + x / guess);                             \
      if (next == guess) {                                                     \
        break;                                                                 \
      }                                                                        \
      guess = next;                                                            \
    }                                                                          \
    return guess;                                                              \
  }                                                                            \
  static inline TYPE MODULE_FUNC(Math, Equation, QuadraticDiscriminant,        \
                                 SUFFIX)(TYPE a, TYPE b, TYPE c) {             \
    return b * b - (TYPE)4 * a * c;                                            \
  }                                                                            \
  static inline OPSTATUS MODULE_FUNC(Math, Equation, Quadratic, SUFFIX)(       \
      TYPE a, TYPE b, TYPE c, TYPE *outResult1, TYPE *outResult2,              \
      MODULE_ENUM(Math, Solution) *outSolution) {                              \
    if (!outResult1 || !outResult2 || !outSolution) {                          \
      return STATUS_CONST(INVALID_ARGUMENT);                                   \
    }                                                                          \
    if (a == (TYPE)0) {                                                        \
      *outResult2 = (TYPE)0;                                                   \
      return MODULE_FUNC(Math, Equation, Linear, SUFFIX)(b, c, outResult1,     \
                                                         outSolution);         \
    }                                                                          \
    TYPE d =                                                                   \
        MODULE_FUNC(Math, Equation, QuadraticDiscriminant, SUFFIX)(a, b, c);   \
    if (d < (TYPE)0) {                                                         \
      *outSolution = CNS(Math, COMPLEX);                                       \
      *outResult1 = (TYPE)0;                                                   \
      *outResult2 = (TYPE)0;                                                   \
      return STATUS_CONST(SUCCESS);                                            \
    }                                                                          \
    if (d == (TYPE)0) {                                                        \
      *outSolution = CNS(Math, ONE);                                           \
      *outResult1 = -b / ((TYPE)2 * a);                                        \
      *outResult2 = *outResult1;                                               \
      return STATUS_CONST(SUCCESS);                                            \
    }                                                                          \
    TYPE sq = MODULE_FUNC(Math, Equation, QuadraticSqrt, SUFFIX)(d);           \
    *outSolution = CNS(Math, TWO);                                             \
    *outResult1 = (-b - sq) / ((TYPE)2 * a);                                   \
    *outResult2 = (-b + sq) / ((TYPE)2 * a);                                   \
    return STATUS_CONST(SUCCESS);                                              \
  }

COSMERON_MATH_EQUATION_QUADRATIC_IMPL(float, Float32)
COSMERON_MATH_EQUATION_QUADRATIC_IMPL(double, Float64)
COSMERON_MATH_EQUATION_QUADRATIC_IMPL(long double, Float128)

/* EOF */
