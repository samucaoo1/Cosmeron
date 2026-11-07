#define TREE_DISABLE_FUNCTION_TABLE

#include "Test.h"
#include "../../Cosmeron/Modules/Container/Tree/BST.h"
#include "../../Cosmeron/Modules/Container/Tree/AVL.h"
#include "../../Cosmeron/Modules/Container/Tree/RedBlack.h"

int main(void) {
  TTree_BST_Set(int, bst)
  TTree_AVL_Set(int, avl)
  TTree_RB_Set(int, rb)

  TEST_ASSERT(TREE_FUNC(TREE_SET_TYPE(BST, int), Insert)(&bst, 1) ==
              STATUS_CONST(SUCCESS));
  TEST_ASSERT(TREE_FUNC(TREE_SET_TYPE(AVL, int), Insert)(&avl, 2) ==
              STATUS_CONST(SUCCESS));
  TEST_ASSERT(TREE_FUNC(TREE_SET_TYPE(RB, int), Insert)(&rb, 3) ==
              STATUS_CONST(SUCCESS));

  TREE_FUNC(TREE_SET_TYPE(BST, int), Destroy)(&bst);
  TREE_FUNC(TREE_SET_TYPE(AVL, int), Destroy)(&avl);
  TREE_FUNC(TREE_SET_TYPE(RB, int), Destroy)(&rb);
  return 0;
}
