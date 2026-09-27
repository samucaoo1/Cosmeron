#pragma once

#include "Tree.space"
#include "AVL.impl"

/* =============================================================
 * Public entry points — AVL SET
 *
 *   TREE_AVL_SET_DEFINE(int)
 *     → generates Tree_AVL_Set_int  (+ Node, VTable, all functions)
 *
 *   TREE_AVL_SET_DEFINE_CMP(cstring, const char *, COMPARISON_NS(cstring))
 *     → generates Tree_AVL_Set_cstring  (custom comparison)
 * ============================================================= */

#define TREE_AVL_SET_DEFINE(KEY_TYPE)                                          \
  TAVL_DEFINE_SET(TTREE_SET_TYPE(AVL, KEY_TYPE), KEY_TYPE, COMPARISON_FUNC(KEY_TYPE))       \
  typedef TTREE_SET_TYPE(AVL, KEY_TYPE) TTREE_PUBLIC_SET_TYPE(AVL, KEY_TYPE); \
  TTREE_FUNCTION_TABLE_SET(TTREE_SET_TYPE(AVL, KEY_TYPE), KEY_TYPE)                    \
  TTREE_FUNCTION_TABLE_INSTANCE_SET(TTREE_SET_TYPE(AVL, KEY_TYPE))

#define TREE_AVL_SET_DEFINE_CMP(SUFFIX, KEY_TYPE, CMP)                        \
  TAVL_DEFINE_SET(TTREE_SET_TYPE(AVL, SUFFIX), KEY_TYPE, CMP)                 \
  typedef TTREE_SET_TYPE(AVL, SUFFIX) TTREE_PUBLIC_SET_TYPE(AVL, SUFFIX); \
  TTREE_FUNCTION_TABLE_SET(TTREE_SET_TYPE(AVL, SUFFIX), KEY_TYPE)                      \
  TTREE_FUNCTION_TABLE_INSTANCE_SET(TTREE_SET_TYPE(AVL, SUFFIX))

/* =============================================================
 * Public entry points — AVL MAP
 *
 *   TREE_AVL_MAP_DEFINE(int, float)
 *     → generates Tree_AVL_Map_int_float
 *
 *   TREE_AVL_MAP_DEFINE_CMP(cstring, const char *, float,
 *                           COMPARISON_NS(cstring))
 *     → generates Tree_AVL_Map_cstring_float
 * ============================================================= */

#define TREE_AVL_MAP_DEFINE(KEY_TYPE, VALUE_TYPE)                              \
  TAVL_DEFINE_MAP(TTREE_MAP_TYPE(AVL, KEY_TYPE, VALUE_TYPE), KEY_TYPE,        \
                   VALUE_TYPE, COMPARISON_FUNC(KEY_TYPE))                                    \
  typedef TTREE_MAP_TYPE(AVL, KEY_TYPE, VALUE_TYPE)                       \
      TTREE_PUBLIC_MAP_TYPE(AVL, KEY_TYPE, VALUE_TYPE);                     \
  TTREE_FUNCTION_TABLE_MAP(TTREE_MAP_TYPE(AVL, KEY_TYPE, VALUE_TYPE), KEY_TYPE,        \
                   VALUE_TYPE)                                                 \
  TTREE_FUNCTION_TABLE_INSTANCE_MAP(TTREE_MAP_TYPE(AVL, KEY_TYPE, VALUE_TYPE))

#define TREE_AVL_MAP_DEFINE_CMP(SUFFIX_K, KEY_TYPE, VALUE_TYPE, CMP)          \
  TAVL_DEFINE_MAP(TTREE_MAP_TYPE(AVL, SUFFIX_K, VALUE_TYPE), KEY_TYPE,        \
                   VALUE_TYPE, CMP)                                            \
  typedef TTREE_MAP_TYPE(AVL, SUFFIX_K, VALUE_TYPE)                       \
      TTREE_PUBLIC_MAP_TYPE(AVL, SUFFIX_K, VALUE_TYPE);                     \
  TTREE_FUNCTION_TABLE_MAP(TTREE_MAP_TYPE(AVL, SUFFIX_K, VALUE_TYPE), KEY_TYPE,        \
                   VALUE_TYPE)                                                 \
  TTREE_FUNCTION_TABLE_INSTANCE_MAP(TTREE_MAP_TYPE(AVL, SUFFIX_K, VALUE_TYPE))

/* =============================================================
 * Convenience macros
 * ============================================================= */

#define TREE_AVL_SET_INSTANCE_DECLARE(KEY, NAME)                                                \
  TTREE_PUBLIC_SET_TYPE(AVL, KEY) NAME;                                               \
  CONTAINER_API_BIND(NAME, TTREE_FN(TTREE_SET_TYPE(AVL, KEY), functions));                   \
  TTREE_FN(TTREE_SET_TYPE(AVL, KEY), Init)(&NAME);
/* Compatibility alias: prefer TREE_AVL_SET_INSTANCE_DECLARE. */
#define TTree_AVL_Set(KEY, NAME) TREE_AVL_SET_INSTANCE_DECLARE(KEY, NAME)

#define TREE_AVL_MAP_INSTANCE_DECLARE(KEY, VAL, NAME)                                           \
  TTREE_PUBLIC_MAP_TYPE(AVL, KEY, VAL) NAME;                                          \
  CONTAINER_API_BIND(                                                     \
      NAME, TTREE_FN(TTREE_MAP_TYPE(AVL, KEY, VAL), functions));              \
  TTREE_FN(TTREE_MAP_TYPE(AVL, KEY, VAL), Init)(&NAME);
/* Compatibility alias: prefer TREE_AVL_MAP_INSTANCE_DECLARE. */
#define TTree_AVL_Map(KEY, VAL, NAME) TREE_AVL_MAP_INSTANCE_DECLARE(KEY, VAL, NAME)

/* =============================================================
 * Default instantiations
 * ============================================================= */

TREE_AVL_SET_DEFINE(int)
TREE_AVL_SET_DEFINE(float)
TREE_AVL_SET_DEFINE(double)

TREE_AVL_MAP_DEFINE(int, int)
TREE_AVL_MAP_DEFINE(int, float)
