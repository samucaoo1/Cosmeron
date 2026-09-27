#pragma once

#include "../../Core/Preprocessor/Compiling.inc"
#include "../../Core/Preprocessor/Detect/Compiler.h"

#include "Struct.space"

#define TTRIPLE_TYPE(SUFFIX) STRUCT_TYPE(TTriple, SUFFIX)
#define TTRIPLE_FUNC(SUFFIX, FUNC) STRUCT_FUNC(TTriple, SUFFIX, FUNC)

#define TTRIPLE_STRUCT(TYPE, SUFFIX)                                    \
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
  static inline TTRIPLE_TYPE(SUFFIX) CAST_TYPE_TO_STRUCT(SUFFIX, TTriple)(     \
      TYPE value1, TYPE value2, TYPE value3)

#define TTRIPLE_CAST_FROM_TYPE_PROTOTYPE(TYPE, SUFFIX)                 \
  static inline void CAST_STRUCT_TO_TYPE(TTriple, SUFFIX)(                     \
      TTRIPLE_TYPE(SUFFIX) triple, TYPE *var1, TYPE *var2, TYPE *var3)

#include "Impl/TTriple.impl"

#define TTRIPLE_IMPLEMENT_ALL(TYPE, SUFFIX)                                          \
  TTRIPLE_STRUCT(TYPE, SUFFIX)                                           \
  TTRIPLE_CAST_TO_TYPE_PROTOTYPE(TYPE, SUFFIX);                        \
  TTRIPLE_CAST_FROM_TYPE_PROTOTYPE(TYPE, SUFFIX);                      \
  TTRIPLE_CAST_IMPLEMENT_ALL(TYPE, SUFFIX)

TTRIPLE_IMPLEMENT_ALL(uint8_t, uint8)
TTRIPLE_IMPLEMENT_ALL(uint16_t, uint16)
TTRIPLE_IMPLEMENT_ALL(uint32_t, uint32)
TTRIPLE_IMPLEMENT_ALL(uint64_t, uint64)
TTRIPLE_IMPLEMENT_ALL(int8_t, int8)
TTRIPLE_IMPLEMENT_ALL(int16_t, int16)
TTRIPLE_IMPLEMENT_ALL(int32_t, int32)
TTRIPLE_IMPLEMENT_ALL(int64_t, int64)
TTRIPLE_IMPLEMENT_ALL(float, float)
TTRIPLE_IMPLEMENT_ALL(double, double)
TTRIPLE_IMPLEMENT_ALL(long double, longdouble)

#if !COMPILER_MSVC
TTRIPLE_IMPLEMENT_ALL(float _Complex, float_complex)
TTRIPLE_IMPLEMENT_ALL(double _Complex, double_complex)
TTRIPLE_IMPLEMENT_ALL(long double _Complex, longdouble_complex)
#endif
/* EOF */
