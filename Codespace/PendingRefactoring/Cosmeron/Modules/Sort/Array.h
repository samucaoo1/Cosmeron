#pragma once

#include "Sort.space"

#define SORT_ARRAY_OPERATIONS                                                  \
  OPERATION(Array, (void *data, size_t count, size_t elementSize,              \
                    TCALLBACK(Comparison) compare)),                           \
  OPERATION(Stable, (void *data, size_t count, size_t elementSize,             \
                     TCALLBACK(Comparison) compare)),                          \
  OPERATION(Insertion, (void *data, size_t count, size_t elementSize,          \
                        TCALLBACK(Comparison) compare)),                       \
  OPERATION(Heap, (void *data, size_t count, size_t elementSize,               \
                   TCALLBACK(Comparison) compare)),                            \
  OPERATION(Intro, (void *data, size_t count, size_t elementSize,              \
                    TCALLBACK(Comparison) compare)),                           \
  OPERATION(Merge, (void *data, size_t count, size_t elementSize,              \
                    TCALLBACK(Comparison) compare)),                           \
  OPERATION(StableWithBuffer, (void *data, size_t count, size_t elementSize,   \
                               TCALLBACK(Comparison) compare, void *buffer,     \
                               size_t bufferSize))

/* EOF */
