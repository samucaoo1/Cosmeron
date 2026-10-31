#pragma once

#include "../Error/Status.h"
#include "../Module/Package.h"
#include "../Preprocessor/Compiling.inc"
#include "Cast.space"
#include "Converting.h"

#include <float.h>
#include <limits.h>
#include <math.h>
#include <stddef.h>
#include <stdint.h>

#define CAST_FLOAT_OPERATIONS                                                   \
    /* 1. Float (32-bit) conversions */                                        \
    CONVERTING_OPERATIONS(                                                     \
        Float,                                                                 \
        TO(Double, double, FLOAT),                                             \
        TO(LongDouble, long double, FLOAT),                                    \
        TO_EX(I64, int64_t, SIGNED, INT64_MIN, INT64_MAX),                     \
        TO_EX(I32, int32_t, SIGNED, INT32_MIN, INT32_MAX),                     \
        TO_EX(I16, int16_t, SIGNED, INT16_MIN, INT16_MAX),                     \
        TO_EX(I8, int8_t, SIGNED, INT8_MIN, INT8_MAX),                         \
        TO_EX(U64, uint64_t, UNSIGNED, UINT64_MAX),                            \
        TO_EX(U32, uint32_t, UNSIGNED, UINT32_MAX),                            \
        TO_EX(U16, uint16_t, UNSIGNED, UINT16_MAX),                            \
        TO_EX(U8, uint8_t, UNSIGNED, UINT8_MAX),                               \
        TO_EX(Size, size_t, UNSIGNED, SIZE_MAX)                                \
    ),                                                                         \
                                                                               \
    /* 2. Double (64-bit) conversions */                                       \
    CONVERTING_OPERATIONS(                                                     \
        Double,                                                                \
        TO_EX(Float, float, FLOAT, FLT_MAX),                                   \
        TO(LongDouble, long double, FLOAT),                                    \
        TO_EX(I64, int64_t, SIGNED, INT64_MIN, INT64_MAX),                     \
        TO_EX(I32, int32_t, SIGNED, INT32_MIN, INT32_MAX),                     \
        TO_EX(I16, int16_t, SIGNED, INT16_MIN, INT16_MAX),                     \
        TO_EX(I8, int8_t, SIGNED, INT8_MIN, INT8_MAX),                         \
        TO_EX(U64, uint64_t, UNSIGNED, UINT64_MAX),                            \
        TO_EX(U32, uint32_t, UNSIGNED, UINT32_MAX),                            \
        TO_EX(U16, uint16_t, UNSIGNED, UINT16_MAX),                            \
        TO_EX(U8, uint8_t, UNSIGNED, UINT8_MAX),                               \
        TO_EX(Size, size_t, UNSIGNED, SIZE_MAX)                                \
    ),                                                                         \
                                                                               \
    /* 3. LongDouble (extended precision) conversions */                       \
    CONVERTING_OPERATIONS(                                                     \
        LongDouble,                                                            \
        TO_EX(Float, float, FLOAT, FLT_MAX),                                   \
        TO_EX(Double, double, FLOAT, DBL_MAX),                                 \
        TO_EX(I64, int64_t, SIGNED, INT64_MIN, INT64_MAX),                     \
        TO_EX(I32, int32_t, SIGNED, INT32_MIN, INT32_MAX),                     \
        TO_EX(I16, int16_t, SIGNED, INT16_MIN, INT16_MAX),                     \
        TO_EX(I8, int8_t, SIGNED, INT8_MIN, INT8_MAX),                         \
        TO_EX(U64, uint64_t, UNSIGNED, UINT64_MAX),                            \
        TO_EX(U32, uint32_t, UNSIGNED, UINT32_MAX),                            \
        TO_EX(U16, uint16_t, UNSIGNED, UINT16_MAX),                            \
        TO_EX(U8, uint8_t, UNSIGNED, UINT8_MAX),                               \
        TO_EX(Size, size_t, UNSIGNED, SIZE_MAX)                                \
    ),                                                                         \
                                                                               \
    /* 4. Integer to Float / Double / LongDouble conversions */                \
    CONVERTING_OPERATIONS(                                                     \
        Int64,                                                                 \
        TO(Float, float, FLOAT),                                               \
        TO(Double, double, FLOAT),                                             \
        TO(LongDouble, long double, FLOAT)                                     \
    ),                                                                         \
    CONVERTING_OPERATIONS(                                                     \
        Int32,                                                                 \
        TO(Float, float, FLOAT),                                               \
        TO(Double, double, FLOAT),                                             \
        TO(LongDouble, long double, FLOAT)                                     \
    ),                                                                         \
    CONVERTING_OPERATIONS(                                                     \
        Uint64,                                                                \
        TO(Float, float, FLOAT),                                               \
        TO(Double, double, FLOAT),                                             \
        TO(LongDouble, long double, FLOAT)                                     \
    ),                                                                         \
    CONVERTING_OPERATIONS(                                                     \
        Uint32,                                                                \
        TO(Float, float, FLOAT),                                               \
        TO(Double, double, FLOAT),                                             \
        TO(LongDouble, long double, FLOAT)                                     \
    ),                                                                         \
    CONVERTING_OPERATIONS(                                                     \
        Size,                                                                  \
        TO(Float, float, FLOAT),                                               \
        TO(Double, double, FLOAT),                                             \
        TO(LongDouble, long double, FLOAT)                                     \
    )

#include "Impl/Float.impl"

PACKAGE(
    GNS2(LIB_PREFIX(CAST_MOD), Float),
    CAST_FLOAT_OPERATIONS
);
/* EOF */

