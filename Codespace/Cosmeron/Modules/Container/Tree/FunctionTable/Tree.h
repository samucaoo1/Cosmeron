#pragma once

#if CONTAINER_ENABLE_PSEUDO_METHODS

/* =============================================================
 * VTable instances — static const binding of functions to slots.
 *   TTREE_FUNCTION_TABLE_INSTANCE_SET(T)
 *   TTREE_FUNCTION_TABLE_INSTANCE_MAP(T)
 *
 * The struct typedefs (TTREE_VTABLE_SET / TTREE_VTABLE_MAP) live
 * in Tree.space, not here. This file only contains the `const`
 * instances that the user code binds to `t.api = &...`.
 * ============================================================= */

#define TTREE_FUNCTION_TABLE_INSTANCE_SET(T)                                           \
  static const TTREE_FN(T, FunctionTable) TTREE_FN(T, functions) = {         \
      .init = TTREE_FN(T, Init),                                               \
      .destroy = TTREE_FN(T, Destroy),                                         \
      .clear = TTREE_FN(T, Clear),                                             \
      .insert = TTREE_FN(T, Insert),                                           \
      .remove = TTREE_FN(T, Remove),                                           \
      .findNode = TTREE_FN(T, FindNode),                                      \
      .find = TTREE_FN(T, Find),                                           \
      .contains = TTREE_FN(T, Contains),                                       \
      .minNode = TTREE_FN(T, MinNode),                                        \
      .maxNode = TTREE_FN(T, MaxNode),                                        \
      .min = TTREE_FN(T, Min),                                                 \
      .max = TTREE_FN(T, Max),                                                 \
      .empty = TTREE_FN(T, Empty),                                             \
      .size = TTREE_FN(T, Size),                                               \
      .begin = TTREE_FN(T, Begin),                                             \
      .end = TTREE_FN(T, End),                                                 \
      .next = TTREE_FN(T, Next),                                               \
      .prev = TTREE_FN(T, Prev),                                               \
      .constBegin = TTREE_FN(T, ConstBegin),                                   \
      .constEnd = TTREE_FN(T, ConstEnd),                                       \
      .constNext = TTREE_FN(T, ConstNext),                                     \
      .constPrev = TTREE_FN(T, ConstPrev),                                     \
  };

#define TTREE_FUNCTION_TABLE_INSTANCE_MAP(T)                                           \
  static const TTREE_FN(T, FunctionTable) TTREE_FN(T, functions) = {         \
      .init = TTREE_FN(T, Init),                                               \
      .destroy = TTREE_FN(T, Destroy),                                         \
      .clear = TTREE_FN(T, Clear),                                             \
      .insert = TTREE_FN(T, Insert),                                           \
      .remove = TTREE_FN(T, Remove),                                           \
      .findNode = TTREE_FN(T, FindNode),                                      \
      .find = TTREE_FN(T, Find),                                         \
      .contains = TTREE_FN(T, Contains),                                       \
      .minNode = TTREE_FN(T, MinNode),                                        \
      .maxNode = TTREE_FN(T, MaxNode),                                        \
      .min = TTREE_FN(T, Min),                                                 \
      .max = TTREE_FN(T, Max),                                                 \
      .empty = TTREE_FN(T, Empty),                                             \
      .size = TTREE_FN(T, Size),                                               \
      .begin = TTREE_FN(T, Begin),                                             \
      .end = TTREE_FN(T, End),                                                 \
      .next = TTREE_FN(T, Next),                                               \
      .prev = TTREE_FN(T, Prev),                                               \
      .constBegin = TTREE_FN(T, ConstBegin),                                   \
      .constEnd = TTREE_FN(T, ConstEnd),                                       \
      .constNext = TTREE_FN(T, ConstNext),                                     \
      .constPrev = TTREE_FN(T, ConstPrev),                                     \
  };

#else
#define TTREE_FUNCTION_TABLE_INSTANCE_SET(...)
#define TTREE_FUNCTION_TABLE_INSTANCE_MAP(...)
#endif
