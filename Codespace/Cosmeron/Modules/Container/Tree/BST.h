#pragma once

/* =============================================================
 * BST — plain Binary Search Tree (no balancing).
 * ============================================================= */

#include "Tree.space"
#include "Impl/BST.impl"

/* =============================================================
 * Public entry points — BST SET
 * ============================================================= */

#define TREE_BST_SET_IMPLEMENT_ALL(KEY_TYPE)                                   \
  COSMERON_MACRO_INTERNAL_TREE_BST_SET_IMPLEMENT_ALL(                          \
      TREE_SET_TYPE(BST, KEY_TYPE), KEY_TYPE, COMPARISON_FUNC(KEY_TYPE))      \
  typedef TREE_SET_TYPE(BST, KEY_TYPE) TREE_PUBLIC_SET_TYPE(BST, KEY_TYPE);   \
  TREE_FUNCTION_TABLE_STRUCT_SET(TREE_SET_TYPE(BST, KEY_TYPE), KEY_TYPE)       \
  TREE_FUNCTION_TABLE_INSTANCE_SET(TREE_SET_TYPE(BST, KEY_TYPE))

#define TREE_BST_SET_IMPLEMENT_ALL_CMP(SUFFIX, KEY_TYPE, CMP)                 \
  COSMERON_MACRO_INTERNAL_TREE_BST_SET_IMPLEMENT_ALL(                          \
      TREE_SET_TYPE(BST, SUFFIX), KEY_TYPE, CMP)                               \
  typedef TREE_SET_TYPE(BST, SUFFIX) TREE_PUBLIC_SET_TYPE(BST, SUFFIX);        \
  TREE_FUNCTION_TABLE_STRUCT_SET(TREE_SET_TYPE(BST, SUFFIX), KEY_TYPE)         \
  TREE_FUNCTION_TABLE_INSTANCE_SET(TREE_SET_TYPE(BST, SUFFIX))

/* =============================================================
 * Public entry points — BST MAP
 * ============================================================= */

#define TREE_BST_MAP_IMPLEMENT_ALL(KEY_TYPE, VALUE_TYPE)                       \
  COSMERON_MACRO_INTERNAL_TREE_BST_MAP_IMPLEMENT_ALL(                          \
      TREE_MAP_TYPE(BST, KEY_TYPE, VALUE_TYPE), KEY_TYPE, VALUE_TYPE,         \
      COMPARISON_FUNC(KEY_TYPE))                                               \
  typedef TREE_MAP_TYPE(BST, KEY_TYPE, VALUE_TYPE)                             \
      TREE_PUBLIC_MAP_TYPE(BST, KEY_TYPE, VALUE_TYPE);                         \
  TREE_FUNCTION_TABLE_STRUCT_MAP(                                              \
      TREE_MAP_TYPE(BST, KEY_TYPE, VALUE_TYPE), KEY_TYPE, VALUE_TYPE)          \
  TREE_FUNCTION_TABLE_INSTANCE_MAP(TREE_MAP_TYPE(BST, KEY_TYPE, VALUE_TYPE))

#define TREE_BST_MAP_IMPLEMENT_ALL_CMP(SUFFIX_K, KEY_TYPE, VALUE_TYPE, CMP)   \
  COSMERON_MACRO_INTERNAL_TREE_BST_MAP_IMPLEMENT_ALL(                          \
      TREE_MAP_TYPE(BST, SUFFIX_K, VALUE_TYPE), KEY_TYPE, VALUE_TYPE, CMP)     \
  typedef TREE_MAP_TYPE(BST, SUFFIX_K, VALUE_TYPE)                             \
      TREE_PUBLIC_MAP_TYPE(BST, SUFFIX_K, VALUE_TYPE);                         \
  TREE_FUNCTION_TABLE_STRUCT_MAP(                                              \
      TREE_MAP_TYPE(BST, SUFFIX_K, VALUE_TYPE), KEY_TYPE, VALUE_TYPE)          \
  TREE_FUNCTION_TABLE_INSTANCE_MAP(TREE_MAP_TYPE(BST, SUFFIX_K, VALUE_TYPE))

/* =============================================================
 * Convenience macros
 * ============================================================= */

#define TREE_BST_SET_INSTANCE_DECLARE(KEY, NAME)                               \
  TREE_PUBLIC_SET_TYPE(BST, KEY) NAME;                                         \
  COSMERON_MACRO_INTERNAL_TREE_API_BIND(                                       \
      NAME, TREE_SET_TYPE(BST, KEY));                    \
  TREE_FUNC(TREE_SET_TYPE(BST, KEY), Init)(&NAME);

#define TTree_BST_Set(KEY, NAME) TREE_BST_SET_INSTANCE_DECLARE(KEY, NAME)

#define TREE_BST_MAP_INSTANCE_DECLARE(KEY, VAL, NAME)                          \
  TREE_PUBLIC_MAP_TYPE(BST, KEY, VAL) NAME;                                    \
  COSMERON_MACRO_INTERNAL_TREE_API_BIND(                                       \
      NAME, TREE_MAP_TYPE(BST, KEY, VAL));               \
  TREE_FUNC(TREE_MAP_TYPE(BST, KEY, VAL), Init)(&NAME);

#define TTree_BST_Map(KEY, VAL, NAME) TREE_BST_MAP_INSTANCE_DECLARE(KEY, VAL, NAME)

/* =============================================================
 * Default instantiations
 * ============================================================= */

TREE_BST_SET_IMPLEMENT_ALL(int)
TREE_BST_SET_IMPLEMENT_ALL(float)
TREE_BST_SET_IMPLEMENT_ALL(double)

TREE_BST_MAP_IMPLEMENT_ALL(int, int)
TREE_BST_MAP_IMPLEMENT_ALL(int, float)
