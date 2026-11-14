#pragma once

#include <limits.h>
#include <stdint.h>

#include "../../../Core/Condition/Status.h"
#include "../../../Core/Module/Module.h"

/* ========================================================================== */
/* Checked Integer Arithmetic (Signed & Unsigned)                             */
/* ========================================================================== */

#define COSMERON_MATH_ARITH_SIGNED_IMPL(TYPE, SUFFIX, MINIMUM, MAXIMUM)        \
  static inline OPSTATUS MODULE_FUNC(Math, Arithmetic, Add, SUFFIX)(           \
      TYPE a, TYPE b, TYPE *outResult) {                                       \
    if (!outResult) {                                                          \
      return STATUS_CONST(INVALID_ARGUMENT);                                   \
    }                                                                          \
    if ((b > 0 && a > (MAXIMUM) - b) || (b < 0 && a < (MINIMUM) - b)) {        \
      return STATUS_CONST(OUT_OF_RANGE);                                       \
    }                                                                          \
    *outResult = (TYPE)(a + b);                                                \
    return STATUS_CONST(SUCCESS);                                              \
  }                                                                            \
  static inline OPSTATUS MODULE_FUNC(Math, Arithmetic, Sub, SUFFIX)(           \
      TYPE a, TYPE b, TYPE *outResult) {                                       \
    if (!outResult) {                                                          \
      return STATUS_CONST(INVALID_ARGUMENT);                                   \
    }                                                                          \
    if ((b < 0 && a > (MAXIMUM) + b) || (b > 0 && a < (MINIMUM) + b)) {        \
      return STATUS_CONST(OUT_OF_RANGE);                                       \
    }                                                                          \
    *outResult = (TYPE)(a - b);                                                \
    return STATUS_CONST(SUCCESS);                                              \
  }                                                                            \
  static inline OPSTATUS MODULE_FUNC(Math, Arithmetic, Mul, SUFFIX)(           \
      TYPE a, TYPE b, TYPE *outResult) {                                       \
    if (!outResult) {                                                          \
      return STATUS_CONST(INVALID_ARGUMENT);                                   \
    }                                                                          \
    if (a == 0 || b == 0) {                                                    \
      *outResult = 0;                                                          \
      return STATUS_CONST(SUCCESS);                                            \
    }                                                                          \
    if (a > 0) {                                                               \
      if (b > 0 ? (a > (MAXIMUM) / b) : (b < (MINIMUM) / a)) {                 \
        return STATUS_CONST(OUT_OF_RANGE);                                     \
      }                                                                        \
    } else {                                                                   \
      if (b > 0 ? (a < (MINIMUM) / b) : (a != 0 && b < (MAXIMUM) / a)) {       \
        return STATUS_CONST(OUT_OF_RANGE);                                     \
      }                                                                        \
    }                                                                          \
    *outResult = (TYPE)(a * b);                                                \
    return STATUS_CONST(SUCCESS);                                              \
  }                                                                            \
  static inline OPSTATUS MODULE_FUNC(Math, Arithmetic, Div, SUFFIX)(           \
      TYPE a, TYPE b, TYPE *outResult) {                                       \
    if (!outResult) {                                                          \
      return STATUS_CONST(INVALID_ARGUMENT);                                   \
    }                                                                          \
    if (b == 0) {                                                              \
      return STATUS_CONST(INVALID_ARGUMENT);                                   \
    }                                                                          \
    if (a == (MINIMUM) && b == -1) {                                           \
      return STATUS_CONST(OUT_OF_RANGE);                                       \
    }                                                                          \
    *outResult = (TYPE)(a / b);                                                \
    return STATUS_CONST(SUCCESS);                                              \
  }                                                                            \
  static inline OPSTATUS MODULE_FUNC(Math, Arithmetic, Mod, SUFFIX)(           \
      TYPE a, TYPE b, TYPE *outResult) {                                       \
    if (!outResult) {                                                          \
      return STATUS_CONST(INVALID_ARGUMENT);                                   \
    }                                                                          \
    if (b == 0) {                                                              \
      return STATUS_CONST(INVALID_ARGUMENT);                                   \
    }                                                                          \
    if (a == (MINIMUM) && b == -1) {                                           \
      return STATUS_CONST(OUT_OF_RANGE);                                       \
    }                                                                          \
    *outResult = (TYPE)(a % b);                                                \
    return STATUS_CONST(SUCCESS);                                              \
  }

#define COSMERON_MATH_ARITH_UNSIGNED_IMPL(TYPE, SUFFIX, MAXIMUM)               \
  static inline OPSTATUS MODULE_FUNC(Math, Arithmetic, Add, SUFFIX)(           \
      TYPE a, TYPE b, TYPE *outResult) {                                       \
    if (!outResult) {                                                          \
      return STATUS_CONST(INVALID_ARGUMENT);                                   \
    }                                                                          \
    if (a > (MAXIMUM) - b) {                                                   \
      return STATUS_CONST(OUT_OF_RANGE);                                       \
    }                                                                          \
    *outResult = (TYPE)(a + b);                                                \
    return STATUS_CONST(SUCCESS);                                              \
  }                                                                            \
  static inline OPSTATUS MODULE_FUNC(Math, Arithmetic, Sub, SUFFIX)(           \
      TYPE a, TYPE b, TYPE *outResult) {                                       \
    if (!outResult) {                                                          \
      return STATUS_CONST(INVALID_ARGUMENT);                                   \
    }                                                                          \
    if (a < b) {                                                               \
      return STATUS_CONST(OUT_OF_RANGE);                                       \
    }                                                                          \
    *outResult = (TYPE)(a - b);                                                \
    return STATUS_CONST(SUCCESS);                                              \
  }                                                                            \
  static inline OPSTATUS MODULE_FUNC(Math, Arithmetic, Mul, SUFFIX)(           \
      TYPE a, TYPE b, TYPE *outResult) {                                       \
    if (!outResult) {                                                          \
      return STATUS_CONST(INVALID_ARGUMENT);                                   \
    }                                                                          \
    if (a != 0 && b > (MAXIMUM) / a) {                                         \
      return STATUS_CONST(OUT_OF_RANGE);                                       \
    }                                                                          \
    *outResult = (TYPE)(a * b);                                                \
    return STATUS_CONST(SUCCESS);                                              \
  }                                                                            \
  static inline OPSTATUS MODULE_FUNC(Math, Arithmetic, Div, SUFFIX)(           \
      TYPE a, TYPE b, TYPE *outResult) {                                       \
    if (!outResult || b == 0) {                                                \
      return STATUS_CONST(INVALID_ARGUMENT);                                   \
    }                                                                          \
    *outResult = (TYPE)(a / b);                                                \
    return STATUS_CONST(SUCCESS);                                              \
  }                                                                            \
  static inline OPSTATUS MODULE_FUNC(Math, Arithmetic, Mod, SUFFIX)(           \
      TYPE a, TYPE b, TYPE *outResult) {                                       \
    if (!outResult || b == 0) {                                                \
      return STATUS_CONST(INVALID_ARGUMENT);                                   \
    }                                                                          \
    *outResult = (TYPE)(a % b);                                                \
    return STATUS_CONST(SUCCESS);                                              \
  }

COSMERON_MATH_ARITH_SIGNED_IMPL(int8_t, Int8, INT8_MIN, INT8_MAX)
COSMERON_MATH_ARITH_SIGNED_IMPL(int16_t, Int16, INT16_MIN, INT16_MAX)
COSMERON_MATH_ARITH_SIGNED_IMPL(int32_t, Int32, INT32_MIN, INT32_MAX)
COSMERON_MATH_ARITH_SIGNED_IMPL(int64_t, Int64, INT64_MIN, INT64_MAX)

COSMERON_MATH_ARITH_UNSIGNED_IMPL(uint8_t, UInt8, UINT8_MAX)
COSMERON_MATH_ARITH_UNSIGNED_IMPL(uint16_t, UInt16, UINT16_MAX)
COSMERON_MATH_ARITH_UNSIGNED_IMPL(uint32_t, UInt32, UINT32_MAX)
COSMERON_MATH_ARITH_UNSIGNED_IMPL(uint64_t, UInt64, UINT64_MAX)

/* EOF */
