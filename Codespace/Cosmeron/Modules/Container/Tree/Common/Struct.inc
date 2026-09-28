#pragma once

/* =============================================================
 * Tree struct generation — shared by set and map.
 * No cmp pointer; comparison is baked in at compile time.
 * ============================================================= */

#define TTREE_STRUCT(T)                                                        \
  CONTAINER_API_FORWARD(TTREE_FUNCTION_TABLE_TYPE(T))                                        \
  typedef struct T {                                                           \
    CONTAINER_API_FIELD(TTREE_FUNCTION_TABLE_TYPE(T))                          \
    TTREE_NODE(T) * root;                                                      \
    size_t size;                                                               \
  } T;
