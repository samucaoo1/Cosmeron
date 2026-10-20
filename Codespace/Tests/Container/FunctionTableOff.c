#define CONTAINER_DISABLE_FUNCTION_TABLE
#include "Test.h"
#include "../../Cosmeron/Modules/Container/Array/Vector.h"
#include "../../Cosmeron/Modules/Container/Array/Heap.h"
#include "../../Cosmeron/Modules/Container/Array/Span.h"
#include "../../Cosmeron/Modules/Container/Linked/List.h"
#include "../../Cosmeron/Modules/Container/Tree/BST.h"
#include "../../Cosmeron/Modules/Container/Tree/AVL.h"

int main(void) {
  TVector(int, vector)
  TFlat_Heap(int, heap)
  TLinked_List(int, list)
  TTree_AVL_Set(int, tree);
  TTree_BST_Set(int, bst);
  int value = 0;
  TEST_ASSERT(Container_Flat_Vector_int_PushBack(&vector, 7) == STATUS_CONST(SUCCESS));
  TEST_ASSERT(Container_Flat_Vector_int_PopBack(&vector, &value) == STATUS_CONST(SUCCESS));
  TEST_ASSERT(value == 7);
  TEST_ASSERT(Container_Linked_List_int_PushBack(&list, 9) == STATUS_CONST(SUCCESS));
  TEST_ASSERT(Container_Tree_AVL_Set_int_Insert(&tree, 11) == STATUS_CONST(SUCCESS));
  {
    int *found = NULL;
    TEST_ASSERT(Container_Tree_AVL_Set_int_Find(&tree, 11, &found) ==
                STATUS_CONST(SUCCESS));
    TEST_ASSERT(found != NULL && *found == 11);
  }
  TEST_ASSERT(Container_Tree_BST_Set_int_Insert(&bst, 12) == STATUS_CONST(SUCCESS));
  {
    int *found = NULL;
    TEST_ASSERT(Container_Tree_BST_Set_int_Find(&bst, 12, &found) ==
                STATUS_CONST(SUCCESS));
    TEST_ASSERT(found != NULL && *found == 12);
  }
  TEST_ASSERT(Container_Tree_BST_Set_int_Begin(NULL) == NULL);
  TEST_ASSERT(Container_Flat_Heap_int_Push(&heap, 15) == STATUS_CONST(SUCCESS));
  TEST_ASSERT(Container_Flat_Heap_int_Pop(&heap, &value) == STATUS_CONST(SUCCESS) && value == 15);
  {
    int rawArr[] = { 10, 20, 30 };
    TSpan(int, span);
    TEST_ASSERT(Container_Flat_Span_int_FromBuffer(&span, rawArr, 3U) == STATUS_CONST(SUCCESS));
    TEST_ASSERT(Container_Flat_Span_int_Size(&span) == 3U);
    int *spanItem = NULL;
    TEST_ASSERT(Container_Flat_Span_int_At(&span, 1U, &spanItem) == STATUS_CONST(SUCCESS) && *spanItem == 20);
  }
  Container_Flat_Heap_int_Destroy(&heap);
  Container_Flat_Vector_int_Destroy(&vector);
  Container_Linked_List_int_Destroy(&list);
  Container_Tree_AVL_Set_int_Destroy(&tree);
  Container_Tree_BST_Set_int_Destroy(&bst);
  return 0;
}

