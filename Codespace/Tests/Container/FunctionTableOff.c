#define CONTAINER_DISABLE_FUNCTION_TABLE
#include "Test.h"
#include "../../Cosmeron/Modules/Container/Array/Vector.h"
#include "../../Cosmeron/Modules/Container/Linked/List.h"
#include "../../Cosmeron/Modules/Container/Tree/BST.h"
#include "../../Cosmeron/Modules/Container/Tree/AVL.h"

int main(void) {
  TVector(int, vector)
  TLinked_List(int, list)
  TTree_AVL_Set(int, tree);
  TTree_BST_Set(int, bst);
  int value = 0;
  TEST_ASSERT(Flat_Vector_int_PushBack(&vector, 7) == STATUS_CONST(SUCCESS));
  TEST_ASSERT(Flat_Vector_int_PopBack(&vector, &value) == STATUS_CONST(SUCCESS));
  TEST_ASSERT(value == 7);
  TEST_ASSERT(Linked_List_int_PushBack(&list, 9) == STATUS_CONST(SUCCESS));
  TEST_ASSERT(Tree_AVL_Set_int_Insert(&tree, 11) == STATUS_CONST(SUCCESS));
  TEST_ASSERT(Tree_AVL_Set_int_Find(&tree, 11) != NULL);
  TEST_ASSERT(Tree_BST_Set_int_Insert(&bst, 12) == STATUS_CONST(SUCCESS));
  TEST_ASSERT(Tree_BST_Set_int_Find(&bst, 12) != NULL);
  TEST_ASSERT(Tree_BST_Set_int_Begin(NULL) == NULL);
  Flat_Vector_int_Destroy(&vector);
  Linked_List_int_Destroy(&list);
  Tree_AVL_Set_int_Destroy(&tree);
  Tree_BST_Set_int_Destroy(&bst);
  return 0;
}

