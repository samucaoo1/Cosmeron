#include "Test.h"
#include "../../Cosmeron/Modules/Container/Tree/BST.h"
#include "../../Cosmeron/Modules/Container/Tree/AVL.h"
#include "../../Cosmeron/Modules/Container/Tree/RedBlack.h"

#define TEST_SET(ALGO, KEY, A, B, C)                                          \
  do {                                                                         \
    TTREE_PUBLIC_SET_TYPE(ALGO, KEY) tree;                                     \
    TTREE_NODE(TTREE_SET_TYPE(ALGO, KEY)) *node;                               \
    const TTREE_NODE(TTREE_SET_TYPE(ALGO, KEY)) *cnode;                        \
    KEY *found = NULL;                                                         \
    KEY *minimum = NULL;                                                       \
    KEY *maximum = NULL;                        \
    TEST_ASSERT(TTREE_FN(TTREE_SET_TYPE(ALGO, KEY), Init)(&tree) == STATUS_CONST(SUCCESS)); \
    TEST_ASSERT(TTREE_FN(TTREE_SET_TYPE(ALGO, KEY), Empty)(&tree));            \
    TEST_ASSERT(TTREE_FN(TTREE_SET_TYPE(ALGO, KEY), Insert)(&tree, (KEY)(B)) == STATUS_CONST(SUCCESS)); \
    TEST_ASSERT(TTREE_FN(TTREE_SET_TYPE(ALGO, KEY), Insert)(&tree, (KEY)(A)) == STATUS_CONST(SUCCESS)); \
    TEST_ASSERT(TTREE_FN(TTREE_SET_TYPE(ALGO, KEY), Insert)(&tree, (KEY)(C)) == STATUS_CONST(SUCCESS)); \
    TEST_ASSERT(TTREE_FN(TTREE_SET_TYPE(ALGO, KEY), Size)(&tree) == 3U);       \
    node = TTREE_FN(TTREE_SET_TYPE(ALGO, KEY), FindNode)(&tree, (KEY)(B));     \
    TEST_ASSERT(node != NULL);                                                 \
    TEST_ASSERT(TTREE_FN(TTREE_SET_TYPE(ALGO, KEY), Find)(                     \
                    &tree, (KEY)(B), &found) == STATUS_CONST(SUCCESS));        \
    TEST_ASSERT(found != NULL && *found == (KEY)(B));                          \
    TEST_ASSERT(TTREE_FN(TTREE_SET_TYPE(ALGO, KEY), Contains)(&tree, (KEY)(B))); \
    TEST_ASSERT(TTREE_FN(TTREE_SET_TYPE(ALGO, KEY), MinNode)(tree.root) != NULL); \
    TEST_ASSERT(TTREE_FN(TTREE_SET_TYPE(ALGO, KEY), MaxNode)(tree.root) != NULL); \
    TEST_ASSERT(TTREE_FN(TTREE_SET_TYPE(ALGO, KEY), Min)(                      \
                    &tree, &minimum) == STATUS_CONST(SUCCESS));                \
    TEST_ASSERT(TTREE_FN(TTREE_SET_TYPE(ALGO, KEY), Max)(                      \
                    &tree, &maximum) == STATUS_CONST(SUCCESS));                \
    TEST_ASSERT(*minimum == (KEY)(A));                                         \
    TEST_ASSERT(*maximum == (KEY)(C)); \
    node = TTREE_FN(TTREE_SET_TYPE(ALGO, KEY), Begin)(&tree);                  \
    TEST_ASSERT(node != NULL);                                                 \
    TEST_ASSERT(TTREE_FN(TTREE_SET_TYPE(ALGO, KEY), End)(&tree) == NULL);      \
    TEST_ASSERT(TTREE_FN(TTREE_SET_TYPE(ALGO, KEY), Next)(node) != NULL);      \
    node = TTREE_FN(TTREE_SET_TYPE(ALGO, KEY), MaxNode)(tree.root);            \
    TEST_ASSERT(TTREE_FN(TTREE_SET_TYPE(ALGO, KEY), Prev)(node) != NULL);      \
    cnode = TTREE_FN(TTREE_SET_TYPE(ALGO, KEY), ConstBegin)(&tree);            \
    TEST_ASSERT(cnode != NULL);                                                \
    TEST_ASSERT(TTREE_FN(TTREE_SET_TYPE(ALGO, KEY), ConstEnd)(&tree) == NULL); \
    TEST_ASSERT(TTREE_FN(TTREE_SET_TYPE(ALGO, KEY), ConstNext)(cnode) != NULL); \
    cnode = TTREE_FN(TTREE_SET_TYPE(ALGO, KEY), MaxNode)(tree.root);           \
    TEST_ASSERT(TTREE_FN(TTREE_SET_TYPE(ALGO, KEY), ConstPrev)(cnode) != NULL); \
    TEST_ASSERT(TTREE_FN(TTREE_SET_TYPE(ALGO, KEY), Remove)(&tree, (KEY)(B)) == STATUS_CONST(SUCCESS)); \
    TTREE_FN(TTREE_SET_TYPE(ALGO, KEY), Clear)(&tree);                         \
    TEST_ASSERT(TTREE_FN(TTREE_SET_TYPE(ALGO, KEY), Empty)(&tree));            \
    TEST_ASSERT(TTREE_FN(TTREE_SET_TYPE(ALGO, KEY), Insert)(&tree, (KEY)(A)) == STATUS_CONST(SUCCESS)); \
    TTREE_FN(TTREE_SET_TYPE(ALGO, KEY), Destroy)(&tree);                       \
  } while (0)

#define TEST_MAP(ALGO, KEY, VALUE, A, B, C)                                   \
  do {                                                                         \
    TTREE_PUBLIC_MAP_TYPE(ALGO, KEY, VALUE) tree;                              \
    TTREE_NODE(TTREE_MAP_TYPE(ALGO, KEY, VALUE)) *node;                        \
    const TTREE_NODE(TTREE_MAP_TYPE(ALGO, KEY, VALUE)) *cnode;                 \
    VALUE *found = NULL;                                                       \
    KEY *minimum = NULL;                                                       \
    KEY *maximum = NULL;                 \
    TEST_ASSERT(TTREE_FN(TTREE_MAP_TYPE(ALGO, KEY, VALUE), Init)(&tree) == STATUS_CONST(SUCCESS)); \
    TEST_ASSERT(TTREE_FN(TTREE_MAP_TYPE(ALGO, KEY, VALUE), Empty)(&tree));     \
    TEST_ASSERT(TTREE_FN(TTREE_MAP_TYPE(ALGO, KEY, VALUE), Insert)(&tree, (KEY)2, (VALUE)(B)) == STATUS_CONST(SUCCESS)); \
    TEST_ASSERT(TTREE_FN(TTREE_MAP_TYPE(ALGO, KEY, VALUE), Insert)(&tree, (KEY)1, (VALUE)(A)) == STATUS_CONST(SUCCESS)); \
    TEST_ASSERT(TTREE_FN(TTREE_MAP_TYPE(ALGO, KEY, VALUE), Insert)(&tree, (KEY)3, (VALUE)(C)) == STATUS_CONST(SUCCESS)); \
    TEST_ASSERT(TTREE_FN(TTREE_MAP_TYPE(ALGO, KEY, VALUE), Size)(&tree) == 3U); \
    node = TTREE_FN(TTREE_MAP_TYPE(ALGO, KEY, VALUE), FindNode)(&tree, (KEY)2); \
    TEST_ASSERT(node != NULL);                                                 \
    TEST_ASSERT(TTREE_FN(TTREE_MAP_TYPE(ALGO, KEY, VALUE), Find)(              \
                    &tree, (KEY)2, &found) == STATUS_CONST(SUCCESS));          \
    TEST_ASSERT(found != NULL && *found == (VALUE)(B));                        \
    TEST_ASSERT(TTREE_FN(TTREE_MAP_TYPE(ALGO, KEY, VALUE), Contains)(&tree, (KEY)2)); \
    TEST_ASSERT(TTREE_FN(TTREE_MAP_TYPE(ALGO, KEY, VALUE), MinNode)(tree.root) != NULL); \
    TEST_ASSERT(TTREE_FN(TTREE_MAP_TYPE(ALGO, KEY, VALUE), MaxNode)(tree.root) != NULL); \
    TEST_ASSERT(TTREE_FN(TTREE_MAP_TYPE(ALGO, KEY, VALUE), Min)(               \
                    &tree, &minimum) == STATUS_CONST(SUCCESS));                \
    TEST_ASSERT(TTREE_FN(TTREE_MAP_TYPE(ALGO, KEY, VALUE), Max)(               \
                    &tree, &maximum) == STATUS_CONST(SUCCESS));                \
    TEST_ASSERT(*minimum == (KEY)1);                                           \
    TEST_ASSERT(*maximum == (KEY)3); \
    node = TTREE_FN(TTREE_MAP_TYPE(ALGO, KEY, VALUE), Begin)(&tree);           \
    TEST_ASSERT(node != NULL);                                                 \
    TEST_ASSERT(TTREE_FN(TTREE_MAP_TYPE(ALGO, KEY, VALUE), End)(&tree) == NULL); \
    TEST_ASSERT(TTREE_FN(TTREE_MAP_TYPE(ALGO, KEY, VALUE), Next)(node) != NULL); \
    node = TTREE_FN(TTREE_MAP_TYPE(ALGO, KEY, VALUE), MaxNode)(tree.root);     \
    TEST_ASSERT(TTREE_FN(TTREE_MAP_TYPE(ALGO, KEY, VALUE), Prev)(node) != NULL); \
    cnode = TTREE_FN(TTREE_MAP_TYPE(ALGO, KEY, VALUE), ConstBegin)(&tree);     \
    TEST_ASSERT(cnode != NULL);                                                \
    TEST_ASSERT(TTREE_FN(TTREE_MAP_TYPE(ALGO, KEY, VALUE), ConstEnd)(&tree) == NULL); \
    TEST_ASSERT(TTREE_FN(TTREE_MAP_TYPE(ALGO, KEY, VALUE), ConstNext)(cnode) != NULL); \
    cnode = TTREE_FN(TTREE_MAP_TYPE(ALGO, KEY, VALUE), MaxNode)(tree.root);    \
    TEST_ASSERT(TTREE_FN(TTREE_MAP_TYPE(ALGO, KEY, VALUE), ConstPrev)(cnode) != NULL); \
    TEST_ASSERT(TTREE_FN(TTREE_MAP_TYPE(ALGO, KEY, VALUE), Remove)(&tree, (KEY)2) == STATUS_CONST(SUCCESS)); \
    TTREE_FN(TTREE_MAP_TYPE(ALGO, KEY, VALUE), Clear)(&tree);                  \
    TEST_ASSERT(TTREE_FN(TTREE_MAP_TYPE(ALGO, KEY, VALUE), Empty)(&tree));     \
    TTREE_FN(TTREE_MAP_TYPE(ALGO, KEY, VALUE), Destroy)(&tree);                \
  } while (0)

int main(void) {
  TEST_SET(BST, int, 1, 2, 3);
  TEST_SET(BST, float, 1, 2, 3);
  TEST_SET(BST, double, 1, 2, 3);
  TEST_SET(AVL, int, 1, 2, 3);
  TEST_SET(AVL, float, 1, 2, 3);
  TEST_SET(AVL, double, 1, 2, 3);
  TEST_SET(RB, int, 1, 2, 3);
  TEST_SET(RB, float, 1, 2, 3);
  TEST_SET(RB, double, 1, 2, 3);

  TEST_MAP(BST, int, int, 10, 20, 30);
  TEST_MAP(BST, int, float, 10, 20, 30);
  TEST_MAP(AVL, int, int, 10, 20, 30);
  TEST_MAP(AVL, int, float, 10, 20, 30);
  TEST_MAP(RB, int, int, 10, 20, 30);
  TEST_MAP(RB, int, float, 10, 20, 30);
  return 0;
}
