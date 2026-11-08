#pragma once

#include "../Module/Module.h"
#include "../Preprocessor/Compiling.inc"
#include "../Preprocessor/Operations.h"
#include "TypeTable/TypeTable.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* ========================================================================== */
/* 1. Declarative Module: Primitive (Category Enum & Conversion Package)      */
/* ========================================================================== */

#define PRIMITIVE_CATEGORY_CASE(ROW)                                           \
  case MODULE_CONST(Primitive, PRIMITIVE_CATEGORY_GET(NAME, ROW)):             \
    return PRIMITIVE_CATEGORY_GET(DESC, ROW);

MODULE(DOMAIN(Core), Primitive,
    ENUM_EX(Category, PRIMITIVE_CATEGORY_TABLE, NAME_ONLY),
    PACKAGE(Category,
        FUNCTION_INLINE(const char *, ToString,
                        (MODULE_ENUM(Primitive, Category) category), {
          switch (category) {
            PRIMITIVE_CATEGORY_TABLE(PRIMITIVE_CATEGORY_CASE)
            default:
              return "Unknown";
          }
        })
    )
);

#undef PRIMITIVE_CATEGORY_CASE

/* ========================================================================== */
/* 2. Canonical Namespaced Primitive Types                                    */
/* ========================================================================== */

#define PRIMITIVE_TYPE_TYPEDEF(ROW)                                            \
  typedef PRIMITIVE_TYPE_GET(TYPE, ROW)                                        \
      MODULE_TYPE(Primitive, PRIMITIVE_TYPE_GET(NAME, ROW));

PRIMITIVE_TYPE_TABLE(PRIMITIVE_TYPE_TYPEDEF)
#undef PRIMITIVE_TYPE_TYPEDEF

/* ========================================================================== */
/* 3. Category Iteration Views                                                */
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
