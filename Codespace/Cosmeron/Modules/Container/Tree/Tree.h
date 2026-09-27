#pragma once

/* =============================================================
 * Public entry — Tree
 *
 *   #include "Container/Tree/Tree.h"
 *
 *   TREE_AVL_SET_DEFINE(int)
 *   TREE_AVL_SET_DEFINE(double)
 *   TREE_RB_SET_DEFINE(int)
 *   TREE_BST_SET_DEFINE(int)
 *
 *   TTREE_SET_TYPE(AVL, int)  my_set;
 *   my_set.api->init(&my_set);
 *   my_set.api->insert(&my_set, 42);
 * ============================================================= */

#include "BST.h"
#include "AVL.h"
#include "RedBlack.h"
