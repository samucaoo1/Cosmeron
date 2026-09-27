#pragma once

#include "Tree.space"
#include "RedBlack.impl"

/* =============================================================
 * Public entry points — RB SET
 * ============================================================= */

#define TREE_RB_SET_DEFINE(KEY_TYPE)                                           \
  TRB_DEFINE_SET(TTREE_SET_TYPE(RB, KEY_TYPE), KEY_TYPE, COMPARISON_FUNC(KEY_TYPE))         \
  typedef TTREE_SET_TYPE(RB, KEY_TYPE) TTREE_PUBLIC_SET_TYPE(RB, KEY_TYPE); \
  TTREE_FUNCTION_TABLE_SET(TTREE_SET_TYPE(RB, KEY_TYPE), KEY_TYPE)                     \
  TTREE_FUNCTION_TABLE_INSTANCE_SET(TTREE_SET_TYPE(RB, KEY_TYPE))

#define TREE_RB_SET_DEFINE_CMP(SUFFIX, KEY_TYPE, CMP)                         \
  TRB_DEFINE_SET(TTREE_SET_TYPE(RB, SUFFIX), KEY_TYPE, CMP)                   \
  typedef TTREE_SET_TYPE(RB, SUFFIX) TTREE_PUBLIC_SET_TYPE(RB, SUFFIX); \
  TTREE_FUNCTION_TABLE_SET(TTREE_SET_TYPE(RB, SUFFIX), KEY_TYPE)                       \
  TTREE_FUNCTION_TABLE_INSTANCE_SET(TTREE_SET_TYPE(RB, SUFFIX))

/* =============================================================
 * Public entry points — RB MAP
 * ============================================================= */

#define TREE_RB_MAP_DEFINE(KEY_TYPE, VALUE_TYPE)                               \
  TRB_DEFINE_MAP(TTREE_MAP_TYPE(RB, KEY_TYPE, VALUE_TYPE), KEY_TYPE,          \
                  VALUE_TYPE, COMPARISON_FUNC(KEY_TYPE))                                     \
  typedef TTREE_MAP_TYPE(RB, KEY_TYPE, VALUE_TYPE)                       \
      TTREE_PUBLIC_MAP_TYPE(RB, KEY_TYPE, VALUE_TYPE);                     \
  TTREE_FUNCTION_TABLE_MAP(TTREE_MAP_TYPE(RB, KEY_TYPE, VALUE_TYPE), KEY_TYPE,         \
                   VALUE_TYPE)                                                 \
  TTREE_FUNCTION_TABLE_INSTANCE_MAP(TTREE_MAP_TYPE(RB, KEY_TYPE, VALUE_TYPE))

#define TREE_RB_MAP_DEFINE_CMP(SUFFIX_K, KEY_TYPE, VALUE_TYPE, CMP)           \
  TRB_DEFINE_MAP(TTREE_MAP_TYPE(RB, SUFFIX_K, VALUE_TYPE), KEY_TYPE,          \
                  VALUE_TYPE, CMP)                                             \
  typedef TTREE_MAP_TYPE(RB, SUFFIX_K, VALUE_TYPE)                       \
      TTREE_PUBLIC_MAP_TYPE(RB, SUFFIX_K, VALUE_TYPE);                     \
  TTREE_FUNCTION_TABLE_MAP(TTREE_MAP_TYPE(RB, SUFFIX_K, VALUE_TYPE), KEY_TYPE,         \
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

TREE_RB_SET_DEFINE(int)
TREE_RB_SET_DEFINE(float)
TREE_RB_SET_DEFINE(double)

TREE_RB_MAP_DEFINE(int, int)
TREE_RB_MAP_DEFINE(int, float)
