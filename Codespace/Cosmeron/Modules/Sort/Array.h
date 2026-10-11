#pragma once

#include "Sort.space"
#include "../../Core/Algorithm/Comparison.h"
#include "../../Core/Memory/Alloc.h"

#define SORT_ARRAY_PROTOTYPE                                                  \
  static inline OPSTATUS SORT_FUNC(Array)(                                    \
      void *data, size_t count, size_t elementSize,                           \
      COMPARISON_TYPE(Comparator) compare)
#define SORT_STABLE_PROTOTYPE                                                 \
  static inline OPSTATUS SORT_FUNC(Stable)(                                   \
      void *data, size_t count, size_t elementSize,                           \
      COMPARISON_TYPE(Comparator) compare)
#define SORT_INSERTION_PROTOTYPE                                              \
  static inline OPSTATUS SORT_FUNC(Insertion)(                                \
      void *data, size_t count, size_t elementSize,                           \
      COMPARISON_TYPE(Comparator) compare)
#define SORT_HEAP_PROTOTYPE                                                   \
  static inline OPSTATUS SORT_FUNC(Heap)(                                     \
      void *data, size_t count, size_t elementSize,                           \
      COMPARISON_TYPE(Comparator) compare)
#define SORT_INTRO_PROTOTYPE                                                  \
  static inline OPSTATUS SORT_FUNC(Intro)(                                    \
      void *data, size_t count, size_t elementSize,                           \
      COMPARISON_TYPE(Comparator) compare)
#define SORT_MERGE_PROTOTYPE                                                  \
  static inline OPSTATUS SORT_FUNC(Merge)(                                    \
      void *data, size_t count, size_t elementSize,                           \
      COMPARISON_TYPE(Comparator) compare)
#define SORT_STABLE_BUFFER_PROTOTYPE                                          \
  static inline OPSTATUS SORT_FUNC(StableWithBuffer)(                         \
      void *data, size_t count, size_t elementSize,                           \
      COMPARISON_TYPE(Comparator) compare, void *buffer, size_t bufferSize)

#define SORT_CONTEXT_PROTOTYPE(NAME)                                          \
  static inline OPSTATUS SORT_FUNC(NAME##WithContext)(                        \
      void *data, size_t count, size_t elementSize,                           \
      COMPARISON_TYPE(ContextComparator) compare, void *context)
#define SORT_STABLE_BUFFER_CONTEXT_PROTOTYPE                                  \
  static inline OPSTATUS SORT_FUNC(StableWithBufferWithContext)(              \
      void *data, size_t count, size_t elementSize,                           \
      COMPARISON_TYPE(ContextComparator) compare, void *context,              \
      void *buffer, size_t bufferSize)

SORT_ARRAY_PROTOTYPE;
SORT_STABLE_PROTOTYPE;
SORT_INSERTION_PROTOTYPE;
SORT_HEAP_PROTOTYPE;
SORT_INTRO_PROTOTYPE;
SORT_MERGE_PROTOTYPE;
SORT_STABLE_BUFFER_PROTOTYPE;
SORT_CONTEXT_PROTOTYPE(Array);
SORT_CONTEXT_PROTOTYPE(Stable);
SORT_CONTEXT_PROTOTYPE(Insertion);
SORT_CONTEXT_PROTOTYPE(Heap);
SORT_CONTEXT_PROTOTYPE(Intro);
SORT_CONTEXT_PROTOTYPE(Merge);
SORT_STABLE_BUFFER_CONTEXT_PROTOTYPE;

#include "Impl/Array.impl"
