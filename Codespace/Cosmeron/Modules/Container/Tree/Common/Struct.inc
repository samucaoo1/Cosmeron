#pragma once

/* =============================================================
 * Tree struct generation — shared by set and map.
 * No cmp pointer; comparison is baked in at compile time.
 * ============================================================= */

#define TTREE_STRUCT(T)                                                        \
  CONTAINER_API_FORWARD(TTREE_FN(T, FunctionTable))                                        \
  typedef struct T {                                                           \
    CONTAINER_API_FIELD(TTREE_FN(T, FunctionTable))                          \
    TTREE_NODE(T) * root;                                                      \
    size_t size;                                                               \
  } T;
