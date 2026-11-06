#pragma once

#include "Struct.space"

#define TDUAL_MOD TDual
#define TDUAL_CMOD TDUAL

#define TDUAL_PKG(SUFFIX) STRUCT_PKG(TDUAL_MOD, SUFFIX)
#define TDUAL_TYPE(SUFFIX) STRUCT_TYPE(TDUAL_MOD, SUFFIX)
#define TDUAL_FUNC(SUFFIX, FUNC) STRUCT_FUNC(TDUAL_MOD, SUFFIX, FUNC)

#define TDUAL_STRUCT(SUFFIX, TYPE)                                             \
  typedef union STRUCT_TYPE(TDUAL_MOD, SUFFIX) {                               \
    struct {                                                                   \
      union {                                                                  \
        TYPE value1;                                                           \
        TYPE a;                                                                \
        TYPE x1;                                                               \
        TYPE x;                                                                \
        TYPE col;                                                              \
        TYPE length;                                                           \
        TYPE real;                                                             \
        TYPE state1;                                                           \
        TYPE begin;                                                            \
        TYPE first;                                                            \
        TYPE start;                                                            \
      };                                                                       \
      union {                                                                  \
        TYPE value2;                                                           \
        TYPE b;                                                                \
        TYPE x2;                                                               \
        TYPE y;                                                                \
        TYPE row;                                                              \
        TYPE width;                                                            \
        TYPE imaginary;                                                        \
        TYPE state2;                                                           \
        TYPE end;                                                              \
        TYPE second;                                                           \
        TYPE last;                                                             \
      };                                                                       \
    };                                                                         \
    union {                                                                    \
      TYPE value[2];                                                           \
      TYPE values[2];                                                          \
    };                                                                         \
  } TDUAL_TYPE(SUFFIX);

/* ==========================================================================
 * Universal Template Declaration: TDUAL
 * ========================================================================== */

#define TEMPLATE_TDUAL(SUFFIX, TYPE)                                           \
  TDUAL_STRUCT(SUFFIX, TYPE)                                                   \
  PACKAGE(TDUAL_PKG(SUFFIX),                                                   \
      FUNCTION(TDUAL_TYPE(SUFFIX), Create, (TYPE value1, TYPE value2)),        \
      OPERATION(Extract,                                                       \
                (TDUAL_TYPE(SUFFIX) dual, TYPE *outValue1, TYPE *outValue2)),   \
      PROCEDURE(Unpack,                                                        \
                (TDUAL_TYPE(SUFFIX) dual, TYPE *outValue1, TYPE *outValue2))    \
  )

#include "Impl/TDual.impl"

/* ==========================================================================
 * Standard Type Instantiations
 * ========================================================================== */

#define TDUAL_INTERNAL_INSTANTIATE(ROW)                                        \
  TEMPLATE_IMPLEMENT(TDUAL, STRUCT_TYPE_GET(SUFFIX, ROW),                      \
                     STRUCT_TYPE_GET(TYPE, ROW));

STRUCT_TYPE_TABLE(TDUAL_INTERNAL_INSTANTIATE)
#undef TDUAL_INTERNAL_INSTANTIATE

/* EOF */
