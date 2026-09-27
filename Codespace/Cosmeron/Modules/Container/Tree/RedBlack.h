#pragma once

#include "Tree.space"
#include "RedBlack.impl"

/* =============================================================
 * Public entry points — RB SET
 * ============================================================= */

#define TREE_RB_SET_IMPLEMENT_ALL(KEY_TYPE)                                           \
  TRB_SET_IMPLEMENT_ALL(TTREE_SET_TYPE(RB, KEY_TYPE), KEY_TYPE, COMPARISON_FUNC(KEY_TYPE))         \
  typedef TTREE_SET_TYPE(RB, KEY_TYPE) TTREE_PUBLIC_SET_TYPE(RB, KEY_TYPE); \
  TTREE_FUNCTION_TABLE_STRUCT_SET(TTREE_SET_TYPE(RB, KEY_TYPE), KEY_TYPE)                     \
  TTREE_FUNCTION_TABLE_INSTANCE_SET(TTREE_SET_TYPE(RB, KEY_TYPE))

#define TREE_RB_SET_IMPLEMENT_ALL_CMP(SUFFIX, KEY_TYPE, CMP)                         \
  TRB_SET_IMPLEMENT_ALL(TTREE_SET_TYPE(RB, SUFFIX), KEY_TYPE, CMP)                   \
  typedef TTREE_SET_TYPE(RB, SUFFIX) TTREE_PUBLIC_SET_TYPE(RB, SUFFIX); \
  TTREE_FUNCTION_TABLE_STRUCT_SET(TTREE_SET_TYPE(RB, SUFFIX), KEY_TYPE)                       \
  TTREE_FUNCTION_TABLE_INSTANCE_SET(TTREE_SET_TYPE(RB, SUFFIX))

/* =============================================================
 * Public entry points — RB MAP
 * ============================================================= */

#define TREE_RB_MAP_IMPLEMENT_ALL(KEY_TYPE, VALUE_TYPE)                               \
  TRB_MAP_IMPLEMENT_ALL(TTREE_MAP_TYPE(RB, KEY_TYPE, VALUE_TYPE), KEY_TYPE,          \
                  VALUE_TYPE, COMPARISON_FUNC(KEY_TYPE))                                     \
  typedef TTREE_MAP_TYPE(RB, KEY_TYPE, VALUE_TYPE)                       \
      TTREE_PUBLIC_MAP_TYPE(RB, KEY_TYPE, VALUE_TYPE);                     \
  TTREE_FUNCTION_TABLE_STRUCT_MAP(TTREE_MAP_TYPE(RB, KEY_TYPE, VALUE_TYPE), KEY_TYPE,         \
                   VALUE_TYPE)                                                 \
  TTREE_FUNCTION_TABLE_INSTANCE_MAP(TTREE_MAP_TYPE(RB, KEY_TYPE, VALUE_TYPE))

#define TREE_RB_MAP_IMPLEMENT_ALL_CMP(SUFFIX_K, KEY_TYPE, VALUE_TYPE, CMP)           \
  TRB_MAP_IMPLEMENT_ALL(TTREE_MAP_TYPE(RB, SUFFIX_K, VALUE_TYPE), KEY_TYPE,          \
                  VALUE_TYPE, CMP)                                             \
  typedef TTREE_MAP_TYPE(RB, SUFFIX_K, VALUE_TYPE)                       \
      TTREE_PUBLIC_MAP_TYPE(RB, SUFFIX_K, VALUE_TYPE);                     \
  TTREE_FUNCTION_TABLE_STRUCT_MAP(TTREE_MAP_TYPE(RB, SUFFIX_K, VALUE_TYPE), KEY_TYPE,         \
                   VALUE_TYPE)                                                 \
  TTREE_FUNCTION_TABLE_INSTANCE_MAP(TTREE_MAP_TYPE(RB, SUFFIX_K, VALUE_TYPE))

/* =============================================================
 * Convenience macros
 * ============================================================= */

#define TREE_RB_SET_INSTANCE_DECLARE(KEY, NAME)                                                 \
  TTREE_PUBLIC_SET_TYPE(RB, KEY) NAME;                                                \
  CONTAINER_API_BIND(NAME, TTREE_FN(TTREE_SET_TYPE(RB, KEY), functions));                    \
  TTREE_FN(TTREE_SET_TYPE(RB, KEY), Init)(&NAME);
/* Compatibility alias: prefer TREE_RB_SET_INSTANCE_DECLARE. */
#define TTree_RB_Set(KEY, NAME) TREE_RB_SET_INSTANCE_DECLARE(KEY, NAME)

#define TREE_RB_MAP_INSTANCE_DECLARE(KEY, VAL, NAME)                                            \
  TTREE_PUBLIC_MAP_TYPE(RB, KEY, VAL) NAME;                                           \
  CONTAINER_API_BIND(                                                     \
      NAME, TTREE_FN(TTREE_MAP_TYPE(RB, KEY, VAL), functions));               \
  TTREE_FN(TTREE_MAP_TYPE(RB, KEY, VAL), Init)(&NAME);
/* Compatibility alias: prefer TREE_RB_MAP_INSTANCE_DECLARE. */
#define TTree_RB_Map(KEY, VAL, NAME) TREE_RB_MAP_INSTANCE_DECLARE(KEY, VAL, NAME)

/* =============================================================
 * Default instantiations
 * ============================================================= */

TREE_RB_SET_IMPLEMENT_ALL(int)
TREE_RB_SET_IMPLEMENT_ALL(float)
TREE_RB_SET_IMPLEMENT_ALL(double)

TREE_RB_MAP_IMPLEMENT_ALL(int, int)
TREE_RB_MAP_IMPLEMENT_ALL(int, float)
