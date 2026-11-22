#pragma once

#include "../Module/Module.h"
#include "../Module/Namespace.h"
#include "../Module/Callable/Callable.h"
#include "../Module/Delegate/Delegate.h"
#include "../Preprocessor/Compiling.inc"
#include "../Preprocessor/Operations.h"
#include "../Types/Primitive.h"

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

#define COSMERON_MACRO_INTERNAL_COMPARISON_RESULT_DISP(TARGET, ENUM_NAME, ROW)   \
  COMPARISON_CONST(COMPARISON_GET(NAME, ROW)) = COMPARISON_GET(VALUE, ROW),

ENUM_DEF_EX(MODULE_NS(Algorithm, Comparison), Result, COMPARISON_TABLE,
            COSMERON_MACRO_INTERNAL_COMPARISON_RESULT_DISP)
#undef COSMERON_MACRO_INTERNAL_COMPARISON_RESULT_DISP

/* Universal 3-way comparison output type alias */
typedef MODULE_ENUM(Algorithm, Comparison, Result) CMPOUT;

/* ========================================================================== */
/* 2. First-Class System Comparator Callable (Callable & Delegate)            */
/* ========================================================================== */

DELEGATE_COMPARISON(Comparison, (const void *left, const void *right));

/* ========================================================================== */
/* 3. Package Declaration: Algorithm_Comparison                               */
/* ========================================================================== */

#define COSMERON_MACRO_INTERNAL_COMPARISON_CASE_ITEM(ROW)                      \
  case COMPARISON_CONST(COMPARISON_GET(NAME, ROW)):                            \
    return COMPARISON_GET(DESC, ROW);

#define COSMERON_MACRO_INTERNAL_COMPARISON_PRIMITIVE_DECL(ROW)                 \
  FUNCTION(CMPOUT, PRIMITIVE_TYPE_GET(NAME, ROW),                              \
           (PRIMITIVE_TYPE_GET(C_TYPE, ROW) left,                                \
            PRIMITIVE_TYPE_GET(C_TYPE, ROW) right)),

#if defined(ALGORITHM_DISABLE_FUNCTION_TABLE) &&                               \
    !defined(COSMERON_DISABLE_FT_Algorithm)
#define COSMERON_DISABLE_FT_Algorithm 1
#endif
#if defined(ALGORITHM_COMPARISON_DISABLE_FUNCTION_TABLE) &&                    \
    !defined(COSMERON_DISABLE_FT_Algorithm_Comparison)
#define COSMERON_DISABLE_FT_Algorithm_Comparison 1
#endif

MODULE(DOMAIN(Core), Algorithm,
       PACKAGE(Comparison,
               FUNCTION_INLINE(const char *, ToString, (CMPOUT result), {
                 switch (result) {
                   COMPARISON_TABLE(COSMERON_MACRO_INTERNAL_COMPARISON_CASE_ITEM)
                   default:
                     return "Unknown";
                 }
               }),
               PRIMITIVE_TYPE_TABLE(COSMERON_MACRO_INTERNAL_COMPARISON_PRIMITIVE_DECL)
               FUNCTION(CMPOUT, CString,
                        (const char *left, const char *right)),
               FUNCTION(CMPOUT, Bytes,
                        (const void *left, size_t leftSize, const void *right,
                         size_t rightSize))));

#undef COSMERON_MACRO_INTERNAL_COMPARISON_CASE_ITEM
#undef COSMERON_MACRO_INTERNAL_COMPARISON_PRIMITIVE_DECL

/* ========================================================================== */
/* 4. Implementation Include                                                  */
/* ========================================================================== */

#include "Impl/Comparison.impl"
/* EOF */
