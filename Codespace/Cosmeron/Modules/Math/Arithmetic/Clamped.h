#pragma once

#include <limits.h>
#include <stdint.h>

#include "../Value/Clamp.h"
#include "Basic.h"

/* ========================================================================== */
/* Clamped & Saturating Integer Arithmetic                                    */
/* ========================================================================== */

#define COSMERON_MATH_ARITH_CLAMP_SIGNED_IMPL(TYPE, SUFFIX, MINIMUM, MAXIMUM)  \
  static inline TYPE MODULE_FUNC(Math, Arithmetic, ClampAdd, SUFFIX)(          \
      TYPE a, TYPE b, TYPE minimum, TYPE maximum) {                            \
    TYPE res = 0;                                                              \
    if (MODULE_FUNC(Math, Arithmetic, Add, SUFFIX)(a, b, &res) !=              \
        STATUS_CONST(SUCCESS)) {                                               \
      return (b > 0) ? maximum : minimum;                                      \
    }                                                                          \
    return MODULE_FUNC(Math, Clamp, SUFFIX)(res, minimum, maximum);            \
  }                                                                            \
  static inline TYPE MODULE_FUNC(Math, Arithmetic, ClampSub, SUFFIX)(          \
      TYPE a, TYPE b, TYPE minimum, TYPE maximum) {                            \
    TYPE res = 0;                                                              \
    if (MODULE_FUNC(Math, Arithmetic, Sub, SUFFIX)(a, b, &res) !=              \
        STATUS_CONST(SUCCESS)) {                                               \
      return (b < 0) ? maximum : minimum;                                      \
    }                                                                          \
    return MODULE_FUNC(Math, Clamp, SUFFIX)(res, minimum, maximum);            \
  }                                                                            \
  static inline TYPE MODULE_FUNC(Math, Arithmetic, ClampMul, SUFFIX)(          \
      TYPE a, TYPE b, TYPE minimum, TYPE maximum) {                            \
    TYPE res = 0;                                                              \
    if (MODULE_FUNC(Math, Arithmetic, Mul, SUFFIX)(a, b, &res) !=              \
        STATUS_CONST(SUCCESS)) {                                               \
      bool sameSign = ((a > 0) && (b > 0)) || ((a < 0) && (b < 0));            \
      return sameSign ? maximum : minimum;                                     \
    }                                                                          \
    return MODULE_FUNC(Math, Clamp, SUFFIX)(res, minimum, maximum);            \
  }                                                                            \
  static inline TYPE MODULE_FUNC(Math, Arithmetic, SaturatingAdd, SUFFIX)(     \
      TYPE a, TYPE b) {                                                        \
    return MODULE_FUNC(Math, Arithmetic, ClampAdd, SUFFIX)(a, b, MINIMUM,      \
                                                           MAXIMUM);           \
  }                                                                            \
  static inline TYPE MODULE_FUNC(Math, Arithmetic, SaturatingSub, SUFFIX)(     \
      TYPE a, TYPE b) {                                                        \
    return MODULE_FUNC(Math, Arithmetic, ClampSub, SUFFIX)(a, b, MINIMUM,      \
                                                           MAXIMUM);           \
  }                                                                            \
  static inline TYPE MODULE_FUNC(Math, Arithmetic, SaturatingMul, SUFFIX)(     \
      TYPE a, TYPE b) {                                                        \
    return MODULE_FUNC(Math, Arithmetic, ClampMul, SUFFIX)(a, b, MINIMUM,      \
                                                           MAXIMUM);           \
  }

#define COSMERON_MATH_ARITH_CLAMP_UNSIGNED_IMPL(TYPE, SUFFIX, MAXIMUM)         \
  static inline TYPE MODULE_FUNC(Math, Arithmetic, ClampAdd, SUFFIX)(          \
      TYPE a, TYPE b, TYPE minimum, TYPE maximum) {                            \
    TYPE res = 0;                                                              \
    if (MODULE_FUNC(Math, Arithmetic, Add, SUFFIX)(a, b, &res) !=              \
        STATUS_CONST(SUCCESS)) {                                               \
      return maximum;                                                          \
    }                                                                          \
    return MODULE_FUNC(Math, Clamp, SUFFIX)(res, minimum, maximum);            \
  }                                                                            \
  static inline TYPE MODULE_FUNC(Math, Arithmetic, ClampSub, SUFFIX)(          \
      TYPE a, TYPE b, TYPE minimum, TYPE maximum) {                            \
    TYPE res = 0;                                                              \
    if (MODULE_FUNC(Math, Arithmetic, Sub, SUFFIX)(a, b, &res) !=              \
        STATUS_CONST(SUCCESS)) {                                               \
      return minimum;                                                          \
    }                                                                          \
    return MODULE_FUNC(Math, Clamp, SUFFIX)(res, minimum, maximum);            \
  }                                                                            \
  static inline TYPE MODULE_FUNC(Math, Arithmetic, ClampMul, SUFFIX)(          \
      TYPE a, TYPE b, TYPE minimum, TYPE maximum) {                            \
    TYPE res = 0;                                                              \
    if (MODULE_FUNC(Math, Arithmetic, Mul, SUFFIX)(a, b, &res) !=              \
        STATUS_CONST(SUCCESS)) {                                               \
      return maximum;                                                          \
    }                                                                          \
    return MODULE_FUNC(Math, Clamp, SUFFIX)(res, minimum, maximum);            \
  }                                                                            \
  static inline TYPE MODULE_FUNC(Math, Arithmetic, SaturatingAdd, SUFFIX)(     \
      TYPE a, TYPE b) {                                                        \
    return MODULE_FUNC(Math, Arithmetic, ClampAdd, SUFFIX)(a, b, 0, MAXIMUM);  \
  }                                                                            \
  static inline TYPE MODULE_FUNC(Math, Arithmetic, SaturatingSub, SUFFIX)(     \
      TYPE a, TYPE b) {                                                        \
    return MODULE_FUNC(Math, Arithmetic, ClampSub, SUFFIX)(a, b, 0, MAXIMUM);  \
  }                                                                            \
  static inline TYPE MODULE_FUNC(Math, Arithmetic, SaturatingMul, SUFFIX)(     \
      TYPE a, TYPE b) {                                                        \
    return MODULE_FUNC(Math, Arithmetic, ClampMul, SUFFIX)(a, b, 0, MAXIMUM);  \
  }

COSMERON_MATH_ARITH_CLAMP_SIGNED_IMPL(int8_t, Int8, INT8_MIN, INT8_MAX)
COSMERON_MATH_ARITH_CLAMP_SIGNED_IMPL(int16_t, Int16, INT16_MIN, INT16_MAX)
COSMERON_MATH_ARITH_CLAMP_SIGNED_IMPL(int32_t, Int32, INT32_MIN, INT32_MAX)
COSMERON_MATH_ARITH_CLAMP_SIGNED_IMPL(int64_t, Int64, INT64_MIN, INT64_MAX)

COSMERON_MATH_ARITH_CLAMP_UNSIGNED_IMPL(uint8_t, UInt8, UINT8_MAX)
COSMERON_MATH_ARITH_CLAMP_UNSIGNED_IMPL(uint16_t, UInt16, UINT16_MAX)
COSMERON_MATH_ARITH_CLAMP_UNSIGNED_IMPL(uint32_t, UInt32, UINT32_MAX)
COSMERON_MATH_ARITH_CLAMP_UNSIGNED_IMPL(uint64_t, UInt64, UINT64_MAX)

/* EOF */
