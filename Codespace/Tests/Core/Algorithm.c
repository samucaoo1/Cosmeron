#include "../../Cosmeron/Core/Algorithm/Comparison.h"
#include "../../Cosmeron/Core/Algorithm/Repeat.h"

#include <string.h>

typedef struct AlgorithmTest_Point {
  int x;
  int y;
} AlgorithmTest_Point;

TEMPLATE_DECLARE(REPEAT, Point, AlgorithmTest_Point);

static CMPOUT compare_int_pointer(const void *left, const void *right,
                                  void *context) {
  (void)context;
  const int leftValue = *(const int *)left;
  const int rightValue = *(const int *)right;
  return COMPARISON_FUNC(Int)(leftValue, rightValue);
}

static void increment_counter(void *context) {
  (*(int *)context)++;
}

static void add_index_to_counter(size_t index, void *context) {
  *(size_t *)context += index;
}

static OPSTATUS step_with_limit(void *context) {
  int *remaining = (int *)context;
  if (*remaining <= 0)
    return STATUS_CONST(OUT_OF_RANGE);
  (*remaining)--;
  return STATUS_CONST(SUCCESS);
}

static OPSTATUS indexed_step_with_limit(size_t index, void *context) {
  size_t *stopAt = (size_t *)context;
  if (index == *stopAt)
    return STATUS_CONST(OUT_OF_RANGE);
  return STATUS_CONST(SUCCESS);
}

static bool countdown_predicate(void *context) {
  int *remaining = (int *)context;
  if (*remaining <= 0)
    return false;
  (*remaining)--;
  return true;
}

static bool indexed_below_threshold(size_t index, void *context) {
  size_t threshold = *(const size_t *)context;
  return index < threshold;
}

static AlgorithmTest_Point generate_constant_point(void *context) {
  int base = *(const int *)context;
  AlgorithmTest_Point point = {base, base * 2};
  return point;
}

static AlgorithmTest_Point generate_indexed_point(size_t index, void *context) {
  int offset = *(const int *)context;
  AlgorithmTest_Point point = {(int)index + offset, (int)(index * 10U)};
  return point;
}

int main(void) {
  const unsigned char lowerBytes[] = {1, 2};
  const unsigned char higherBytes[] = {1, 3};
  int left = 4;
  int right = 9;

  if (COMPARISON_FUNC(Int)(1, 2) != COMPARISON_CONST(LOWER))
    return 1;
  if (COMPARISON_FUNC(Double)(3.0, 3.0) != COMPARISON_CONST(EQUAL))
    return 2;
  if (COMPARISON_FUNC(CString)("b", "a") != COMPARISON_CONST(HIGHER))
    return 3;
  if (COMPARISON_FUNC(LongDouble)(1.0L, 2.0L) != COMPARISON_CONST(LOWER))
    return 4;

  if (COMPARISON_FUNC(Bytes)(lowerBytes, sizeof(lowerBytes), higherBytes,
                             sizeof(higherBytes)) != COMPARISON_CONST(LOWER))
    return 5;
  if (COMPARISON_FUNC(Bytes)(lowerBytes, 1, higherBytes, 2) !=
      COMPARISON_CONST(LOWER))
    return 6;

  TCALLBACK(Comparison) comparison =
      CALLBACK_FUNC(Comparison_Bind)(compare_int_pointer, NULL);
  if (CALLBACK_FUNC(Comparison_Invoke)(comparison, &left, &right) !=
      COMPARISON_CONST(LOWER))
    return 7;

  /* Test Comparison FunctionTable call */
  if (PACKAGE_CALL(ALGORITHM_NS(COMPARISON_MOD), Int, 1, 2) !=
      COMPARISON_CONST(LOWER))
    return 8;
  if (PACKAGE_CALL(ALGORITHM_NS(COMPARISON_MOD), Float, 2.0f, 1.0f) !=
      COMPARISON_CONST(HIGHER))
    return 9;

  /* ======================================================================== */
  /* Test Algorithm_Repeat: Callbacks                                         */
  /* ======================================================================== */

  int counter = 0;
  TCALLBACK(RepeatProcedure) proc =
      CALLBACK_FUNC(RepeatProcedure_Bind)(increment_counter, &counter);
  TCALLBACK(RepeatProcedure) invalidProc =
      CALLBACK_FUNC(RepeatProcedure_Bind)(NULL, NULL);

  REPEAT_FUNC(Procedure)(5U, proc);
  if (counter != 5)
    return 10;
  REPEAT_FUNC(Procedure)(0U, invalidProc);
  REPEAT_FUNC(Procedure)(3U, invalidProc);
  if (counter != 5)
    return 11;

  size_t indexSum = 0U;
  TCALLBACK(RepeatIndexedProcedure) indexedProc =
      CALLBACK_FUNC(RepeatIndexedProcedure_Bind)(add_index_to_counter,
                                                 &indexSum);
  REPEAT_FUNC(IndexedProcedure)(4U, indexedProc);
  if (indexSum != 6U) /* 0 + 1 + 2 + 3 */
    return 12;

  int opRemaining = 3;
  TCALLBACK(RepeatOperation) op =
      CALLBACK_FUNC(RepeatOperation_Bind)(step_with_limit, &opRemaining);
  TCALLBACK(RepeatOperation) invalidOp =
      CALLBACK_FUNC(RepeatOperation_Bind)(NULL, NULL);

  if (REPEAT_FUNC(Operation)(0U, invalidOp) != STATUS_CONST(SUCCESS))
    return 13;
  if (REPEAT_FUNC(Operation)(2U, invalidOp) != STATUS_CONST(INVALID_ARGUMENT))
    return 14;
  if (REPEAT_FUNC(Operation)(2U, op) != STATUS_CONST(SUCCESS) ||
      opRemaining != 1)
    return 15;
  if (REPEAT_FUNC(Operation)(3U, op) != STATUS_CONST(OUT_OF_RANGE) ||
      opRemaining != 0)
    return 16;

  size_t stopAt = 2U;
  TCALLBACK(RepeatIndexedOperation) indexedOp =
      CALLBACK_FUNC(RepeatIndexedOperation_Bind)(indexed_step_with_limit,
                                                 &stopAt);
  if (REPEAT_FUNC(IndexedOperation)(2U, indexedOp) != STATUS_CONST(SUCCESS))
    return 17;
  if (REPEAT_FUNC(IndexedOperation)(4U, indexedOp) !=
      STATUS_CONST(OUT_OF_RANGE))
    return 18;

  int predRemaining = 2;
  TCALLBACK(RepeatPredicate) pred =
      CALLBACK_FUNC(RepeatPredicate_Bind)(countdown_predicate, &predRemaining);
  TCALLBACK(RepeatPredicate) invalidPred =
      CALLBACK_FUNC(RepeatPredicate_Bind)(NULL, NULL);

  if (!REPEAT_FUNC(Predicate)(0U, invalidPred))
    return 19;
  if (REPEAT_FUNC(Predicate)(1U, invalidPred))
    return 20;
  if (!REPEAT_FUNC(Predicate)(2U, pred) || predRemaining != 0)
    return 21;
  if (REPEAT_FUNC(Predicate)(1U, pred))
    return 22;

  size_t threshold = 3U;
  TCALLBACK(RepeatIndexedPredicate) indexedPred =
      CALLBACK_FUNC(RepeatIndexedPredicate_Bind)(indexed_below_threshold,
                                                 &threshold);
  if (!REPEAT_FUNC(IndexedPredicate)(3U, indexedPred))
    return 23;
  if (REPEAT_FUNC(IndexedPredicate)(4U, indexedPred))
    return 24;

  /* ======================================================================== */
  /* Test Algorithm_Repeat: Bytes, Primitives & CString                       */
  /* ======================================================================== */

  const unsigned char pattern[2] = {0xAAU, 0x55U};
  unsigned char byteBuffer[6] = {0};
  if (REPEAT_FUNC(Bytes)(byteBuffer, pattern, sizeof(pattern), 3U) !=
      STATUS_CONST(SUCCESS))
    return 25;
  for (size_t i = 0U; i < 3U; ++i) {
    if (byteBuffer[i * 2U] != 0xAAU || byteBuffer[i * 2U + 1U] != 0x55U)
      return 26;
  }
  if (REPEAT_FUNC(Bytes)(NULL, NULL, 0U, 5U) != STATUS_CONST(SUCCESS))
    return 27;
  if (REPEAT_FUNC(Bytes)(NULL, pattern, sizeof(pattern), 1U) !=
      STATUS_CONST(INVALID_ARGUMENT))
    return 28;
  /* Overlap detection */
  if (REPEAT_FUNC(Bytes)(byteBuffer + 1U, byteBuffer, 2U, 2U) !=
      STATUS_CONST(INVALID_ARGUMENT))
    return 29;

  int intArray[4] = {0};
  if (REPEAT_FUNC(Int)(intArray, 42, 4U) != STATUS_CONST(SUCCESS))
    return 30;
  for (size_t i = 0U; i < 4U; ++i) {
    if (intArray[i] != 42)
      return 31;
  }

  const char *strArray[3] = {NULL};
  if (REPEAT_FUNC(CString)(strArray, "cosmeron", 3U) != STATUS_CONST(SUCCESS))
    return 32;
  for (size_t i = 0U; i < 3U; ++i) {
    if (strcmp(strArray[i], "cosmeron") != 0)
      return 33;
  }

  if (PACKAGE_CALL(ALGORITHM_NS(REPEAT_MOD), Int, intArray, 99, 4U) !=
      STATUS_CONST(SUCCESS))
    return 34;
  if (intArray[0] != 99 || intArray[3] != 99)
    return 35;

  /* ======================================================================== */
  /* Test TEMPLATE_REPEAT                                                     */
  /* ======================================================================== */

  AlgorithmTest_Point points[3] = {{0, 0}};
  AlgorithmTest_Point fillValue = {7, 14};
  if (REPEAT_FUNC(Point_Fill)(points, fillValue, 3U) != STATUS_CONST(SUCCESS))
    return 36;
  for (size_t i = 0U; i < 3U; ++i) {
    if (points[i].x != 7 || points[i].y != 14)
      return 37;
  }

  int baseVal = 5;
  TCALLBACK(REPEAT_INS(Generator_Point)) gen =
      GNS2(CALLBACK_FUNC(REPEAT_INS(Generator_Point)), Bind)(
          generate_constant_point, &baseVal);
  if (REPEAT_FUNC(Point_Generate)(points, 3U, gen) != STATUS_CONST(SUCCESS))
    return 38;
  for (size_t i = 0U; i < 3U; ++i) {
    if (points[i].x != 5 || points[i].y != 10)
      return 39;
  }

  int offsetVal = 100;
  TCALLBACK(REPEAT_INS(IndexedGenerator_Point)) indexedGen =
      GNS2(CALLBACK_FUNC(REPEAT_INS(IndexedGenerator_Point)), Bind)(
          generate_indexed_point, &offsetVal);
  if (REPEAT_FUNC(Point_GenerateIndexed)(points, 3U, indexedGen) !=
      STATUS_CONST(SUCCESS))
    return 40;
  for (size_t i = 0U; i < 3U; ++i) {
    if (points[i].x != (int)i + 100 || points[i].y != (int)(i * 10U))
      return 41;
  }

  return 0;
}
