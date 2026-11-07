#pragma once

#include "../Module/Namespace.h"
#include "../Preprocessor/Compiling.inc"
#include "StatusTable/StatusTable.h"

/* ========================================================================== */
/* 1. Category and Status Constants Macros                                     */
/* ========================================================================== */

#ifndef STATUS_CATEGORY_CONST
#define STATUS_CATEGORY_CONST(NAME) MODULE_CONST(STATUS, CATEGORY, NAME)
#endif

#ifndef STATUS_CONST
#define STATUS_CONST(CODE) STATUS(CODE)
#endif

/* ========================================================================== */
/* 2. Category Enum (Decoupled Row-Tuple Pattern)                             */
/* ========================================================================== */

typedef enum MODULE_ENUM(Outcome, Status, Category) {
#define STATUS_CATEGORY_ENUM_ITEM(ROW) \
  STATUS_CATEGORY_CONST(STATUS_CATEGORY_GET(NAME, ROW)),
  STATUS_CATEGORY_TABLE(STATUS_CATEGORY_ENUM_ITEM)
#undef STATUS_CATEGORY_ENUM_ITEM
} MODULE_ENUM(Outcome, Status, Category);

/* ========================================================================== */
/* 3. Universal OPSTATUS Enum (Decoupled Row-Tuple Pattern)                   */
/* ========================================================================== */

typedef enum MODULE_ENUM(Outcome, Status, Status) {
#define STATUS_ENUM_ITEM(ROW) STATUS_CONST(STATUS_GET(NAME, ROW)),
  STATUS_TABLE(STATUS_ENUM_ITEM)
#undef STATUS_ENUM_ITEM
} OPSTATUS;

typedef OPSTATUS MODULE_ENUM(Outcome, Status, Status);

/* ========================================================================== */
/* 4. Declarative Package: Outcome_Status                                     */
/* ========================================================================== */

#include "../Module/Module.h"

MODULE(DOMAIN(Core), Outcome,
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
        FUNCTION(MODULE_ENUM(Outcome, Status, Category), CategoryOf, (OPSTATUS status)),
        PREDICATE_INLINE(IsCategory,
                         (OPSTATUS status, MODULE_ENUM(Outcome, Status, Category) category), {
                           return MODULE_FUNC(Outcome, Status, CategoryOf)(status) == category;
                         }),
        FUNCTION(const char *, CategoryName, (MODULE_ENUM(Outcome, Status, Category) category)),
        FUNCTION(const char *, Name, (OPSTATUS status)),
        FUNCTION(const char *, Message, (OPSTATUS status))
    )
);

/* ========================================================================== */
/* 5. Implementation Include                                                  */
/* ========================================================================== */

#include "Impl/Status.impl"
/* EOF */
