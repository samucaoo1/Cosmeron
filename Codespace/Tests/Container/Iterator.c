#include "../../../Cosmeron/Modules/Container/Iterator/Iterator.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

/* Internal helpers for closures in tests (strictly namespaced) */
INTERNAL_DEF(
    IteratorTest,
    PROCEDURE(DoubleElement, (int32_t *element, void *context), {
      int32_t *callCount = (int32_t *)context;
      if (callCount) {
        (*callCount)++;
      }
      if (element) {
        *element *= 2;
      }
    }),
    PROCEDURE(SumElement, (int32_t value, void *context), {
      int64_t *sum = (int64_t *)context;
      if (sum) {
        *sum += value;
      }
    }),
    PROCEDURE(SumIndexedElement, (size_t index, int32_t value, void *context), {
      int64_t *weightedSum = (int64_t *)context;
      if (weightedSum) {
        *weightedSum += (int64_t)index * (int64_t)value;
      }
    }),
    PREDICATE(IsGreaterThanThreshold, (int32_t value, void *context), {
      int32_t threshold = context ? *(const int32_t *)context : 0;
      return value > threshold;
    }),
    PREDICATE(IsEven, (int32_t value, void *context), {
      (void)context;
      return (value % 2) == 0;
    }))

static void NS(IteratorTest, ContiguousForwardAndReverse)(void) {
  int32_t data[5] = {10, 20, 30, 40, 50};
  MODULE_TYPED(Iterator, Contiguous, Int32) iter;
  MODULE_TYPED(Iterator, Contiguous, Int32) endIter;

  assert(MODULE_FUNC(Iterator, Contiguous, Int32, FromBuffer)(
             &iter, data, 5U, 0U, false) == STATUS_CONST(SUCCESS));
  assert(MODULE_FUNC(Iterator, Contiguous, Int32, FromBuffer)(
             &endIter, data, 5U, 5U, false) == STATUS_CONST(SUCCESS));

  assert(OBJECT_CALL_0(&iter, IsValid));
  assert(OBJECT_CALL_0(&iter, HasNext));
  assert(!OBJECT_CALL_0(&iter, HasPrev));
  assert(OBJECT_CALL_0(&iter, Remaining) == 5U);
  assert(OBJECT_CALL_0(&iter, Index) == 0U);
  assert(!OBJECT_CALL(&iter, Equal, &endIter));

  int32_t val = 0;
  assert(OBJECT_CALL(&iter, Value, &val) == STATUS_CONST(SUCCESS));
  assert(val == 10);

  /* Mutate via Get / Set */
  int32_t *ptr = NULL;
  assert(OBJECT_CALL(&iter, Get, &ptr) == STATUS_CONST(SUCCESS));
  assert(ptr == &data[0]);
  *ptr = 11;
  assert(data[0] == 11);
  assert(OBJECT_CALL(&iter, Set, 10) == STATUS_CONST(SUCCESS));
  assert(data[0] == 10);

  /* StepValue traversal */
  int32_t expected = 10;
  size_t count = 0U;
  while (OBJECT_CALL_0(&iter, HasNext)) {
    int32_t item = 0;
    assert(OBJECT_CALL(&iter, StepValue, &item) == STATUS_CONST(SUCCESS));
    assert(item == expected);
    expected += 10;
    count++;
  }
  assert(count == 5U);
  assert(!OBJECT_CALL_0(&iter, HasNext));
  assert(OBJECT_CALL(&iter, Equal, &endIter));

  /* Prev & Distance & Advance */
  assert(OBJECT_CALL_0(&iter, Prev) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL_0(&iter, Index) == 4U);
  assert(OBJECT_CALL(&iter, Value, &val) == STATUS_CONST(SUCCESS) && val == 50);

  assert(OBJECT_CALL(&iter, Advance, -3) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL_0(&iter, Index) == 1U);
  assert(OBJECT_CALL(&iter, Value, &val) == STATUS_CONST(SUCCESS) && val == 20);

  ptrdiff_t dist = 0;
  assert(OBJECT_CALL(&iter, Distance, &endIter, &dist) == STATUS_CONST(SUCCESS));
  assert(dist == 4);

  /* Reverse Contiguous Iterator */
  MODULE_TYPED(Iterator, Contiguous, Int32) riter;
  assert(MODULE_FUNC(Iterator, Contiguous, Int32, FromBuffer)(
             &riter, data, 5U, 0U, true) == STATUS_CONST(SUCCESS));
  expected = 50;
  count = 0U;
  while (OBJECT_CALL_0(&riter, HasNext)) {
    int32_t item = 0;
    assert(OBJECT_CALL(&riter, StepValue, &item) == STATUS_CONST(SUCCESS));
    assert(item == expected);
    expected -= 10;
    count++;
  }
  assert(count == 5U);
}

static void NS(IteratorTest, ContiguousForEachAndStream)(void) {
  int32_t data[4] = {1, 2, 3, 4};
  MODULE_TYPED(Iterator, Contiguous, Int32) iter;
  assert(MODULE_FUNC(Iterator, Contiguous, Int32, FromBuffer)(
             &iter, data, 4U, 0U, false) == STATUS_CONST(SUCCESS));

  int32_t callCount = 0;
  MODULE_CLOSURE(Iterator, Contiguous, Int32, Visitor) visitor =
      NS_JOIN(MODULE_CLOSURE_FUNC(Iterator, Contiguous, Int32, Visitor), Bind)(
          MODULE_IFUNC(IteratorTest, DoubleElement), &callCount);

  assert(OBJECT_CALL(&iter, ForEach, visitor) == STATUS_CONST(SUCCESS));
  assert(callCount == 4);
  assert(data[0] == 2 && data[1] == 4 && data[2] == 6 && data[3] == 8);

  /* Reset and convert to Polymorphic Stream Iterator */
  assert(OBJECT_CALL_0(&iter, Reset) == STATUS_CONST(SUCCESS));
  MODULE_TYPED(Iterator, Stream, Int32) stream;
  assert(OBJECT_CALL(&iter, ToStream, &stream) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL_0(&stream, IsValid));
  assert(OBJECT_CALL_0(&stream, HasNext));

  int64_t sum = 0;
  MODULE_CLOSURE(Iterator, Stream, Int32, Visitor) sumVisitor =
      NS_JOIN(MODULE_CLOSURE_FUNC(Iterator, Stream, Int32, Visitor), Bind)(
          MODULE_IFUNC(IteratorTest, SumElement), &sum);
  assert(OBJECT_CALL(&stream, ForEach, sumVisitor) == STATUS_CONST(SUCCESS));
  assert(sum == (2 + 4 + 6 + 8));
  assert(!OBJECT_CALL_0(&stream, HasNext));

  /* Stream Reset, FindIf, Any, All, None, CollectToBuffer */
  assert(OBJECT_CALL_0(&stream, Reset) == STATUS_CONST(SUCCESS));
  int32_t threshold = 5;
  MODULE_CLOSURE(Iterator, Stream, Int32, Predicate) gtPred =
      NS_JOIN(MODULE_CLOSURE_FUNC(Iterator, Stream, Int32, Predicate), Bind)(
          MODULE_IFUNC(IteratorTest, IsGreaterThanThreshold), &threshold);

  int32_t found = 0;
  assert(OBJECT_CALL(&stream, FindIf, gtPred, &found) == STATUS_CONST(SUCCESS));
  assert(found == 6);

  assert(OBJECT_CALL_0(&stream, Reset) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL(&stream, Any, gtPred));

  assert(OBJECT_CALL_0(&stream, Reset) == STATUS_CONST(SUCCESS));
  assert(!OBJECT_CALL(&stream, All, gtPred));

  MODULE_CLOSURE(Iterator, Stream, Int32, Predicate) evenPred =
      NS_JOIN(MODULE_CLOSURE_FUNC(Iterator, Stream, Int32, Predicate), Bind)(
          MODULE_IFUNC(IteratorTest, IsEven), NULL);
  assert(OBJECT_CALL_0(&stream, Reset) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL(&stream, All, evenPred));

  int32_t highThreshold = 100;
  MODULE_CLOSURE(Iterator, Stream, Int32, Predicate) gt100Pred =
      NS_JOIN(MODULE_CLOSURE_FUNC(Iterator, Stream, Int32, Predicate), Bind)(
          MODULE_IFUNC(IteratorTest, IsGreaterThanThreshold), &highThreshold);
  assert(OBJECT_CALL_0(&stream, Reset) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL(&stream, None, gt100Pred));

  /* ForEachIndexed */
  assert(OBJECT_CALL_0(&stream, Reset) == STATUS_CONST(SUCCESS));
  int64_t weightedSum = 0;
  MODULE_CLOSURE(Iterator, Stream, Int32, IndexedVisitor) idxVisitor =
      NS_JOIN(MODULE_CLOSURE_FUNC(Iterator, Stream, Int32, IndexedVisitor),
              Bind)(MODULE_IFUNC(IteratorTest, SumIndexedElement),
                    &weightedSum);
  assert(OBJECT_CALL(&stream, ForEachIndexed, idxVisitor) ==
         STATUS_CONST(SUCCESS));
  /* 0*2 + 1*4 + 2*6 + 3*8 = 0 + 4 + 12 + 24 = 40 */
  assert(weightedSum == 40);

  /* Skip & CollectToBuffer */
  assert(OBJECT_CALL_0(&stream, Reset) == STATUS_CONST(SUCCESS));
  size_t skipped = 0U;
  assert(OBJECT_CALL(&stream, Skip, 1U, &skipped) == STATUS_CONST(SUCCESS));
  assert(skipped == 1U);

  int32_t collected[4] = {0};
  size_t collectedCount = 0U;
  assert(OBJECT_CALL(&stream, CollectToBuffer, collected, 4U, &collectedCount) ==
         STATUS_CONST(SUCCESS));
  assert(collectedCount == 3U);
  assert(collected[0] == 4 && collected[1] == 6 && collected[2] == 8);
}

static void NS(IteratorTest, CircularIterator)(void) {
  /* Ring buffer of capacity 5, head at index 3, count = 4:
   * Physical layout: [300, 400, 999, 100, 200]
   * Logical order:   100 (idx 3), 200 (idx 4), 300 (idx 0), 400 (idx 1) */
  int32_t ringBuf[5] = {300, 400, 999, 100, 200};
  MODULE_TYPED(Iterator, Circular, Int32) citer;

  assert(MODULE_FUNC(Iterator, Circular, Int32, FromRing)(
             &citer, ringBuf, 5U, 3U, 4U, 0U, false) == STATUS_CONST(SUCCESS));

  int32_t expected = 100;
  size_t seen = 0U;
  while (OBJECT_CALL_0(&citer, HasNext)) {
    int32_t v = 0;
    assert(OBJECT_CALL(&citer, StepValue, &v) == STATUS_CONST(SUCCESS));
    assert(v == expected);
    expected += 100;
    seen++;
  }
  assert(seen == 4U);

  /* Reverse circular iterator */
  MODULE_TYPED(Iterator, Circular, Int32) rciter;
  assert(MODULE_FUNC(Iterator, Circular, Int32, FromRing)(
             &rciter, ringBuf, 5U, 3U, 4U, 0U, true) == STATUS_CONST(SUCCESS));
  expected = 400;
  seen = 0U;
  while (OBJECT_CALL_0(&rciter, HasNext)) {
    int32_t v = 0;
    assert(OBJECT_CALL(&rciter, StepValue, &v) == STATUS_CONST(SUCCESS));
    assert(v == expected);
    expected -= 100;
    seen++;
  }
  assert(seen == 4U);

  /* Circular to Stream */
  assert(OBJECT_CALL_0(&citer, Reset) == STATUS_CONST(SUCCESS));
  MODULE_TYPED(Iterator, Stream, Int32) cstream;
  assert(OBJECT_CALL(&citer, ToStream, &cstream) == STATUS_CONST(SUCCESS));
  size_t streamCount = 0U;
  assert(OBJECT_CALL(&cstream, Count, &streamCount) == STATUS_CONST(SUCCESS));
  assert(streamCount == 4U);
}

int main(void) {
  NS(IteratorTest, ContiguousForwardAndReverse)();
  NS(IteratorTest, ContiguousForEachAndStream)();
  NS(IteratorTest, CircularIterator)();
  printf("Iterator tests passed!\n");
  return 0;
}
