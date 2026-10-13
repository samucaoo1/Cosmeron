#pragma once
#include "Array.h"

/* Every classical algorithm has a plain and a context comparator interface.
 * Opaque records are moved bytewise; no global state and no allocations. */
#define SORT_CLASSICAL_PROTOTYPE(NAME)                                       \
  static inline OPSTATUS SORT_FUNC(NAME)(                                    \
      void *data, size_t count, size_t elementSize,                           \
      COMPARISON_TYPE(Comparator) compare)

#define SORT_CLASSICAL_CONTEXT_PROTOTYPE(NAME)                               \
  static inline OPSTATUS SORT_FUNC(PP_OP_CAT2(NAME, WithContext))(            \
      void *data, size_t count, size_t elementSize,                           \
      COMPARISON_TYPE(ContextComparator) compare, void *context)

SORT_CLASSICAL_PROTOTYPE(Bubble);
SORT_CLASSICAL_PROTOTYPE(Cocktail);
SORT_CLASSICAL_PROTOTYPE(Comb);
SORT_CLASSICAL_PROTOTYPE(Gnome);
SORT_CLASSICAL_PROTOTYPE(OddEven);
SORT_CLASSICAL_PROTOTYPE(Selection);
SORT_CLASSICAL_PROTOTYPE(DoubleSelection);
SORT_CLASSICAL_PROTOTYPE(Shell);
SORT_CLASSICAL_PROTOTYPE(Pancake);
SORT_CLASSICAL_PROTOTYPE(Quick);
SORT_CLASSICAL_PROTOTYPE(QuickThreeWay);

SORT_CLASSICAL_CONTEXT_PROTOTYPE(Bubble);
SORT_CLASSICAL_CONTEXT_PROTOTYPE(Cocktail);
SORT_CLASSICAL_CONTEXT_PROTOTYPE(Comb);
SORT_CLASSICAL_CONTEXT_PROTOTYPE(Gnome);
SORT_CLASSICAL_CONTEXT_PROTOTYPE(OddEven);
SORT_CLASSICAL_CONTEXT_PROTOTYPE(Selection);
SORT_CLASSICAL_CONTEXT_PROTOTYPE(DoubleSelection);
SORT_CLASSICAL_CONTEXT_PROTOTYPE(Shell);
SORT_CLASSICAL_CONTEXT_PROTOTYPE(Pancake);
SORT_CLASSICAL_CONTEXT_PROTOTYPE(Quick);
SORT_CLASSICAL_CONTEXT_PROTOTYPE(QuickThreeWay);

#include "Impl/Classical.impl"
