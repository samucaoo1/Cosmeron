#pragma once

#if defined(MATH_DISABLE_FUNCTION_TABLE) && !defined(COSMERON_DISABLE_FT_Math)
#define COSMERON_DISABLE_FT_Math 1
#endif

#include "../../../Core/Condition/Status.h"
#include "../../../Core/Module/Module.h"
#include "../../../Core/Preprocessor/Operations.h"

#include <limits.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* ==========================================================================
 * Math Domain Enums
 * ========================================================================== */

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

MODULE(DOMAIN(Modules), Math,
    ENUM(RangeResult, MATH_RANGE_RESULT_TABLE),
    ENUM(Solution, MATH_SOLUTION_TABLE)
);

/* ==========================================================================
 * Math Instantiation Tables (Row-Tuple Pattern)
 * ========================================================================== */

#define MATH_ORDERED_TYPE_TABLE(X)                                             \
  X((Bool, bool))                                                              \
  X((Int8, int8_t))                                                            \
  X((Int16, int16_t))                                                          \
  X((Int32, int32_t))                                                          \
  X((Int64, int64_t))                                                          \
  X((UInt8, uint8_t))                                                          \
  X((UInt16, uint16_t))                                                        \
  X((UInt32, uint32_t))                                                        \
  X((UInt64, uint64_t))                                                        \
  X((Char, char))                                                              \
  X((Float32, float))                                                          \
  X((Float64, double))                                                         \
  X((Float128, long double))                                                   \
  X((Int, int))                                                                \
  X((UInt, unsigned int))                                                      \
  X((Size, size_t))                                                            \
  X((PtrDiff, ptrdiff_t))                                                      \
  X((IntPtr, intptr_t))                                                        \
  X((UIntPtr, uintptr_t))

#define MATH_SIGNED_INT_TABLE(X)                                               \
  X((Int8, int8_t, INT8_MIN, INT8_MAX))                                        \
  X((Int16, int16_t, INT16_MIN, INT16_MAX))                                    \
  X((Int32, int32_t, INT32_MIN, INT32_MAX))                                    \
  X((Int64, int64_t, INT64_MIN, INT64_MAX))

#define MATH_UNSIGNED_INT_TABLE(X)                                             \
  X((UInt8, uint8_t, UINT8_MAX))                                               \
  X((UInt16, uint16_t, UINT16_MAX))                                            \
  X((UInt32, uint32_t, UINT32_MAX))                                            \
  X((UInt64, uint64_t, UINT64_MAX))

#define MATH_FLOAT_TYPE_TABLE(X)                                               \
  X((Float32, float))                                                          \
  X((Float64, double))                                                         \
  X((Float128, long double))

/* EOF */
