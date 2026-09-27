#pragma once

#include "../../Core/Preprocessor/Compiling.inc"
#include "../../Core/Preprocessor/Detect/Compiler.h"

#include "Struct.space"

#define TPENTA_TYPE(SUFFIX) STRUCT_TYPE(TPenta, SUFFIX)
#define TPENTA_FUNC(SUFFIX, FUNC) STRUCT_FUNC(TPenta, SUFFIX, FUNC)

#define TPENTA_TYPE_DECLARE(TYPE, SUFFIX)                                     \
  typedef union {                                                             \
    struct {                                                                  \
      union {                                                                 \
        TYPE value1;                                                          \
        TYPE a;                                                               \
        TYPE x1;                                                              \
        TYPE x;                                                               \
        TYPE c1;                                                              \
        TYPE y1;                                                              \
        TYPE state1;                                                          \
      };                                                                      \
      union {                                                                 \
        TYPE value2;                                                          \
        TYPE b;                                                               \
        TYPE x2;                                                              \
        TYPE y;                                                               \
        TYPE c2;                                                              \
        TYPE y2;                                                              \
        TYPE state2;                                                          \
      };                                                                      \
      union {                                                                 \
        TYPE value3;                                                          \
        TYPE c;                                                               \
        TYPE x3;                                                              \
        TYPE z;                                                               \
        TYPE c3;                                                              \
        TYPE y3;                                                              \
        TYPE state3;                                                          \
      };                                                                      \
      union {                                                                 \
        TYPE value4;                                                          \
        TYPE d;                                                               \
        TYPE x4;                                                              \
        TYPE w;                                                               \
        TYPE c4;                                                              \
        TYPE y4;                                                              \
        TYPE state4;                                                          \
      };                                                                      \
      union {                                                                 \
        TYPE value5;                                                          \
        TYPE e;                                                               \
        TYPE x5;                                                              \
        TYPE t;                                                               \
        TYPE c5;                                                              \
        TYPE y5;                                                              \
        TYPE state5;                                                          \
      };                                                                      \
    };                                                                        \
    TYPE values[5];                                                           \
  } TPENTA_TYPE(SUFFIX);

#define TPENTA_CAST_TO_TYPE_PROTOTYPE(TYPE, SUFFIX)                     \
  inline static TPENTA_TYPE(SUFFIX) CAST_TYPE_TO_STRUCT(SUFFIX, TPenta)(        \
      TYPE value1, TYPE value2, TYPE value3, TYPE value4,TYPE value5)

#define TPENTA_CAST_FROM_TYPE_PROTOTYPE(TYPE, SUFFIX)                   \
  inline static void CAST_STRUCT_TO_TYPE(TPenta, SUFFIX)(                       \
      TPENTA_TYPE(SUFFIX) penta, TYPE *var1, TYPE *var2, TYPE *var3,         \
      TYPE *var4,TYPE *var5)

#include "Impl/TPenta.impl"

#define TPENTA_DECLARE(TYPE, SUFFIX)                                            \
  TPENTA_TYPE_DECLARE(TYPE, SUFFIX)                                             \
  TPENTA_CAST_TO_TYPE_PROTOTYPE(TYPE, SUFFIX);                          \
  TPENTA_CAST_FROM_TYPE_PROTOTYPE(TYPE, SUFFIX);                        \
  TPENTA_IMPLEMENT(TYPE, SUFFIX)

TPENTA_DECLARE(uint8_t, uint8)
TPENTA_DECLARE(uint16_t, uint16)
TPENTA_DECLARE(uint32_t, uint32)
TPENTA_DECLARE(uint64_t, uint64)
TPENTA_DECLARE(int8_t, int8)
TPENTA_DECLARE(int16_t, int16)
TPENTA_DECLARE(int32_t, int32)
TPENTA_DECLARE(int64_t, int64)
TPENTA_DECLARE(float, float)
TPENTA_DECLARE(double, double)
TPENTA_DECLARE(long double, longdouble)

#if !COMPILER_MSVC
TPENTA_DECLARE(float _Complex, float_complex)
TPENTA_DECLARE(double _Complex, double_complex)
TPENTA_DECLARE(long double _Complex, longdouble_complex)
#endif
/* EOF */
