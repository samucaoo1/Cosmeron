#pragma once

#include "../Module/Module.h"
#include "../Preprocessor/Compiling.inc"
#include "../Preprocessor/Operations.h"
#include "TypeTable/TypeTable.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* ========================================================================== */
/* 1. Declarative Module: Type (Category Enum & Primitive Typedefs)           */
/* ========================================================================== */

#define COSMERON_MACRO_INTERNAL_TYPE_CATEGORY_CASE(ROW)                        \
  case MODULE_CONST(Type, Category, TYPE_CATEGORY_GET(NAME, ROW)):             \
    return TYPE_CATEGORY_GET(DESC, ROW);

#if defined(TYPE_DISABLE_FUNCTION_TABLE) && !defined(COSMERON_DISABLE_FT_Type)
#define COSMERON_DISABLE_FT_Type 1
#endif
#if defined(TYPE_CATEGORY_DISABLE_FUNCTION_TABLE) &&                           \
    !defined(COSMERON_DISABLE_FT_Type_Category)
#define COSMERON_DISABLE_FT_Type_Category 1
#endif

MODULE(
    DOMAIN(Core), Type, ENUM_EX(Category, TYPE_CATEGORY_TABLE, QUALIFIED_NAME),
    PACKAGE(Category,
            FUNCTION_INLINE(const char *, ToString,
                            (MODULE_ENUM(Type, Category) category), {
                              switch (category) {
                                TYPE_CATEGORY_TABLE(
                                    COSMERON_MACRO_INTERNAL_TYPE_CATEGORY_CASE)
                              default:
                                return "Unknown";
                              }
                            })));

#undef COSMERON_MACRO_INTERNAL_TYPE_CATEGORY_CASE

/* ========================================================================== */
/* 2. Canonical Namespaced Primitive Types                                    */
/* ========================================================================== */

#define COSMERON_MACRO_INTERNAL_TYPE_PRIMITIVE_TYPEDEF(ROW)                    \
  typedef PRIMITIVE_TYPE_GET(C_TYPE, ROW)                                        \
      MODULE_TYPE(Type, PRIMITIVE_TYPE_GET(NAME, ROW));

PRIMITIVE_TYPE_TABLE(COSMERON_MACRO_INTERNAL_TYPE_PRIMITIVE_TYPEDEF)
#undef COSMERON_MACRO_INTERNAL_TYPE_PRIMITIVE_TYPEDEF

/* EOF */
