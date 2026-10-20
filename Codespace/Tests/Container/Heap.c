#include "Test.h"
#include "../../Cosmeron/Modules/Container/Array/Heap.h"

#include <stdint.h>
#include <string.h>

static int Test_LifecycleAndArguments(void) {
  FLAT_HEAP_TYPE(int) heap;

  TEST_ASSERT(FLAT_HEAP_FUNC(int, Init)(NULL) == STATUS_CONST(INVALID_ARGUMENT));
  TEST_ASSERT(FLAT_HEAP_FUNC(int, Init)(&heap) == STATUS_CONST(SUCCESS));
  TEST_ASSERT(FLAT_HEAP_FUNC(int, Empty)(&heap));
  TEST_ASSERT(FLAT_HEAP_FUNC(int, Size)(&heap) == 0U);
  TEST_ASSERT(FLAT_HEAP_FUNC(int, Capacity)(&heap) == 0U);

  int out = -1;
  TEST_ASSERT(FLAT_HEAP_FUNC(int, Peek)(&heap, NULL) == STATUS_CONST(INVALID_ARGUMENT));
  TEST_ASSERT(FLAT_HEAP_FUNC(int, Peek)(&heap, &out) == STATUS_CONST(OUT_OF_RANGE));
  TEST_ASSERT(out == -1);

  TEST_ASSERT(FLAT_HEAP_FUNC(int, Pop)(&heap, NULL) == STATUS_CONST(INVALID_ARGUMENT));
  TEST_ASSERT(FLAT_HEAP_FUNC(int, Pop)(&heap, &out) == STATUS_CONST(OUT_OF_RANGE));
  TEST_ASSERT(out == -1);

  int *topPtr = NULL;
  TEST_ASSERT(FLAT_HEAP_FUNC(int, Top)(&heap, NULL) == STATUS_CONST(INVALID_ARGUMENT));
  TEST_ASSERT(FLAT_HEAP_FUNC(int, Top)(&heap, &topPtr) == STATUS_CONST(OUT_OF_RANGE));
  TEST_ASSERT(topPtr == NULL);

  FLAT_HEAP_FUNC(int, Destroy)(&heap);
  TEST_ASSERT(FLAT_HEAP_FUNC(int, Capacity)(&heap) == 0U);
  TEST_ASSERT(FLAT_HEAP_FUNC(int, Size)(&heap) == 0U);
  TEST_ASSERT(heap.data == NULL);

  FLAT_HEAP_FUNC(int, Destroy)(NULL);
  return 0;
}

static int Test_MinHeapBasicOperations(void) {
  FLAT_HEAP_TYPE(int) heap;
  TEST_ASSERT(FLAT_HEAP_FUNC(int, Init)(&heap) == STATUS_CONST(SUCCESS));

  const int values[] = {50, 20, 80, 10, 30, 70, 5};
  const size_t count = sizeof(values) / sizeof(values[0]);

  for (size_t i = 0U; i < count; ++i) {
    TEST_ASSERT(FLAT_HEAP_FUNC(int, Push)(&heap, values[i]) == STATUS_CONST(SUCCESS));
    TEST_ASSERT(FLAT_HEAP_FUNC(int, Size)(&heap) == i + 1U);
  }

  int minVal = 0;
  TEST_ASSERT(FLAT_HEAP_FUNC(int, Peek)(&heap, &minVal) == STATUS_CONST(SUCCESS));
  TEST_ASSERT(minVal == 5);

  int *topPtr = NULL;
  TEST_ASSERT(FLAT_HEAP_FUNC(int, Top)(&heap, &topPtr) == STATUS_CONST(SUCCESS));
  TEST_ASSERT(topPtr != NULL && *topPtr == 5);

  const int expectedSorted[] = {5, 10, 20, 30, 50, 70, 80};
  for (size_t i = 0U; i < count; ++i) {
    int popped = 0;
    TEST_ASSERT(FLAT_HEAP_FUNC(int, Pop)(&heap, &popped) == STATUS_CONST(SUCCESS));
    TEST_ASSERT(popped == expectedSorted[i]);
    TEST_ASSERT(FLAT_HEAP_FUNC(int, Size)(&heap) == count - 1U - i);
  }

  TEST_ASSERT(FLAT_HEAP_FUNC(int, Empty)(&heap));
  int poppedAfter = -1;
  TEST_ASSERT(FLAT_HEAP_FUNC(int, Pop)(&heap, &poppedAfter) == STATUS_CONST(OUT_OF_RANGE));
  TEST_ASSERT(poppedAfter == -1);

  FLAT_HEAP_FUNC(int, Destroy)(&heap);
  return 0;
}

static int Test_MaxHeap(void) {
  FLAT_HEAP_TYPE(int) heap;
  TEST_ASSERT(FLAT_HEAP_FUNC(int, InitMax)(&heap) == STATUS_CONST(SUCCESS));

  const int values[] = {50, 20, 80, 10, 30, 70, 5};
  const size_t count = sizeof(values) / sizeof(values[0]);

  for (size_t i = 0U; i < count; ++i) {
    TEST_ASSERT(FLAT_HEAP_FUNC(int, Push)(&heap, values[i]) == STATUS_CONST(SUCCESS));
  }

  int maxVal = 0;
  TEST_ASSERT(FLAT_HEAP_FUNC(int, Peek)(&heap, &maxVal) == STATUS_CONST(SUCCESS));
  TEST_ASSERT(maxVal == 80);

  const int expectedSortedDesc[] = {80, 70, 50, 30, 20, 10, 5};
  for (size_t i = 0U; i < count; ++i) {
    int popped = 0;
    TEST_ASSERT(FLAT_HEAP_FUNC(int, Pop)(&heap, &popped) == STATUS_CONST(SUCCESS));
    TEST_ASSERT(popped == expectedSortedDesc[i]);
  }

  TEST_ASSERT(FLAT_HEAP_FUNC(int, Empty)(&heap));
  FLAT_HEAP_FUNC(int, Destroy)(&heap);
  return 0;
}

static int Test_ReplaceAndPushPop(void) {
  FLAT_HEAP_TYPE(int) heap;
  TEST_ASSERT(FLAT_HEAP_FUNC(int, Init)(&heap) == STATUS_CONST(SUCCESS));

  TEST_ASSERT(FLAT_HEAP_FUNC(int, Push)(&heap, 10) == STATUS_CONST(SUCCESS));
  TEST_ASSERT(FLAT_HEAP_FUNC(int, Push)(&heap, 20) == STATUS_CONST(SUCCESS));
  TEST_ASSERT(FLAT_HEAP_FUNC(int, Push)(&heap, 30) == STATUS_CONST(SUCCESS));

  /* Replace 10 with 25 -> new top should be 20 */
  int out = 0;
  TEST_ASSERT(FLAT_HEAP_FUNC(int, Replace)(&heap, 25, &out) == STATUS_CONST(SUCCESS));
  TEST_ASSERT(out == 10);
  int peekVal = 0;
  TEST_ASSERT(FLAT_HEAP_FUNC(int, Peek)(&heap, &peekVal) == STATUS_CONST(SUCCESS));
  TEST_ASSERT(peekVal == 20);

  /* Replace 20 with 5 -> new top should be 5 */
  TEST_ASSERT(FLAT_HEAP_FUNC(int, Replace)(&heap, 5, &out) == STATUS_CONST(SUCCESS));
  TEST_ASSERT(out == 20);
  TEST_ASSERT(FLAT_HEAP_FUNC(int, Peek)(&heap, &peekVal) == STATUS_CONST(SUCCESS));
  TEST_ASSERT(peekVal == 5);

  /* PushPop with 2: 2 is smaller than top (5), so 2 returns immediately, heap stays 5, 25, 30 */
  TEST_ASSERT(FLAT_HEAP_FUNC(int, PushPop)(&heap, 2, &out) == STATUS_CONST(SUCCESS));
  TEST_ASSERT(out == 2);
  TEST_ASSERT(FLAT_HEAP_FUNC(int, Peek)(&heap, &peekVal) == STATUS_CONST(SUCCESS));
  TEST_ASSERT(peekVal == 5);

  /* PushPop with 40: 40 is greater than top (5), so 5 returns, 40 inserted, new top should be 25 */
  TEST_ASSERT(FLAT_HEAP_FUNC(int, PushPop)(&heap, 40, &out) == STATUS_CONST(SUCCESS));
  TEST_ASSERT(out == 5);
  TEST_ASSERT(FLAT_HEAP_FUNC(int, Peek)(&heap, &peekVal) == STATUS_CONST(SUCCESS));
  TEST_ASSERT(peekVal == 25);

  FLAT_HEAP_FUNC(int, Destroy)(&heap);
  return 0;
}

static int Test_FromBufferAndHeapify(void) {
  const int raw[] = {45, 12, 89, 34, 7, 23, 67, 1};
  const size_t count = sizeof(raw) / sizeof(raw[0]);

  FLAT_HEAP_TYPE(int) heap;
  TEST_ASSERT(FLAT_HEAP_FUNC(int, FromBuffer)(&heap, raw, count) == STATUS_CONST(SUCCESS));
  TEST_ASSERT(FLAT_HEAP_FUNC(int, Size)(&heap) == count);

  const int expected[] = {1, 7, 12, 23, 34, 45, 67, 89};
  for (size_t i = 0U; i < count; ++i) {
    int popped = 0;
    TEST_ASSERT(FLAT_HEAP_FUNC(int, Pop)(&heap, &popped) == STATUS_CONST(SUCCESS));
    TEST_ASSERT(popped == expected[i]);
  }

  FLAT_HEAP_FUNC(int, Destroy)(&heap);
  return 0;
}

static int Test_ReserveShrinkClear(void) {
  FLAT_HEAP_TYPE(int) heap;
  TEST_ASSERT(FLAT_HEAP_FUNC(int, Init)(&heap) == STATUS_CONST(SUCCESS));

  TEST_ASSERT(FLAT_HEAP_FUNC(int, Reserve)(&heap, 64U) == STATUS_CONST(SUCCESS));
  TEST_ASSERT(FLAT_HEAP_FUNC(int, Capacity)(&heap) >= 64U);
  TEST_ASSERT(FLAT_HEAP_FUNC(int, Size)(&heap) == 0U);

  for (int i = 0; i < 5; ++i) {
    TEST_ASSERT(FLAT_HEAP_FUNC(int, Push)(&heap, i * 10) == STATUS_CONST(SUCCESS));
  }
  TEST_ASSERT(FLAT_HEAP_FUNC(int, Size)(&heap) == 5U);

  TEST_ASSERT(FLAT_HEAP_FUNC(int, ShrinkToFit)(&heap) == STATUS_CONST(SUCCESS));
  TEST_ASSERT(FLAT_HEAP_FUNC(int, Capacity)(&heap) == 5U);

  FLAT_HEAP_FUNC(int, Clear)(&heap);
  TEST_ASSERT(FLAT_HEAP_FUNC(int, Empty)(&heap));
  TEST_ASSERT(FLAT_HEAP_FUNC(int, Size)(&heap) == 0U);
  TEST_ASSERT(FLAT_HEAP_FUNC(int, Capacity)(&heap) == 5U);

  TEST_ASSERT(FLAT_HEAP_FUNC(int, Push)(&heap, 42) == STATUS_CONST(SUCCESS));
  int peekVal = 0;
  TEST_ASSERT(FLAT_HEAP_FUNC(int, Peek)(&heap, &peekVal) == STATUS_CONST(SUCCESS));
  TEST_ASSERT(peekVal == 42);

  FLAT_HEAP_FUNC(int, Destroy)(&heap);
  return 0;
}

typedef struct TaskItem {
  int priority;
  int id;
} TaskItem;

static CMPOUT CompareTaskItem(TaskItem left, TaskItem right) {
  return (left.priority < right.priority) ? COMPARISON_LOWER_CONST
       : ((left.priority > right.priority) ? COMPARISON_HIGHER_CONST
                                           : COMPARISON_EQUAL_CONST);
}

FLAT_HEAP_IMPLEMENT_ALL_CMP(TaskItem, TaskItem, CompareTaskItem)

static int Test_CustomStructAndComparator(void) {
  FLAT_HEAP_TYPE(TaskItem) taskHeap;
  CONTAINER_API_BIND(taskHeap, FLAT_HEAP_FUNC(TaskItem, functions));
  TEST_ASSERT(FLAT_HEAP_FUNC(TaskItem, Init)(&taskHeap) == STATUS_CONST(SUCCESS));

  const int priorities[] = {30, 10, 40, 15, 50, 90, 20};
  const size_t count = sizeof(priorities) / sizeof(priorities[0]);

  for (size_t i = 0U; i < count; ++i) {
    TaskItem item = {.priority = priorities[i], .id = (int)i};
    TEST_ASSERT(FLAT_HEAP_FUNC(TaskItem, Push)(&taskHeap, item) == STATUS_CONST(SUCCESS));
  }

  const int expectedPriorities[] = {10, 15, 20, 30, 40, 50, 90};
  for (size_t i = 0U; i < count; ++i) {
    TaskItem popped = {0, 0};
    TEST_ASSERT(FLAT_HEAP_FUNC(TaskItem, Pop)(&taskHeap, &popped) == STATUS_CONST(SUCCESS));
    TEST_ASSERT(popped.priority == expectedPriorities[i]);
  }

  FLAT_HEAP_FUNC(TaskItem, Destroy)(&taskHeap);
  return 0;
}

static int Test_FunctionTable(void) {
  FLAT_HEAP_INSTANCE_DECLARE(int, heap);
  TEST_ASSERT(heap.api != NULL);

  TEST_ASSERT(heap.api->empty(&heap));
  TEST_ASSERT(heap.api->size(&heap) == 0U);

  TEST_ASSERT(heap.api->push(&heap, 30) == STATUS_CONST(SUCCESS));
  TEST_ASSERT(heap.api->push(&heap, 10) == STATUS_CONST(SUCCESS));
  TEST_ASSERT(heap.api->push(&heap, 20) == STATUS_CONST(SUCCESS));

  TEST_ASSERT(heap.api->size(&heap) == 3U);
  int peekVal = 0;
  TEST_ASSERT(heap.api->peek(&heap, &peekVal) == STATUS_CONST(SUCCESS));
  TEST_ASSERT(peekVal == 10);

  int popVal = 0;
  TEST_ASSERT(heap.api->pop(&heap, &popVal) == STATUS_CONST(SUCCESS));
  TEST_ASSERT(popVal == 10);
  TEST_ASSERT(heap.api->pop(&heap, &popVal) == STATUS_CONST(SUCCESS));
  TEST_ASSERT(popVal == 20);
  TEST_ASSERT(heap.api->pop(&heap, &popVal) == STATUS_CONST(SUCCESS));
  TEST_ASSERT(popVal == 30);

  heap.api->destroy(&heap);
  return 0;
}

int main(void) {
  if (Test_LifecycleAndArguments() != 0) return 1;
  if (Test_MinHeapBasicOperations() != 0) return 1;
  if (Test_MaxHeap() != 0) return 1;
  if (Test_ReplaceAndPushPop() != 0) return 1;
  if (Test_FromBufferAndHeapify() != 0) return 1;
  if (Test_ReserveShrinkClear() != 0) return 1;
  if (Test_CustomStructAndComparator() != 0) return 1;
  if (Test_FunctionTable() != 0) return 1;
  return 0;
}
