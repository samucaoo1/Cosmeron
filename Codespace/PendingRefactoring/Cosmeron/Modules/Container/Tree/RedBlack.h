#pragma once

#include "Tree.space"
#include "Impl/RedBlack.impl"

/* =============================================================
 * Public entry points — RB SET
 * ============================================================= */

#define TREE_RB_SET_IMPLEMENT_ALL(KEY_TYPE)                                    \
  COSMERON_MACRO_INTERNAL_TREE_RB_SET_IMPLEMENT_ALL(                           \
      TREE_SET_TYPE(RB, KEY_TYPE), KEY_TYPE, COMPARISON_FUNC(KEY_TYPE))       \
  typedef TREE_SET_TYPE(RB, KEY_TYPE) TREE_PUBLIC_SET_TYPE(RB, KEY_TYPE);     \
  TREE_FUNCTION_TABLE_STRUCT_SET(TREE_SET_TYPE(RB, KEY_TYPE), KEY_TYPE)        \
  TREE_FUNCTION_TABLE_INSTANCE_SET(TREE_SET_TYPE(RB, KEY_TYPE))

#define TREE_RB_SET_IMPLEMENT_ALL_CMP(SUFFIX, KEY_TYPE, CMP)                  \
  COSMERON_MACRO_INTERNAL_TREE_RB_SET_IMPLEMENT_ALL(                           \
      TREE_SET_TYPE(RB, SUFFIX), KEY_TYPE, CMP)                                \
  typedef TREE_SET_TYPE(RB, SUFFIX) TREE_PUBLIC_SET_TYPE(RB, SUFFIX);         \
  TREE_FUNCTION_TABLE_STRUCT_SET(TREE_SET_TYPE(RB, SUFFIX), KEY_TYPE)          \
  TREE_FUNCTION_TABLE_INSTANCE_SET(TREE_SET_TYPE(RB, SUFFIX))

/* =============================================================
 * Public entry points — RB MAP
 * ============================================================= */

#define TREE_RB_MAP_IMPLEMENT_ALL(KEY_TYPE, VALUE_TYPE)                        \
  COSMERON_MACRO_INTERNAL_TREE_RB_MAP_IMPLEMENT_ALL(                           \
      TREE_MAP_TYPE(RB, KEY_TYPE, VALUE_TYPE), KEY_TYPE, VALUE_TYPE,          \
      COMPARISON_FUNC(KEY_TYPE))                                                \
  typedef TREE_MAP_TYPE(RB, KEY_TYPE, VALUE_TYPE)                              \
      TREE_PUBLIC_MAP_TYPE(RB, KEY_TYPE, VALUE_TYPE);                          \
  TREE_FUNCTION_TABLE_STRUCT_MAP(                                               \
      TREE_MAP_TYPE(RB, KEY_TYPE, VALUE_TYPE), KEY_TYPE, VALUE_TYPE)           \
  TREE_FUNCTION_TABLE_INSTANCE_MAP(TREE_MAP_TYPE(RB, KEY_TYPE, VALUE_TYPE))

#define TREE_RB_MAP_IMPLEMENT_ALL_CMP(SUFFIX_K, KEY_TYPE, VALUE_TYPE, CMP)    \
  COSMERON_MACRO_INTERNAL_TREE_RB_MAP_IMPLEMENT_ALL(                           \
      TREE_MAP_TYPE(RB, SUFFIX_K, VALUE_TYPE), KEY_TYPE, VALUE_TYPE, CMP)      \
  typedef TREE_MAP_TYPE(RB, SUFFIX_K, VALUE_TYPE)                              \
      TREE_PUBLIC_MAP_TYPE(RB, SUFFIX_K, VALUE_TYPE);                          \
  TREE_FUNCTION_TABLE_STRUCT_MAP(                                               \
      TREE_MAP_TYPE(RB, SUFFIX_K, VALUE_TYPE), KEY_TYPE, VALUE_TYPE)           \
  TREE_FUNCTION_TABLE_INSTANCE_MAP(TREE_MAP_TYPE(RB, SUFFIX_K, VALUE_TYPE))

/* =============================================================
 * Convenience macros
 * ============================================================= */

#define TREE_RB_SET_INSTANCE_DECLARE(KEY, NAME)                                \
  TREE_PUBLIC_SET_TYPE(RB, KEY) NAME;                                          \
  COSMERON_MACRO_INTERNAL_TREE_API_BIND(                                       \
      NAME, TREE_SET_TYPE(RB, KEY));                     \
  TREE_FUNC(TREE_SET_TYPE(RB, KEY), Init)(&NAME);

#define TTree_RB_Set(KEY, NAME) TREE_RB_SET_INSTANCE_DECLARE(KEY, NAME)

#define TREE_RB_MAP_INSTANCE_DECLARE(KEY, VAL, NAME)                           \
  TREE_PUBLIC_MAP_TYPE(RB, KEY, VAL) NAME;                                     \
  COSMERON_MACRO_INTERNAL_TREE_API_BIND(                                       \
      NAME, TREE_MAP_TYPE(RB, KEY, VAL));                \
  TREE_FUNC(TREE_MAP_TYPE(RB, KEY, VAL), Init)(&NAME);

#define TTree_RB_Map(KEY, VAL, NAME) TREE_RB_MAP_INSTANCE_DECLARE(KEY, VAL, NAME)

/* =============================================================
 * Default instantiations
 * ============================================================= */

TREE_RB_SET_IMPLEMENT_ALL(int)
TREE_RB_SET_IMPLEMENT_ALL(float)
TREE_RB_SET_IMPLEMENT_ALL(double)

TREE_RB_MAP_IMPLEMENT_ALL(int, int)
TREE_RB_MAP_IMPLEMENT_ALL(int, float)
