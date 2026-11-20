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

#define MATH_EQUATION_CONFIG_TABLE(X)                                          \
  X((MAX_SQRT_ITERATIONS, 64U))

MODULE(DOMAIN(Modules), Math,
    ENUM(RangeResult, MATH_RANGE_RESULT_TABLE),
    ENUM(Solution, MATH_SOLUTION_TABLE),

    INTERNAL_EX(Equation,
        ENUM_EX(Config, MATH_EQUATION_CONFIG_TABLE, PLAIN)
    ),

    INTERNAL_EX(Arithmetic,
        VARIANT(Signed,
            OPERATION(Add, (int64_t a, int64_t b, int64_t minVal,
                            int64_t maxVal, int64_t *outResult), {
              if (!outResult) {
                return STATUS_CONST(INVALID_ARGUMENT);
              }
              if ((b > 0 && a > maxVal - b) || (b < 0 && a < minVal - b)) {
                return STATUS_CONST(OUT_OF_RANGE);
              }
              *outResult = a + b;
              return STATUS_CONST(SUCCESS);
            }),

            OPERATION(Sub, (int64_t a, int64_t b, int64_t minVal,
                            int64_t maxVal, int64_t *outResult), {
              if (!outResult) {
                return STATUS_CONST(INVALID_ARGUMENT);
              }
              if ((b < 0 && a > maxVal + b) || (b > 0 && a < minVal + b)) {
                return STATUS_CONST(OUT_OF_RANGE);
              }
              *outResult = a - b;
              return STATUS_CONST(SUCCESS);
            }),

            OPERATION(Mul, (int64_t a, int64_t b, int64_t minVal,
                            int64_t maxVal, int64_t *outResult), {
              if (!outResult) {
                return STATUS_CONST(INVALID_ARGUMENT);
              }
              if (a == 0 || b == 0) {
                *outResult = 0;
                return STATUS_CONST(SUCCESS);
              }
              if (a > 0) {
                if (b > 0 ? (a > maxVal / b) : (b < minVal / a)) {
                  return STATUS_CONST(OUT_OF_RANGE);
                }
              } else {
                if (b > 0 ? (a < minVal / b) : (b < maxVal / a)) {
                  return STATUS_CONST(OUT_OF_RANGE);
                }
              }
              *outResult = a * b;
              return STATUS_CONST(SUCCESS);
            }),

            OPERATION(Div, (int64_t a, int64_t b, int64_t minVal,
                            int64_t *outResult), {
              if (!outResult || b == 0) {
                return STATUS_CONST(INVALID_ARGUMENT);
              }
              if (a == minVal && b == -1) {
                return STATUS_CONST(OUT_OF_RANGE);
              }
              *outResult = a / b;
              return STATUS_CONST(SUCCESS);
            }),

            OPERATION(Mod, (int64_t a, int64_t b, int64_t minVal,
                            int64_t *outResult), {
              if (!outResult || b == 0) {
                return STATUS_CONST(INVALID_ARGUMENT);
              }
              if (a == minVal && b == -1) {
                return STATUS_CONST(OUT_OF_RANGE);
              }
              *outResult = a % b;
              return STATUS_CONST(SUCCESS);
            })
        ),

        VARIANT(Unsigned,
            OPERATION(Add, (uint64_t a, uint64_t b, uint64_t maxVal,
                            uint64_t *outResult), {
              if (!outResult) {
                return STATUS_CONST(INVALID_ARGUMENT);
              }
              if (a > maxVal - b) {
                return STATUS_CONST(OUT_OF_RANGE);
              }
              *outResult = a + b;
              return STATUS_CONST(SUCCESS);
            }),

            OPERATION(Sub, (uint64_t a, uint64_t b, uint64_t *outResult), {
              if (!outResult) {
                return STATUS_CONST(INVALID_ARGUMENT);
              }
              if (a < b) {
                return STATUS_CONST(OUT_OF_RANGE);
              }
              *outResult = a - b;
              return STATUS_CONST(SUCCESS);
            }),

            OPERATION(Mul, (uint64_t a, uint64_t b, uint64_t maxVal,
                            uint64_t *outResult), {
              if (!outResult) {
                return STATUS_CONST(INVALID_ARGUMENT);
              }
              if (a != 0U && b > maxVal / a) {
                return STATUS_CONST(OUT_OF_RANGE);
              }
              *outResult = a * b;
              return STATUS_CONST(SUCCESS);
            }),

            OPERATION(Div, (uint64_t a, uint64_t b, uint64_t *outResult), {
              if (!outResult || b == 0U) {
                return STATUS_CONST(INVALID_ARGUMENT);
              }
              *outResult = a / b;
              return STATUS_CONST(SUCCESS);
            }),

            OPERATION(Mod, (uint64_t a, uint64_t b, uint64_t *outResult), {
              if (!outResult || b == 0U) {
                return STATUS_CONST(INVALID_ARGUMENT);
              }
              *outResult = a % b;
              return STATUS_CONST(SUCCESS);
            })
        )
    )
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

#ifndef TEMPLATE_Math
#define TEMPLATE_Math(TARGET, ...) NS(TEMPLATE_Math, TARGET)(__VA_ARGS__)
#endif

/* EOF */
