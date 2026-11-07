#pragma once

#include "../Module/Namespace.h"
#include "../Preprocessor/Compiling.inc"
#include "../Preprocessor/Operations.h"

#include <stdbool.h>
#include <stddef.h>

/* ========================================================================== */
/* 1. Comparison Result Table (Decoupled Row-Tuple Pattern)                   */
/* ========================================================================== */

#define COMPARISON_COL_NAME(NAME, VALUE, DESC) NAME
#define COMPARISON_COL_VALUE(NAME, VALUE, DESC) VALUE
#define COMPARISON_COL_DESC(NAME, VALUE, DESC) DESC
#define COMPARISON_GET(FIELD, ROW) PP_OP_CAT2(COMPARISON_COL_, FIELD) ROW

#define COMPARISON_TABLE(X)                                                    \
  X((LOWER, -1, "Lower"))                                                      \
  X((EQUAL, 0, "Equal"))                                                       \
  X((HIGHER, 1, "Higher"))

#ifndef COMPARISON_CONST
#define COMPARISON_CONST(NAME) MODULE_CONST(ALGORITHM, COMPARISON, NAME)
#endif

typedef enum MODULE_ENUM(Algorithm, Comparison, Result) {
#define COMPARISON_ENUM_ITEM(ROW)                                              \
  COMPARISON_CONST(COMPARISON_GET(NAME, ROW)) = COMPARISON_GET(VALUE, ROW),
  COMPARISON_TABLE(COMPARISON_ENUM_ITEM)
#undef COMPARISON_ENUM_ITEM
} MODULE_ENUM(Algorithm, Comparison, Result);

/* Universal 3-way comparison output type alias */
typedef MODULE_ENUM(Algorithm, Comparison, Result) CMPOUT;

/* String representation helper */
static inline const char *MODULE_FUNC(Algorithm, Comparison,
                                      ToString)(CMPOUT result) {
  switch (result) {
#define COMPARISON_CASE_ITEM(ROW)                                              \
  case COMPARISON_CONST(COMPARISON_GET(NAME, ROW)):                            \
    return COMPARISON_GET(DESC, ROW);
    COMPARISON_TABLE(COMPARISON_CASE_ITEM)
#undef COMPARISON_CASE_ITEM
  default:
    return "Unknown";
  }
}

/* ========================================================================== */
/* 2. First-Class System Comparator Callback                                  */
/* ========================================================================== */

#include "../Module/Callback/Callback.h"
CALLBACK_COMPARISON(Comparison, (const void *left, const void *right));

/* ========================================================================== */
/* 3. Module Framework & Primitive Types                                      */
/* ========================================================================== */

#include "../Module/Module.h"
#include "../Primitive/Types.h"

/* ========================================================================== */
/* 4. Package Declaration: Algorithm_Comparison                               */
/* ========================================================================== */

#define COMPARISON_PRIMITIVE_DECL(ROW)                                         \
  FUNCTION(CMPOUT, PRIMITIVE_TYPE_GET(NAME, ROW),                              \
           (PRIMITIVE_TYPE_GET(TYPE, ROW) left,                                \
            PRIMITIVE_TYPE_GET(TYPE, ROW) right)),

MODULE(DOMAIN(Core), Algorithm,
       PACKAGE(Comparison,
               PRIMITIVE_TYPE_TABLE(COMPARISON_PRIMITIVE_DECL)
                   FUNCTION(CMPOUT, CString,
                            (const char *left, const char *right)),
               FUNCTION(CMPOUT, Bytes,
                        (const void *left, size_t leftSize, const void *right,
                         size_t rightSize))));

#undef COMPARISON_PRIMITIVE_DECL

/* ========================================================================== */
/* 5. Implementation Include                                                  */
/* ========================================================================== */

#include "Impl/Comparison.impl"
/* EOF */
