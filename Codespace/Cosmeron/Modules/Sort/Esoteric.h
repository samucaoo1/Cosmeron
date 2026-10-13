#pragma once
#include "Array.h"

/* Deliberately impractical algorithms: all potentially unbounded searches
 * take an explicit work budget. TIMEOUT can leave Bogo/Bozo permuted. */
#define SORT_STALIN_PROTOTYPE                                                 \
  static inline OPSTATUS SORT_FUNC(Stalin)(                                   \
      void *data, size_t count, size_t elementSize,                           \
      COMPARISON_TYPE(Comparator) compare, size_t *outCount)
#define SORT_STALIN_CONTEXT_PROTOTYPE                                         \
  static inline OPSTATUS SORT_FUNC(StalinWithContext)(                        \
      void *data, size_t count, size_t elementSize,                           \
      COMPARISON_TYPE(ContextComparator) compare, void *context,             \
      size_t *outCount)

#define SORT_RANDOM_PROTOTYPE(NAME)                                           \
  static inline OPSTATUS SORT_FUNC(NAME)(                                     \
      void *data, size_t count, size_t elementSize,                           \
      COMPARISON_TYPE(Comparator) compare, uint64_t seed, size_t maxSteps)
#define SORT_RANDOM_CONTEXT_PROTOTYPE(NAME)                                   \
  static inline OPSTATUS SORT_FUNC(PP_OP_CAT2(NAME, WithContext))(            \
      void *data, size_t count, size_t elementSize,                           \
      COMPARISON_TYPE(ContextComparator) compare, void *context,             \
      uint64_t seed, size_t maxSteps)

#define SORT_ESOTERIC_PROTOTYPE(NAME)                                         \
  static inline OPSTATUS SORT_FUNC(NAME)(                                     \
      void *data, size_t count, size_t elementSize,                           \
      COMPARISON_TYPE(Comparator) compare)
#define SORT_ESOTERIC_CONTEXT_PROTOTYPE(NAME)                                 \
  static inline OPSTATUS SORT_FUNC(PP_OP_CAT2(NAME, WithContext))(            \
      void *data, size_t count, size_t elementSize,                           \
      COMPARISON_TYPE(ContextComparator) compare, void *context)
#define SORT_MIRACLE_PROTOTYPE                                                \
  static inline OPSTATUS SORT_FUNC(Miracle)(                                  \
      const void *data, size_t count, size_t elementSize,                     \
      COMPARISON_TYPE(Comparator) compare, size_t maxChecks)

SORT_STALIN_PROTOTYPE;
SORT_STALIN_CONTEXT_PROTOTYPE;
SORT_RANDOM_PROTOTYPE(Bogo);
SORT_RANDOM_PROTOTYPE(Bozo);
SORT_RANDOM_CONTEXT_PROTOTYPE(Bogo);
SORT_RANDOM_CONTEXT_PROTOTYPE(Bozo);
SORT_ESOTERIC_PROTOTYPE(Stooge);
SORT_ESOTERIC_PROTOTYPE(Slow);
SORT_ESOTERIC_CONTEXT_PROTOTYPE(Stooge);
SORT_ESOTERIC_CONTEXT_PROTOTYPE(Slow);
SORT_MIRACLE_PROTOTYPE;

#include "Impl/Esoteric.impl"
