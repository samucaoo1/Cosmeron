#pragma once

#include "../../Preprocessor/Compiling.inc"
#include "../../Preprocessor/Operations.h"

#include "Category.impl"
#include "Boolean.impl"
#include "Signed.impl"
#include "Unsigned.impl"
#include "Character.impl"
#include "Float.impl"
#include "Architecture.impl"

#define PRIMITIVE_TYPE_COL_TYPE(TYPE, NAME, SHORT_NAME, T_NAME, CATEGORY, BITS)       TYPE
#define PRIMITIVE_TYPE_COL_NAME(TYPE, NAME, SHORT_NAME, T_NAME, CATEGORY, BITS)       NAME
#define PRIMITIVE_TYPE_COL_SHORT_NAME(TYPE, NAME, SHORT_NAME, T_NAME, CATEGORY, BITS) SHORT_NAME
#define PRIMITIVE_TYPE_COL_T_NAME(TYPE, NAME, SHORT_NAME, T_NAME, CATEGORY, BITS)     T_NAME
#define PRIMITIVE_TYPE_COL_CATEGORY(TYPE, NAME, SHORT_NAME, T_NAME, CATEGORY, BITS)   CATEGORY
#define PRIMITIVE_TYPE_COL_BITS(TYPE, NAME, SHORT_NAME, T_NAME, CATEGORY, BITS)       BITS

#define PRIMITIVE_TYPE_GET(FIELD, ROW) PP_OP_CAT2(PRIMITIVE_TYPE_COL_, FIELD) ROW

/* ========================================================================== */
/* Categorical Aggregation Tables                                             */
/* ========================================================================== */

#define PRIMITIVE_TYPE_TABLE_EXACT_INTEGER(X)                                  \
  PRIMITIVE_TYPE_TABLE_SIGNED(X)                                               \
  PRIMITIVE_TYPE_TABLE_UNSIGNED(X)

#define PRIMITIVE_TYPE_TABLE_INTEGER(X)                                        \
  PRIMITIVE_TYPE_TABLE_SIGNED(X)                                               \
  PRIMITIVE_TYPE_TABLE_UNSIGNED(X)                                             \
  PRIMITIVE_TYPE_TABLE_ARCHITECTURE(X)

#define PRIMITIVE_TYPE_TABLE_NUMERIC(X)                                        \
  PRIMITIVE_TYPE_TABLE_INTEGER(X)                                              \
  PRIMITIVE_TYPE_TABLE_FLOAT(X)

#define PRIMITIVE_TYPE_TABLE(X)                                                \
  PRIMITIVE_TYPE_TABLE_BOOLEAN(X)                                              \
  PRIMITIVE_TYPE_TABLE_SIGNED(X)                                               \
  PRIMITIVE_TYPE_TABLE_UNSIGNED(X)                                             \
  PRIMITIVE_TYPE_TABLE_CHARACTER(X)                                            \
  PRIMITIVE_TYPE_TABLE_FLOAT(X)                                                \
  PRIMITIVE_TYPE_TABLE_ARCHITECTURE(X)
/* EOF */

