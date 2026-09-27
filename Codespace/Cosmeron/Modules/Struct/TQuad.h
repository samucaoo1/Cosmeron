#pragma once

#include "../../Core/Preprocessor/Compiling.inc"
#include "../../Core/Preprocessor/Detect/Compiler.h"

#include "Struct.space"

#define TQUAD_TYPE(SUFFIX) STRUCT_TYPE(TQuad, SUFFIX)
#define TQUAD_FUNC(SUFFIX, FUNC) STRUCT_FUNC(TQuad, SUFFIX, FUNC)

#define TQUAD_STRUCT(TYPE, SUFFIX)                                      \
  typedef union {                                                             \
    struct {                                                                  \
      union {                                                                 \
        TYPE value1;                                                          \
        TYPE a;                                                               \
        TYPE x1;                                                              \
        TYPE x;                                                               \
        TYPE left;                                                            \
        TYPE state1;                                                          \
      };                                                                      \
      union {                                                                 \
        TYPE value2;                                                          \
        TYPE b;                                                               \
        TYPE x2;                                                              \
        TYPE y;                                                               \
        TYPE right;                                                           \
        TYPE state2;                                                          \
      };                                                                      \
      union {                                                                 \
        TYPE value3;                                                          \
        TYPE c;                                                               \
        TYPE x3;                                                              \
        TYPE z;                                                               \
        TYPE top;                                                             \
        TYPE y1;                                                              \
        TYPE state3;                                                          \
      };                                                                      \
      union {                                                                 \
        TYPE value4;                                                          \
        TYPE d;                                                               \
        TYPE x4;                                                              \
        TYPE w;                                                               \
        TYPE bottom;                                                          \
        TYPE y2;                                                              \
        TYPE state4;                                                          \
      };                                                                      \
    };                                                                        \
    TYPE values[4];                                                           \
  } TQUAD_TYPE(SUFFIX);

#define TQUAD_CAST_TO_TYPE_PROTOTYPE(TYPE, SUFFIX)                     \
  static inline TQUAD_TYPE(SUFFIX) CAST_TYPE_TO_STRUCT(SUFFIX, TQuad)(         \
      TYPE value1, TYPE value2, TYPE value3, TYPE value4)

#define TQUAD_CAST_FROM_TYPE_PROTOTYPE(TYPE, SUFFIX)                   \
  static inline void CAST_STRUCT_TO_TYPE(TQuad, SUFFIX)(                       \
      TQUAD_TYPE(SUFFIX) quad, TYPE *var1, TYPE *var2, TYPE *var3,          \
      TYPE *var4)

#include "Impl/TQuad.impl"

#define TQUAD_IMPLEMENT_ALL(TYPE, SUFFIX)                                            \
  TQUAD_STRUCT(TYPE, SUFFIX)                                             \
  TQUAD_CAST_TO_TYPE_PROTOTYPE(TYPE, SUFFIX);                          \
  TQUAD_CAST_FROM_TYPE_PROTOTYPE(TYPE, SUFFIX);                        \
  TQUAD_CAST_IMPLEMENT_ALL(TYPE, SUFFIX)

TQUAD_IMPLEMENT_ALL(uint8_t, uint8)
TQUAD_IMPLEMENT_ALL(uint16_t, uint16)
TQUAD_IMPLEMENT_ALL(uint32_t, uint32)
TQUAD_IMPLEMENT_ALL(uint64_t, uint64)
TQUAD_IMPLEMENT_ALL(int8_t, int8)
TQUAD_IMPLEMENT_ALL(int16_t, int16)
TQUAD_IMPLEMENT_ALL(int32_t, int32)
TQUAD_IMPLEMENT_ALL(int64_t, int64)
TQUAD_IMPLEMENT_ALL(float, float)
TQUAD_IMPLEMENT_ALL(double, double)
TQUAD_IMPLEMENT_ALL(long double, longdouble)

#if !COMPILER_MSVC
TQUAD_IMPLEMENT_ALL(float _Complex, float_complex)
TQUAD_IMPLEMENT_ALL(double _Complex, double_complex)
TQUAD_IMPLEMENT_ALL(long double _Complex, longdouble_complex)
#endif
/* EOF */
