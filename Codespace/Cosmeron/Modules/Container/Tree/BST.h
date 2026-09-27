#pragma once

/* =============================================================
 * BST — plain Binary Search Tree (no balancing).
 *
 * BST does NOT auto-instantiate TREE_BST_SET_DEFINE for int /
 * float / double the way AVL and RedBlack do. Reason: BST has
 * worst-case O(n) height, which makes Insert / Remove / Find
 * degrade to O(n) on sorted input. AVL and RedBlack are strictly
 * better for any production use. BST is kept here as a teaching
 * reference and a baseline for correctness checks.
 *
 * If you really need a BST, define it explicitly:
 *   TREE_BST_SET_DEFINE(my_key)
 * ============================================================= */

#include "Tree.space"
#include "BST.impl"

/* =============================================================
 * Public entry points — BST SET
 * ============================================================= */

#define TREE_BST_SET_DEFINE(KEY_TYPE)                                          \
  TBST_DEFINE_SET(TTREE_SET_TYPE(BST, KEY_TYPE), KEY_TYPE, COMPARISON_FUNC(KEY_TYPE))       \
  typedef TTREE_SET_TYPE(BST, KEY_TYPE) TTREE_PUBLIC_SET_TYPE(BST, KEY_TYPE); \
  TTREE_FUNCTION_TABLE_SET(TTREE_SET_TYPE(BST, KEY_TYPE), KEY_TYPE)                    \
  TTREE_FUNCTION_TABLE_INSTANCE_SET(TTREE_SET_TYPE(BST, KEY_TYPE))

#define TREE_BST_SET_DEFINE_CMP(SUFFIX, KEY_TYPE, CMP)                        \
  TBST_DEFINE_SET(TTREE_SET_TYPE(BST, SUFFIX), KEY_TYPE, CMP)                 \
  typedef TTREE_SET_TYPE(BST, SUFFIX) TTREE_PUBLIC_SET_TYPE(BST, SUFFIX); \
  TTREE_FUNCTION_TABLE_SET(TTREE_SET_TYPE(BST, SUFFIX), KEY_TYPE)                      \
  TTREE_FUNCTION_TABLE_INSTANCE_SET(TTREE_SET_TYPE(BST, SUFFIX))

/* =============================================================
 * Public entry points — BST MAP
 * ============================================================= */

#define TREE_BST_MAP_DEFINE(KEY_TYPE, VALUE_TYPE)                              \
  TBST_DEFINE_MAP(TTREE_MAP_TYPE(BST, KEY_TYPE, VALUE_TYPE), KEY_TYPE,        \
                   VALUE_TYPE, COMPARISON_FUNC(KEY_TYPE))                                    \
  typedef TTREE_MAP_TYPE(BST, KEY_TYPE, VALUE_TYPE)                       \
      TTREE_PUBLIC_MAP_TYPE(BST, KEY_TYPE, VALUE_TYPE);                     \
  TTREE_FUNCTION_TABLE_MAP(TTREE_MAP_TYPE(BST, KEY_TYPE, VALUE_TYPE), KEY_TYPE,        \
                   VALUE_TYPE)                                                 \
  TTREE_FUNCTION_TABLE_INSTANCE_MAP(TTREE_MAP_TYPE(BST, KEY_TYPE, VALUE_TYPE))

#define TREE_BST_MAP_DEFINE_CMP(SUFFIX_K, KEY_TYPE, VALUE_TYPE, CMP)          \
  TBST_DEFINE_MAP(TTREE_MAP_TYPE(BST, SUFFIX_K, VALUE_TYPE), KEY_TYPE,        \
                   VALUE_TYPE, CMP)                                            \
  typedef TTREE_MAP_TYPE(BST, SUFFIX_K, VALUE_TYPE)                       \
      TTREE_PUBLIC_MAP_TYPE(BST, SUFFIX_K, VALUE_TYPE);                     \
  TTREE_FUNCTION_TABLE_MAP(TTREE_MAP_TYPE(BST, SUFFIX_K, VALUE_TYPE), KEY_TYPE,        \
                   VALUE_TYPE)                                                 \
  TTREE_FUNCTION_TABLE_INSTANCE_MAP(TTREE_MAP_TYPE(BST, SUFFIX_K, VALUE_TYPE))

/* =============================================================
 * Convenience macros
 * ============================================================= */

#define TREE_BST_SET_INSTANCE_DECLARE(KEY, NAME)                                                \
  TTREE_PUBLIC_SET_TYPE(BST, KEY) NAME;                                               \
  CONTAINER_API_BIND(NAME, TTREE_FN(TTREE_SET_TYPE(BST, KEY), functions));                   \
  TTREE_FN(TTREE_SET_TYPE(BST, KEY), Init)(&NAME);
/* Compatibility alias: prefer TREE_BST_SET_INSTANCE_DECLARE. */
#define TTree_BST_Set(KEY, NAME) TREE_BST_SET_INSTANCE_DECLARE(KEY, NAME)

#define TREE_BST_MAP_INSTANCE_DECLARE(KEY, VAL, NAME)                                           \
  TTREE_PUBLIC_MAP_TYPE(BST, KEY, VAL) NAME;                                          \
  CONTAINER_API_BIND(                                                     \
      NAME, TTREE_FN(TTREE_MAP_TYPE(BST, KEY, VAL), functions));              \
  TTREE_FN(TTREE_MAP_TYPE(BST, KEY, VAL), Init)(&NAME);
/* Compatibility alias: prefer TREE_BST_MAP_INSTANCE_DECLARE. */
#define TTree_BST_Map(KEY, VAL, NAME) TREE_BST_MAP_INSTANCE_DECLARE(KEY, VAL, NAME)

/* =============================================================
 * Default instantiations
 * ============================================================= */

TREE_BST_SET_DEFINE(int)
TREE_BST_SET_DEFINE(float)
TREE_BST_SET_DEFINE(double)

TREE_BST_MAP_DEFINE(int, int)
TREE_BST_MAP_DEFINE(int, float)
