#pragma once

#include "Sort.space"

/* ==========================================================================
 * Typed Sort Template Engine
 * Generates typed wrappers over generic Sort_Array, Sort_Stable, etc.
 * ========================================================================== */

#define TEMPLATE_SORT(SUFFIX, TYPE, COMPARE)                                   \
  static inline CMPOUT SORT_TYPED_INS(CompareAdapter, SUFFIX)(                 \
      const void *left, const void *right, void *context) {                    \
    (void)context;                                                             \
    return COMPARE((const TYPE *)left, (const TYPE *)right);                   \
  }                                                                            \
  static inline OPSTATUS SORT_TYPED_FUNC(Array, SUFFIX)(                       \
      TYPE *data, size_t count) {                                              \
    TCALLBACK(Comparison) cb =                                                 \
        CALLBACK_FUNC(Comparison_Bind)(SORT_TYPED_INS(CompareAdapter, SUFFIX), \
                                       NULL);                                  \
    return SORT_FUNC(Array)(data, count, sizeof(TYPE), cb);                    \
  }                                                                            \
  static inline OPSTATUS SORT_TYPED_FUNC(Stable, SUFFIX)(                      \
      TYPE *data, size_t count) {                                              \
    TCALLBACK(Comparison) cb =                                                 \
        CALLBACK_FUNC(Comparison_Bind)(SORT_TYPED_INS(CompareAdapter, SUFFIX), \
                                       NULL);                                  \
    return SORT_FUNC(Stable)(data, count, sizeof(TYPE), cb);                   \
  }                                                                            \
  static inline OPSTATUS SORT_TYPED_FUNC(Insertion, SUFFIX)(                   \
      TYPE *data, size_t count) {                                              \
    TCALLBACK(Comparison) cb =                                                 \
        CALLBACK_FUNC(Comparison_Bind)(SORT_TYPED_INS(CompareAdapter, SUFFIX), \
                                       NULL);                                  \
    return SORT_FUNC(Insertion)(data, count, sizeof(TYPE), cb);                \
  }                                                                            \
  static inline OPSTATUS SORT_TYPED_FUNC(Heap, SUFFIX)(                        \
      TYPE *data, size_t count) {                                              \
    TCALLBACK(Comparison) cb =                                                 \
        CALLBACK_FUNC(Comparison_Bind)(SORT_TYPED_INS(CompareAdapter, SUFFIX), \
                                       NULL);                                  \
    return SORT_FUNC(Heap)(data, count, sizeof(TYPE), cb);                     \
  }                                                                            \
  static inline OPSTATUS SORT_TYPED_FUNC(Intro, SUFFIX)(                       \
      TYPE *data, size_t count) {                                              \
    return SORT_TYPED_FUNC(Array, SUFFIX)(data, count);                        \
  }                                                                            \
  static inline OPSTATUS SORT_TYPED_FUNC(Merge, SUFFIX)(                       \
      TYPE *data, size_t count) {                                              \
    return SORT_TYPED_FUNC(Stable, SUFFIX)(data, count);                       \
  }

#define SORT_IMPLEMENT_ALL(TYPE, SUFFIX, COMPARE)                              \
  TEMPLATE_SORT(SUFFIX, TYPE, COMPARE)

/* EOF */
