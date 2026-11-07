#pragma once

#include "Sort.space"

#define SORT_CLASSICAL_OPERATIONS                                              \
  OPERATION(Bubble, (void *data, size_t count, size_t elementSize,             \
                     TCALLBACK(Comparison) compare)),                          \
  OPERATION(Cocktail, (void *data, size_t count, size_t elementSize,           \
                       TCALLBACK(Comparison) compare)),                        \
  OPERATION(Comb, (void *data, size_t count, size_t elementSize,               \
                   TCALLBACK(Comparison) compare)),                            \
  OPERATION(Gnome, (void *data, size_t count, size_t elementSize,              \
                    TCALLBACK(Comparison) compare)),                           \
  OPERATION(OddEven, (void *data, size_t count, size_t elementSize,            \
                      TCALLBACK(Comparison) compare)),                         \
  OPERATION(Selection, (void *data, size_t count, size_t elementSize,          \
                        TCALLBACK(Comparison) compare)),                       \
  OPERATION(DoubleSelection, (void *data, size_t count, size_t elementSize,    \
                              TCALLBACK(Comparison) compare)),                 \
  OPERATION(Shell, (void *data, size_t count, size_t elementSize,              \
                    TCALLBACK(Comparison) compare)),                           \
  OPERATION(Pancake, (void *data, size_t count, size_t elementSize,            \
                      TCALLBACK(Comparison) compare)),                         \
  OPERATION(Quick, (void *data, size_t count, size_t elementSize,              \
                    TCALLBACK(Comparison) compare)),                           \
  OPERATION(QuickThreeWay, (void *data, size_t count, size_t elementSize,      \
                            TCALLBACK(Comparison) compare))

/* EOF */
