#pragma once

#include "Sort.space"

#define SORT_UTILITY_OPERATIONS                                                \
  OPERATION(IsSorted, (const void *data, size_t count, size_t elementSize,     \
                       TCALLBACK(Comparison) compare, bool *outSorted)),       \
  OPERATION(Reverse, (void *data, size_t count, size_t elementSize))

/* EOF */
