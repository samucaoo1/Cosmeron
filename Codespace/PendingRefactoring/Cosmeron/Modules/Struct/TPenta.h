#pragma once

#include "Struct.space"

#define TPENTA_MOD TPenta
#define TPENTA_CMOD TPENTA

#define TPENTA_PKG(SUFFIX) STRUCT_PKG(TPENTA_MOD, SUFFIX)
#define TPENTA_TYPE(SUFFIX) STRUCT_TYPE(TPENTA_MOD, SUFFIX)
#define TPENTA_FUNC(SUFFIX, FUNC) STRUCT_FUNC(TPENTA_MOD, SUFFIX, FUNC)

#define TPENTA_STRUCT(SUFFIX, TYPE)                                            \
  typedef union STRUCT_TYPE(TPENTA_MOD, SUFFIX) {                              \
    struct {                                                                   \
      union {                                                                  \
        TYPE value1;                                                           \
        TYPE a;                                                                \
        TYPE x1;                                                               \
        TYPE x;                                                                \
        TYPE c1;                                                               \
        TYPE y1;                                                               \
        TYPE state1;                                                           \
      };                                                                       \
      union {                                                                  \
        TYPE value2;                                                           \
        TYPE b;                                                                \
        TYPE x2;                                                               \
        TYPE y;                                                                \
        TYPE c2;                                                               \
        TYPE y2;                                                               \
        TYPE state2;                                                           \
      };                                                                       \
      union {                                                                  \
        TYPE value3;                                                           \
        TYPE c;                                                                \
        TYPE x3;                                                               \
        TYPE z;                                                                \
        TYPE c3;                                                               \
        TYPE y3;                                                               \
        TYPE state3;                                                           \
      };                                                                       \
      union {                                                                  \
        TYPE value4;                                                           \
        TYPE d;                                                                \
        TYPE x4;                                                               \
        TYPE w;                                                                \
        TYPE c4;                                                               \
        TYPE y4;                                                               \
        TYPE state4;                                                           \
      };                                                                       \
      union {                                                                  \
        TYPE value5;                                                           \
        TYPE e;                                                                \
        TYPE x5;                                                               \
        TYPE t;                                                                \
        TYPE c5;                                                               \
        TYPE y5;                                                               \
        TYPE state5;                                                           \
      };                                                                       \
    };                                                                         \
    union {                                                                    \
      TYPE value[5];                                                           \
      TYPE values[5];                                                          \
    };                                                                         \
  } TPENTA_TYPE(SUFFIX);

/* ==========================================================================
 * Universal Template Declaration: TPENTA
 * ========================================================================== */

#define TEMPLATE_TPENTA(SUFFIX, TYPE)                                          \
  TPENTA_STRUCT(SUFFIX, TYPE)                                                  \
  PACKAGE(TPENTA_PKG(SUFFIX),                                                  \
      FUNCTION(TPENTA_TYPE(SUFFIX), Create,                                    \
               (TYPE value1, TYPE value2, TYPE value3, TYPE value4,            \
                TYPE value5)),                                                 \
      OPERATION(Extract,                                                       \
                (TPENTA_TYPE(SUFFIX) penta, TYPE *outValue1, TYPE *outValue2,  \
                 TYPE *outValue3, TYPE *outValue4, TYPE *outValue5)),          \
      PROCEDURE(Unpack,                                                        \
                (TPENTA_TYPE(SUFFIX) penta, TYPE *outValue1, TYPE *outValue2,  \
                 TYPE *outValue3, TYPE *outValue4, TYPE *outValue5))           \
  )

#include "Impl/TPenta.impl"

/* ==========================================================================
 * Standard Type Instantiations
 * ========================================================================== */

#define TPENTA_INTERNAL_INSTANTIATE(ROW)                                       \
  TEMPLATE_IMPLEMENT(TPENTA, STRUCT_TYPE_GET(SUFFIX, ROW),                     \
                     STRUCT_TYPE_GET(TYPE, ROW));

STRUCT_TYPE_TABLE(TPENTA_INTERNAL_INSTANTIATE)
#undef TPENTA_INTERNAL_INSTANTIATE

/* EOF */
