#ifndef COSMERON_CONTAINER_TREE_COMMON_INC
#define COSMERON_CONTAINER_TREE_COMMON_INC

#if defined(CONTAINER_DISABLE_FUNCTION_TABLE) &&                               \
    !defined(OBJECT_DISABLE_FUNCTION_TABLE)
#define OBJECT_DISABLE_FUNCTION_TABLE 1
#endif

#if defined(TREE_DISABLE_FUNCTION_TABLE) &&                                    \
    !defined(OBJECT_DISABLE_FUNCTION_TABLE)
#define OBJECT_DISABLE_FUNCTION_TABLE 1
#endif

#include "../../../Core/Algorithm/Comparison.h"
#include "../../../Core/Memory/Alloc.h"
#include "../../../Core/Module/Module.h"
#include "../../../Core/Outcome/Status.h"
#include "../../../Core/Preprocessor/Compiling.inc"
#include "../../../Core/Preprocessor/Operations.h"
#include "../../../Core/Primitive/Types.h"
#include "../Iterator/Iterator.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* ==========================================================================
 * Module Template Forwarding Helper
 * Enables: TEMPLATE_INSTANTIATE(Tree, Target, Params...)
 * ========================================================================== */

#ifndef TEMPLATE_Tree
#define TEMPLATE_Tree(TARGET, ...) NS(TEMPLATE_Tree, TARGET)(__VA_ARGS__)
#endif

#ifndef BTREE_DEFAULT_DEGREE
#define BTREE_DEFAULT_DEGREE 3U
#endif

/* ==========================================================================
 * Tree Enums (Row-Tuple X-Macro Tables)
 * ========================================================================== */

#define TREE_RB_COLOR_COL_NAME(NAME, VALUE) NAME
#define TREE_RB_COLOR_COL_VALUE(NAME, VALUE) VALUE
#define TREE_RB_COLOR_GET(FIELD, ROW)                                          \
  PP_OP_CAT2(TREE_RB_COLOR_COL_, FIELD) ROW

#define TREE_RB_COLOR_TABLE(X)                                                 \
  X((BLACK, 0))                                                                \
  X((RED, 1))

#define TREE_TRAVERSAL_COL_NAME(NAME, VALUE) NAME
#define TREE_TRAVERSAL_COL_VALUE(NAME, VALUE) VALUE
#define TREE_TRAVERSAL_GET(FIELD, ROW)                                         \
  PP_OP_CAT2(TREE_TRAVERSAL_COL_, FIELD) ROW

#define TREE_TRAVERSAL_TABLE(X)                                                \
  X((IN_ORDER, 0))                                                             \
  X((PRE_ORDER, 1))                                                            \
  X((POST_ORDER, 2))

/* ==========================================================================
 * Shared Internal Node Allocation Helpers (Zero FunctionTable Overhead)
 * ========================================================================== */

MODULE(DOMAIN(Container), Tree,
  ENUM_EX(RBColor, TREE_RB_COLOR_TABLE, QUALIFIED),
  ENUM_EX(Traversal, TREE_TRAVERSAL_TABLE, QUALIFIED),

  INTERNAL(
    OPERATION(AllocNode, (void **outNode, size_t nodeSize), {
      if (!outNode || nodeSize == 0U)
        return STATUS_CONST(INVALID_ARGUMENT);
      return MODULE_FUNC(Memory, Alloc, Bytes)(outNode, nodeSize);
    }),

    PROCEDURE(FreeNode, (void **inoutNode), {
      if (inoutNode && *inoutNode) {
        MODULE_FUNC(Memory, Alloc, Free)(inoutNode);
      }
    })
  )
);

/* ==========================================================================
 * Shared Binary Tree Internal Helpers & Iterators (BST / AVL / RedBlack)
 * ========================================================================== */

#define COSMERON_MACRO_INTERNAL_TREE_BIN_HELPERS(TARGET, SUFFIX, NODE_TYPE,    \
                                                 KEY_TYPE, COMP_CLOSURE_TYPE)  \
  INTERNAL_DEF_EX(                                                             \
      Tree, NS(TARGET, SUFFIX),                                                \
      COMPARISON(DefaultCompare,                                               \
                 (KEY_TYPE left, KEY_TYPE right, void *context), {             \
                   (void)context;                                              \
                   return (left < right)                                       \
                              ? COMPARISON_CONST(LOWER)                        \
                              : ((left > right) ? COMPARISON_CONST(HIGHER)     \
                                                : COMPARISON_CONST(EQUAL));    \
                 }),                                                           \
                                                                               \
      FUNCTION(NODE_TYPE *, MinNode, (NODE_TYPE * node), {                     \
        if (!node)                                                             \
          return NULL;                                                         \
        while (node->left != NULL)                                             \
          node = node->left;                                                   \
        return node;                                                           \
      }),                                                                      \
                                                                               \
      FUNCTION(NODE_TYPE *, MaxNode, (NODE_TYPE * node), {                     \
        if (!node)                                                             \
          return NULL;                                                         \
        while (node->right != NULL)                                            \
          node = node->right;                                                  \
        return node;                                                           \
      }),                                                                      \
                                                                               \
      FUNCTION(NODE_TYPE *, InOrderNext, (NODE_TYPE * node), {                 \
        if (!node)                                                             \
          return NULL;                                                         \
        if (node->right != NULL)                                               \
          return MODULE_IFUNC(Tree, NS(TARGET, SUFFIX), MinNode)(node->right); \
        NODE_TYPE *parent = node->parent;                                      \
        while (parent != NULL && node == parent->right) {                      \
          node = parent;                                                       \
          parent = parent->parent;                                             \
        }                                                                      \
        return parent;                                                         \
      }),                                                                      \
                                                                               \
      FUNCTION(NODE_TYPE *, InOrderPrev, (NODE_TYPE * node), {                 \
        if (!node)                                                             \
          return NULL;                                                         \
        if (node->left != NULL)                                                \
          return MODULE_IFUNC(Tree, NS(TARGET, SUFFIX), MaxNode)(node->left);  \
        NODE_TYPE *parent = node->parent;                                      \
        while (parent != NULL && node == parent->left) {                       \
          node = parent;                                                       \
          parent = parent->parent;                                             \
        }                                                                      \
        return parent;                                                         \
      }),                                                                      \
                                                                               \
      PROCEDURE(DestroyNodes, (NODE_TYPE * node), {                            \
        if (!node)                                                             \
          return;                                                              \
        MODULE_IFUNC(Tree, NS(TARGET, SUFFIX), DestroyNodes)(node->left);      \
        MODULE_IFUNC(Tree, NS(TARGET, SUFFIX), DestroyNodes)(node->right);     \
        void *raw = (void *)node;                                              \
        MODULE_IFUNC(Tree, FreeNode)(&raw);                                    \
      }),                                                                      \
                                                                               \
      FUNCTION(size_t, ComputeHeight, (const NODE_TYPE *node), {               \
        if (!node)                                                             \
          return 0U;                                                           \
        size_t leftH =                                                         \
            MODULE_IFUNC(Tree, NS(TARGET, SUFFIX), ComputeHeight)(node->left); \
        size_t rightH =                                                        \
            MODULE_IFUNC(Tree, NS(TARGET, SUFFIX), ComputeHeight)(node->right);\
        return 1U + (leftH > rightH ? leftH : rightH);                         \
      }),                                                                      \
                                                                               \
      PROCEDURE(Transplant,                                                    \
                (NODE_TYPE **root, NODE_TYPE *u, NODE_TYPE *v), {              \
                  if (!u->parent) {                                            \
                    *root = v;                                                 \
                  } else if (u == u->parent->left) {                           \
                    u->parent->left = v;                                       \
                  } else {                                                     \
                    u->parent->right = v;                                      \
                  }                                                            \
                  if (v)                                                       \
                    v->parent = u->parent;                                     \
                }),                                                            \
                                                                               \
      FUNCTION(NODE_TYPE *, FindNode,                                          \
               (NODE_TYPE * root, COMP_CLOSURE_TYPE cmp, KEY_TYPE key), {      \
                 NODE_TYPE *current = root;                                    \
                 while (current != NULL) {                                     \
                   CMPOUT r = CLOSURE_CALL(cmp, key, current->key);            \
                   if (r < COMPARISON_CONST(EQUAL))                            \
                     current = current->left;                                  \
                   else if (r > COMPARISON_CONST(EQUAL))                       \
                     current = current->right;                                 \
                   else                                                        \
                     return current;                                           \
                 }                                                             \
                 return NULL;                                                  \
               }),                                                             \
                                                                               \
      FUNCTION(NODE_TYPE *, LowerBoundNode,                                    \
               (NODE_TYPE * root, COMP_CLOSURE_TYPE cmp, KEY_TYPE key), {      \
                 NODE_TYPE *current = root;                                    \
                 NODE_TYPE *candidate = NULL;                                  \
                 while (current != NULL) {                                     \
                   CMPOUT r = CLOSURE_CALL(cmp, current->key, key);            \
                   if (r >= COMPARISON_CONST(EQUAL)) {                         \
                     candidate = current;                                      \
                     current = current->left;                                  \
                   } else {                                                    \
                     current = current->right;                                 \
                   }                                                           \
                 }                                                             \
                 return candidate;                                             \
               }),                                                             \
                                                                               \
      FUNCTION(NODE_TYPE *, UpperBoundNode,                                    \
               (NODE_TYPE * root, COMP_CLOSURE_TYPE cmp, KEY_TYPE key), {      \
                 NODE_TYPE *current = root;                                    \
                 NODE_TYPE *candidate = NULL;                                  \
                 while (current != NULL) {                                     \
                   CMPOUT r = CLOSURE_CALL(cmp, current->key, key);            \
                   if (r > COMPARISON_CONST(EQUAL)) {                          \
                     candidate = current;                                      \
                     current = current->left;                                  \
                   } else {                                                    \
                     current = current->right;                                 \
                   }                                                           \
                 }                                                             \
                 return candidate;                                             \
               }))

#define COSMERON_MACRO_INTERNAL_TREE_BIN_SET_ITER(TREE_TARGET, ITER_TARGET,    \
                                                  SUFFIX, NODE_TYPE, KEY_TYPE) \
  TEMPLATE_OBJECT(Tree, ITER_TARGET, SUFFIX,                                   \
      FIELDS(                                                                  \
          NODE_TYPE *root;                                                     \
          NODE_TYPE *current;                                                  \
          bool reverse;                                                        \
      ),                                                                       \
                                                                               \
      CLOSURE(PROCEDURE(Visitor, (const KEY_TYPE *key))),                      \
                                                                               \
      OPERATION_INLINE(                                                        \
          FromNode, (NODE_TYPE *root, NODE_TYPE *current, bool reverse), {     \
            if (!self)                                                         \
              return STATUS_CONST(INVALID_ARGUMENT);                           \
            MODULE_FUNC(Tree, ITER_TARGET, SUFFIX, Init)(self);                \
            self->root = root;                                                 \
            self->current = current;                                           \
            self->reverse = reverse;                                           \
            return STATUS_CONST(SUCCESS);                                      \
          }),                                                                  \
                                                                               \
      CONST_PREDICATE_INLINE(IsValid, (), {                                    \
        return self != NULL && self->current != NULL;                          \
      }),                                                                      \
                                                                               \
      CONST_PREDICATE_INLINE(HasNext, (), {                                    \
        return self != NULL && self->current != NULL;                          \
      }),                                                                      \
                                                                               \
      CONST_PREDICATE_INLINE(HasPrev, (), {                                    \
        if (!self)                                                             \
          return false;                                                        \
        if (self->current != NULL) {                                           \
          NODE_TYPE *prev =                                                    \
              self->reverse                                                    \
                  ? MODULE_IFUNC(Tree, NS(TREE_TARGET, SUFFIX), InOrderNext)(  \
                        self->current)                                         \
                  : MODULE_IFUNC(Tree, NS(TREE_TARGET, SUFFIX), InOrderPrev)(  \
                        self->current);                                        \
          return prev != NULL;                                                 \
        }                                                                      \
        return self->root != NULL;                                             \
      }),                                                                      \
                                                                               \
      OPERATION_INLINE(Reset, (), {                                            \
        if (!self)                                                             \
          return STATUS_CONST(INVALID_ARGUMENT);                               \
        self->current =                                                        \
            self->reverse                                                      \
                ? MODULE_IFUNC(Tree, NS(TREE_TARGET, SUFFIX), MaxNode)(        \
                      self->root)                                              \
                : MODULE_IFUNC(Tree, NS(TREE_TARGET, SUFFIX), MinNode)(        \
                      self->root);                                             \
        return STATUS_CONST(SUCCESS);                                          \
      }),                                                                      \
                                                                               \
      CONST_OPERATION_INLINE(Get, (const KEY_TYPE **outKey), {                 \
        if (!self || !outKey)                                                  \
          return STATUS_CONST(INVALID_ARGUMENT);                               \
        if (!self->current)                                                    \
          return STATUS_CONST(OUT_OF_RANGE);                                   \
        *outKey = &self->current->key;                                         \
        return STATUS_CONST(SUCCESS);                                          \
      }),                                                                      \
                                                                               \
      CONST_OPERATION_INLINE(ConstGet, (const KEY_TYPE **outKey), {            \
        return MODULE_FUNC(Tree, ITER_TARGET, SUFFIX, Get)(self, outKey);      \
      }),                                                                      \
                                                                               \
      CONST_OPERATION_INLINE(Key, (const KEY_TYPE **outKey), {                 \
        return MODULE_FUNC(Tree, ITER_TARGET, SUFFIX, Get)(self, outKey);      \
      }),                                                                      \
                                                                               \
      CONST_OPERATION_INLINE(Value, (KEY_TYPE *outKey), {                      \
        if (!self || !outKey)                                                  \
          return STATUS_CONST(INVALID_ARGUMENT);                               \
        if (!self->current)                                                    \
          return STATUS_CONST(OUT_OF_RANGE);                                   \
        *outKey = self->current->key;                                          \
        return STATUS_CONST(SUCCESS);                                          \
      }),                                                                      \
                                                                               \
      OPERATION_INLINE(StepNext, (), {                                         \
        if (!self)                                                             \
          return STATUS_CONST(INVALID_ARGUMENT);                               \
        if (!self->current)                                                    \
          return STATUS_CONST(OUT_OF_RANGE);                                   \
        self->current =                                                        \
            self->reverse                                                      \
                ? MODULE_IFUNC(Tree, NS(TREE_TARGET, SUFFIX), InOrderPrev)(    \
                      self->current)                                           \
                : MODULE_IFUNC(Tree, NS(TREE_TARGET, SUFFIX), InOrderNext)(    \
                      self->current);                                          \
        return STATUS_CONST(SUCCESS);                                          \
      }),                                                                      \
                                                                               \
      OPERATION_INLINE(StepPrev, (), {                                         \
        if (!self)                                                             \
          return STATUS_CONST(INVALID_ARGUMENT);                               \
        if (self->current == NULL) {                                           \
          if (!self->root)                                                     \
            return STATUS_CONST(OUT_OF_RANGE);                                 \
          self->current =                                                      \
              self->reverse                                                    \
                  ? MODULE_IFUNC(Tree, NS(TREE_TARGET, SUFFIX), MinNode)(      \
                        self->root)                                            \
                  : MODULE_IFUNC(Tree, NS(TREE_TARGET, SUFFIX), MaxNode)(      \
                        self->root);                                           \
          return STATUS_CONST(SUCCESS);                                        \
        }                                                                      \
        NODE_TYPE *prev =                                                      \
            self->reverse                                                      \
                ? MODULE_IFUNC(Tree, NS(TREE_TARGET, SUFFIX), InOrderNext)(    \
                      self->current)                                           \
                : MODULE_IFUNC(Tree, NS(TREE_TARGET, SUFFIX), InOrderPrev)(    \
                      self->current);                                          \
        if (!prev)                                                             \
          return STATUS_CONST(OUT_OF_RANGE);                                   \
        self->current = prev;                                                  \
        return STATUS_CONST(SUCCESS);                                          \
      }),                                                                      \
                                                                               \
      OPERATION_INLINE(Next, (const KEY_TYPE **outKey), {                      \
        if (!self)                                                             \
          return STATUS_CONST(INVALID_ARGUMENT);                               \
        if (!self->current)                                                    \
          return STATUS_CONST(OUT_OF_RANGE);                                   \
        if (outKey)                                                            \
          *outKey = &self->current->key;                                       \
        self->current =                                                        \
            self->reverse                                                      \
                ? MODULE_IFUNC(Tree, NS(TREE_TARGET, SUFFIX), InOrderPrev)(    \
                      self->current)                                           \
                : MODULE_IFUNC(Tree, NS(TREE_TARGET, SUFFIX), InOrderNext)(    \
                      self->current);                                          \
        return STATUS_CONST(SUCCESS);                                          \
      }),                                                                      \
                                                                               \
      OPERATION_INLINE(Prev, (const KEY_TYPE **outKey), {                      \
        OPSTATUS status =                                                      \
            MODULE_FUNC(Tree, ITER_TARGET, SUFFIX, StepPrev)(self);            \
        if (status != STATUS_CONST(SUCCESS))                                   \
          return status;                                                       \
        if (outKey)                                                            \
          *outKey = &self->current->key;                                       \
        return STATUS_CONST(SUCCESS);                                          \
      }),                                                                      \
                                                                               \
      CONST_PREDICATE_INLINE(                                                  \
          Equals, (const struct MODULE_TYPED(Tree, ITER_TARGET, SUFFIX) *other),\
          {                                                                    \
            if (!self || !other)                                               \
              return false;                                                    \
            return self->current == other->current &&                          \
                   self->reverse == other->reverse;                            \
          }),                                                                  \
                                                                               \
      OPERATION_INLINE(                                                        \
          ForEach,                                                             \
          (MODULE_CLOSURE(Tree, ITER_TARGET, SUFFIX, Visitor) visitor), {      \
            if (!self || !CLOSURE_IS_VALID(visitor))                           \
              return STATUS_CONST(INVALID_ARGUMENT);                           \
            while (self->current != NULL) {                                    \
              CLOSURE_CALL(visitor, &self->current->key);                      \
              self->current =                                                  \
                  self->reverse                                                \
                      ? MODULE_IFUNC(Tree, NS(TREE_TARGET, SUFFIX),            \
                                     InOrderPrev)(self->current)               \
                      : MODULE_IFUNC(Tree, NS(TREE_TARGET, SUFFIX),            \
                                     InOrderNext)(self->current);              \
            }                                                                  \
            return STATUS_CONST(SUCCESS);                                      \
          })                                                                   \
  )

#define COSMERON_MACRO_INTERNAL_TREE_BIN_MAP_ITER(TREE_TARGET, ITER_TARGET,    \
                                                  SUFFIX, NODE_TYPE, KEY_TYPE, \
                                                  VALUE_TYPE)                  \
  TEMPLATE_OBJECT(Tree, ITER_TARGET, SUFFIX,                                   \
      FIELDS(                                                                  \
          NODE_TYPE *root;                                                     \
          NODE_TYPE *current;                                                  \
          bool reverse;                                                        \
      ),                                                                       \
                                                                               \
      CLOSURE(PROCEDURE(Visitor, (KEY_TYPE key, VALUE_TYPE *value))),          \
                                                                               \
      OPERATION_INLINE(                                                        \
          FromNode, (NODE_TYPE *root, NODE_TYPE *current, bool reverse), {     \
            if (!self)                                                         \
              return STATUS_CONST(INVALID_ARGUMENT);                           \
            MODULE_FUNC(Tree, ITER_TARGET, SUFFIX, Init)(self);                \
            self->root = root;                                                 \
            self->current = current;                                           \
            self->reverse = reverse;                                           \
            return STATUS_CONST(SUCCESS);                                      \
          }),                                                                  \
                                                                               \
      CONST_PREDICATE_INLINE(IsValid, (), {                                    \
        return self != NULL && self->current != NULL;                          \
      }),                                                                      \
                                                                               \
      CONST_PREDICATE_INLINE(HasNext, (), {                                    \
        return self != NULL && self->current != NULL;                          \
      }),                                                                      \
                                                                               \
      CONST_PREDICATE_INLINE(HasPrev, (), {                                    \
        if (!self)                                                             \
          return false;                                                        \
        if (self->current != NULL) {                                           \
          NODE_TYPE *prev =                                                    \
              self->reverse                                                    \
                  ? MODULE_IFUNC(Tree, NS(TREE_TARGET, SUFFIX), InOrderNext)(  \
                        self->current)                                         \
                  : MODULE_IFUNC(Tree, NS(TREE_TARGET, SUFFIX), InOrderPrev)(  \
                        self->current);                                        \
          return prev != NULL;                                                 \
        }                                                                      \
        return self->root != NULL;                                             \
      }),                                                                      \
                                                                               \
      OPERATION_INLINE(Reset, (), {                                            \
        if (!self)                                                             \
          return STATUS_CONST(INVALID_ARGUMENT);                               \
        self->current =                                                        \
            self->reverse                                                      \
                ? MODULE_IFUNC(Tree, NS(TREE_TARGET, SUFFIX), MaxNode)(        \
                      self->root)                                              \
                : MODULE_IFUNC(Tree, NS(TREE_TARGET, SUFFIX), MinNode)(        \
                      self->root);                                             \
        return STATUS_CONST(SUCCESS);                                          \
      }),                                                                      \
                                                                               \
      OPERATION_INLINE(Get, (VALUE_TYPE **outValue), {                         \
        if (!self || !outValue)                                                \
          return STATUS_CONST(INVALID_ARGUMENT);                               \
        if (!self->current)                                                    \
          return STATUS_CONST(OUT_OF_RANGE);                                   \
        *outValue = &self->current->value;                                     \
        return STATUS_CONST(SUCCESS);                                          \
      }),                                                                      \
                                                                               \
      CONST_OPERATION_INLINE(ConstGet, (const VALUE_TYPE **outValue), {        \
        if (!self || !outValue)                                                \
          return STATUS_CONST(INVALID_ARGUMENT);                               \
        if (!self->current)                                                    \
          return STATUS_CONST(OUT_OF_RANGE);                                   \
        *outValue = &self->current->value;                                     \
        return STATUS_CONST(SUCCESS);                                          \
      }),                                                                      \
                                                                               \
      CONST_OPERATION_INLINE(Key, (const KEY_TYPE **outKey), {                 \
        if (!self || !outKey)                                                  \
          return STATUS_CONST(INVALID_ARGUMENT);                               \
        if (!self->current)                                                    \
          return STATUS_CONST(OUT_OF_RANGE);                                   \
        *outKey = &self->current->key;                                         \
        return STATUS_CONST(SUCCESS);                                          \
      }),                                                                      \
                                                                               \
      CONST_OPERATION_INLINE(Value, (VALUE_TYPE *outValue), {                  \
        if (!self || !outValue)                                                \
          return STATUS_CONST(INVALID_ARGUMENT);                               \
        if (!self->current)                                                    \
          return STATUS_CONST(OUT_OF_RANGE);                                   \
        *outValue = self->current->value;                                      \
        return STATUS_CONST(SUCCESS);                                          \
      }),                                                                      \
                                                                               \
      OPERATION_INLINE(StepNext, (), {                                         \
        if (!self)                                                             \
          return STATUS_CONST(INVALID_ARGUMENT);                               \
        if (!self->current)                                                    \
          return STATUS_CONST(OUT_OF_RANGE);                                   \
        self->current =                                                        \
            self->reverse                                                      \
                ? MODULE_IFUNC(Tree, NS(TREE_TARGET, SUFFIX), InOrderPrev)(    \
                      self->current)                                           \
                : MODULE_IFUNC(Tree, NS(TREE_TARGET, SUFFIX), InOrderNext)(    \
                      self->current);                                          \
        return STATUS_CONST(SUCCESS);                                          \
      }),                                                                      \
                                                                               \
      OPERATION_INLINE(StepPrev, (), {                                         \
        if (!self)                                                             \
          return STATUS_CONST(INVALID_ARGUMENT);                               \
        if (self->current == NULL) {                                           \
          if (!self->root)                                                     \
            return STATUS_CONST(OUT_OF_RANGE);                                 \
          self->current =                                                      \
              self->reverse                                                    \
                  ? MODULE_IFUNC(Tree, NS(TREE_TARGET, SUFFIX), MinNode)(      \
                        self->root)                                            \
                  : MODULE_IFUNC(Tree, NS(TREE_TARGET, SUFFIX), MaxNode)(      \
                        self->root);                                           \
          return STATUS_CONST(SUCCESS);                                        \
        }                                                                      \
        NODE_TYPE *prev =                                                      \
            self->reverse                                                      \
                ? MODULE_IFUNC(Tree, NS(TREE_TARGET, SUFFIX), InOrderNext)(    \
                      self->current)                                           \
                : MODULE_IFUNC(Tree, NS(TREE_TARGET, SUFFIX), InOrderPrev)(    \
                      self->current);                                          \
        if (!prev)                                                             \
          return STATUS_CONST(OUT_OF_RANGE);                                   \
        self->current = prev;                                                  \
        return STATUS_CONST(SUCCESS);                                          \
      }),                                                                      \
                                                                               \
      OPERATION_INLINE(Next, (VALUE_TYPE **outValue), {                        \
        if (!self)                                                             \
          return STATUS_CONST(INVALID_ARGUMENT);                               \
        if (!self->current)                                                    \
          return STATUS_CONST(OUT_OF_RANGE);                                   \
        if (outValue)                                                          \
          *outValue = &self->current->value;                                   \
        self->current =                                                        \
            self->reverse                                                      \
                ? MODULE_IFUNC(Tree, NS(TREE_TARGET, SUFFIX), InOrderPrev)(    \
                      self->current)                                           \
                : MODULE_IFUNC(Tree, NS(TREE_TARGET, SUFFIX), InOrderNext)(    \
                      self->current);                                          \
        return STATUS_CONST(SUCCESS);                                          \
      }),                                                                      \
                                                                               \
      OPERATION_INLINE(Prev, (VALUE_TYPE **outValue), {                        \
        OPSTATUS status =                                                      \
            MODULE_FUNC(Tree, ITER_TARGET, SUFFIX, StepPrev)(self);            \
        if (status != STATUS_CONST(SUCCESS))                                   \
          return status;                                                       \
        if (outValue)                                                          \
          *outValue = &self->current->value;                                   \
        return STATUS_CONST(SUCCESS);                                          \
      }),                                                                      \
                                                                               \
      CONST_PREDICATE_INLINE(                                                  \
          Equals, (const struct MODULE_TYPED(Tree, ITER_TARGET, SUFFIX) *other),\
          {                                                                    \
            if (!self || !other)                                               \
              return false;                                                    \
            return self->current == other->current &&                          \
                   self->reverse == other->reverse;                            \
          }),                                                                  \
                                                                               \
      OPERATION_INLINE(                                                        \
          ForEach,                                                             \
          (MODULE_CLOSURE(Tree, ITER_TARGET, SUFFIX, Visitor) visitor), {      \
            if (!self || !CLOSURE_IS_VALID(visitor))                           \
              return STATUS_CONST(INVALID_ARGUMENT);                           \
            while (self->current != NULL) {                                    \
              CLOSURE_CALL(visitor, self->current->key, &self->current->value);\
              self->current =                                                  \
                  self->reverse                                                \
                      ? MODULE_IFUNC(Tree, NS(TREE_TARGET, SUFFIX),            \
                                     InOrderPrev)(self->current)               \
                      : MODULE_IFUNC(Tree, NS(TREE_TARGET, SUFFIX),            \
                                     InOrderNext)(self->current);              \
            }                                                                  \
            return STATUS_CONST(SUCCESS);                                      \
          })                                                                   \
  )

/* ==========================================================================
 * Shared Binary Tree Traversal & Object Generators (BST / AVL / RedBlack)
 * ========================================================================== */

#define COSMERON_MACRO_INTERNAL_TREE_BIN_SET_TRAVERSE(TARGET, SUFFIX,          \
                                                      NODE_TYPE,               \
                                                      VISITOR_CLOSURE_TYPE)    \
  INTERNAL_DEF_EX(                                                             \
      Tree, NS(TARGET, Traverse, SUFFIX),                                      \
      PROCEDURE(PreOrder, (NODE_TYPE *node, VISITOR_CLOSURE_TYPE visitor), {   \
        if (!node)                                                             \
          return;                                                              \
        CLOSURE_CALL(visitor, &node->key);                                     \
        MODULE_IFUNC(Tree, NS(TARGET, Traverse, SUFFIX), PreOrder)(            \
            node->left, visitor);                                              \
        MODULE_IFUNC(Tree, NS(TARGET, Traverse, SUFFIX), PreOrder)(            \
            node->right, visitor);                                             \
      }),                                                                      \
      PROCEDURE(InOrder, (NODE_TYPE *node, VISITOR_CLOSURE_TYPE visitor), {    \
        if (!node)                                                             \
          return;                                                              \
        MODULE_IFUNC(Tree, NS(TARGET, Traverse, SUFFIX), InOrder)(             \
            node->left, visitor);                                              \
        CLOSURE_CALL(visitor, &node->key);                                     \
        MODULE_IFUNC(Tree, NS(TARGET, Traverse, SUFFIX), InOrder)(             \
            node->right, visitor);                                             \
      }),                                                                      \
      PROCEDURE(PostOrder, (NODE_TYPE *node, VISITOR_CLOSURE_TYPE visitor), {  \
        if (!node)                                                             \
          return;                                                              \
        MODULE_IFUNC(Tree, NS(TARGET, Traverse, SUFFIX), PostOrder)(           \
            node->left, visitor);                                              \
        MODULE_IFUNC(Tree, NS(TARGET, Traverse, SUFFIX), PostOrder)(           \
            node->right, visitor);                                             \
        CLOSURE_CALL(visitor, &node->key);                                     \
      }))

#define COSMERON_MACRO_INTERNAL_TREE_BIN_MAP_TRAVERSE(TARGET, SUFFIX,          \
                                                      NODE_TYPE,               \
                                                      VISITOR_CLOSURE_TYPE)    \
  INTERNAL_DEF_EX(                                                             \
      Tree, NS(TARGET, Traverse, SUFFIX),                                      \
      PROCEDURE(PreOrder, (NODE_TYPE *node, VISITOR_CLOSURE_TYPE visitor), {   \
        if (!node)                                                             \
          return;                                                              \
        CLOSURE_CALL(visitor, node->key, &node->value);                        \
        MODULE_IFUNC(Tree, NS(TARGET, Traverse, SUFFIX), PreOrder)(            \
            node->left, visitor);                                              \
        MODULE_IFUNC(Tree, NS(TARGET, Traverse, SUFFIX), PreOrder)(            \
            node->right, visitor);                                             \
      }),                                                                      \
      PROCEDURE(InOrder, (NODE_TYPE *node, VISITOR_CLOSURE_TYPE visitor), {    \
        if (!node)                                                             \
          return;                                                              \
        MODULE_IFUNC(Tree, NS(TARGET, Traverse, SUFFIX), InOrder)(             \
            node->left, visitor);                                              \
        CLOSURE_CALL(visitor, node->key, &node->value);                        \
        MODULE_IFUNC(Tree, NS(TARGET, Traverse, SUFFIX), InOrder)(             \
            node->right, visitor);                                             \
      }),                                                                      \
      PROCEDURE(PostOrder, (NODE_TYPE *node, VISITOR_CLOSURE_TYPE visitor), {  \
        if (!node)                                                             \
          return;                                                              \
        MODULE_IFUNC(Tree, NS(TARGET, Traverse, SUFFIX), PostOrder)(           \
            node->left, visitor);                                              \
        MODULE_IFUNC(Tree, NS(TARGET, Traverse, SUFFIX), PostOrder)(           \
            node->right, visitor);                                             \
        CLOSURE_CALL(visitor, node->key, &node->value);                        \
      }))

#define COSMERON_MACRO_INTERNAL_TREE_BIN_SET_OBJECT(TARGET, ITER_TARGET,       \
                                                    SUFFIX, NODE_TYPE,         \
                                                    KEY_TYPE)                  \
  TEMPLATE_OBJECT(Tree, TARGET, SUFFIX,                                        \
      FIELDS(                                                                  \
          NODE_TYPE *root;                                                     \
          size_t size;                                                         \
          MODULE_CLOSURE(Tree, TARGET, SUFFIX, Comparator) compare;            \
      ),                                                                       \
                                                                               \
      OPERATION_INLINE(                                                        \
          CreateWithComparator,                                                \
          (CMPOUT (*compareFn)(KEY_TYPE left, KEY_TYPE right, void *context),  \
           void *context),                                                     \
          {                                                                    \
            if (!self)                                                         \
              return STATUS_CONST(INVALID_ARGUMENT);                           \
            MODULE_FUNC(Tree, TARGET, SUFFIX, Init)(self);                     \
            self->root = NULL;                                                 \
            self->size = 0U;                                                   \
            if (compareFn) {                                                   \
              self->compare = NS_JOIN(                                         \
                  MODULE_CLOSURE_FUNC(Tree, TARGET, SUFFIX, Comparator),       \
                  Bind)(compareFn, context);                                   \
            } else {                                                           \
              self->compare = NS_JOIN(                                         \
                  MODULE_CLOSURE_FUNC(Tree, TARGET, SUFFIX, Comparator),       \
                  Bind)(                                                       \
                  MODULE_IFUNC(Tree, NS(TARGET, SUFFIX), DefaultCompare),      \
                  NULL);                                                       \
            }                                                                  \
            return STATUS_CONST(SUCCESS);                                      \
          }),                                                                  \
                                                                               \
      OPERATION_INLINE(Create, (), {                                           \
        return MODULE_FUNC(Tree, TARGET, SUFFIX, CreateWithComparator)(        \
            self, NULL, NULL);                                                 \
      }),                                                                      \
                                                                               \
      PROCEDURE_INLINE(Clear, (), {                                            \
        if (!self)                                                             \
          return;                                                              \
        MODULE_IFUNC(Tree, NS(TARGET, SUFFIX), DestroyNodes)(self->root);      \
        self->root = NULL;                                                     \
        self->size = 0U;                                                       \
      }),                                                                      \
                                                                               \
      OPERATION_INLINE(Dispose, (), {                                          \
        if (!self)                                                             \
          return STATUS_CONST(INVALID_ARGUMENT);                               \
        MODULE_FUNC(Tree, TARGET, SUFFIX, Clear)(self);                        \
        return STATUS_CONST(SUCCESS);                                          \
      }),                                                                      \
                                                                               \
      PROCEDURE_INLINE(Destroy, (), {                                          \
        (void)MODULE_FUNC(Tree, TARGET, SUFFIX, Dispose)(self);                \
      }),                                                                      \
                                                                               \
      OPERATION_INLINE(Insert, (KEY_TYPE key), {                               \
        if (!self)                                                             \
          return STATUS_CONST(INVALID_ARGUMENT);                               \
        return MODULE_IFUNC(Tree, NS(TARGET, Algo, SUFFIX), InsertSet)(        \
            &self->root, &self->size, self->compare, key);                     \
      }),                                                                      \
                                                                               \
      OPERATION_INLINE(Remove, (KEY_TYPE key), {                               \
        if (!self)                                                             \
          return STATUS_CONST(INVALID_ARGUMENT);                               \
        NODE_TYPE *target = MODULE_IFUNC(Tree, NS(TARGET, SUFFIX), FindNode)(  \
            self->root, self->compare, key);                                   \
        if (!target)                                                           \
          return STATUS_CONST(NOT_FOUND);                                      \
        MODULE_IFUNC(Tree, NS(TARGET, Algo, SUFFIX), RemoveNode)(              \
            &self->root, &self->size, target);                                 \
        return STATUS_CONST(SUCCESS);                                          \
      }),                                                                      \
                                                                               \
      OPERATION_INLINE(                                                        \
          EraseIter,                                                           \
          (struct MODULE_TYPED(Tree, ITER_TARGET, SUFFIX) *iter), {            \
            if (!self || !iter || !iter->current)                              \
              return STATUS_CONST(INVALID_ARGUMENT);                           \
            NODE_TYPE *target = iter->current;                                 \
            NODE_TYPE *nextInOrder =                                           \
                iter->reverse                                                  \
                    ? MODULE_IFUNC(Tree, NS(TARGET, SUFFIX), InOrderPrev)(     \
                          target)                                              \
                    : MODULE_IFUNC(Tree, NS(TARGET, SUFFIX), InOrderNext)(     \
                          target);                                             \
            MODULE_IFUNC(Tree, NS(TARGET, Algo, SUFFIX), RemoveNode)(          \
                &self->root, &self->size, target);                             \
            iter->root = self->root;                                           \
            iter->current = nextInOrder;                                       \
            return STATUS_CONST(SUCCESS);                                      \
          }),                                                                  \
                                                                               \
      CONST_OPERATION_INLINE(Find, (KEY_TYPE key, const KEY_TYPE **outKey), {  \
        if (!self || !outKey)                                                  \
          return STATUS_CONST(INVALID_ARGUMENT);                               \
        NODE_TYPE *node = MODULE_IFUNC(Tree, NS(TARGET, SUFFIX), FindNode)(    \
            self->root, self->compare, key);                                   \
        if (!node)                                                             \
          return STATUS_CONST(NOT_FOUND);                                      \
        *outKey = &node->key;                                                  \
        return STATUS_CONST(SUCCESS);                                          \
      }),                                                                      \
                                                                               \
      CONST_PREDICATE_INLINE(Contains, (KEY_TYPE key), {                       \
        if (!self)                                                             \
          return false;                                                        \
        return MODULE_IFUNC(Tree, NS(TARGET, SUFFIX), FindNode)(               \
                   self->root, self->compare, key) != NULL;                    \
      }),                                                                      \
                                                                               \
      CONST_OPERATION_INLINE(Min, (const KEY_TYPE **outKey), {                 \
        if (!self || !outKey)                                                  \
          return STATUS_CONST(INVALID_ARGUMENT);                               \
        NODE_TYPE *node =                                                      \
            MODULE_IFUNC(Tree, NS(TARGET, SUFFIX), MinNode)(self->root);       \
        if (!node)                                                             \
          return STATUS_CONST(NOT_FOUND);                                      \
        *outKey = &node->key;                                                  \
        return STATUS_CONST(SUCCESS);                                          \
      }),                                                                      \
                                                                               \
      CONST_OPERATION_INLINE(Max, (const KEY_TYPE **outKey), {                 \
        if (!self || !outKey)                                                  \
          return STATUS_CONST(INVALID_ARGUMENT);                               \
        NODE_TYPE *node =                                                      \
            MODULE_IFUNC(Tree, NS(TARGET, SUFFIX), MaxNode)(self->root);       \
        if (!node)                                                             \
          return STATUS_CONST(NOT_FOUND);                                      \
        *outKey = &node->key;                                                  \
        return STATUS_CONST(SUCCESS);                                          \
      }),                                                                      \
                                                                               \
      CONST_PREDICATE_INLINE(Empty, (), {                                      \
        return !self || self->size == 0U;                                      \
      }),                                                                      \
                                                                               \
      CONST_PREDICATE_INLINE(IsEmpty, (), {                                    \
        return !self || self->size == 0U;                                      \
      }),                                                                      \
                                                                               \
      CONST_FUNCTION_INLINE(size_t, Size, (), {                                \
        return self ? self->size : 0U;                                         \
      }),                                                                      \
                                                                               \
      CONST_FUNCTION_INLINE(size_t, Count, (), {                               \
        return self ? self->size : 0U;                                         \
      }),                                                                      \
                                                                               \
      CONST_FUNCTION_INLINE(size_t, Height, (), {                              \
        if (!self)                                                             \
          return 0U;                                                           \
        return MODULE_IFUNC(Tree, NS(TARGET, Algo, SUFFIX), NodeHeight)(       \
            self->root);                                                       \
      }),                                                                      \
                                                                               \
      /* --- Optional Iterator Protocol --- */                                 \
      CONST_OPERATION_INLINE(                                                  \
          Begin, (struct MODULE_TYPED(Tree, ITER_TARGET, SUFFIX) *outIter), {  \
            if (!self || !outIter)                                             \
              return STATUS_CONST(INVALID_ARGUMENT);                           \
            return MODULE_FUNC(Tree, ITER_TARGET, SUFFIX, FromNode)(           \
                outIter, self->root,                                           \
                MODULE_IFUNC(Tree, NS(TARGET, SUFFIX), MinNode)(self->root),   \
                false);                                                        \
          }),                                                                  \
                                                                               \
      CONST_OPERATION_INLINE(                                                  \
          End, (struct MODULE_TYPED(Tree, ITER_TARGET, SUFFIX) *outIter), {    \
            if (!self || !outIter)                                             \
              return STATUS_CONST(INVALID_ARGUMENT);                           \
            return MODULE_FUNC(Tree, ITER_TARGET, SUFFIX, FromNode)(           \
                outIter, self->root, NULL, false);                             \
          }),                                                                  \
                                                                               \
      CONST_OPERATION_INLINE(                                                  \
          RBegin, (struct MODULE_TYPED(Tree, ITER_TARGET, SUFFIX) *outIter), { \
            if (!self || !outIter)                                             \
              return STATUS_CONST(INVALID_ARGUMENT);                           \
            return MODULE_FUNC(Tree, ITER_TARGET, SUFFIX, FromNode)(           \
                outIter, self->root,                                           \
                MODULE_IFUNC(Tree, NS(TARGET, SUFFIX), MaxNode)(self->root),   \
                true);                                                         \
          }),                                                                  \
                                                                               \
      CONST_OPERATION_INLINE(                                                  \
          REnd, (struct MODULE_TYPED(Tree, ITER_TARGET, SUFFIX) *outIter), {   \
            if (!self || !outIter)                                             \
              return STATUS_CONST(INVALID_ARGUMENT);                           \
            return MODULE_FUNC(Tree, ITER_TARGET, SUFFIX, FromNode)(           \
                outIter, self->root, NULL, true);                              \
          }),                                                                  \
                                                                               \
      CONST_OPERATION_INLINE(                                                  \
          FindIter,                                                            \
          (KEY_TYPE key,                                                       \
           struct MODULE_TYPED(Tree, ITER_TARGET, SUFFIX) *outIter),           \
          {                                                                    \
            if (!self || !outIter)                                             \
              return STATUS_CONST(INVALID_ARGUMENT);                           \
            NODE_TYPE *node =                                                  \
                MODULE_IFUNC(Tree, NS(TARGET, SUFFIX), FindNode)(              \
                    self->root, self->compare, key);                           \
            MODULE_FUNC(Tree, ITER_TARGET, SUFFIX, FromNode)(                  \
                outIter, self->root, node, false);                             \
            return node ? STATUS_CONST(SUCCESS) : STATUS_CONST(NOT_FOUND);     \
          }),                                                                  \
                                                                               \
      CONST_OPERATION_INLINE(                                                  \
          LowerBound,                                                          \
          (KEY_TYPE key,                                                       \
           struct MODULE_TYPED(Tree, ITER_TARGET, SUFFIX) *outIter),           \
          {                                                                    \
            if (!self || !outIter)                                             \
              return STATUS_CONST(INVALID_ARGUMENT);                           \
            NODE_TYPE *node =                                                  \
                MODULE_IFUNC(Tree, NS(TARGET, SUFFIX), LowerBoundNode)(        \
                    self->root, self->compare, key);                           \
            return MODULE_FUNC(Tree, ITER_TARGET, SUFFIX, FromNode)(           \
                outIter, self->root, node, false);                             \
          }),                                                                  \
                                                                               \
      CONST_OPERATION_INLINE(                                                  \
          UpperBound,                                                          \
          (KEY_TYPE key,                                                       \
           struct MODULE_TYPED(Tree, ITER_TARGET, SUFFIX) *outIter),           \
          {                                                                    \
            if (!self || !outIter)                                             \
              return STATUS_CONST(INVALID_ARGUMENT);                           \
            NODE_TYPE *node =                                                  \
                MODULE_IFUNC(Tree, NS(TARGET, SUFFIX), UpperBoundNode)(        \
                    self->root, self->compare, key);                           \
            return MODULE_FUNC(Tree, ITER_TARGET, SUFFIX, FromNode)(           \
                outIter, self->root, node, false);                             \
          }),                                                                  \
                                                                               \
      CONST_OPERATION_INLINE(                                                  \
          ForEach,                                                             \
          (MODULE_ENUM(Tree, Traversal) order,                                 \
           MODULE_CLOSURE(Tree, TARGET, SUFFIX, Visitor) visitor),             \
          {                                                                    \
            if (!self || !CLOSURE_IS_VALID(visitor))                           \
              return STATUS_CONST(INVALID_ARGUMENT);                           \
            switch (order) {                                                   \
            case Tree_Traversal_IN_ORDER:                                      \
              MODULE_IFUNC(Tree, NS(TARGET, Traverse, SUFFIX), InOrder)(       \
                  self->root, visitor);                                        \
              return STATUS_CONST(SUCCESS);                                    \
            case Tree_Traversal_PRE_ORDER:                                     \
              MODULE_IFUNC(Tree, NS(TARGET, Traverse, SUFFIX), PreOrder)(      \
                  self->root, visitor);                                        \
              return STATUS_CONST(SUCCESS);                                    \
            case Tree_Traversal_POST_ORDER:                                    \
              MODULE_IFUNC(Tree, NS(TARGET, Traverse, SUFFIX), PostOrder)(     \
                  self->root, visitor);                                        \
              return STATUS_CONST(SUCCESS);                                    \
            default:                                                           \
              return STATUS_CONST(INVALID_ARGUMENT);                           \
            }                                                                  \
          })                                                                   \
  )

#define COSMERON_MACRO_INTERNAL_TREE_BIN_MAP_OBJECT(TARGET, ITER_TARGET,       \
                                                    SUFFIX, NODE_TYPE,         \
                                                    KEY_TYPE, VALUE_TYPE)      \
  TEMPLATE_OBJECT(Tree, TARGET, SUFFIX,                                        \
      FIELDS(                                                                  \
          NODE_TYPE *root;                                                     \
          size_t size;                                                         \
          MODULE_CLOSURE(Tree, TARGET, SUFFIX, Comparator) compare;            \
      ),                                                                       \
                                                                               \
      OPERATION_INLINE(                                                        \
          CreateWithComparator,                                                \
          (CMPOUT (*compareFn)(KEY_TYPE left, KEY_TYPE right, void *context),  \
           void *context),                                                     \
          {                                                                    \
            if (!self)                                                         \
              return STATUS_CONST(INVALID_ARGUMENT);                           \
            MODULE_FUNC(Tree, TARGET, SUFFIX, Init)(self);                     \
            self->root = NULL;                                                 \
            self->size = 0U;                                                   \
            if (compareFn) {                                                   \
              self->compare = NS_JOIN(                                         \
                  MODULE_CLOSURE_FUNC(Tree, TARGET, SUFFIX, Comparator),       \
                  Bind)(compareFn, context);                                   \
            } else {                                                           \
              self->compare = NS_JOIN(                                         \
                  MODULE_CLOSURE_FUNC(Tree, TARGET, SUFFIX, Comparator),       \
                  Bind)(                                                       \
                  MODULE_IFUNC(Tree, NS(TARGET, SUFFIX), DefaultCompare),      \
                  NULL);                                                       \
            }                                                                  \
            return STATUS_CONST(SUCCESS);                                      \
          }),                                                                  \
                                                                               \
      OPERATION_INLINE(Create, (), {                                           \
        return MODULE_FUNC(Tree, TARGET, SUFFIX, CreateWithComparator)(        \
            self, NULL, NULL);                                                 \
      }),                                                                      \
                                                                               \
      PROCEDURE_INLINE(Clear, (), {                                            \
        if (!self)                                                             \
          return;                                                              \
        MODULE_IFUNC(Tree, NS(TARGET, SUFFIX), DestroyNodes)(self->root);      \
        self->root = NULL;                                                     \
        self->size = 0U;                                                       \
      }),                                                                      \
                                                                               \
      OPERATION_INLINE(Dispose, (), {                                          \
        if (!self)                                                             \
          return STATUS_CONST(INVALID_ARGUMENT);                               \
        MODULE_FUNC(Tree, TARGET, SUFFIX, Clear)(self);                        \
        return STATUS_CONST(SUCCESS);                                          \
      }),                                                                      \
                                                                               \
      PROCEDURE_INLINE(Destroy, (), {                                          \
        (void)MODULE_FUNC(Tree, TARGET, SUFFIX, Dispose)(self);                \
      }),                                                                      \
                                                                               \
      OPERATION_INLINE(Insert, (KEY_TYPE key, VALUE_TYPE value), {             \
        if (!self)                                                             \
          return STATUS_CONST(INVALID_ARGUMENT);                               \
        return MODULE_IFUNC(Tree, NS(TARGET, Algo, SUFFIX), InsertMap)(        \
            &self->root, &self->size, self->compare, key, value, false);       \
      }),                                                                      \
                                                                               \
      OPERATION_INLINE(Upsert, (KEY_TYPE key, VALUE_TYPE value), {             \
        if (!self)                                                             \
          return STATUS_CONST(INVALID_ARGUMENT);                               \
        return MODULE_IFUNC(Tree, NS(TARGET, Algo, SUFFIX), InsertMap)(        \
            &self->root, &self->size, self->compare, key, value, true);        \
      }),                                                                      \
                                                                               \
      OPERATION_INLINE(Remove, (KEY_TYPE key), {                               \
        if (!self)                                                             \
          return STATUS_CONST(INVALID_ARGUMENT);                               \
        NODE_TYPE *target = MODULE_IFUNC(Tree, NS(TARGET, SUFFIX), FindNode)(  \
            self->root, self->compare, key);                                   \
        if (!target)                                                           \
          return STATUS_CONST(NOT_FOUND);                                      \
        MODULE_IFUNC(Tree, NS(TARGET, Algo, SUFFIX), RemoveNode)(              \
            &self->root, &self->size, target);                                 \
        return STATUS_CONST(SUCCESS);                                          \
      }),                                                                      \
                                                                               \
      OPERATION_INLINE(                                                        \
          EraseIter,                                                           \
          (struct MODULE_TYPED(Tree, ITER_TARGET, SUFFIX) *iter), {            \
            if (!self || !iter || !iter->current)                              \
              return STATUS_CONST(INVALID_ARGUMENT);                           \
            NODE_TYPE *target = iter->current;                                 \
            NODE_TYPE *nextInOrder =                                           \
                iter->reverse                                                  \
                    ? MODULE_IFUNC(Tree, NS(TARGET, SUFFIX), InOrderPrev)(     \
                          target)                                              \
                    : MODULE_IFUNC(Tree, NS(TARGET, SUFFIX), InOrderNext)(     \
                          target);                                             \
            MODULE_IFUNC(Tree, NS(TARGET, Algo, SUFFIX), RemoveNode)(          \
                &self->root, &self->size, target);                             \
            iter->root = self->root;                                           \
            iter->current = nextInOrder;                                       \
            return STATUS_CONST(SUCCESS);                                      \
          }),                                                                  \
                                                                               \
      OPERATION_INLINE(Find, (KEY_TYPE key, VALUE_TYPE **outValue), {          \
        if (!self || !outValue)                                                \
          return STATUS_CONST(INVALID_ARGUMENT);                               \
        NODE_TYPE *node = MODULE_IFUNC(Tree, NS(TARGET, SUFFIX), FindNode)(    \
            self->root, self->compare, key);                                   \
        if (!node)                                                             \
          return STATUS_CONST(NOT_FOUND);                                      \
        *outValue = &node->value;                                              \
        return STATUS_CONST(SUCCESS);                                          \
      }),                                                                      \
                                                                               \
      CONST_OPERATION_INLINE(                                                  \
          ConstFind, (KEY_TYPE key, const VALUE_TYPE **outValue), {            \
            if (!self || !outValue)                                            \
              return STATUS_CONST(INVALID_ARGUMENT);                           \
            NODE_TYPE *node =                                                  \
                MODULE_IFUNC(Tree, NS(TARGET, SUFFIX), FindNode)(              \
                    self->root, self->compare, key);                           \
            if (!node)                                                         \
              return STATUS_CONST(NOT_FOUND);                                  \
            *outValue = &node->value;                                          \
            return STATUS_CONST(SUCCESS);                                      \
          }),                                                                  \
                                                                               \
      CONST_PREDICATE_INLINE(Contains, (KEY_TYPE key), {                       \
        if (!self)                                                             \
          return false;                                                        \
        return MODULE_IFUNC(Tree, NS(TARGET, SUFFIX), FindNode)(               \
                   self->root, self->compare, key) != NULL;                    \
      }),                                                                      \
                                                                               \
      CONST_OPERATION_INLINE(MinKey, (const KEY_TYPE **outKey), {              \
        if (!self || !outKey)                                                  \
          return STATUS_CONST(INVALID_ARGUMENT);                               \
        NODE_TYPE *node =                                                      \
            MODULE_IFUNC(Tree, NS(TARGET, SUFFIX), MinNode)(self->root);       \
        if (!node)                                                             \
          return STATUS_CONST(NOT_FOUND);                                      \
        *outKey = &node->key;                                                  \
        return STATUS_CONST(SUCCESS);                                          \
      }),                                                                      \
                                                                               \
      CONST_OPERATION_INLINE(MaxKey, (const KEY_TYPE **outKey), {              \
        if (!self || !outKey)                                                  \
          return STATUS_CONST(INVALID_ARGUMENT);                               \
        NODE_TYPE *node =                                                      \
            MODULE_IFUNC(Tree, NS(TARGET, SUFFIX), MaxNode)(self->root);       \
        if (!node)                                                             \
          return STATUS_CONST(NOT_FOUND);                                      \
        *outKey = &node->key;                                                  \
        return STATUS_CONST(SUCCESS);                                          \
      }),                                                                      \
                                                                               \
      CONST_PREDICATE_INLINE(Empty, (), {                                      \
        return !self || self->size == 0U;                                      \
      }),                                                                      \
                                                                               \
      CONST_PREDICATE_INLINE(IsEmpty, (), {                                    \
        return !self || self->size == 0U;                                      \
      }),                                                                      \
                                                                               \
      CONST_FUNCTION_INLINE(size_t, Size, (), {                                \
        return self ? self->size : 0U;                                         \
      }),                                                                      \
                                                                               \
      CONST_FUNCTION_INLINE(size_t, Count, (), {                               \
        return self ? self->size : 0U;                                         \
      }),                                                                      \
                                                                               \
      CONST_FUNCTION_INLINE(size_t, Height, (), {                              \
        if (!self)                                                             \
          return 0U;                                                           \
        return MODULE_IFUNC(Tree, NS(TARGET, Algo, SUFFIX), NodeHeight)(       \
            self->root);                                                       \
      }),                                                                      \
                                                                               \
      /* --- Optional Iterator Protocol --- */                                 \
      OPERATION_INLINE(                                                        \
          Begin, (struct MODULE_TYPED(Tree, ITER_TARGET, SUFFIX) *outIter), {  \
            if (!self || !outIter)                                             \
              return STATUS_CONST(INVALID_ARGUMENT);                           \
            return MODULE_FUNC(Tree, ITER_TARGET, SUFFIX, FromNode)(           \
                outIter, self->root,                                           \
                MODULE_IFUNC(Tree, NS(TARGET, SUFFIX), MinNode)(self->root),   \
                false);                                                        \
          }),                                                                  \
                                                                               \
      OPERATION_INLINE(                                                        \
          End, (struct MODULE_TYPED(Tree, ITER_TARGET, SUFFIX) *outIter), {    \
            if (!self || !outIter)                                             \
              return STATUS_CONST(INVALID_ARGUMENT);                           \
            return MODULE_FUNC(Tree, ITER_TARGET, SUFFIX, FromNode)(           \
                outIter, self->root, NULL, false);                             \
          }),                                                                  \
                                                                               \
      OPERATION_INLINE(                                                        \
          RBegin, (struct MODULE_TYPED(Tree, ITER_TARGET, SUFFIX) *outIter), { \
            if (!self || !outIter)                                             \
              return STATUS_CONST(INVALID_ARGUMENT);                           \
            return MODULE_FUNC(Tree, ITER_TARGET, SUFFIX, FromNode)(           \
                outIter, self->root,                                           \
                MODULE_IFUNC(Tree, NS(TARGET, SUFFIX), MaxNode)(self->root),   \
                true);                                                         \
          }),                                                                  \
                                                                               \
      OPERATION_INLINE(                                                        \
          REnd, (struct MODULE_TYPED(Tree, ITER_TARGET, SUFFIX) *outIter), {   \
            if (!self || !outIter)                                             \
              return STATUS_CONST(INVALID_ARGUMENT);                           \
            return MODULE_FUNC(Tree, ITER_TARGET, SUFFIX, FromNode)(           \
                outIter, self->root, NULL, true);                              \
          }),                                                                  \
                                                                               \
      OPERATION_INLINE(                                                        \
          FindIter,                                                            \
          (KEY_TYPE key,                                                       \
           struct MODULE_TYPED(Tree, ITER_TARGET, SUFFIX) *outIter),           \
          {                                                                    \
            if (!self || !outIter)                                             \
              return STATUS_CONST(INVALID_ARGUMENT);                           \
            NODE_TYPE *node =                                                  \
                MODULE_IFUNC(Tree, NS(TARGET, SUFFIX), FindNode)(              \
                    self->root, self->compare, key);                           \
            MODULE_FUNC(Tree, ITER_TARGET, SUFFIX, FromNode)(                  \
                outIter, self->root, node, false);                             \
            return node ? STATUS_CONST(SUCCESS) : STATUS_CONST(NOT_FOUND);     \
          }),                                                                  \
                                                                               \
      OPERATION_INLINE(                                                        \
          LowerBound,                                                          \
          (KEY_TYPE key,                                                       \
           struct MODULE_TYPED(Tree, ITER_TARGET, SUFFIX) *outIter),           \
          {                                                                    \
            if (!self || !outIter)                                             \
              return STATUS_CONST(INVALID_ARGUMENT);                           \
            NODE_TYPE *node =                                                  \
                MODULE_IFUNC(Tree, NS(TARGET, SUFFIX), LowerBoundNode)(        \
                    self->root, self->compare, key);                           \
            return MODULE_FUNC(Tree, ITER_TARGET, SUFFIX, FromNode)(           \
                outIter, self->root, node, false);                             \
          }),                                                                  \
                                                                               \
      OPERATION_INLINE(                                                        \
          UpperBound,                                                          \
          (KEY_TYPE key,                                                       \
           struct MODULE_TYPED(Tree, ITER_TARGET, SUFFIX) *outIter),           \
          {                                                                    \
            if (!self || !outIter)                                             \
              return STATUS_CONST(INVALID_ARGUMENT);                           \
            NODE_TYPE *node =                                                  \
                MODULE_IFUNC(Tree, NS(TARGET, SUFFIX), UpperBoundNode)(        \
                    self->root, self->compare, key);                           \
            return MODULE_FUNC(Tree, ITER_TARGET, SUFFIX, FromNode)(           \
                outIter, self->root, node, false);                             \
          }),                                                                  \
                                                                               \
      OPERATION_INLINE(                                                        \
          ForEach,                                                             \
          (MODULE_ENUM(Tree, Traversal) order,                                 \
           MODULE_CLOSURE(Tree, TARGET, SUFFIX, Visitor) visitor),             \
          {                                                                    \
            if (!self || !CLOSURE_IS_VALID(visitor))                           \
              return STATUS_CONST(INVALID_ARGUMENT);                           \
            switch (order) {                                                   \
            case Tree_Traversal_IN_ORDER:                                      \
              MODULE_IFUNC(Tree, NS(TARGET, Traverse, SUFFIX), InOrder)(       \
                  self->root, visitor);                                        \
              return STATUS_CONST(SUCCESS);                                    \
            case Tree_Traversal_PRE_ORDER:                                     \
              MODULE_IFUNC(Tree, NS(TARGET, Traverse, SUFFIX), PreOrder)(      \
                  self->root, visitor);                                        \
              return STATUS_CONST(SUCCESS);                                    \
            case Tree_Traversal_POST_ORDER:                                    \
              MODULE_IFUNC(Tree, NS(TARGET, Traverse, SUFFIX), PostOrder)(     \
                  self->root, visitor);                                        \
              return STATUS_CONST(SUCCESS);                                    \
            default:                                                           \
              return STATUS_CONST(INVALID_ARGUMENT);                           \
            }                                                                  \
          })                                                                   \
  )

#endif /* COSMERON_CONTAINER_TREE_COMMON_INC */
