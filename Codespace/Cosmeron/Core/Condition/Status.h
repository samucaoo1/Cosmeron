#pragma once

#include "../Module/Module.h"
#include "../Module/Namespace.h"
#include "../Preprocessor/Compiling.inc"
#include "StatusTable/StatusTable.h"

/* ========================================================================== */
/* 1. Category and Status Constants Macros                                     */
/* ========================================================================== */

#ifndef STATUS_CATEGORY_CONST
#define STATUS_CATEGORY_CONST(NAME) MODULE_CONST(OPSTATUS, CATEGORY, NAME)
#endif

#ifndef STATUS_CONST
#define STATUS_CONST(CODE) STATUS(CODE)
#endif

/* ========================================================================== */
/* 2. Category Enum (Declarative ENUM_DEF_EX)                                 */
/* ========================================================================== */

#define COSMERON_MACRO_INTERNAL_STATUS_CAT_DISP(TARGET, ENUM_NAME, ROW)        \
  STATUS_CATEGORY_CONST(STATUS_CATEGORY_GET(NAME, ROW)),

ENUM_DEF_EX(MODULE_NS(Condition, Status), Category, STATUS_CATEGORY_TABLE,
            COSMERON_MACRO_INTERNAL_STATUS_CAT_DISP)
#undef COSMERON_MACRO_INTERNAL_STATUS_CAT_DISP

/* ========================================================================== */
/* 3. Universal OPSTATUS Enum (Declarative ENUM_DEF_EX)                       */
/* ========================================================================== */

#define COSMERON_MACRO_INTERNAL_STATUS_DISP(TARGET, ENUM_NAME, ROW)            \
  STATUS_CONST(STATUS_GET(NAME, ROW)),

ENUM_DEF_EX(Condition, Status, STATUS_TABLE,
            COSMERON_MACRO_INTERNAL_STATUS_DISP)
#undef COSMERON_MACRO_INTERNAL_STATUS_DISP

typedef MODULE_ENUM(Condition, Status) OPSTATUS;

/* ========================================================================== */
/* 4. Declarative Package: Condition_Status                                   */
/* ========================================================================== */

#if defined(CONDITION_DISABLE_FUNCTION_TABLE) &&                               \
    !defined(COSMERON_DISABLE_FT_Condition)
#define COSMERON_DISABLE_FT_Condition 1
#endif
#if defined(CONDITION_STATUS_DISABLE_FUNCTION_TABLE) &&                        \
    !defined(COSMERON_DISABLE_FT_Condition_Status)
#define COSMERON_DISABLE_FT_Condition_Status 1
#endif

MODULE(DOMAIN(Core), Condition,
    PACKAGE(Status,
        PREDICATE_INLINE(IsSuccess, (OPSTATUS status), {
          return status == STATUS_CONST(SUCCESS);
        }),
        PREDICATE_INLINE(IsFailure, (OPSTATUS status), {
          return status != STATUS_CONST(SUCCESS);
        }),
        PREDICATE_INLINE(IsSpecial, (OPSTATUS status), {
          return status == STATUS_CONST(WOULD_BLOCK) ||
                 status == STATUS_CONST(INCOMPLETE_SEQUENCE);
        }),
        FUNCTION(MODULE_ENUM(Condition, Status, Category), CategoryOf, (OPSTATUS status)),
        PREDICATE_INLINE(IsCategory,
                         (OPSTATUS status, MODULE_ENUM(Condition, Status, Category) category), {
                           return MODULE_FUNC(Condition, Status, CategoryOf)(status) == category;
                         }),
        FUNCTION(const char *, CategoryName, (MODULE_ENUM(Condition, Status, Category) category)),
        FUNCTION(const char *, Name, (OPSTATUS status)),
        FUNCTION(const char *, Message, (OPSTATUS status))
    )
);

/* ========================================================================== */
/* 5. Implementation Include                                                  */
/* ========================================================================== */

#include "Impl/Status.impl"
/* EOF */
