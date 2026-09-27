#pragma once

#include "../Tree/BST.h"
#include "../Tree/AVL.h"
#include "../Tree/RedBlack.h"

#if CONTAINER_ENABLE_PSEUDO_METHODS

#define TREE_ALIAS_SELECT_INSERT(_1, _2, _3, NAME, ...) NAME

#define TREE_SET_ASSOC(ALG, OP)                                                \
  TTREE_SET_TYPE(ALG, int) *: TTREE_FN(TTREE_SET_TYPE(ALG, int), OP),        \
  TTREE_SET_TYPE(ALG, float) *: TTREE_FN(TTREE_SET_TYPE(ALG, float), OP),    \
  TTREE_SET_TYPE(ALG, double) *: TTREE_FN(TTREE_SET_TYPE(ALG, double), OP)

#define TREE_MAP_ASSOC(ALG, OP)                                                \
  TTREE_MAP_TYPE(ALG, int, int) *: TTREE_FN(TTREE_MAP_TYPE(ALG, int, int), OP), \
  TTREE_MAP_TYPE(ALG, int, float) *: TTREE_FN(TTREE_MAP_TYPE(ALG, int, float), OP)

#define TREE_ALL_ASSOC(OP)                                                     \
  TREE_SET_ASSOC(BST, OP), TREE_SET_ASSOC(AVL, OP), TREE_SET_ASSOC(RB, OP),  \
  TREE_MAP_ASSOC(BST, OP), TREE_MAP_ASSOC(AVL, OP), TREE_MAP_ASSOC(RB, OP)

#define TreeInit(self) _Generic(&(self), TREE_ALL_ASSOC(Init))(&(self))
#define TreeDestroy(self) _Generic(&(self), TREE_ALL_ASSOC(Destroy))(&(self))
#define TreeClear(self) _Generic(&(self), TREE_ALL_ASSOC(Clear))(&(self))

#define TREE_INSERT_SET(self, key)                                             \
  _Generic(&(self), TREE_SET_ASSOC(BST, Insert), TREE_SET_ASSOC(AVL, Insert), \
                    TREE_SET_ASSOC(RB, Insert))(&(self), (key))
#define TREE_INSERT_MAP(self, key, value)                                      \
  _Generic(&(self), TREE_MAP_ASSOC(BST, Insert), TREE_MAP_ASSOC(AVL, Insert), \
                    TREE_MAP_ASSOC(RB, Insert))(&(self), (key), (value))
#define TreeInsert(...)                                                        \
  TREE_ALIAS_SELECT_INSERT(__VA_ARGS__, TREE_INSERT_MAP, TREE_INSERT_SET,     \
                           TREE_ALIAS_UNUSED)(__VA_ARGS__)

#define TreeRemove(self, key)                                                  \
  _Generic(&(self), TREE_ALL_ASSOC(Remove))(&(self), (key))
#define TreeFind(self, key)                                                    \
  _Generic(&(self), TREE_ALL_ASSOC(Find))(&(self), (key))
#define TreeFindNode(self, key)                                                \
  _Generic(&(self), TREE_ALL_ASSOC(FindNode))(&(self), (key))
#define TreeContains(self, key)                                                \
  _Generic(&(self), TREE_ALL_ASSOC(Contains))(&(self), (key))
#define TreeMin(self) _Generic(&(self), TREE_ALL_ASSOC(Min))(&(self))
#define TreeMax(self) _Generic(&(self), TREE_ALL_ASSOC(Max))(&(self))
#define TreeEmpty(self) _Generic(&(self), TREE_ALL_ASSOC(Empty))(&(self))
#define TreeSize(self) _Generic(&(self), TREE_ALL_ASSOC(Size))(&(self))
#define TreeBegin(self) _Generic(&(self), TREE_ALL_ASSOC(Begin))(&(self))
#define TreeEnd(self) _Generic(&(self), TREE_ALL_ASSOC(End))(&(self))
#define TreeNext(self, node) _Generic(&(self), TREE_ALL_ASSOC(Next))((node))
#define TreePrev(self, node) _Generic(&(self), TREE_ALL_ASSOC(Prev))((node))
#define TreeConstBegin(self) _Generic(&(self), TREE_ALL_ASSOC(ConstBegin))(&(self))
#define TreeConstEnd(self) _Generic(&(self), TREE_ALL_ASSOC(ConstEnd))(&(self))
#define TreeConstNext(self, node) _Generic(&(self), TREE_ALL_ASSOC(ConstNext))((node))
#define TreeConstPrev(self, node) _Generic(&(self), TREE_ALL_ASSOC(ConstPrev))((node))

#endif
