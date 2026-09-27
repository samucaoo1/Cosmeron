#pragma once

#include "../../Core/Preprocessor/Compiling.inc"
#include "../../Core/Preprocessor/Detect/Compiler.h"

#include "Struct.space"

#define TDUAL_TYPE(SUFFIX) STRUCT_TYPE(TDual, SUFFIX)
#define TDUAL_FUNC(SUFFIX, FUNC) STRUCT_FUNC(TDual, SUFFIX, FUNC)

#define TDUAL_STRUCT(TYPE, SUFFIX)                                      \
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
  static inline TDUAL_TYPE(SUFFIX)                                             \
      CAST_TYPE_TO_STRUCT(SUFFIX, TDual)(TYPE value1, TYPE value2)

#define TDUAL_CAST_FROM_TYPE_PROTOTYPE(TYPE, SUFFIX)                   \
  static inline void CAST_STRUCT_TO_TYPE(TDual, SUFFIX)(                       \
      TDUAL_TYPE(SUFFIX) dual, TYPE *var1, TYPE *var2)

#include "Impl/TDual.impl"

#define TDUAL_IMPLEMENT_ALL(TYPE, SUFFIX)                                            \
  TDUAL_STRUCT(TYPE, SUFFIX)                                             \
  TDUAL_CAST_TO_TYPE_PROTOTYPE(TYPE, SUFFIX);                          \
  TDUAL_CAST_FROM_TYPE_PROTOTYPE(TYPE, SUFFIX);                        \
  TDUAL_CAST_IMPLEMENT_ALL(TYPE, SUFFIX)

TDUAL_IMPLEMENT_ALL(uint8_t, uint8)
TDUAL_IMPLEMENT_ALL(uint16_t, uint16)
TDUAL_IMPLEMENT_ALL(uint32_t, uint32)
TDUAL_IMPLEMENT_ALL(uint64_t, uint64)
TDUAL_IMPLEMENT_ALL(size_t, size)
TDUAL_IMPLEMENT_ALL(int8_t, int8)
TDUAL_IMPLEMENT_ALL(int16_t, int16)
TDUAL_IMPLEMENT_ALL(int32_t, int32)
TDUAL_IMPLEMENT_ALL(int64_t, int64)
TDUAL_IMPLEMENT_ALL(float, float)
TDUAL_IMPLEMENT_ALL(double, double)
TDUAL_IMPLEMENT_ALL(long double, longdouble)

#if !COMPILER_MSVC
TDUAL_IMPLEMENT_ALL(float _Complex, float_complex)
TDUAL_IMPLEMENT_ALL(double _Complex, double_complex)
TDUAL_IMPLEMENT_ALL(long double _Complex, longdouble_complex)
#endif
/* EOF */
