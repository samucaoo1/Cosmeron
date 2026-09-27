#pragma once

#include "../../Core/Preprocessor/Compiling.inc"

#include "Struct.space"

#define TPAIR_TYPE(SUFFIX) STRUCT_TYPE(TPair, SUFFIX)
#define TPAIR_FUNC(SUFFIX, FUNC) STRUCT_FUNC(TPair, SUFFIX, FUNC)

#define TPAIR_STRUCT(TYPE, SUFFIX)                                      \
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
  static inline TPAIR_TYPE(SUFFIX)                                             \
      CAST_TYPE_TO_STRUCT(SUFFIX, TPair)(TYPE value1, TYPE value2)

#define TPAIR_CAST_FROM_TYPE_PROTOTYPE(TYPE, SUFFIX)                   \
  static inline void CAST_STRUCT_TO_TYPE(TPair, SUFFIX)(                       \
      TPAIR_TYPE(SUFFIX) pair, TYPE *var1, TYPE *var2)

#include "Impl/TPair.impl"

#define TPAIR_IMPLEMENT_ALL(TYPE, SUFFIX)                                            \
  TPAIR_STRUCT(TYPE, SUFFIX)                                             \
  TPAIR_CAST_TO_TYPE_PROTOTYPE(TYPE, SUFFIX);                          \
  TPAIR_CAST_FROM_TYPE_PROTOTYPE(TYPE, SUFFIX);                        \
  TPAIR_CAST_IMPLEMENT_ALL(TYPE, SUFFIX)

TPAIR_IMPLEMENT_ALL(uint8_t, uint8)
TPAIR_IMPLEMENT_ALL(uint16_t, uint16)
TPAIR_IMPLEMENT_ALL(uint32_t, uint32)
TPAIR_IMPLEMENT_ALL(uint64_t, uint64)
TPAIR_IMPLEMENT_ALL(int8_t, int8)
TPAIR_IMPLEMENT_ALL(int16_t, int16)
TPAIR_IMPLEMENT_ALL(int32_t, int32)
TPAIR_IMPLEMENT_ALL(int64_t, int64)
TPAIR_IMPLEMENT_ALL(float, float)
TPAIR_IMPLEMENT_ALL(double, double)
TPAIR_IMPLEMENT_ALL(long double, longdouble)
/* EOF */
