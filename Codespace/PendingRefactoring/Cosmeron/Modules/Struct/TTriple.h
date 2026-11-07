#pragma once

#include "Struct.space"

#define TTRIPLE_MOD TTriple
#define TTRIPLE_CMOD TTRIPLE

#define TTRIPLE_PKG(SUFFIX) STRUCT_PKG(TTRIPLE_MOD, SUFFIX)
#define TTRIPLE_TYPE(SUFFIX) STRUCT_TYPE(TTRIPLE_MOD, SUFFIX)
#define TTRIPLE_FUNC(SUFFIX, FUNC) STRUCT_FUNC(TTRIPLE_MOD, SUFFIX, FUNC)

#define TTRIPLE_STRUCT(SUFFIX, TYPE)                                           \
  typedef union STRUCT_TYPE(TTRIPLE_MOD, SUFFIX) {                             \
    struct {                                                                   \
      union {                                                                  \
        TYPE value1;                                                           \
        TYPE a;                                                                \
        TYPE x1;                                                               \
        TYPE x;                                                                \
        TYPE length;                                                           \
        TYPE state1;                                                           \
      };                                                                       \
      union {                                                                  \
        TYPE value2;                                                           \
        TYPE b;                                                                \
        TYPE x2;                                                               \
        TYPE y;                                                                \
        TYPE width;                                                            \
        TYPE state2;                                                           \
      };                                                                       \
      union {                                                                  \
        TYPE value3;                                                           \
        TYPE c;                                                                \
        TYPE x3;                                                               \
        TYPE z;                                                                \
        TYPE height;                                                           \
        TYPE state3;                                                           \
      };                                                                       \
    };                                                                         \
    union {                                                                    \
      TYPE value[3];                                                           \
      TYPE values[3];                                                          \
    };                                                                         \
  } TTRIPLE_TYPE(SUFFIX);

/* ==========================================================================
 * Universal Template Declaration: TTRIPLE
 * ========================================================================== */

#define TEMPLATE_TTRIPLE(SUFFIX, TYPE)                                         \
  TTRIPLE_STRUCT(SUFFIX, TYPE)                                                 \
  PACKAGE(TTRIPLE_PKG(SUFFIX),                                                 \
      FUNCTION(TTRIPLE_TYPE(SUFFIX), Create,                                   \
               (TYPE value1, TYPE value2, TYPE value3)),                       \
      OPERATION(Extract,                                                       \
                (TTRIPLE_TYPE(SUFFIX) triple, TYPE *outValue1,                 \
                 TYPE *outValue2, TYPE *outValue3)),                           \
      PROCEDURE(Unpack,                                                        \
                (TTRIPLE_TYPE(SUFFIX) triple, TYPE *outValue1,                 \
                 TYPE *outValue2, TYPE *outValue3))                            \
  )

#include "Impl/TTriple.impl"

/* ==========================================================================
 * Standard Type Instantiations
 * ========================================================================== */

#define TTRIPLE_INTERNAL_INSTANTIATE(ROW)                                      \
  TEMPLATE_IMPLEMENT(TTRIPLE, STRUCT_TYPE_GET(SUFFIX, ROW),                    \
                     STRUCT_TYPE_GET(TYPE, ROW));

STRUCT_TYPE_TABLE(TTRIPLE_INTERNAL_INSTANTIATE)
#undef TTRIPLE_INTERNAL_INSTANTIATE

/* EOF */
