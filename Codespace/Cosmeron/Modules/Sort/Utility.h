#pragma once
#include "Array.h"

#define SORT_IS_SORTED_PROTOTYPE                                               \
  static inline OPSTATUS SORT_FUNC(IsSorted)(                                 \
      const void *data, size_t count, size_t elementSize,                    \
      COMPARISON_TYPE(Comparator) compare, bool *outSorted)
#define SORT_REVERSE_PROTOTYPE                                                 \
  static inline OPSTATUS SORT_FUNC(Reverse)(                                  \
      void *data, size_t count, size_t elementSize)

SORT_IS_SORTED_PROTOTYPE;
SORT_REVERSE_PROTOTYPE;
#include "Impl/Utility.impl"
