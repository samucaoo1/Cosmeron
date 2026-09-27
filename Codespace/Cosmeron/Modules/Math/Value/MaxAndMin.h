#pragma once

#include <stdarg.h>
#include <stddef.h>

#include "../../../Core/Error/Status.h"

#include "../Math.space"

#define MATH_VALUE_MAX_PROTOTYPE(TYPE, SUFFIX)                      \
  inline static TYPE VALUE_TYPED_FUNC(Max, SUFFIX)(                    \
      TYPE a, TYPE b)

#define MATH_VALUE_MIN_PROTOTYPE(TYPE, SUFFIX)                      \
  inline static TYPE VALUE_TYPED_FUNC(Min, SUFFIX)(                    \
      TYPE a, TYPE b)

#define MATH_VALUE_SMALLEST_PROTOTYPE(TYPE, SUFFIX)                 \
  inline static OPSTATUS VALUE_TYPED_FUNC(Smallest, SUFFIX)(           \
      TYPE *result, size_t size, ...)

#define MATH_VALUE_BIGGEST_PROTOTYPE(TYPE, SUFFIX)                  \
  inline static OPSTATUS VALUE_TYPED_FUNC(Biggest, SUFFIX)(            \
      TYPE *result, size_t size, ...)

#include "Impl/MaxAndMin.impl"

#define MATH_VALUE_MAX_MIN_INSTANTIATE(TYPE, SUFFIX, VA_ARG_TYPE)            \
  MATH_VALUE_MAX_IMPLEMENT(TYPE, SUFFIX)                                     \
  MATH_VALUE_MIN_IMPLEMENT(TYPE, SUFFIX)                                     \
  MATH_VALUE_SMALLEST_IMPLEMENT(TYPE, SUFFIX, VA_ARG_TYPE)                   \
  MATH_VALUE_BIGGEST_IMPLEMENT(TYPE, SUFFIX, VA_ARG_TYPE)

MATH_VALUE_MAX_MIN_INSTANTIATE(int8_t, I8, int)
MATH_VALUE_MAX_MIN_INSTANTIATE(int16_t, I16, int)
MATH_VALUE_MAX_MIN_INSTANTIATE(int32_t, I32, int)
MATH_VALUE_MAX_MIN_INSTANTIATE(int64_t, I64, int64_t)

MATH_VALUE_MAX_MIN_INSTANTIATE(uint8_t, U8, int)
MATH_VALUE_MAX_MIN_INSTANTIATE(uint16_t, U16, int)
MATH_VALUE_MAX_MIN_INSTANTIATE(uint32_t, U32, uint32_t)
MATH_VALUE_MAX_MIN_INSTANTIATE(uint64_t, U64, uint64_t)

MATH_VALUE_MAX_MIN_INSTANTIATE(float, F32, double)
MATH_VALUE_MAX_MIN_INSTANTIATE(double, F64, double)
MATH_VALUE_MAX_MIN_INSTANTIATE(long double, F128, long double)

/* EOF */
