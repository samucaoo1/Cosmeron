#include "../../../Cosmeron/Modules/Container/Array/Array.h"
#include "../../../Cosmeron/Modules/Struct/Struct.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

/* Custom POD struct instantiation test */
TEMPLATE_INSTANTIATE(Iterator, Stream, PairInt32, MODULE_TYPED(Struct, Pair, Int32));
TEMPLATE_INSTANTIATE(Iterator, Contiguous, PairInt32, MODULE_TYPED(Struct, Pair, Int32));
TEMPLATE_INSTANTIATE(Array, Vector, PairInt32, MODULE_TYPED(Struct, Pair, Int32));

/* Internal helpers for closures in tests (strictly namespaced) */
INTERNAL_DEF(
    ArrayTest,
    PROCEDURE(AddOffset, (int32_t *element, void *context), {
      int32_t offset = context ? *(const int32_t *)context : 0;
      if (element) {
        *element += offset;
      }
    }),
    COMPARISON(CompareAbsAsc, (int32_t left, int32_t right, void *context), {
      (void)context;
      int32_t a = (left < 0) ? -left : left;
      int32_t b = (right < 0) ? -right : right;
      return (a < b) ? COMPARISON_CONST(LOWER)
                     : ((a > b) ? COMPARISON_CONST(HIGHER)
                                : COMPARISON_CONST(EQUAL));
    }))

static void NS(ArrayTest, VectorAndSpan)(void) {
  MODULE_TYPED(Array, Vector, Int32) vec;
  assert(MODULE_FUNC(Array, Vector, Int32, Create)(&vec, 4U) ==
         STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL_0(&vec, IsEmpty));
  assert(OBJECT_CALL_0(&vec, Capacity) == 4U);

  for (int32_t i = 1; i <= 6; ++i) {
    assert(OBJECT_CALL(&vec, PushBack, i * 10) == STATUS_CONST(SUCCESS));
  }
  assert(OBJECT_CALL_0(&vec, Size) == 6U);
  assert(OBJECT_CALL_0(&vec, Capacity) >= 6U);

  /* Insert and RemoveAt */
  assert(OBJECT_CALL(&vec, Insert, 2U, 25) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL_0(&vec, Size) == 7U);

  int32_t val = 0;
  assert(OBJECT_CALL(&vec, Get, 2U, &val) == STATUS_CONST(SUCCESS) && val == 25);

  int32_t removed = 0;
  assert(OBJECT_CALL(&vec, RemoveAt, 2U, &removed) == STATUS_CONST(SUCCESS));
  assert(removed == 25);
  assert(OBJECT_CALL_0(&vec, Size) == 6U);

  /* SwapRemove */
  assert(OBJECT_CALL(&vec, SwapRemove, 1U, &removed) == STATUS_CONST(SUCCESS));
  assert(removed == 20);
  assert(OBJECT_CALL_0(&vec, Size) == 5U);
  /* Element at index 1 is now old back (60) */
  assert(OBJECT_CALL(&vec, Get, 1U, &val) == STATUS_CONST(SUCCESS) && val == 60);

  /* Restore sorted order via Set */
  assert(OBJECT_CALL(&vec, Set, 1U, 20) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL(&vec, PushBack, 60) == STATUS_CONST(SUCCESS));

  /* Vector Iterator */
  MODULE_TYPED(Iterator, Contiguous, Int32) it;
  assert(OBJECT_CALL(&vec, Begin, &it) == STATUS_CONST(SUCCESS));
  int32_t expected = 10;
  while (OBJECT_CALL_0(&it, HasNext)) {
    int32_t item = 0;
    assert(OBJECT_CALL(&it, StepValue, &item) == STATUS_CONST(SUCCESS));
    assert(item == expected);
    expected += 10;
  }

  /* Vector Reverse Iterator */
  MODULE_TYPED(Iterator, Contiguous, Int32) rit;
  assert(OBJECT_CALL(&vec, RBegin, &rit) == STATUS_CONST(SUCCESS));
  expected = 60;
  while (OBJECT_CALL_0(&rit, HasNext)) {
    int32_t item = 0;
    assert(OBJECT_CALL(&rit, StepValue, &item) == STATUS_CONST(SUCCESS));
    assert(item == expected);
    expected -= 10;
  }

  /* Vector ForEach */
  int32_t offset = 5;
  MODULE_CLOSURE(Array, Vector, Int32, Visitor) visitor =
      NS_JOIN(MODULE_CLOSURE_FUNC(Array, Vector, Int32, Visitor), Bind)(
          MODULE_IFUNC(ArrayTest, AddOffset), &offset);
  assert(OBJECT_CALL(&vec, ForEach, visitor) == STATUS_CONST(SUCCESS));

  /* Span from Vector & Subspan */
  MODULE_TYPED(Array, Span, Int32) span;
  assert(MODULE_FUNC(Array, Span, Int32, FromVector)(&span, &vec) ==
         STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL_0(&span, Size) == 6U);

  MODULE_TYPED(Array, Span, Int32) sub;
  assert(OBJECT_CALL(&span, Subspan, 1U, 3U, &sub) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL_0(&sub, Size) == 3U);

  assert(OBJECT_CALL(&sub, Begin, &it) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL(&it, StepValue, &val) == STATUS_CONST(SUCCESS) &&
         val == 25);
  assert(OBJECT_CALL(&it, StepValue, &val) == STATUS_CONST(SUCCESS) &&
         val == 35);
  assert(OBJECT_CALL(&it, StepValue, &val) == STATUS_CONST(SUCCESS) &&
         val == 45);
  assert(!OBJECT_CALL_0(&it, HasNext));

  assert(OBJECT_CALL_0(&vec, ShrinkToFit) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL_0(&vec, Capacity) == 6U);
  assert(OBJECT_CALL_0(&vec, Dispose) == STATUS_CONST(SUCCESS));
}

static void NS(ArrayTest, StackAndQueue)(void) {
  /* Stack LIFO + Iterator */
  MODULE_TYPED(Array, Stack, Int32) stack;
  assert(MODULE_FUNC(Array, Stack, Int32, Create)(&stack, 4U) ==
         STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL(&stack, Push, 100) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL(&stack, Push, 200) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL(&stack, Push, 300) == STATUS_CONST(SUCCESS));

  /* RBegin on Stack iterates from top (300) to bottom (100) */
  MODULE_TYPED(Iterator, Contiguous, Int32) it;
  assert(OBJECT_CALL(&stack, RBegin, &it) == STATUS_CONST(SUCCESS));
  int32_t v = 0;
  assert(OBJECT_CALL(&it, StepValue, &v) == STATUS_CONST(SUCCESS) && v == 300);
  assert(OBJECT_CALL(&it, StepValue, &v) == STATUS_CONST(SUCCESS) && v == 200);
  assert(OBJECT_CALL(&it, StepValue, &v) == STATUS_CONST(SUCCESS) && v == 100);
  assert(!OBJECT_CALL_0(&it, HasNext));

  assert(OBJECT_CALL(&stack, Pop, &v) == STATUS_CONST(SUCCESS) && v == 300);
  assert(OBJECT_CALL(&stack, Peek, &v) == STATUS_CONST(SUCCESS) && v == 200);
  assert(OBJECT_CALL_0(&stack, Dispose) == STATUS_CONST(SUCCESS));

  /* Queue FIFO + Compact + Iterator */
  MODULE_TYPED(Array, Queue, Int32) queue;
  assert(MODULE_FUNC(Array, Queue, Int32, Create)(&queue, 4U) ==
         STATUS_CONST(SUCCESS));
  for (int32_t i = 1; i <= 5; ++i) {
    assert(OBJECT_CALL(&queue, Enqueue, i * 10) == STATUS_CONST(SUCCESS));
  }
  assert(OBJECT_CALL(&queue, Dequeue, &v) == STATUS_CONST(SUCCESS) && v == 10);
  assert(OBJECT_CALL(&queue, Dequeue, &v) == STATUS_CONST(SUCCESS) && v == 20);
  assert(OBJECT_CALL_0(&queue, Size) == 3U);

  /* Queue Begin starts at current head (30, 40, 50) */
  assert(OBJECT_CALL(&queue, Begin, &it) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL(&it, StepValue, &v) == STATUS_CONST(SUCCESS) && v == 30);
  assert(OBJECT_CALL(&it, StepValue, &v) == STATUS_CONST(SUCCESS) && v == 40);
  assert(OBJECT_CALL(&it, StepValue, &v) == STATUS_CONST(SUCCESS) && v == 50);
  assert(!OBJECT_CALL_0(&it, HasNext));

  assert(OBJECT_CALL_0(&queue, Dispose) == STATUS_CONST(SUCCESS));
}

static void NS(ArrayTest, RingAndCircularIterator)(void) {
  int32_t backing[4] = {0};
  MODULE_TYPED(Array, Ring, Int32) ring;
  assert(MODULE_FUNC(Array, Ring, Int32, FromBuffer)(&ring, backing, 4U) ==
         STATUS_CONST(SUCCESS));

  assert(OBJECT_CALL(&ring, PushBack, 1) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL(&ring, PushBack, 2) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL(&ring, PushBack, 3) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL(&ring, PushBack, 4) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL_0(&ring, IsFull));
  assert(OBJECT_CALL(&ring, PushBack, 5) == STATUS_CONST(INSUFFICIENT_SPACE));

  /* Overwrite oldest (1) with 5, and (2) with 6 -> logical: [3, 4, 5, 6] */
  int32_t dropped = 0;
  assert(OBJECT_CALL(&ring, Overwrite, 5, &dropped) == STATUS_CONST(SUCCESS));
  assert(dropped == 1);
  assert(OBJECT_CALL(&ring, Overwrite, 6, NULL) == STATUS_CONST(SUCCESS));

  MODULE_TYPED(Iterator, Circular, Int32) cit;
  assert(OBJECT_CALL(&ring, Begin, &cit) == STATUS_CONST(SUCCESS));
  int32_t expected = 3;
  while (OBJECT_CALL_0(&cit, HasNext)) {
    int32_t item = 0;
    assert(OBJECT_CALL(&cit, StepValue, &item) == STATUS_CONST(SUCCESS));
    assert(item == expected);
    expected++;
  }
  assert(expected == 7);

  /* Heap-allocated Ring */
  MODULE_TYPED(Array, Ring, Int32) heapRing;
  assert(MODULE_FUNC(Array, Ring, Int32, Create)(&heapRing, 8U) ==
         STATUS_CONST(SUCCESS));
  int32_t writeBatch[3] = {11, 22, 33};
  size_t written = 0U;
  assert(OBJECT_CALL(&heapRing, Write, writeBatch, 3U, &written) ==
         STATUS_CONST(SUCCESS));
  assert(written == 3U);

  int32_t readBatch[3] = {0};
  size_t readCount = 0U;
  assert(OBJECT_CALL(&heapRing, Read, readBatch, 3U, &readCount) ==
         STATUS_CONST(SUCCESS));
  assert(readCount == 3U);
  assert(readBatch[0] == 11 && readBatch[1] == 22 && readBatch[2] == 33);
  assert(OBJECT_CALL_0(&heapRing, Dispose) == STATUS_CONST(SUCCESS));
}

static void NS(ArrayTest, HeapPriorityQueue)(void) {
  /* Min-Heap from buffer */
  int32_t raw[6] = {40, 10, 50, 20, 60, 30};
  MODULE_TYPED(Array, Heap, Int32) minHeap;
  assert(MODULE_FUNC(Array, Heap, Int32, FromBuffer)(&minHeap, raw, 6U) ==
         STATUS_CONST(SUCCESS));

  int32_t topVal = 0;
  assert(OBJECT_CALL(&minHeap, Peek, &topVal) == STATUS_CONST(SUCCESS) &&
         topVal == 10);

  /* Level-order iterator on Heap */
  MODULE_TYPED(Iterator, Contiguous, Int32) hit;
  assert(OBJECT_CALL(&minHeap, Begin, &hit) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL_0(&hit, Remaining) == 6U);

  int32_t expected = 10;
  while (!OBJECT_CALL_0(&minHeap, IsEmpty)) {
    int32_t popped = 0;
    assert(OBJECT_CALL(&minHeap, Pop, &popped) == STATUS_CONST(SUCCESS));
    assert(popped == expected);
    expected += 10;
  }
  assert(OBJECT_CALL_0(&minHeap, Dispose) == STATUS_CONST(SUCCESS));

  /* Max-Heap */
  MODULE_TYPED(Array, Heap, Int32) maxHeap;
  assert(MODULE_FUNC(Array, Heap, Int32, CreateMax)(&maxHeap, 4U) ==
         STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL(&maxHeap, Push, 15) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL(&maxHeap, Push, 45) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL(&maxHeap, Push, 30) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL(&maxHeap, Pop, &topVal) == STATUS_CONST(SUCCESS) &&
         topVal == 45);
  assert(OBJECT_CALL(&maxHeap, Pop, &topVal) == STATUS_CONST(SUCCESS) &&
         topVal == 30);
  assert(OBJECT_CALL(&maxHeap, Pop, &topVal) == STATUS_CONST(SUCCESS) &&
         topVal == 15);
  assert(OBJECT_CALL_0(&maxHeap, Dispose) == STATUS_CONST(SUCCESS));

  /* Custom Comparator Closure Heap (by absolute value ascending) */
  MODULE_TYPED(Array, Heap, Int32) absHeap;
  MODULE_CLOSURE(Array, Heap, Int32, Comparator) absCmp =
      NS_JOIN(MODULE_CLOSURE_FUNC(Array, Heap, Int32, Comparator), Bind)(
          MODULE_IFUNC(ArrayTest, CompareAbsAsc), NULL);
  assert(MODULE_FUNC(Array, Heap, Int32, CreateWithComparator)(
             &absHeap, 4U, absCmp) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL(&absHeap, Push, -50) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL(&absHeap, Push, 2) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL(&absHeap, Push, -10) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL(&absHeap, Pop, &topVal) == STATUS_CONST(SUCCESS) &&
         topVal == 2);
  assert(OBJECT_CALL(&absHeap, Pop, &topVal) == STATUS_CONST(SUCCESS) &&
         topVal == -10);
  assert(OBJECT_CALL(&absHeap, Pop, &topVal) == STATUS_CONST(SUCCESS) &&
         topVal == -50);
  assert(OBJECT_CALL_0(&absHeap, Dispose) == STATUS_CONST(SUCCESS));
}

static void NS(ArrayTest, CustomStructVector)(void) {
  MODULE_TYPED(Array, Vector, PairInt32) pairVec;
  assert(MODULE_FUNC(Array, Vector, PairInt32, Create)(&pairVec, 2U) ==
         STATUS_CONST(SUCCESS));

  MODULE_TYPED(Struct, Pair, Int32) p1 = {.value1 = 1, .value2 = 10};
  MODULE_TYPED(Struct, Pair, Int32) p2 = {.value1 = 2, .value2 = 20};
  assert(OBJECT_CALL(&pairVec, PushBack, p1) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL(&pairVec, PushBack, p2) == STATUS_CONST(SUCCESS));

  MODULE_TYPED(Iterator, Contiguous, PairInt32) it;
  assert(OBJECT_CALL(&pairVec, Begin, &it) == STATUS_CONST(SUCCESS));

  MODULE_TYPED(Struct, Pair, Int32) outPair;
  assert(OBJECT_CALL(&it, StepValue, &outPair) == STATUS_CONST(SUCCESS));
  assert(outPair.value1 == 1 && outPair.value2 == 10);
  assert(OBJECT_CALL(&it, StepValue, &outPair) == STATUS_CONST(SUCCESS));
  assert(outPair.value1 == 2 && outPair.value2 == 20);
  assert(!OBJECT_CALL_0(&it, HasNext));

  assert(OBJECT_CALL_0(&pairVec, Dispose) == STATUS_CONST(SUCCESS));
}

int main(void) {
  NS(ArrayTest, VectorAndSpan)();
  NS(ArrayTest, StackAndQueue)();
  NS(ArrayTest, RingAndCircularIterator)();
  NS(ArrayTest, HeapPriorityQueue)();
  NS(ArrayTest, CustomStructVector)();
  printf("Array tests passed!\n");
  return 0;
}
