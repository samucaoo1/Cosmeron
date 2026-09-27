#pragma once

#include "../../Core/Preprocessor/Compiling.inc"
#include "../../Core/Preprocessor/Detect/Compiler.h"

#include "Struct.space"

#define TTRIPLE_TYPE(SUFFIX) STRUCT_TYPE(TTriple, SUFFIX)
#define TTRIPLE_FUNC(SUFFIX, FUNC) STRUCT_FUNC(TTriple, SUFFIX, FUNC)

#define TTRIPLE_TYPE_DECLARE(TYPE, SUFFIX)                                    \
  typedef union {                                                             \
    struct {                                                                  \
      union {                                                                 \
        TYPE value1;                                                          \
        TYPE a;                                                               \
        TYPE x1;                                                              \
        TYPE x;                                                               \
        TYPE length;                                                          \
        TYPE state1;                                                          \
      };                                                                      \
      union {                                                                 \
        TYPE value2;                                                          \
        TYPE b;                                                               \
        TYPE x2;                                                              \
        TYPE y;                                                               \
        TYPE width;                                                           \
        TYPE state2;                                                          \
      };                                                                      \
      union {                                                                 \
        TYPE value3;                                                          \
        TYPE c;                                                               \
        TYPE x3;                                                              \
        TYPE z;                                                               \
        TYPE height;                                                          \
        TYPE state3;                                                          \
      };                                                                      \
    };                                                                        \
    TYPE values[3];                                                           \
  } TTRIPLE_TYPE(SUFFIX);

#define TTRIPLE_CAST_TO_TYPE_PROTOTYPE(TYPE, SUFFIX)                   \
  inline static TTRIPLE_TYPE(SUFFIX) CAST_TYPE_TO_STRUCT(SUFFIX, TTriple)(     \
      TYPE value1, TYPE value2, TYPE value3)

#define TTRIPLE_CAST_FROM_TYPE_PROTOTYPE(TYPE, SUFFIX)                 \
  inline static void CAST_STRUCT_TO_TYPE(TTriple, SUFFIX)(                     \
      TTRIPLE_TYPE(SUFFIX) triple, TYPE *var1, TYPE *var2, TYPE *var3)

#include "Impl/TTriple.impl"

#define TTRIPLE_DECLARE(TYPE, SUFFIX)                                          \
  TTRIPLE_TYPE_DECLARE(TYPE, SUFFIX)                                           \
  TTRIPLE_CAST_TO_TYPE_PROTOTYPE(TYPE, SUFFIX);                        \
  TTRIPLE_CAST_FROM_TYPE_PROTOTYPE(TYPE, SUFFIX);                      \
  TTRIPLE_IMPLEMENT(TYPE, SUFFIX)

TTRIPLE_DECLARE(uint8_t, uint8)
TTRIPLE_DECLARE(uint16_t, uint16)
TTRIPLE_DECLARE(uint32_t, uint32)
TTRIPLE_DECLARE(uint64_t, uint64)
TTRIPLE_DECLARE(int8_t, int8)
TTRIPLE_DECLARE(int16_t, int16)
TTRIPLE_DECLARE(int32_t, int32)
TTRIPLE_DECLARE(int64_t, int64)
TTRIPLE_DECLARE(float, float)
TTRIPLE_DECLARE(double, double)
TTRIPLE_DECLARE(long double, longdouble)

#if !COMPILER_MSVC
TTRIPLE_DECLARE(float _Complex, float_complex)
TTRIPLE_DECLARE(double _Complex, double_complex)
TTRIPLE_DECLARE(long double _Complex, longdouble_complex)
#endif
/* EOF */
