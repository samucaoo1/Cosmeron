#pragma once

#include "../Preprocessor/Compiling.inc"
#include "../Preprocessor/Operations.h"
#include "Primitive.space"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* ========================================================================== */
/* Single Source of Truth: Master Primitive Types Table                       */
/* ========================================================================== */
/*
 * Layout for PRIMITIVE_TYPES_TABLE(X):
 * X(TYPE, LONG_NAME, SHORT_NAME, T_PATTERN, CATEGORY, BITS)
 *
 * - TYPE:        C standard / stdint type
 * - LONG_NAME:   PascalCase descriptive name (e.g. Int32, Float32)
 * - SHORT_NAME:  Abbreviated token (e.g. I32, F32)
 * - T_PATTERN:   Cosmeron public T-type name (e.g. TInt32, TFloat32)
 * - CATEGORY:    Semantic kind (BOOL, INT, UINT, CHAR, FLOAT, PTR)
 * - BITS:        Bit width (8, 16, 32, 64; 0 if architecture-dependent)
 */

#define PRIMITIVE_TYPES_TABLE(X)                                               \
  /* Boolean */                                                                \
  X(bool, Boolean, Bool, TBoolean, BOOL, 1)                                    \
  /* Exact-width signed integers */                                            \
  X(int8_t, Int8, I8, TInt8, INT, 8)                                           \
  X(int16_t, Int16, I16, TInt16, INT, 16)                                      \
  X(int32_t, Int32, I32, TInt32, INT, 32)                                      \
  X(int64_t, Int64, I64, TInt64, INT, 64)                                      \
  /* Exact-width unsigned integers */                                          \
  X(uint8_t, UInt8, U8, TUInt8, UINT, 8)                                       \
  X(uint16_t, UInt16, U16, TUInt16, UINT, 16)                                  \
  X(uint32_t, UInt32, U32, TUInt32, UINT, 32)                                  \
  X(uint64_t, UInt64, U64, TUInt64, UINT, 64)                                  \
  /* Characters */                                                             \
  X(char, Char, Ch, TChar, CHAR, 8)                                            \
  X(uint8_t, Char8, Ch8, TChar8, CHAR, 8)                                      \
  X(uint16_t, Char16, Ch16, TChar16, CHAR, 16)                                 \
  X(uint32_t, Char32, Ch32, TChar32, CHAR, 32)                                 \
  /* Floating-point (exact width) */                                           \
  X(float, Float32, F32, TFloat32, FLOAT, 32)                                  \
  X(double, Float64, F64, TFloat64, FLOAT, 64)                                 \
  /* Standard Floating-point */                                                \
  X(float, Float, Float, TFloat, FLOAT, 32)                                    \
  X(double, Double, Double, TDouble, FLOAT, 64)                                \
  X(long double, LongDouble, LD, TLongDouble, FLOAT, 0)                        \
  /* Architecture / Pointer-sized */                                           \
  X(int, Int, Int, TInt, INT, 0)                                               \
  X(unsigned int, UInt, UInt, TUInt, UINT, 0)                                  \
  X(size_t, Size, Size, TSize, UINT, 0)                                        \
  X(ptrdiff_t, PtrDiff, Diff, TPtrDiff, INT, 0)                                \
  X(intptr_t, IntPtr, IPtr, TIntPtr, INT, 0)                                   \
  X(uintptr_t, UIntPtr, UPtr, TUIntPtr, UINT, 0)

/* ========================================================================== */
/* One-Source Category Dispatch (Zero Duplication)                            */
/* ========================================================================== */

/* Filter: INT */
#define COSMERON_MACRO_INTERNAL_FILTER_INT_BOOL(X, TYPE, L, S, T, BITS)
#define COSMERON_MACRO_INTERNAL_FILTER_INT_INT(X, TYPE, L, S, T, BITS)        X(TYPE, L, S, T, INT, BITS)
#define COSMERON_MACRO_INTERNAL_FILTER_INT_UINT(X, TYPE, L, S, T, BITS)
#define COSMERON_MACRO_INTERNAL_FILTER_INT_CHAR(X, TYPE, L, S, T, BITS)
#define COSMERON_MACRO_INTERNAL_FILTER_INT_FLOAT(X, TYPE, L, S, T, BITS)
#define COSMERON_MACRO_INTERNAL_FILTER_INT_PTR(X, TYPE, L, S, T, BITS)

/* Filter: UINT */
#define COSMERON_MACRO_INTERNAL_FILTER_UINT_BOOL(X, TYPE, L, S, T, BITS)
#define COSMERON_MACRO_INTERNAL_FILTER_UINT_INT(X, TYPE, L, S, T, BITS)
#define COSMERON_MACRO_INTERNAL_FILTER_UINT_UINT(X, TYPE, L, S, T, BITS)       X(TYPE, L, S, T, UINT, BITS)
#define COSMERON_MACRO_INTERNAL_FILTER_UINT_CHAR(X, TYPE, L, S, T, BITS)
#define COSMERON_MACRO_INTERNAL_FILTER_UINT_FLOAT(X, TYPE, L, S, T, BITS)
#define COSMERON_MACRO_INTERNAL_FILTER_UINT_PTR(X, TYPE, L, S, T, BITS)

/* Filter: INTEGER (INT | UINT) */
#define COSMERON_MACRO_INTERNAL_FILTER_INTEGER_BOOL(X, TYPE, L, S, T, BITS)
#define COSMERON_MACRO_INTERNAL_FILTER_INTEGER_INT(X, TYPE, L, S, T, BITS)     X(TYPE, L, S, T, INT, BITS)
#define COSMERON_MACRO_INTERNAL_FILTER_INTEGER_UINT(X, TYPE, L, S, T, BITS)    X(TYPE, L, S, T, UINT, BITS)
#define COSMERON_MACRO_INTERNAL_FILTER_INTEGER_CHAR(X, TYPE, L, S, T, BITS)
#define COSMERON_MACRO_INTERNAL_FILTER_INTEGER_FLOAT(X, TYPE, L, S, T, BITS)
#define COSMERON_MACRO_INTERNAL_FILTER_INTEGER_PTR(X, TYPE, L, S, T, BITS)

/* Filter: FLOAT */
#define COSMERON_MACRO_INTERNAL_FILTER_FLOAT_BOOL(X, TYPE, L, S, T, BITS)
#define COSMERON_MACRO_INTERNAL_FILTER_FLOAT_INT(X, TYPE, L, S, T, BITS)
#define COSMERON_MACRO_INTERNAL_FILTER_FLOAT_UINT(X, TYPE, L, S, T, BITS)
#define COSMERON_MACRO_INTERNAL_FILTER_FLOAT_CHAR(X, TYPE, L, S, T, BITS)
#define COSMERON_MACRO_INTERNAL_FILTER_FLOAT_FLOAT(X, TYPE, L, S, T, BITS)     X(TYPE, L, S, T, FLOAT, BITS)
#define COSMERON_MACRO_INTERNAL_FILTER_FLOAT_PTR(X, TYPE, L, S, T, BITS)

/* Filter: NUMERIC (INT | UINT | FLOAT) */
#define COSMERON_MACRO_INTERNAL_FILTER_NUMERIC_BOOL(X, TYPE, L, S, T, BITS)
#define COSMERON_MACRO_INTERNAL_FILTER_NUMERIC_INT(X, TYPE, L, S, T, BITS)     X(TYPE, L, S, T, INT, BITS)
#define COSMERON_MACRO_INTERNAL_FILTER_NUMERIC_UINT(X, TYPE, L, S, T, BITS)    X(TYPE, L, S, T, UINT, BITS)
#define COSMERON_MACRO_INTERNAL_FILTER_NUMERIC_CHAR(X, TYPE, L, S, T, BITS)
#define COSMERON_MACRO_INTERNAL_FILTER_NUMERIC_FLOAT(X, TYPE, L, S, T, BITS)   X(TYPE, L, S, T, FLOAT, BITS)
#define COSMERON_MACRO_INTERNAL_FILTER_NUMERIC_PTR(X, TYPE, L, S, T, BITS)

/* Filter: CHAR */
#define COSMERON_MACRO_INTERNAL_FILTER_CHAR_BOOL(X, TYPE, L, S, T, BITS)
#define COSMERON_MACRO_INTERNAL_FILTER_CHAR_INT(X, TYPE, L, S, T, BITS)
#define COSMERON_MACRO_INTERNAL_FILTER_CHAR_UINT(X, TYPE, L, S, T, BITS)
#define COSMERON_MACRO_INTERNAL_FILTER_CHAR_CHAR(X, TYPE, L, S, T, BITS)       X(TYPE, L, S, T, CHAR, BITS)
#define COSMERON_MACRO_INTERNAL_FILTER_CHAR_FLOAT(X, TYPE, L, S, T, BITS)
#define COSMERON_MACRO_INTERNAL_FILTER_CHAR_PTR(X, TYPE, L, S, T, BITS)

/* Filter: BOOL */
#define COSMERON_MACRO_INTERNAL_FILTER_BOOL_BOOL(X, TYPE, L, S, T, BITS)       X(TYPE, L, S, T, BOOL, BITS)
#define COSMERON_MACRO_INTERNAL_FILTER_BOOL_INT(X, TYPE, L, S, T, BITS)
#define COSMERON_MACRO_INTERNAL_FILTER_BOOL_UINT(X, TYPE, L, S, T, BITS)
#define COSMERON_MACRO_INTERNAL_FILTER_BOOL_CHAR(X, TYPE, L, S, T, BITS)
#define COSMERON_MACRO_INTERNAL_FILTER_BOOL_FLOAT(X, TYPE, L, S, T, BITS)
#define COSMERON_MACRO_INTERNAL_FILTER_BOOL_PTR(X, TYPE, L, S, T, BITS)

/* ========================================================================== */
/* Public Derived Tables (100% Projected from PRIMITIVE_TYPES_TABLE)          */
/* ========================================================================== */

#define PRIMITIVE_FILTER_EXPAND(X, KIND, TYPE, L, S, T, CAT, BITS)            \
  PP_OP_CAT4(COSMERON_MACRO_INTERNAL_FILTER_, KIND, _, CAT)(X, TYPE, L, S, T, BITS)

#define PRIMITIVE_INTEGER_TYPES_TABLE(X)                                       \
  PRIMITIVE_TYPES_TABLE(COSMERON_MACRO_INTERNAL_BIND_INTEGER(X))
#define COSMERON_MACRO_INTERNAL_BIND_INTEGER(X)                                \
  COSMERON_MACRO_INTERNAL_ADAPT_INTEGER_##X
#define COSMERON_MACRO_INTERNAL_ADAPT_INTEGER_X(TYPE, L, S, T, CAT, BITS)      \
  PRIMITIVE_FILTER_EXPAND(X, INTEGER, TYPE, L, S, T, CAT, BITS)

/* Generic Category Projection Macro:
 * PRIMITIVE_TYPES_TABLE_FILTER(KIND, X)
 * Example: PRIMITIVE_TYPES_TABLE_FILTER(NUMERIC, MY_MACRO)
 */
#define PRIMITIVE_TYPES_TABLE_FILTER(KIND, X)                                  \
  PRIMITIVE_TYPES_TABLE(COSMERON_MACRO_INTERNAL_FILTER_PROJECTOR_##KIND(X))

#define COSMERON_MACRO_INTERNAL_FILTER_PROJECTOR_INTEGER(X)                   \
  COSMERON_MACRO_INTERNAL_PROJ_INTEGER(X)
#define COSMERON_MACRO_INTERNAL_FILTER_PROJECTOR_NUMERIC(X)                   \
  COSMERON_MACRO_INTERNAL_PROJ_NUMERIC(X)
#define COSMERON_MACRO_INTERNAL_FILTER_PROJECTOR_FLOAT(X)                     \
  COSMERON_MACRO_INTERNAL_PROJ_FLOAT(X)
#define COSMERON_MACRO_INTERNAL_FILTER_PROJECTOR_CHAR(X)                      \
  COSMERON_MACRO_INTERNAL_PROJ_CHAR(X)
#define COSMERON_MACRO_INTERNAL_FILTER_PROJECTOR_BOOL(X)                      \
  COSMERON_MACRO_INTERNAL_PROJ_BOOL(X)

#define COSMERON_MACRO_INTERNAL_PROJ_INTEGER(X)                               \
  COSMERON_MACRO_INTERNAL_INTEGER_APPLY_##X
#define COSMERON_MACRO_INTERNAL_PROJ_NUMERIC(X)                               \
  COSMERON_MACRO_INTERNAL_NUMERIC_APPLY_##X
#define COSMERON_MACRO_INTERNAL_PROJ_FLOAT(X)                                 \
  COSMERON_MACRO_INTERNAL_FLOAT_APPLY_##X
#define COSMERON_MACRO_INTERNAL_PROJ_CHAR(X)                                  \
  COSMERON_MACRO_INTERNAL_CHAR_APPLY_##X
#define COSMERON_MACRO_INTERNAL_PROJ_BOOL(X)                                  \
  COSMERON_MACRO_INTERNAL_BOOL_APPLY_##X

#define COSMERON_MACRO_INTERNAL_INTEGER_APPLY_X(TYPE, L, S, T, CAT, BITS)      \
  PRIMITIVE_FILTER_EXPAND(X, INTEGER, TYPE, L, S, T, CAT, BITS)
#define COSMERON_MACRO_INTERNAL_NUMERIC_APPLY_X(TYPE, L, S, T, CAT, BITS)      \
  PRIMITIVE_FILTER_EXPAND(X, NUMERIC, TYPE, L, S, T, CAT, BITS)
#define COSMERON_MACRO_INTERNAL_FLOAT_APPLY_X(TYPE, L, S, T, CAT, BITS)        \
  PRIMITIVE_FILTER_EXPAND(X, FLOAT, TYPE, L, S, T, CAT, BITS)
#define COSMERON_MACRO_INTERNAL_CHAR_APPLY_X(TYPE, L, S, T, CAT, BITS)         \
  PRIMITIVE_FILTER_EXPAND(X, CHAR, TYPE, L, S, T, CAT, BITS)
#define COSMERON_MACRO_INTERNAL_BOOL_APPLY_X(TYPE, L, S, T, CAT, BITS)         \
  PRIMITIVE_FILTER_EXPAND(X, BOOL, TYPE, L, S, T, CAT, BITS)

/* Convenient Category Views */
#define PRIMITIVE_FOR_EACH_INTEGER(X) PRIMITIVE_TYPES_TABLE_FILTER(INTEGER, X)
#define PRIMITIVE_FOR_EACH_NUMERIC(X) PRIMITIVE_TYPES_TABLE_FILTER(NUMERIC, X)
#define PRIMITIVE_FOR_EACH_FLOAT(X)   PRIMITIVE_TYPES_TABLE_FILTER(FLOAT, X)
#define PRIMITIVE_FOR_EACH_CHAR(X)    PRIMITIVE_TYPES_TABLE_FILTER(CHAR, X)
#define PRIMITIVE_FOR_EACH_BOOL(X)    PRIMITIVE_TYPES_TABLE_FILTER(BOOL, X)
/* EOF */
