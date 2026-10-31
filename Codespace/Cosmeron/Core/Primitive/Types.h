#pragma once

#include "../Preprocessor/Compiling.inc"
#include "../Preprocessor/Operations.h"
#include "Primitive.space"
#include "TypeTable/TypeTable.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* ========================================================================== */
/* Category Enumeration & Conversion                                          */
/* ========================================================================== */

typedef enum PRIMITIVE_TYPE(Category) {
#define PRIMITIVE_CATEGORY_ENUM_ITEM(ROW)                                     \
  PRIMITIVE_CONST(PRIMITIVE_CATEGORY_GET(NAME, ROW)),
  PRIMITIVE_CATEGORY_TABLE(PRIMITIVE_CATEGORY_ENUM_ITEM)
#undef PRIMITIVE_CATEGORY_ENUM_ITEM
} PRIMITIVE_TYPE(Category);

static inline const char *PRIMITIVE_FUNC(CategoryToString)(PRIMITIVE_TYPE(Category) category) {
  switch (category) {
#define PRIMITIVE_CATEGORY_CASE_ITEM(ROW)                                     \
  case PRIMITIVE_CONST(PRIMITIVE_CATEGORY_GET(NAME, ROW)):                     \
    return PRIMITIVE_CATEGORY_GET(DESC, ROW);
    PRIMITIVE_CATEGORY_TABLE(PRIMITIVE_CATEGORY_CASE_ITEM)
#undef PRIMITIVE_CATEGORY_CASE_ITEM
    default:
      return "Unknown";
  }
}

/* ========================================================================== */
/* Canonical Namespaced Primitive Types                                       */
/* ========================================================================== */

#define PRIMITIVE_TYPE_TYPEDEF(ROW)                                            \
  typedef PRIMITIVE_TYPE_GET(TYPE, ROW)                                        \
      PRIMITIVE_TYPE(PRIMITIVE_TYPE_GET(T_NAME, ROW));

PRIMITIVE_TYPE_TABLE(PRIMITIVE_TYPE_TYPEDEF)
#undef PRIMITIVE_TYPE_TYPEDEF

/* ========================================================================== */
/* Category Iteration Views                                                   */
/* ========================================================================== */

#define PRIMITIVE_FOR_EACH(X)               PRIMITIVE_TYPE_TABLE(X)
#define PRIMITIVE_FOR_EACH_BOOLEAN(X)       PRIMITIVE_TYPE_TABLE_BOOLEAN(X)
#define PRIMITIVE_FOR_EACH_SIGNED(X)        PRIMITIVE_TYPE_TABLE_SIGNED(X)
#define PRIMITIVE_FOR_EACH_UNSIGNED(X)      PRIMITIVE_TYPE_TABLE_UNSIGNED(X)
#define PRIMITIVE_FOR_EACH_EXACT_INTEGER(X) PRIMITIVE_TYPE_TABLE_EXACT_INTEGER(X)
#define PRIMITIVE_FOR_EACH_INTEGER(X)       PRIMITIVE_TYPE_TABLE_INTEGER(X)
#define PRIMITIVE_FOR_EACH_CHARACTER(X)     PRIMITIVE_TYPE_TABLE_CHARACTER(X)
#define PRIMITIVE_FOR_EACH_FLOAT(X)         PRIMITIVE_TYPE_TABLE_FLOAT(X)
#define PRIMITIVE_FOR_EACH_ARCHITECTURE(X)  PRIMITIVE_TYPE_TABLE_ARCHITECTURE(X)
#define PRIMITIVE_FOR_EACH_NUMERIC(X)       PRIMITIVE_TYPE_TABLE_NUMERIC(X)
/* EOF */
