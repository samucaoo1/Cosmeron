#pragma once

#include "../../Core/Preprocessor/Compiling.inc"

#include "Struct.space"

#define TPAIR_TYPE(SUFFIX) STRUCT_TYPE(TPair, SUFFIX)
#define TPAIR_FUNC(SUFFIX, FUNC) STRUCT_FUNC(TPair, SUFFIX, FUNC)

#define TPAIR_TYPE_DECLARE(TYPE, SUFFIX)                                      \
  typedef union {                                                             \
    struct {                                                                  \
      union {                                                                 \
        TYPE value1;                                                          \
        TYPE first;                                                           \
        TYPE a;                                                               \
        TYPE x1;                                                              \
        TYPE x;                                                               \
      };                                                                      \
      union {                                                                 \
        TYPE value2;                                                          \
        TYPE second;                                                          \
        TYPE b;                                                               \
        TYPE x2;                                                              \
        TYPE y;                                                               \
      };                                                                      \
    };                                                                        \
    TYPE values[2];                                                           \
  } TPAIR_TYPE(SUFFIX);

#define TPAIR_CAST_TO_TYPE_PROTOTYPE(TYPE, SUFFIX)                     \
  inline static TPAIR_TYPE(SUFFIX)                                             \
      CAST_TYPE_TO_STRUCT(SUFFIX, TPair)(TYPE value1, TYPE value2)

#define TPAIR_CAST_FROM_TYPE_PROTOTYPE(TYPE, SUFFIX)                   \
  inline static void CAST_STRUCT_TO_TYPE(TPair, SUFFIX)(                       \
      TPAIR_TYPE(SUFFIX) pair, TYPE *var1, TYPE *var2)

#include "Impl/TPair.impl"

#define TPAIR_DECLARE(TYPE, SUFFIX)                                            \
  TPAIR_TYPE_DECLARE(TYPE, SUFFIX)                                             \
  TPAIR_CAST_TO_TYPE_PROTOTYPE(TYPE, SUFFIX);                          \
  TPAIR_CAST_FROM_TYPE_PROTOTYPE(TYPE, SUFFIX);                        \
  TPAIR_IMPLEMENT(TYPE, SUFFIX)

TPAIR_DECLARE(uint8_t, uint8)
TPAIR_DECLARE(uint16_t, uint16)
TPAIR_DECLARE(uint32_t, uint32)
TPAIR_DECLARE(uint64_t, uint64)
TPAIR_DECLARE(int8_t, int8)
TPAIR_DECLARE(int16_t, int16)
TPAIR_DECLARE(int32_t, int32)
TPAIR_DECLARE(int64_t, int64)
TPAIR_DECLARE(float, float)
TPAIR_DECLARE(double, double)
TPAIR_DECLARE(long double, longdouble)
/* EOF */
