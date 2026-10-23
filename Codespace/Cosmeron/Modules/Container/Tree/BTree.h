#pragma once

/* =============================================================
 * BTree — in-memory balanced multiway search tree (Set & Map).
 * Declarative Object-based implementation for Cosmeron Container.
 * ============================================================= */

#include "Impl/BTree.impl"

/* =============================================================
 * Public entry points — BTree SET
 * ============================================================= */

#define BTREE_SET_IMPLEMENT_ALL_DEGREE_CMP(SUFFIX, KEY_TYPE, DEGREE, CMP)     \
  TEMPLATE_TBTREE_SET_DEGREE(SUFFIX, KEY_TYPE, DEGREE);                       \
  TEMPLATE_TBTREE_SET_DEGREE_IMPLEMENT(SUFFIX, KEY_TYPE, DEGREE, CMP)

#define BTREE_SET_IMPLEMENT_ALL_CMP(SUFFIX, KEY_TYPE, CMP)                     \
  BTREE_SET_IMPLEMENT_ALL_DEGREE_CMP(SUFFIX, KEY_TYPE, BTREE_DEFAULT_DEGREE,  \
                                      CMP)

#define BTREE_SET_IMPLEMENT_ALL(KEY_TYPE)                                     \
  BTREE_SET_IMPLEMENT_ALL_DEGREE_CMP(                                         \
      KEY_TYPE, KEY_TYPE, BTREE_DEFAULT_DEGREE, COMPARISON_FUNC(KEY_TYPE))

/* =============================================================
 * Public entry points — BTree MAP
 * ============================================================= */

#define BTREE_MAP_IMPLEMENT_ALL_DEGREE_CMP(SUFFIX_K, SUFFIX_V, KEY_TYPE,      \
                                            VALUE_TYPE, DEGREE, CMP)           \
  TEMPLATE_TBTREE_MAP_DEGREE(SUFFIX_K, SUFFIX_V, KEY_TYPE, VALUE_TYPE,        \
                             DEGREE);                                          \
  TEMPLATE_TBTREE_MAP_DEGREE_IMPLEMENT(SUFFIX_K, SUFFIX_V, KEY_TYPE,          \
                                        VALUE_TYPE, DEGREE, CMP)

#define BTREE_MAP_IMPLEMENT_ALL_CMP(SUFFIX_K, SUFFIX_V, KEY_TYPE, VALUE_TYPE, \
                                    CMP)                                       \
  BTREE_MAP_IMPLEMENT_ALL_DEGREE_CMP(SUFFIX_K, SUFFIX_V, KEY_TYPE,            \
                                      VALUE_TYPE, BTREE_DEFAULT_DEGREE, CMP)

#define BTREE_MAP_IMPLEMENT_ALL(KEY_TYPE, VALUE_TYPE)                         \
  BTREE_MAP_IMPLEMENT_ALL_DEGREE_CMP(                                         \
      KEY_TYPE, VALUE_TYPE, KEY_TYPE, VALUE_TYPE, BTREE_DEFAULT_DEGREE,         \
      COMPARISON_FUNC(KEY_TYPE))

/* =============================================================
 * Default instantiations via Universal Template Engine
 * ============================================================= */

TEMPLATE_IMPLEMENT(TBTREE_SET, int, int);
TEMPLATE_IMPLEMENT(TBTREE_SET, float, float);
TEMPLATE_IMPLEMENT(TBTREE_SET, double, double);

TEMPLATE_TBTREE_MAP(int, int, int, int);
TEMPLATE_TBTREE_MAP_IMPLEMENT(int, int, int, int)

TEMPLATE_TBTREE_MAP(int, float, int, float);
TEMPLATE_TBTREE_MAP_IMPLEMENT(int, float, int, float)
