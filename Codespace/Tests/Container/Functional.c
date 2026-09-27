#include "Test.h"

#include "../../Cosmeron/Modules/Container/Array/Vector.h"
#include "../../Cosmeron/Modules/Container/Array/Queue.h"
#include "../../Cosmeron/Modules/Container/Array/Stack.h"
#include "../../Cosmeron/Modules/Container/Array/String.h"
#include "../../Cosmeron/Modules/Container/Linked/List.h"
#include "../../Cosmeron/Modules/Container/Linked/Forward.h"
#include "../../Cosmeron/Modules/Container/Linked/Deque.h"
#include "../../Cosmeron/Modules/Container/Tree/BST.h"
#include "../../Cosmeron/Modules/Container/Tree/AVL.h"
#include "../../Cosmeron/Modules/Container/Tree/RedBlack.h"
#include "../../Cosmeron/Modules/Container/Aliases/Linear.h"
#include "../../Cosmeron/Modules/Container/Aliases/Tree.h"

static int check_avl_node(Tree_AVL_Set_int_Node *node, int *height) {
  if (node == NULL) {
    *height = 0;
    return 1;
  }
  int left_height;
  int right_height;
  if (!check_avl_node(node->left, &left_height) ||
      !check_avl_node(node->right, &right_height))
    return 0;
  if (node->left != NULL &&
      (node->left->parent != node || node->left->key >= node->key))
    return 0;
  if (node->right != NULL &&
      (node->right->parent != node || node->right->key <= node->key))
    return 0;
  int difference = left_height - right_height;
  *height = 1 + (left_height > right_height ? left_height : right_height);
  return difference >= -1 && difference <= 1 && node->height == *height;
}

static int check_rb_node(Tree_RB_Set_int_Node *node, int *black_height) {
  if (node == NULL) {
    *black_height = 1;
    return 1;
  }
  int left_black_height;
  int right_black_height;
  if (!check_rb_node(node->left, &left_black_height) ||
      !check_rb_node(node->right, &right_black_height))
    return 0;
  if (left_black_height != right_black_height)
    return 0;
  if (node->left != NULL &&
      (node->left->parent != node || node->left->key >= node->key))
    return 0;
  if (node->right != NULL &&
      (node->right->parent != node || node->right->key <= node->key))
    return 0;
  if (node->color == TREE_RB_RED &&
      ((node->left != NULL && node->left->color == TREE_RB_RED) ||
       (node->right != NULL && node->right->color == TREE_RB_RED)))
    return 0;
  *black_height =
      left_black_height + (node->color == TREE_RB_BLACK ? 1 : 0);
  return 1;
}



static int test_capacity_overflow(void) {
  TVector(int, vector)
  vector.size = SIZE_MAX;
  vector.capacity = SIZE_MAX;
  TEST_ASSERT(Flat_Vector_int_PushBack(&vector, 1) ==
              STATUS_CONST(ARITHMETIC_OVERFLOW));
  TEST_ASSERT(vector.size == SIZE_MAX);
  TEST_ASSERT(vector.capacity == SIZE_MAX);
  TEST_ASSERT(vector.data == NULL);
  return 0;
}

static int test_queue(void) {
  TFlat_Queue(int, queue)
  for (int value = 0; value < 64; ++value)
    TEST_ASSERT(Push(queue, value) == STATUS_CONST(SUCCESS));
  TEST_ASSERT(Size(queue) == 64);
  TEST_ASSERT(*Front(queue) == 0);
  TEST_ASSERT(*Back(queue) == 63);
  for (int expected = 0; expected < 64; ++expected) {
    int value = -1;
    TEST_ASSERT(Pop(queue, &value) == STATUS_CONST(SUCCESS));
    TEST_ASSERT(value == expected);
  }
  TEST_ASSERT(Empty(queue));
  Destroy(queue);
  return 0;
}

static int test_stack(void) {
  TFlat_Stack(int, stack)
  for (int value = 0; value < 64; ++value)
    TEST_ASSERT(Push(stack, value) == STATUS_CONST(SUCCESS));
  TEST_ASSERT(Size(stack) == 64);
  TEST_ASSERT(*Top(stack) == 63);
  for (int expected = 63; expected >= 0; --expected) {
    int value = -1;
    TEST_ASSERT(Pop(stack, &value) == STATUS_CONST(SUCCESS));
    TEST_ASSERT(value == expected);
  }
  TEST_ASSERT(Empty(stack));
  Destroy(stack);
  return 0;
}

static int test_string(void) {
  static const utf8 cosmeron[] = {'C', 'o', 'n', 'g', 'r', 'o', 0};
  static const utf8 library[] = {' ', 'L', 'i', 'b', 'r', 'a', 'r', 'y', 0};
  static const utf8 expected[] = {
      'C', 'o', 'n', 'g', 'r', 'o', ' ', 'L',
      'i', 'b', 'r', 'a', 'r', 'y', '!', 0};
  TString(8, string)
  TEST_ASSERT(string.api->fromCStr(&string, cosmeron) == STATUS_CONST(SUCCESS));
  TEST_ASSERT(string.api->pushBack(&string, (utf8)'!') == STATUS_CONST(SUCCESS));
  TEST_ASSERT(string.api->length(&string) == 7);
  TEST_ASSERT(string.api->insertStr(&string, 6, library) == STATUS_CONST(SUCCESS));
  TEST_ASSERT(string.api->length(&string) == 15);
  TEST_ASSERT(memcmp(string.api->cStr(&string), expected, sizeof(expected)) == 0);
  string.api->destroy(&string);
  return 0;
}

static int test_deque(void) {
  TLinked_Deque(int, deque)
  TEST_ASSERT(PushFront(deque, 2) == STATUS_CONST(SUCCESS));
  TEST_ASSERT(PushFront(deque, 1) == STATUS_CONST(SUCCESS));
  TEST_ASSERT(PushBack(deque, 3) == STATUS_CONST(SUCCESS));
  TEST_ASSERT(*Front(deque) == 1);
  TEST_ASSERT(*Back(deque) == 3);
  int value = 0;
  TEST_ASSERT(PopFront(deque, &value) == STATUS_CONST(SUCCESS) && value == 1);
  TEST_ASSERT(PopBack(deque, &value) == STATUS_CONST(SUCCESS) && value == 3);
  TEST_ASSERT(PopFront(deque, &value) == STATUS_CONST(SUCCESS) && value == 2);
  TEST_ASSERT(Empty(deque));
  Destroy(deque);
  return 0;
}

static int test_vector(void) {
  TVector(int, vector)
  TEST_ASSERT(Empty(vector));
  for (int value = 0; value < 1024; ++value)
    TEST_ASSERT(PushBack(vector, value) == STATUS_CONST(SUCCESS));
  TEST_ASSERT(Size(vector) == 1024);
  TEST_ASSERT(Insert(vector, 512, -1) == STATUS_CONST(SUCCESS));
  TEST_ASSERT(*At(vector, 512) == -1);
  TEST_ASSERT(Erase(vector, 500, 25) == STATUS_CONST(SUCCESS));
  TEST_ASSERT(Size(vector) == 1000);
  TEST_ASSERT(Erase(vector, Size(vector), 0) == STATUS_CONST(SUCCESS));
  TEST_ASSERT(Erase(vector, Size(vector), 1) == STATUS_CONST(OUT_OF_RANGE));
  int removed = -1;
  TEST_ASSERT(PopBack(vector, &removed) == STATUS_CONST(SUCCESS));
  TEST_ASSERT(PopBack(vector) == STATUS_CONST(SUCCESS));
  Destroy(vector);
  return 0;
}


static int test_linked_node_ownership(void) {
  TLinked_List(int, first)
  TLinked_List(int, second)
  TEST_ASSERT(PushBack(first, 1) == STATUS_CONST(SUCCESS));
  TEST_ASSERT(PushBack(second, 2) == STATUS_CONST(SUCCESS));
  Linked_List_TNode_int *foreign = second.head;
  TEST_ASSERT(Linked_List_int_Insert(&first, foreign, 3, NULL) ==
              STATUS_CONST(INVALID_ARGUMENT));
  TEST_ASSERT(Linked_List_int_Erase(&first, foreign, NULL) ==
              STATUS_CONST(INVALID_ARGUMENT));
  TEST_ASSERT(Size(first) == 1);
  TEST_ASSERT(Size(second) == 1);
  Destroy(first);
  Destroy(second);

  TLinked_ForwardList(int, forwardA)
  TLinked_ForwardList(int, forwardB)
  TEST_ASSERT(Linked_ForwardList_int_PushFront(&forwardA, 1) ==
              STATUS_CONST(SUCCESS));
  TEST_ASSERT(Linked_ForwardList_int_PushFront(&forwardB, 2) ==
              STATUS_CONST(SUCCESS));
  Linked_ForwardList_TNode_int *foreignForward = forwardB.head;
  TEST_ASSERT(Linked_ForwardList_int_InsertAfter(
                  &forwardA, foreignForward, 3, NULL) ==
              STATUS_CONST(INVALID_ARGUMENT));
  TEST_ASSERT(Linked_ForwardList_int_EraseAfter(&forwardA, foreignForward) ==
              STATUS_CONST(INVALID_ARGUMENT));
  TEST_ASSERT(Linked_ForwardList_int_Size(&forwardA) == 1);
  TEST_ASSERT(Linked_ForwardList_int_Size(&forwardB) == 1);
  Linked_ForwardList_int_Destroy(&forwardA);
  Linked_ForwardList_int_Destroy(&forwardB);
  return 0;
}

static int test_list(void) {
  TLinked_List(int, list)
  for (int value = 0; value < 256; ++value)
    TEST_ASSERT(PushBack(list, value) == STATUS_CONST(SUCCESS));
  TEST_ASSERT(Size(list) == 256);
  size_t count = 0;
  Linked_List_TNode_int *node = list.head;
  Linked_List_TNode_int *previous = NULL;
  while (node != NULL) {
    TEST_ASSERT(node->prev == previous);
    previous = node;
    node = node->next;
    ++count;
  }
  TEST_ASSERT(count == Size(list) && previous == list.tail);
  for (int expected = 0; expected < 256; ++expected) {
    int value;
    TEST_ASSERT(PopFront(list, &value) == STATUS_CONST(SUCCESS));
    TEST_ASSERT(value == expected);
  }
  TEST_ASSERT(PopFront(list) == STATUS_CONST(OUT_OF_RANGE));
  Destroy(list);
  return 0;
}


static int test_bst_and_iterators(void) {
  TTree_BST_Set(int, tree)
  TEST_ASSERT(TreeBegin(tree) == NULL);
  TEST_ASSERT(Tree_BST_Set_int_Begin(NULL) == NULL);

  const int values[] = {4, 2, 6, 1, 3, 5, 7};
  for (size_t i = 0; i < sizeof(values) / sizeof(values[0]); ++i)
    TEST_ASSERT(TreeInsert(tree, values[i]) == STATUS_CONST(SUCCESS));

  int expected = 1;
  Tree_BST_Set_int_Node *node = TreeBegin(tree);
  while (node != TreeEnd(tree)) {
    TEST_ASSERT(node->key == expected);
    ++expected;
    node = TreeNext(tree, node);
  }
  TEST_ASSERT(expected == 8);

  expected = 7;
  node = Tree_BST_Set_int_MaxNode(tree.root);
  while (node != NULL) {
    TEST_ASSERT(node->key == expected);
    --expected;
    node = TreePrev(tree, node);
  }
  TEST_ASSERT(expected == 0);

  TEST_ASSERT(TreeRemove(tree, 4) == STATUS_CONST(SUCCESS));
  TEST_ASSERT(!TreeContains(tree, 4));
  TEST_ASSERT(TreeSize(tree) == 6);
  TreeDestroy(tree);
  return 0;
}

static int test_avl(void) {
  TTree_AVL_Set(int, tree);
  for (int index = 0; index < 257; ++index) {
    int value = (index * 73) % 257;
    TEST_ASSERT(TreeInsert(tree, value) == STATUS_CONST(SUCCESS));
    int height;
    TEST_ASSERT(check_avl_node(tree.root, &height));
  }
  TEST_ASSERT(TreeInsert(tree, 42) == STATUS_CONST(ALREADY_EXISTS));
  for (int value = 0; value < 257; value += 2) {
    TEST_ASSERT(TreeRemove(tree, value) == STATUS_CONST(SUCCESS));
    int height;
    TEST_ASSERT(check_avl_node(tree.root, &height));
  }
  TEST_ASSERT(TreeSize(tree) == 128);
  TreeDestroy(tree);
  return 0;
}

static int test_red_black(void) {
  TTree_RB_Set(int, tree);
  for (int index = 0; index < 257; ++index) {
    int value = (index * 73) % 257;
    TEST_ASSERT(TreeInsert(tree, value) == STATUS_CONST(SUCCESS));
    int black_height;
    TEST_ASSERT(tree.root == NULL || tree.root->color == TREE_RB_BLACK);
    TEST_ASSERT(check_rb_node(tree.root, &black_height));
  }
  for (int value = 0; value < 257; value += 2) {
    TEST_ASSERT(TreeRemove(tree, value) == STATUS_CONST(SUCCESS));
    int black_height;
    TEST_ASSERT(tree.root == NULL || tree.root->color == TREE_RB_BLACK);
    TEST_ASSERT(check_rb_node(tree.root, &black_height));
  }
  TEST_ASSERT(TreeSize(tree) == 128);
  TreeDestroy(tree);
  return 0;
}

static int test_map(void) {
  TTree_AVL_Map(int, int, map);
  for (int key = 0; key < 100; ++key)
    TEST_ASSERT(TreeInsert(map, key, key * 10) == STATUS_CONST(SUCCESS));
  for (int key = 0; key < 100; ++key) {
    int *value = TreeFind(map, key);
    TEST_ASSERT(value != NULL && *value == key * 10);
  }
  TreeDestroy(map);
  return 0;
}


static int test_null_contracts(void) {
  TEST_ASSERT(Flat_Vector_int_Init(NULL) == STATUS_CONST(INVALID_ARGUMENT));
  TEST_ASSERT(Flat_Vector_int_Reserve(NULL, 16U) == STATUS_CONST(INVALID_ARGUMENT));
  TEST_ASSERT(Flat_Vector_int_PopBack(NULL, NULL) == STATUS_CONST(INVALID_ARGUMENT));
  TEST_ASSERT(Flat_Vector_int_Insert(NULL, 0U, 1) == STATUS_CONST(INVALID_ARGUMENT));
  TEST_ASSERT(Flat_Vector_int_Erase(NULL, 0U, 0U) == STATUS_CONST(INVALID_ARGUMENT));
  Flat_Vector_int_Clear(NULL);
  TEST_ASSERT(Flat_Vector_int_At(NULL, 0U) == NULL);
  TEST_ASSERT(!Flat_Vector_int_Empty(NULL));
  TEST_ASSERT(Flat_Vector_int_Size(NULL) == 0U);
  Flat_Vector_int_Destroy(NULL);

  TEST_ASSERT(Flat_Queue_int_Init(NULL) == STATUS_CONST(INVALID_ARGUMENT));
  TEST_ASSERT(Flat_Queue_int_Push(NULL, 1) == STATUS_CONST(INVALID_ARGUMENT));
  TEST_ASSERT(Flat_Queue_int_Pop(NULL, NULL) == STATUS_CONST(INVALID_ARGUMENT));
  TEST_ASSERT(Flat_Queue_int_Front(NULL) == NULL);
  TEST_ASSERT(!Flat_Queue_int_Empty(NULL));
  TEST_ASSERT(Flat_Queue_int_Size(NULL) == 0U);
  Flat_Queue_int_Destroy(NULL);

  TEST_ASSERT(Linked_List_int_Init(NULL) == STATUS_CONST(INVALID_ARGUMENT));
  TEST_ASSERT(Linked_List_int_PushBack(NULL, 1) == STATUS_CONST(INVALID_ARGUMENT));
  TEST_ASSERT(Linked_List_int_PopFront(NULL, NULL) == STATUS_CONST(INVALID_ARGUMENT));
  TEST_ASSERT(Linked_List_int_Front(NULL) == NULL);
  TEST_ASSERT(!Linked_List_int_Empty(NULL));
  TEST_ASSERT(Linked_List_int_Size(NULL) == 0U);
  Linked_List_int_Destroy(NULL);

  TEST_ASSERT(Tree_AVL_Set_int_Init(NULL) == STATUS_CONST(INVALID_ARGUMENT));
  TEST_ASSERT(Tree_AVL_Set_int_Insert(NULL, 1) == STATUS_CONST(INVALID_ARGUMENT));
  TEST_ASSERT(Tree_AVL_Set_int_Remove(NULL, 1) == STATUS_CONST(INVALID_ARGUMENT));
  TEST_ASSERT(Tree_AVL_Set_int_Find(NULL, 1) == NULL);
  TEST_ASSERT(!Tree_AVL_Set_int_Contains(NULL, 1));
  TEST_ASSERT(Tree_AVL_Set_int_Min(NULL) == NULL);
  TEST_ASSERT(Tree_AVL_Set_int_Max(NULL) == NULL);
  TEST_ASSERT(!Tree_AVL_Set_int_Empty(NULL));
  TEST_ASSERT(Tree_AVL_Set_int_Size(NULL) == 0U);
  Tree_AVL_Set_int_Destroy(NULL);
  return 0;
}


static int test_string_contracts(void) {
  TString(8, text)
  TString(8, other)
  TString(8, slice)
  const utf8 hello[] = {'h', 'e', 'l', 'l', 'o', 0};
  const utf8 ell[] = {'e', 'l', 'l', 0};
  size_t index = 99U;
  TComparisonResult comparison;

  TEST_ASSERT(Flat_String_8_FromCStr(&text, hello) == STATUS_CONST(SUCCESS));
  TEST_ASSERT(Flat_String_8_Find(&text, (utf8)'l', 0U, &index) ==
              STATUS_CONST(SUCCESS));
  TEST_ASSERT(index == 2U);
  TEST_ASSERT(Flat_String_8_Find(&text, (utf8)'x', 0U, &index) ==
              STATUS_CONST(NOT_FOUND));
  TEST_ASSERT(Flat_String_8_FindStr(&text, ell, 0U, &index) ==
              STATUS_CONST(SUCCESS));
  TEST_ASSERT(index == 1U);
  TEST_ASSERT(Flat_String_8_FindStr(&text, ell, 4U, &index) ==
              STATUS_CONST(NOT_FOUND));

  TEST_ASSERT(Flat_String_8_FromCStr(&other, hello) == STATUS_CONST(SUCCESS));
  TEST_ASSERT(Flat_String_8_Compare(&text, &other, &comparison) ==
              STATUS_CONST(SUCCESS));
  TEST_ASSERT(comparison == COMPARISON_CONST(EQUAL));

  TEST_ASSERT(Flat_String_8_Substr(&text, 1U, 3U, &slice) ==
              STATUS_CONST(SUCCESS));
  TEST_ASSERT(slice.size == 3U);
  TEST_ASSERT(slice.data[0] == (utf8)'e' && slice.data[2] == (utf8)'l');
  TEST_ASSERT(Flat_String_8_Substr(&text, 0U, 1U, &text) ==
              STATUS_CONST(INVALID_ARGUMENT));

  TEST_ASSERT(Flat_String_8_PopBack(NULL, NULL) == STATUS_CONST(INVALID_ARGUMENT));
  TEST_ASSERT(Flat_String_8_InsertChar(NULL, 0U, (utf8)'x') ==
              STATUS_CONST(INVALID_ARGUMENT));
  TEST_ASSERT(Flat_String_8_Append(NULL, hello) == STATUS_CONST(INVALID_ARGUMENT));
  TEST_ASSERT(Flat_String_8_Erase(NULL, 0U, 1U) ==
              STATUS_CONST(INVALID_ARGUMENT));
  TEST_ASSERT(Flat_String_8_Compare(NULL, &other, &comparison) ==
              STATUS_CONST(INVALID_ARGUMENT));
  TEST_ASSERT(Flat_String_8_Find(NULL, (utf8)'x', 0U, &index) ==
              STATUS_CONST(INVALID_ARGUMENT));
  TEST_ASSERT(Flat_String_8_Data(NULL) == NULL);

  Flat_String_8_Destroy(&text);
  Flat_String_8_Destroy(&other);
  Flat_String_8_Destroy(&slice);
  return 0;
}


static int test_alias_single_evaluation(void) {
  FLAT_VECTOR_TYPE(int) vectors[2];
  size_t index = 0U;
  TEST_ASSERT(Flat_Vector_int_Init(&vectors[0]) == STATUS_CONST(SUCCESS));
  TEST_ASSERT(Flat_Vector_int_Init(&vectors[1]) == STATUS_CONST(SUCCESS));
  TEST_ASSERT(PushBack(vectors[index++], 7) == STATUS_CONST(SUCCESS));
  TEST_ASSERT(index == 1U);
  TEST_ASSERT(Size(vectors[0]) == 1U);
  Destroy(vectors[0]);
  Destroy(vectors[1]);
  return 0;
}


static int test_instance_declare_grammar(void) {
  FLAT_VECTOR_INSTANCE_DECLARE(int, vector)
  LINKED_LIST_INSTANCE_DECLARE(int, list)
  TREE_AVL_SET_INSTANCE_DECLARE(int, tree)

  TEST_ASSERT(PushBack(vector, 11) == STATUS_CONST(SUCCESS));
  TEST_ASSERT(PushBack(list, 13) == STATUS_CONST(SUCCESS));
  TEST_ASSERT(TreeInsert(tree, 17) == STATUS_CONST(SUCCESS));
  TEST_ASSERT(Size(vector) == 1U);
  TEST_ASSERT(Size(list) == 1U);
  TEST_ASSERT(TreeSize(tree) == 1U);

  Destroy(vector);
  Destroy(list);
  TreeDestroy(tree);
  return 0;
}

int main(void) {
  if (test_instance_declare_grammar()) return 99;
  if (test_alias_single_evaluation()) return 100;
  TEST_ASSERT(test_vector() == 0);
  TEST_ASSERT(test_capacity_overflow() == 0);
  TEST_ASSERT(test_queue() == 0);
  TEST_ASSERT(test_stack() == 0);
  TEST_ASSERT(test_string() == 0);
  TEST_ASSERT(test_deque() == 0);
  TEST_ASSERT(test_linked_node_ownership() == 0);
  TEST_ASSERT(test_list() == 0);
  TEST_ASSERT(test_bst_and_iterators() == 0);
  TEST_ASSERT(test_avl() == 0);
  TEST_ASSERT(test_red_black() == 0);
  TEST_ASSERT(test_map() == 0);
  TEST_ASSERT(test_null_contracts() == 0);
  TEST_ASSERT(test_string_contracts() == 0);
  return 0;
}
