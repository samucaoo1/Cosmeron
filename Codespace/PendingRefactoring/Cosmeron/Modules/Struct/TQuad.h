#pragma once

#include "Struct.space"

#define TQUAD_MOD TQuad
#define TQUAD_CMOD TQUAD

#define TQUAD_PKG(SUFFIX) STRUCT_PKG(TQUAD_MOD, SUFFIX)
#define TQUAD_TYPE(SUFFIX) STRUCT_TYPE(TQUAD_MOD, SUFFIX)
#define TQUAD_FUNC(SUFFIX, FUNC) STRUCT_FUNC(TQUAD_MOD, SUFFIX, FUNC)

#define TQUAD_STRUCT(SUFFIX, TYPE)                                             \
  typedef union STRUCT_TYPE(TQUAD_MOD, SUFFIX) {                               \
    struct {                                                                   \
      union {                                                                  \
        TYPE value1;                                                           \
        TYPE a;                                                                \
        TYPE x1;                                                               \
        TYPE x;                                                                \
        TYPE left;                                                             \
        TYPE state1;                                                           \
      };                                                                       \
      union {                                                                  \
        TYPE value2;                                                           \
        TYPE b;                                                                \
        TYPE x2;                                                               \
        TYPE y;                                                                \
        TYPE right;                                                            \
        TYPE state2;                                                           \
      };                                                                       \
      union {                                                                  \
        TYPE value3;                                                           \
        TYPE c;                                                                \
        TYPE x3;                                                               \
        TYPE z;                                                                \
        TYPE top;                                                              \
        TYPE y1;                                                               \
        TYPE state3;                                                           \
      };                                                                       \
      union {                                                                  \
        TYPE value4;                                                           \
        TYPE d;                                                                \
        TYPE x4;                                                               \
        TYPE w;                                                                \
        TYPE bottom;                                                           \
        TYPE y2;                                                               \
        TYPE state4;                                                           \
      };                                                                       \
    };                                                                         \
    union {                                                                    \
      TYPE value[4];                                                           \
      TYPE values[4];                                                          \
    };                                                                         \
  } TQUAD_TYPE(SUFFIX);

/* ==========================================================================
 * Universal Template Declaration: TQUAD
 * ========================================================================== */

#define TEMPLATE_TQUAD(SUFFIX, TYPE)                                           \
  TQUAD_STRUCT(SUFFIX, TYPE)                                                   \
  PACKAGE(TQUAD_PKG(SUFFIX),                                                   \
      FUNCTION(TQUAD_TYPE(SUFFIX), Create,                                     \
               (TYPE value1, TYPE value2, TYPE value3, TYPE value4)),          \
      OPERATION(Extract,                                                       \
                (TQUAD_TYPE(SUFFIX) quad, TYPE *outValue1, TYPE *outValue2,    \
                 TYPE *outValue3, TYPE *outValue4)),                           \
      PROCEDURE(Unpack,                                                        \
                (TQUAD_TYPE(SUFFIX) quad, TYPE *outValue1, TYPE *outValue2,    \
                 TYPE *outValue3, TYPE *outValue4))                            \
  )

#include "Impl/TQuad.impl"

/* ==========================================================================
 * Standard Type Instantiations
 * ========================================================================== */

#define TQUAD_INTERNAL_INSTANTIATE(ROW)                                        \
  TEMPLATE_IMPLEMENT(TQUAD, STRUCT_TYPE_GET(SUFFIX, ROW),                      \
                     STRUCT_TYPE_GET(TYPE, ROW));

STRUCT_TYPE_TABLE(TQUAD_INTERNAL_INSTANTIATE)
#undef TQUAD_INTERNAL_INSTANTIATE

/* EOF */
