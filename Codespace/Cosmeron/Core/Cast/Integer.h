#pragma once

#include "../Error/Status.h"
#include "../Module/Package.h"
#include "../Preprocessor/Compiling.inc"
#include "Cast.space"
#include "Converting.h"

#include <limits.h>
#include <stddef.h>
#include <stdint.h>

#define CAST_INTEGER_OPERATIONS                                                \
    /* 1. Unsigned 64-bit conversions */                                       \
    CONVERTING_OPERATIONS(                                                     \
        Uint64,                                                                \
        TO_EX(32, uint32_t, UNSIGNED, UINT32_MAX),                             \
        TO_EX(16, uint16_t, UNSIGNED, UINT16_MAX),                             \
        TO_EX(8, uint8_t, UNSIGNED, UINT8_MAX),                                \
        TO_EX(I64, int64_t, UNSIGNED, INT64_MAX),                              \
        TO_EX(I32, int32_t, UNSIGNED, INT32_MAX),                              \
        TO_EX(I16, int16_t, UNSIGNED, INT16_MAX),                              \
        TO_EX(I8, int8_t, UNSIGNED, INT8_MAX),                                 \
        TO_EX(Size, size_t, UNSIGNED, SIZE_MAX)                                \
    ),                                                                         \
                                                                               \
    /* 2. Unsigned 32-bit conversions */                                       \
    CONVERTING_OPERATIONS(                                                     \
        Uint32,                                                                \
        TO_EX(16, uint16_t, UNSIGNED, UINT16_MAX),                             \
        TO_EX(8, uint8_t, UNSIGNED, UINT8_MAX),                                \
        TO(I64, int64_t, SIGNED),                                              \
        TO_EX(I32, int32_t, UNSIGNED, INT32_MAX),                              \
        TO_EX(I16, int16_t, UNSIGNED, INT16_MAX),                              \
        TO_EX(I8, int8_t, UNSIGNED, INT8_MAX)                                  \
    ),                                                                         \
                                                                               \
    /* 3. Unsigned 16-bit conversions */                                       \
    CONVERTING_OPERATIONS(                                                     \
        Uint16,                                                                \
        TO_EX(8, uint8_t, UNSIGNED, UINT8_MAX),                                \
        TO(I64, int64_t, SIGNED),                                              \
        TO(I32, int32_t, SIGNED),                                              \
        TO_EX(I16, int16_t, UNSIGNED, INT16_MAX),                              \
        TO_EX(I8, int8_t, UNSIGNED, INT8_MAX)                                  \
    ),                                                                         \
                                                                               \
    /* 4. Unsigned 8-bit conversions */                                        \
    CONVERTING_OPERATIONS(                                                     \
        Uint8,                                                                 \
        TO(I64, int64_t, SIGNED),                                              \
        TO(I32, int32_t, SIGNED),                                              \
        TO(I16, int16_t, SIGNED),                                              \
        TO_EX(I8, int8_t, UNSIGNED, INT8_MAX)                                  \
    ),                                                                         \
                                                                               \
    /* 5. Signed 64-bit conversions */                                         \
    CONVERTING_OPERATIONS(                                                     \
        Int64,                                                                 \
        TO_EX(32, int32_t, SIGNED, INT32_MIN, INT32_MAX),                     \
        TO_EX(16, int16_t, SIGNED, INT16_MIN, INT16_MAX),                     \
        TO_EX(8, int8_t, SIGNED, INT8_MIN, INT8_MAX),                         \
        TO(U64, uint64_t, UNSIGNED),                                           \
        TO_EX(U32, uint32_t, UNSIGNED, UINT32_MAX),                            \
        TO_EX(U16, uint16_t, UNSIGNED, UINT16_MAX),                            \
        TO_EX(U8, uint8_t, UNSIGNED, UINT8_MAX),                               \
        TO_EX(Size, size_t, UNSIGNED, SIZE_MAX)                                \
    ),                                                                         \
                                                                               \
    /* 6. Signed 32-bit conversions */                                         \
    CONVERTING_OPERATIONS(                                                     \
        Int32,                                                                 \
        TO_EX(16, int16_t, SIGNED, INT16_MIN, INT16_MAX),                     \
        TO_EX(8, int8_t, SIGNED, INT8_MIN, INT8_MAX),                         \
        TO(U64, uint64_t, UNSIGNED),                                           \
        TO(U32, uint32_t, UNSIGNED),                                           \
        TO_EX(U16, uint16_t, UNSIGNED, UINT16_MAX),                            \
        TO_EX(U8, uint8_t, UNSIGNED, UINT8_MAX),                               \
        TO_EX(Size, size_t, UNSIGNED, SIZE_MAX)                                \
    ),                                                                         \
                                                                               \
    /* 7. Signed 16-bit conversions */                                         \
    CONVERTING_OPERATIONS(                                                     \
        Int16,                                                                 \
        TO_EX(8, int8_t, SIGNED, INT8_MIN, INT8_MAX),                         \
        TO(U64, uint64_t, UNSIGNED),                                           \
        TO(U32, uint32_t, UNSIGNED),                                           \
        TO(U16, uint16_t, UNSIGNED),                                           \
        TO_EX(U8, uint8_t, UNSIGNED, UINT8_MAX)                                \
    ),                                                                         \
                                                                               \
    /* 8. Signed 8-bit conversions */                                          \
    CONVERTING_OPERATIONS(                                                     \
        Int8,                                                                  \
        TO(U64, uint64_t, UNSIGNED),                                           \
        TO(U32, uint32_t, UNSIGNED),                                           \
        TO(U16, uint16_t, UNSIGNED),                                           \
        TO(U8, uint8_t, UNSIGNED)                                              \
    ),                                                                         \
                                                                               \
    /* 9. Size (size_t) conversions */                                         \
    CONVERTING_OPERATIONS(                                                     \
        Size,                                                                  \
        TO_EX(U64, uint64_t, UNSIGNED, UINT64_MAX),                            \
        TO_EX(U32, uint32_t, UNSIGNED, UINT32_MAX),                            \
        TO_EX(U16, uint16_t, UNSIGNED, UINT16_MAX),                            \
        TO_EX(U8, uint8_t, UNSIGNED, UINT8_MAX),                               \
        TO_EX(I64, int64_t, UNSIGNED, INT64_MAX),                              \
        TO_EX(I32, int32_t, UNSIGNED, INT32_MAX)                               \
    )

PACKAGE(
    LIB_PREFIX(CAST_MOD),
    CAST_INTEGER_OPERATIONS
);

#include "Impl/Integer.impl"
/* EOF */