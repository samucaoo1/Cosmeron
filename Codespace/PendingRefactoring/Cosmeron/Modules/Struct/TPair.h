#pragma once

#include "Struct.space"

#define TPAIR_MOD TPair
#define TPAIR_CMOD TPAIR

#define TPAIR_PKG(SUFFIX) STRUCT_PKG(TPAIR_MOD, SUFFIX)
#define TPAIR_TYPE(SUFFIX) STRUCT_TYPE(TPAIR_MOD, SUFFIX)
#define TPAIR_FUNC(SUFFIX, FUNC) STRUCT_FUNC(TPAIR_MOD, SUFFIX, FUNC)

#define TPAIR_STRUCT(SUFFIX, TYPE)                                             \
  typedef union STRUCT_TYPE(TPAIR_MOD, SUFFIX) {                               \
    struct {                                                                   \
      union {                                                                  \
        TYPE value1;                                                           \
        TYPE first;                                                            \
        TYPE a;                                                                \
        TYPE x1;                                                               \
        TYPE x;                                                                \
      };                                                                       \
      union {                                                                  \
        TYPE value2;                                                           \
        TYPE second;                                                           \
        TYPE b;                                                                \
        TYPE x2;                                                               \
        TYPE y;                                                                \
      };                                                                       \
    };                                                                         \
    union {                                                                    \
      TYPE value[2];                                                           \
      TYPE values[2];                                                          \
    };                                                                         \
  } TPAIR_TYPE(SUFFIX);

/* ==========================================================================
 * Universal Template Declaration: TPAIR
 * ========================================================================== */

#define TEMPLATE_TPAIR(SUFFIX, TYPE)                                           \
  TPAIR_STRUCT(SUFFIX, TYPE)                                                   \
  PACKAGE(TPAIR_PKG(SUFFIX),                                                   \
      FUNCTION(TPAIR_TYPE(SUFFIX), Create, (TYPE value1, TYPE value2)),        \
      OPERATION(Extract,                                                       \
                (TPAIR_TYPE(SUFFIX) pair, TYPE *outValue1, TYPE *outValue2)),   \
      PROCEDURE(Unpack,                                                        \
                (TPAIR_TYPE(SUFFIX) pair, TYPE *outValue1, TYPE *outValue2))    \
  )

#include "Impl/TPair.impl"

/* ==========================================================================
 * Standard Type Instantiations
 * ========================================================================== */

#define TPAIR_INTERNAL_INSTANTIATE(ROW)                                        \
  TEMPLATE_IMPLEMENT(TPAIR, STRUCT_TYPE_GET(SUFFIX, ROW),                      \
                     STRUCT_TYPE_GET(TYPE, ROW));

STRUCT_TYPE_TABLE(TPAIR_INTERNAL_INSTANTIATE)
#undef TPAIR_INTERNAL_INSTANTIATE

/* EOF */
