#pragma once

#include "../../Core/Preprocessor/Compiling.inc"
#include "../../Core/Preprocessor/Detect/Compiler.h"

#include "Struct.space"

#define TDUAL_TYPE(SUFFIX) STRUCT_TYPE(TDual, SUFFIX)
#define TDUAL_FUNC(SUFFIX, FUNC) STRUCT_FUNC(TDual, SUFFIX, FUNC)

#define TDUAL_TYPE_DECLARE(TYPE, SUFFIX)                                      \
  typedef union {                                                             \
    struct {                                                                  \
      union {                                                                 \
        TYPE value1;                                                          \
        TYPE a;                                                               \
        TYPE x1;                                                              \
        TYPE x;                                                               \
        TYPE X;                                                               \
        TYPE col;                                                             \
        TYPE length;                                                          \
        TYPE real;                                                            \
        TYPE state1;                                                          \
        TYPE begin;                                                           \
        TYPE first;                                                           \
        TYPE start;                                                           \
      };                                                                      \
      union {                                                                 \
        TYPE value2;                                                          \
        TYPE b;                                                               \
        TYPE x2;                                                              \
        TYPE y;                                                               \
        TYPE Y;                                                               \
        TYPE row;                                                             \
        TYPE width;                                                           \
        TYPE imaginary;                                                       \
        TYPE state2;                                                          \
        TYPE end;                                                             \
        TYPE second;                                                          \
        TYPE last;                                                            \
      };                                                                      \
    };                                                                        \
    TYPE value[2];                                                            \
  } TDUAL_TYPE(SUFFIX);

#define TDUAL_CAST_TO_TYPE_PROTOTYPE(TYPE, SUFFIX)                     \
  inline static TDUAL_TYPE(SUFFIX)                                             \
      CAST_TYPE_TO_STRUCT(SUFFIX, TDual)(TYPE value1, TYPE value2)

#define TDUAL_CAST_FROM_TYPE_PROTOTYPE(TYPE, SUFFIX)                   \
  inline static void CAST_STRUCT_TO_TYPE(TDual, SUFFIX)(                       \
      TDUAL_TYPE(SUFFIX) dual, TYPE *var1, TYPE *var2)

#include "Impl/TDual.impl"

#define TDUAL_DECLARE(TYPE, SUFFIX)                                            \
  TDUAL_TYPE_DECLARE(TYPE, SUFFIX)                                             \
  TDUAL_CAST_TO_TYPE_PROTOTYPE(TYPE, SUFFIX);                          \
  TDUAL_CAST_FROM_TYPE_PROTOTYPE(TYPE, SUFFIX);                        \
  TDUAL_IMPLEMENT(TYPE, SUFFIX)

TDUAL_DECLARE(uint8_t, uint8)
TDUAL_DECLARE(uint16_t, uint16)
TDUAL_DECLARE(uint32_t, uint32)
TDUAL_DECLARE(uint64_t, uint64)
TDUAL_DECLARE(size_t, size)
TDUAL_DECLARE(int8_t, int8)
TDUAL_DECLARE(int16_t, int16)
TDUAL_DECLARE(int32_t, int32)
TDUAL_DECLARE(int64_t, int64)
TDUAL_DECLARE(float, float)
TDUAL_DECLARE(double, double)
TDUAL_DECLARE(long double, longdouble)

#if !COMPILER_MSVC
TDUAL_DECLARE(float _Complex, float_complex)
TDUAL_DECLARE(double _Complex, double_complex)
TDUAL_DECLARE(long double _Complex, longdouble_complex)
#endif
/* EOF */
