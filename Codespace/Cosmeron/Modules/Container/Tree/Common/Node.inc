#pragma once

/* =============================================================
 * Node struct generation — SET (key only) and MAP (key + value)
 *
 *   EXTRA_FIELDS: algorithm-specific fields injected into the node.
 *     AVL  → int height;
 *     RB   → TREE_RBColor color;
 *     BST  → (empty)
 * ============================================================= */

#define TTREE_NODE_SET(T, KEY_TYPE, EXTRA_FIELDS)                              \
  typedef struct TTREE_NODE(T) {                                               \
    KEY_TYPE key;                                                              \
    struct TTREE_NODE(T) * left;                                               \
    struct TTREE_NODE(T) * right;                                              \
    struct TTREE_NODE(T) * parent;                                             \
    EXTRA_FIELDS                                                               \
  } TTREE_NODE(T);

#define TTREE_NODE_MAP(T, KEY_TYPE, VALUE_TYPE, EXTRA_FIELDS)                  \
  typedef struct TTREE_NODE(T) {                                               \
    KEY_TYPE key;                                                              \
    VALUE_TYPE value;                                                          \
    struct TTREE_NODE(T) * left;                                               \
    struct TTREE_NODE(T) * right;                                              \
    struct TTREE_NODE(T) * parent;                                             \
    EXTRA_FIELDS                                                               \
  } TTREE_NODE(T);
