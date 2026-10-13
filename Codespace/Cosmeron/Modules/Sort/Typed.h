#pragma once
#include "Array.h"

/* Typed compare takes pointers; the generated wrappers delegate to the tested
 * generic engine and therefore retain its guarantees and status handling.
 * Use a distinct SUFFIX per TYPE. No state is stored globally. */
#define SORT_TYPED_COMPARE_PROTOTYPE(TYPE, SUFFIX)                             \
  static inline CMPOUT SORT_TYPED_INS(Compare, SUFFIX)(                       \
      const void *a, const void *b, void *context)
/* Generated family members may be intentionally unused by a translation unit.
 * Preserve strict -Werror builds without requiring users to call each member. */
#if defined(__clang__) || defined(__GNUC__)
#define SORT_TYPED_UNUSED __attribute__((unused))
#else
#define SORT_TYPED_UNUSED
#endif
#define SORT_TYPED_OPERATION_PROTOTYPE(TYPE, NAME, SUFFIX)                     \
  static inline SORT_TYPED_UNUSED OPSTATUS SORT_TYPED_FUNC(NAME, SUFFIX)(TYPE *data, size_t count)

#define SORT_IMPLEMENT_ALL(TYPE, SUFFIX, COMPARE)                              \
  SORT_TYPED_COMPARE_PROTOTYPE(TYPE, SUFFIX) {                                \
    (void)context; return COMPARE((const TYPE *)a, (const TYPE *)b);          \
  }                                                                           \
  SORT_TYPED_OPERATION_PROTOTYPE(TYPE, Array, SUFFIX) {                       \
    return SORT_FUNC(ArrayWithContext)(data, count, sizeof(TYPE),            \
        SORT_TYPED_INS(Compare, SUFFIX), NULL);                               \
  }                                                                           \
  SORT_TYPED_OPERATION_PROTOTYPE(TYPE, Insertion, SUFFIX) {                   \
    return SORT_FUNC(InsertionWithContext)(data, count, sizeof(TYPE),        \
        SORT_TYPED_INS(Compare, SUFFIX), NULL);                               \
  }                                                                           \
  SORT_TYPED_OPERATION_PROTOTYPE(TYPE, Heap, SUFFIX) {                        \
    return SORT_FUNC(HeapWithContext)(data, count, sizeof(TYPE),             \
        SORT_TYPED_INS(Compare, SUFFIX), NULL);                               \
  }                                                                           \
  SORT_TYPED_OPERATION_PROTOTYPE(TYPE, Stable, SUFFIX) {                      \
    return SORT_FUNC(StableWithContext)(data, count, sizeof(TYPE),           \
        SORT_TYPED_INS(Compare, SUFFIX), NULL);                               \
  }                                                                           \
  SORT_TYPED_OPERATION_PROTOTYPE(TYPE, Intro, SUFFIX) {                       \
    return SORT_TYPED_FUNC(Array, SUFFIX)(data, count);                       \
  }                                                                           \
  SORT_TYPED_OPERATION_PROTOTYPE(TYPE, Merge, SUFFIX) {                       \
    return SORT_TYPED_FUNC(Stable, SUFFIX)(data, count);                      \
  }
