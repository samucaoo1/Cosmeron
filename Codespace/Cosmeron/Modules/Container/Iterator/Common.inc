#ifndef COSMERON_CONTAINER_ITERATOR_COMMON_INC
#define COSMERON_CONTAINER_ITERATOR_COMMON_INC

#if defined(CONTAINER_DISABLE_FUNCTION_TABLE) &&                               \
    !defined(OBJECT_DISABLE_FUNCTION_TABLE)
#define OBJECT_DISABLE_FUNCTION_TABLE 1
#endif

#if defined(ITERATOR_DISABLE_FUNCTION_TABLE) &&                                \
    !defined(OBJECT_DISABLE_FUNCTION_TABLE)
#define OBJECT_DISABLE_FUNCTION_TABLE 1
#endif

#include "../../../Core/Algorithm/Comparison.h"
#include "../../../Core/Module/Module.h"
#include "../../../Core/Outcome/Status.h"
#include "../../../Core/Preprocessor/Compiling.inc"
#include "../../../Core/Preprocessor/Operations.h"
#include "../../../Core/Primitive/Types.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* ==========================================================================
 * Module Template Forwarding Helper
 * Enables: TEMPLATE_INSTANTIATE(Iterator, Target, Params...)
 * ========================================================================== */

#ifndef TEMPLATE_Iterator
#define TEMPLATE_Iterator(TARGET, ...)                                         \
  NS(TEMPLATE_Iterator, TARGET)(__VA_ARGS__)
#endif

/* ==========================================================================
 * Iterator Category Table (Row-Tuple Pattern)
 * ========================================================================== */

#define ITERATOR_CATEGORY_COL_NAME(NAME, VALUE) NAME
#define ITERATOR_CATEGORY_COL_VALUE(NAME, VALUE) VALUE
#define ITERATOR_CATEGORY_GET(FIELD, ROW)                                      \
  PP_OP_CAT2(ITERATOR_CATEGORY_COL_, FIELD) ROW

#define ITERATOR_CATEGORY_TABLE(X)                                             \
  X((FORWARD, 0))                                                              \
  X((BIDIRECTIONAL, 1))                                                        \
  X((RANDOM_ACCESS, 2))                                                        \
  X((CONTIGUOUS, 3))

MODULE(DOMAIN(Container), Iterator,
       ENUM_EX(Category, ITERATOR_CATEGORY_TABLE, QUALIFIED));

#endif /* COSMERON_CONTAINER_ITERATOR_COMMON_INC */
