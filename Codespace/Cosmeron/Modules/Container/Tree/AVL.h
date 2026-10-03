#pragma once

#include "Tree.space"
#include "Impl/AVL.impl"

/* =============================================================
 * Public entry points — AVL SET
 * ============================================================= */

#define TREE_AVL_SET_IMPLEMENT_ALL(KEY_TYPE)                                   \
  COSMERON_MACRO_INTERNAL_TREE_AVL_SET_IMPLEMENT_ALL(                          \
      TREE_SET_TYPE(AVL, KEY_TYPE), KEY_TYPE, COMPARISON_FUNC(KEY_TYPE))      \
  typedef TREE_SET_TYPE(AVL, KEY_TYPE) TREE_PUBLIC_SET_TYPE(AVL, KEY_TYPE);   \
  TREE_FUNCTION_TABLE_STRUCT_SET(TREE_SET_TYPE(AVL, KEY_TYPE), KEY_TYPE)       \
  TREE_FUNCTION_TABLE_INSTANCE_SET(TREE_SET_TYPE(AVL, KEY_TYPE))

#define TREE_AVL_SET_IMPLEMENT_ALL_CMP(SUFFIX, KEY_TYPE, CMP)                 \
  COSMERON_MACRO_INTERNAL_TREE_AVL_SET_IMPLEMENT_ALL(                          \
      TREE_SET_TYPE(AVL, SUFFIX), KEY_TYPE, CMP)                               \
  typedef TREE_SET_TYPE(AVL, SUFFIX) TREE_PUBLIC_SET_TYPE(AVL, SUFFIX);        \
  TREE_FUNCTION_TABLE_STRUCT_SET(TREE_SET_TYPE(AVL, SUFFIX), KEY_TYPE)         \
  TREE_FUNCTION_TABLE_INSTANCE_SET(TREE_SET_TYPE(AVL, SUFFIX))

/* =============================================================
 * Public entry points — AVL MAP
 * ============================================================= */

#define TREE_AVL_MAP_IMPLEMENT_ALL(KEY_TYPE, VALUE_TYPE)                       \
  COSMERON_MACRO_INTERNAL_TREE_AVL_MAP_IMPLEMENT_ALL(                          \
      TREE_MAP_TYPE(AVL, KEY_TYPE, VALUE_TYPE), KEY_TYPE, VALUE_TYPE,         \
      COMPARISON_FUNC(KEY_TYPE))                                               \
  typedef TREE_MAP_TYPE(AVL, KEY_TYPE, VALUE_TYPE)                             \
      TREE_PUBLIC_MAP_TYPE(AVL, KEY_TYPE, VALUE_TYPE);                         \
  TREE_FUNCTION_TABLE_STRUCT_MAP(                                              \
      TREE_MAP_TYPE(AVL, KEY_TYPE, VALUE_TYPE), KEY_TYPE, VALUE_TYPE)          \
  TREE_FUNCTION_TABLE_INSTANCE_MAP(TREE_MAP_TYPE(AVL, KEY_TYPE, VALUE_TYPE))

#define TREE_AVL_MAP_IMPLEMENT_ALL_CMP(SUFFIX_K, KEY_TYPE, VALUE_TYPE, CMP)   \
  COSMERON_MACRO_INTERNAL_TREE_AVL_MAP_IMPLEMENT_ALL(                          \
      TREE_MAP_TYPE(AVL, SUFFIX_K, VALUE_TYPE), KEY_TYPE, VALUE_TYPE, CMP)     \
  typedef TREE_MAP_TYPE(AVL, SUFFIX_K, VALUE_TYPE)                             \
      TREE_PUBLIC_MAP_TYPE(AVL, SUFFIX_K, VALUE_TYPE);                         \
  TREE_FUNCTION_TABLE_STRUCT_MAP(                                              \
      TREE_MAP_TYPE(AVL, SUFFIX_K, VALUE_TYPE), KEY_TYPE, VALUE_TYPE)          \
  TREE_FUNCTION_TABLE_INSTANCE_MAP(TREE_MAP_TYPE(AVL, KEY_TYPE, VALUE_TYPE))

/* =============================================================
 * Convenience macros
 * ============================================================= */

#define TREE_AVL_SET_INSTANCE_DECLARE(KEY, NAME)                               \
  TREE_PUBLIC_SET_TYPE(AVL, KEY) NAME;                                         \
  CONTAINER_API_BIND(                                                          \
      NAME, TREE_FUNC(TREE_SET_TYPE(AVL, KEY), functions));                    \
  TREE_FUNC(TREE_SET_TYPE(AVL, KEY), Init)(&NAME);

#define TTree_AVL_Set(KEY, NAME) TREE_AVL_SET_INSTANCE_DECLARE(KEY, NAME)

#define TREE_AVL_MAP_INSTANCE_DECLARE(KEY, VAL, NAME)                          \
  TREE_PUBLIC_MAP_TYPE(AVL, KEY, VAL) NAME;                                    \
  CONTAINER_API_BIND(                                                          \
      NAME, TREE_FUNC(TREE_MAP_TYPE(AVL, KEY, VAL), functions));               \
  TREE_FUNC(TREE_MAP_TYPE(AVL, KEY, VAL), Init)(&NAME);

#define TTree_AVL_Map(KEY, VAL, NAME) TREE_AVL_MAP_INSTANCE_DECLARE(KEY, VAL, NAME)

/* =============================================================
 * Default instantiations
 * ============================================================= */

TREE_AVL_SET_IMPLEMENT_ALL(int)
TREE_AVL_SET_IMPLEMENT_ALL(float)
TREE_AVL_SET_IMPLEMENT_ALL(double)

TREE_AVL_MAP_IMPLEMENT_ALL(int, int)
TREE_AVL_MAP_IMPLEMENT_ALL(int, float)
