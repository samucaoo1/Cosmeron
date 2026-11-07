#pragma once

#include "Sort.space"

#define SORT_ESOTERIC_OPERATIONS                                               \
  OPERATION(Stalin, (void *data, size_t count, size_t elementSize,             \
                     TCALLBACK(Comparison) compare, size_t *outCount)),        \
  OPERATION(Bogo, (void *data, size_t count, size_t elementSize,               \
                   TCALLBACK(Comparison) compare, uint64_t seed,               \
                   size_t maxSteps)),                                          \
  OPERATION(Bozo, (void *data, size_t count, size_t elementSize,               \
                   TCALLBACK(Comparison) compare, uint64_t seed,               \
                   size_t maxSteps)),                                          \
  OPERATION(Stooge, (void *data, size_t count, size_t elementSize,             \
                     TCALLBACK(Comparison) compare)),                          \
  OPERATION(Slow, (void *data, size_t count, size_t elementSize,               \
                   TCALLBACK(Comparison) compare)),                            \
  OPERATION(Miracle, (const void *data, size_t count, size_t elementSize,      \
                      TCALLBACK(Comparison) compare, size_t maxChecks))

/* EOF */
