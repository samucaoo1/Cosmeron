#include "../../../Cosmeron/Modules/Container/Linked/Linked.h"
#include "../../../Cosmeron/Modules/Struct/Struct.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

/* Custom POD struct instantiation test */
TEMPLATE_INSTANTIATE(Linked, DoubleNode, PairInt32,
                     MODULE_TYPED(Struct, Pair, Int32));
TEMPLATE_INSTANTIATE(Iterator, Stream, PairInt32,
                     MODULE_TYPED(Struct, Pair, Int32));
TEMPLATE_INSTANTIATE(Iterator, Bidirectional, PairInt32,
                     MODULE_TYPED(Struct, Pair, Int32));
TEMPLATE_INSTANTIATE(Linked, List, PairInt32,
                     MODULE_TYPED(Struct, Pair, Int32));

/* Internal helpers for closures in tests (strictly namespaced) */
INTERNAL_DEF(
    LinkedTest,
    PROCEDURE(AddTen, (int32_t *element, void *context), {
      (void)context;
      if (element) {
        *element += 10;
      }
    }),
    PROCEDURE(Accumulate, (int32_t value, void *context), {
      int64_t *sum = (int64_t *)context;
      if (sum) {
        *sum += value;
      }
    }))

static void NS(LinkedTest, ForwardListNodeAndIter)(void) {
  MODULE_TYPED(Linked, Forward, Int32) fwd;
  assert(MODULE_FUNC(Linked, Forward, Int32, Create)(&fwd) ==
         STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL_0(&fwd, IsEmpty));

  assert(OBJECT_CALL(&fwd, PushBack, 10) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL(&fwd, PushBack, 30) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL(&fwd, PushFront, 5) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL_0(&fwd, Size) == 3U);

  /* Node-based InsertAfter & EraseAfter (no iterator needed) */
  MODULE_TYPED(Linked, SingleNode, Int32) *head = OBJECT_CALL_0(&fwd, Head);
  MODULE_TYPED(Linked, SingleNode, Int32) *inserted = NULL;
  assert(OBJECT_CALL(&fwd, InsertAfter, head->next, 20, &inserted) ==
         STATUS_CONST(SUCCESS));
  assert(inserted != NULL && inserted->value == 20);
  /* Sequence: [5, 10, 20, 30] */
  assert(OBJECT_CALL_0(&fwd, Size) == 4U);

  int32_t erasedVal = 0;
  assert(OBJECT_CALL(&fwd, EraseAfter, head, &erasedVal) ==
         STATUS_CONST(SUCCESS));
  assert(erasedVal == 10);
  /* Sequence: [5, 20, 30] */

  /* Iterator-based InsertAfterIter & EraseAfterIter */
  MODULE_TYPED(Iterator, Forward, Int32) it;
  assert(OBJECT_CALL(&fwd, Begin, &it) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL_0(&it, Next) == STATUS_CONST(SUCCESS)); /* at 20 */
  assert(OBJECT_CALL(&fwd, InsertAfterIter, &it, 25) == STATUS_CONST(SUCCESS));
  /* Sequence: [5, 20, 25, 30] */

  assert(OBJECT_CALL(&fwd, EraseAfterIter, &it, &erasedVal) ==
         STATUS_CONST(SUCCESS));
  assert(erasedVal == 25);
  /* Sequence: [5, 20, 30] */

  /* Traverse with Iterator_TForward and convert to Stream */
  assert(OBJECT_CALL(&fwd, Begin, &it) == STATUS_CONST(SUCCESS));
  int32_t expected[3] = {5, 20, 30};
  size_t idx = 0U;
  while (OBJECT_CALL_0(&it, HasNext)) {
    int32_t v = 0;
    assert(OBJECT_CALL(&it, StepValue, &v) == STATUS_CONST(SUCCESS));
    assert(v == expected[idx++]);
  }
  assert(idx == 3U);

  assert(OBJECT_CALL_0(&it, Reset) == STATUS_CONST(SUCCESS));
  MODULE_TYPED(Iterator, Stream, Int32) stream;
  assert(OBJECT_CALL(&it, ToStream, &stream) == STATUS_CONST(SUCCESS));
  int64_t sum = 0;
  MODULE_CLOSURE(Iterator, Stream, Int32, Visitor) acc =
      NS_JOIN(MODULE_CLOSURE_FUNC(Iterator, Stream, Int32, Visitor), Bind)(
          MODULE_IFUNC(LinkedTest, Accumulate), &sum);
  assert(OBJECT_CALL(&stream, ForEach, acc) == STATUS_CONST(SUCCESS));
  assert(sum == (5 + 20 + 30));

  assert(OBJECT_CALL_0(&fwd, Dispose) == STATUS_CONST(SUCCESS));
}

static void NS(LinkedTest, DoublyLinkedListNodeAndIter)(void) {
  MODULE_TYPED(Linked, List, Int32) list;
  assert(MODULE_FUNC(Linked, List, Int32, Create)(&list) ==
         STATUS_CONST(SUCCESS));

  assert(OBJECT_CALL(&list, PushBack, 10) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL(&list, PushBack, 30) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL(&list, PushFront, 5) == STATUS_CONST(SUCCESS));

  /* Node-based InsertBefore, InsertAfter, Erase (no iterator needed) */
  MODULE_TYPED(Linked, DoubleNode, Int32) *tail = OBJECT_CALL_0(&list, Tail);
  MODULE_TYPED(Linked, DoubleNode, Int32) *mid = NULL;
  assert(OBJECT_CALL(&list, InsertBefore, tail, 20, &mid) ==
         STATUS_CONST(SUCCESS));
  assert(mid != NULL && mid->value == 20);
  /* Sequence: [5, 10, 20, 30] */

  MODULE_TYPED(Linked, DoubleNode, Int32) *afterMid = NULL;
  assert(OBJECT_CALL(&list, InsertAfter, mid, 25, &afterMid) ==
         STATUS_CONST(SUCCESS));
  /* Sequence: [5, 10, 20, 25, 30] */

  int32_t removed = 0;
  MODULE_TYPED(Linked, DoubleNode, Int32) *nextNode = NULL;
  assert(OBJECT_CALL(&list, Erase, afterMid, &removed, &nextNode) ==
         STATUS_CONST(SUCCESS));
  assert(removed == 25);
  assert(nextNode == tail);
  /* Sequence: [5, 10, 20, 30] */

  /* Iterator-based InsertAt & EraseAt */
  MODULE_TYPED(Iterator, Bidirectional, Int32) bit;
  assert(OBJECT_CALL(&list, Begin, &bit) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL_0(&bit, Next) == STATUS_CONST(SUCCESS)); /* at 10 */
  assert(OBJECT_CALL(&list, InsertAt, &bit, 7) == STATUS_CONST(SUCCESS));
  /* Sequence: [5, 7, 10, 20, 30]; bit still at 10 */
  assert(OBJECT_CALL(&list, EraseAt, &bit, &removed) == STATUS_CONST(SUCCESS));
  assert(removed == 10);
  /* Sequence: [5, 7, 20, 30]; bit automatically advanced to 20 */
  int32_t curVal = 0;
  assert(OBJECT_CALL(&bit, Value, &curVal) == STATUS_CONST(SUCCESS));
  assert(curVal == 20);

  /* Reverse Bidirectional Iterator */
  MODULE_TYPED(Iterator, Bidirectional, Int32) rbit;
  assert(OBJECT_CALL(&list, RBegin, &rbit) == STATUS_CONST(SUCCESS));
  int32_t revExpected[4] = {30, 20, 7, 5};
  size_t idx = 0U;
  while (OBJECT_CALL_0(&rbit, HasNext)) {
    int32_t v = 0;
    assert(OBJECT_CALL(&rbit, StepValue, &v) == STATUS_CONST(SUCCESS));
    assert(v == revExpected[idx++]);
  }
  assert(idx == 4U);

  /* Prev from End restores tail (5 in reverse, 30 in forward) */
  assert(OBJECT_CALL_0(&rbit, Prev) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL(&rbit, Value, &curVal) == STATUS_CONST(SUCCESS) &&
         curVal == 5);

  /* ForEach */
  MODULE_CLOSURE(Linked, List, Int32, Visitor) addTen =
      NS_JOIN(MODULE_CLOSURE_FUNC(Linked, List, Int32, Visitor), Bind)(
          MODULE_IFUNC(LinkedTest, AddTen), NULL);
  assert(OBJECT_CALL(&list, ForEach, addTen) == STATUS_CONST(SUCCESS));
  int32_t *frontPtr = NULL;
  assert(OBJECT_CALL(&list, Front, &frontPtr) == STATUS_CONST(SUCCESS) &&
         *frontPtr == 15);

  assert(OBJECT_CALL_0(&list, Dispose) == STATUS_CONST(SUCCESS));
}

static void NS(LinkedTest, DequeStackQueueRing)(void) {
  /* Deque */
  MODULE_TYPED(Linked, Deque, Int32) deque;
  assert(MODULE_FUNC(Linked, Deque, Int32, Create)(&deque) ==
         STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL(&deque, PushFront, 2) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL(&deque, PushFront, 1) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL(&deque, PushBack, 3) == STATUS_CONST(SUCCESS));
  int32_t v = 0;
  assert(OBJECT_CALL(&deque, PopBack, &v) == STATUS_CONST(SUCCESS) && v == 3);
  assert(OBJECT_CALL(&deque, PopFront, &v) == STATUS_CONST(SUCCESS) && v == 1);
  assert(OBJECT_CALL(&deque, PopFront, &v) == STATUS_CONST(SUCCESS) && v == 2);
  assert(OBJECT_CALL_0(&deque, IsEmpty));
  assert(OBJECT_CALL_0(&deque, Dispose) == STATUS_CONST(SUCCESS));

  /* Stack */
  MODULE_TYPED(Linked, Stack, Int32) stack;
  assert(MODULE_FUNC(Linked, Stack, Int32, Create)(&stack) ==
         STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL(&stack, Push, 10) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL(&stack, Push, 20) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL(&stack, Peek, &v) == STATUS_CONST(SUCCESS) && v == 20);
  MODULE_TYPED(Iterator, Forward, Int32) fit;
  assert(OBJECT_CALL(&stack, Begin, &fit) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL(&fit, StepValue, &v) == STATUS_CONST(SUCCESS) && v == 20);
  assert(OBJECT_CALL(&fit, StepValue, &v) == STATUS_CONST(SUCCESS) && v == 10);
  assert(!OBJECT_CALL_0(&fit, HasNext));
  assert(OBJECT_CALL_0(&stack, Dispose) == STATUS_CONST(SUCCESS));

  /* Queue */
  MODULE_TYPED(Linked, Queue, Int32) queue;
  assert(MODULE_FUNC(Linked, Queue, Int32, Create)(&queue) ==
         STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL(&queue, Enqueue, 100) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL(&queue, Enqueue, 200) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL(&queue, Dequeue, &v) == STATUS_CONST(SUCCESS) && v == 100);
  assert(OBJECT_CALL(&queue, Dequeue, &v) == STATUS_CONST(SUCCESS) && v == 200);
  assert(OBJECT_CALL_0(&queue, Dispose) == STATUS_CONST(SUCCESS));

  /* Ring (owned + external nodes + bounded Forward iterator) */
  MODULE_TYPED(Linked, SingleNode, Int32) staticNodes[3];
  MODULE_TYPED(Linked, Ring, Int32) ring;
  assert(MODULE_FUNC(Linked, Ring, Int32, FromNodes)(&ring, staticNodes, 3U) ==
         STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL(&ring, Push, 1) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL(&ring, Push, 2) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL(&ring, Push, 3) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL_0(&ring, IsFull));
  assert(OBJECT_CALL(&ring, Push, 4) == STATUS_CONST(INSUFFICIENT_SPACE));

  int32_t dropped = 0;
  assert(OBJECT_CALL(&ring, Overwrite, 4, &dropped) == STATUS_CONST(SUCCESS));
  assert(dropped == 1);

  /* Bounded Forward iterator stops after 3 elements despite circular cycle */
  assert(OBJECT_CALL(&ring, Begin, &fit) == STATUS_CONST(SUCCESS));
  int32_t ringExpected[3] = {2, 3, 4};
  size_t idx = 0U;
  while (OBJECT_CALL_0(&fit, HasNext)) {
    assert(OBJECT_CALL(&fit, StepValue, &v) == STATUS_CONST(SUCCESS));
    assert(v == ringExpected[idx++]);
  }
  assert(idx == 3U);

  MODULE_TYPED(Linked, Ring, Int32) heapRing;
  assert(MODULE_FUNC(Linked, Ring, Int32, Create)(&heapRing, 4U) ==
         STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL(&heapRing, PushBack, 77) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL(&heapRing, PopFront, &v) == STATUS_CONST(SUCCESS) &&
         v == 77);
  assert(OBJECT_CALL_0(&heapRing, Dispose) == STATUS_CONST(SUCCESS));
}

static void NS(LinkedTest, CustomStructList)(void) {
  MODULE_TYPED(Linked, List, PairInt32) pairList;
  assert(MODULE_FUNC(Linked, List, PairInt32, Create)(&pairList) ==
         STATUS_CONST(SUCCESS));

  MODULE_TYPED(Struct, Pair, Int32) p1 = {.value1 = 3, .value2 = 30};
  MODULE_TYPED(Struct, Pair, Int32) p2 = {.value1 = 4, .value2 = 40};
  assert(OBJECT_CALL(&pairList, PushBack, p1) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL(&pairList, PushBack, p2) == STATUS_CONST(SUCCESS));

  MODULE_TYPED(Iterator, Bidirectional, PairInt32) it;
  assert(OBJECT_CALL(&pairList, Begin, &it) == STATUS_CONST(SUCCESS));
  MODULE_TYPED(Struct, Pair, Int32) outPair;
  assert(OBJECT_CALL(&it, StepValue, &outPair) == STATUS_CONST(SUCCESS));
  assert(outPair.value1 == 3 && outPair.value2 == 30);
  assert(OBJECT_CALL(&it, StepValue, &outPair) == STATUS_CONST(SUCCESS));
  assert(outPair.value1 == 4 && outPair.value2 == 40);
  assert(!OBJECT_CALL_0(&it, HasNext));

  assert(OBJECT_CALL_0(&pairList, Dispose) == STATUS_CONST(SUCCESS));
}

int main(void) {
  NS(LinkedTest, ForwardListNodeAndIter)();
  NS(LinkedTest, DoublyLinkedListNodeAndIter)();
  NS(LinkedTest, DequeStackQueueRing)();
  NS(LinkedTest, CustomStructList)();
  printf("Linked tests passed!\n");
  return 0;
}

