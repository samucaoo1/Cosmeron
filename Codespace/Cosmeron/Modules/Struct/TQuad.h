#pragma once

#include "../../Core/Preprocessor/Compiling.inc"
#include "../../Core/Preprocessor/Detect/Compiler.h"

#include "Struct.space"

#define TQUAD_TYPE(SUFFIX) STRUCT_TYPE(TQuad, SUFFIX)
#define TQUAD_FUNC(SUFFIX, FUNC) STRUCT_FUNC(TQuad, SUFFIX, FUNC)

#define TQUAD_TYPE_DECLARE(TYPE, SUFFIX)                                      \
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
  inline static TQUAD_TYPE(SUFFIX) CAST_TYPE_TO_STRUCT(SUFFIX, TQuad)(         \
      TYPE value1, TYPE value2, TYPE value3, TYPE value4)

#define TQUAD_CAST_FROM_TYPE_PROTOTYPE(TYPE, SUFFIX)                   \
  inline static void CAST_STRUCT_TO_TYPE(TQuad, SUFFIX)(                       \
      TQUAD_TYPE(SUFFIX) quad, TYPE *var1, TYPE *var2, TYPE *var3,          \
      TYPE *var4)

#include "Impl/TQuad.impl"

#define TQUAD_DECLARE(TYPE, SUFFIX)                                            \
  TQUAD_TYPE_DECLARE(TYPE, SUFFIX)                                             \
  TQUAD_CAST_TO_TYPE_PROTOTYPE(TYPE, SUFFIX);                          \
  TQUAD_CAST_FROM_TYPE_PROTOTYPE(TYPE, SUFFIX);                        \
  TQUAD_IMPLEMENT(TYPE, SUFFIX)

TQUAD_DECLARE(uint8_t, uint8)
TQUAD_DECLARE(uint16_t, uint16)
TQUAD_DECLARE(uint32_t, uint32)
TQUAD_DECLARE(uint64_t, uint64)
TQUAD_DECLARE(int8_t, int8)
TQUAD_DECLARE(int16_t, int16)
TQUAD_DECLARE(int32_t, int32)
TQUAD_DECLARE(int64_t, int64)
TQUAD_DECLARE(float, float)
TQUAD_DECLARE(double, double)
TQUAD_DECLARE(long double, longdouble)

#if !COMPILER_MSVC
TQUAD_DECLARE(float _Complex, float_complex)
TQUAD_DECLARE(double _Complex, double_complex)
TQUAD_DECLARE(long double _Complex, longdouble_complex)
#endif
/* EOF */
