#include "Cosmeron/Core/Algorithm/Comparison.h"
#include "Cosmeron/Core/Algorithm/Repeat.h"
#include "Cosmeron/Core/Algorithm/Repeat.def"

#include <string.h>

typedef struct AlgorithmTest_Point {
  int x;
  int y;
} AlgorithmTest_Point;

TEMPLATE_Algorithm_Repeat(Point, AlgorithmTest_Point);

static CMPOUT compare_int_pointer(const void *left, const void *right,
                                  void *context) {
  (void)context;
  const int leftValue = *(const int *)left;
  const int rightValue = *(const int *)right;
  return Algorithm_Comparison_Int(leftValue, rightValue);
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

  if (Algorithm_Comparison_Int(1, 2) != COMPARISON_CONST(LOWER))
    return 1;
  if (Algorithm_Comparison_Double(3.0, 3.0) != COMPARISON_CONST(EQUAL))
    return 2;
  if (Algorithm_Comparison_CString("b", "a") != COMPARISON_CONST(HIGHER))
    return 3;
  if (Algorithm_Comparison_LongDouble(1.0L, 2.0L) != COMPARISON_CONST(LOWER))
    return 4;

  if (Algorithm_Comparison_Bytes(lowerBytes, sizeof(lowerBytes), higherBytes,
                                sizeof(higherBytes)) != COMPARISON_CONST(LOWER))
    return 5;
  if (Algorithm_Comparison_Bytes(lowerBytes, 1, higherBytes, 2) !=
      COMPARISON_CONST(LOWER))
    return 6;

  TCLOSURE(Comparison) comparison =
      CLOSURE_FUNC(Comparison_Bind)(compare_int_pointer, NULL);
  if (CLOSURE_INVOKE(comparison, &left, &right) !=
      COMPARISON_CONST(LOWER))
    return 7;

  /* Test Comparison FunctionTable call */
  if (PACKAGE_CALL(Algorithm_Comparison, Int, 1, 2) !=
      COMPARISON_CONST(LOWER))
    return 8;
  if (PACKAGE_CALL(Algorithm_Comparison, Float, 2.0f, 1.0f) !=
      COMPARISON_CONST(HIGHER))
    return 9;

  /* ======================================================================== */
  /* Test Algorithm_Repeat: Closures                                          */
  /* ======================================================================== */

  int counter = 0;
  MODULE_CLOSURE(Algorithm, Repeat, Procedure) proc =
      NS_JOIN(MODULE_CLOSURE_FUNC(Algorithm, Repeat, Procedure), Bind)(
          increment_counter, &counter);
  MODULE_CLOSURE(Algorithm, Repeat, Procedure) invalidProc =
      NS_JOIN(MODULE_CLOSURE_FUNC(Algorithm, Repeat, Procedure), Bind)(NULL, NULL);

  Algorithm_Repeat_Procedure(5U, proc);
  if (counter != 5)
    return 10;
  Algorithm_Repeat_Procedure(0U, invalidProc);
  Algorithm_Repeat_Procedure(3U, invalidProc);
  if (counter != 5)
    return 11;

  /* Test MODULE_CALL on Procedure */
  MODULE_CALL(Algorithm, Repeat, Procedure, (2U, proc));
  if (counter != 7)
    return 11;

  size_t indexSum = 0U;
  MODULE_CLOSURE(Algorithm, Repeat, ProcedureIndexed) indexedProc =
      NS_JOIN(MODULE_CLOSURE_FUNC(Algorithm, Repeat, ProcedureIndexed), Bind)(
          add_index_to_counter, &indexSum);
  Algorithm_Repeat_ProcedureIndexed(4U, indexedProc);
  if (indexSum != 6U) /* 0 + 1 + 2 + 3 */
    return 12;

  int opRemaining = 3;
  MODULE_CLOSURE(Algorithm, Repeat, Operation) op =
      NS_JOIN(MODULE_CLOSURE_FUNC(Algorithm, Repeat, Operation), Bind)(
          step_with_limit, &opRemaining);
  MODULE_CLOSURE(Algorithm, Repeat, Operation) invalidOp =
      NS_JOIN(MODULE_CLOSURE_FUNC(Algorithm, Repeat, Operation), Bind)(NULL, NULL);

  if (Algorithm_Repeat_Operation(0U, invalidOp) != STATUS_CONST(SUCCESS))
    return 13;
  if (Algorithm_Repeat_Operation(2U, invalidOp) != STATUS_CONST(INVALID_ARGUMENT))
    return 14;
  if (Algorithm_Repeat_Operation(2U, op) != STATUS_CONST(SUCCESS) ||
      opRemaining != 1)
    return 15;
  if (Algorithm_Repeat_Operation(3U, op) != STATUS_CONST(OUT_OF_RANGE) ||
      opRemaining != 0)
    return 16;

  size_t stopAt = 2U;
  MODULE_CLOSURE(Algorithm, Repeat, OperationIndexed) indexedOp =
      NS_JOIN(MODULE_CLOSURE_FUNC(Algorithm, Repeat, OperationIndexed), Bind)(
          indexed_step_with_limit, &stopAt);
  if (Algorithm_Repeat_OperationIndexed(2U, indexedOp) != STATUS_CONST(SUCCESS))
    return 17;
  if (Algorithm_Repeat_OperationIndexed(4U, indexedOp) !=
      STATUS_CONST(OUT_OF_RANGE))
    return 18;

  int predRemaining = 2;
  MODULE_CLOSURE(Algorithm, Repeat, Predicate) pred =
      NS_JOIN(MODULE_CLOSURE_FUNC(Algorithm, Repeat, Predicate), Bind)(
          countdown_predicate, &predRemaining);
  MODULE_CLOSURE(Algorithm, Repeat, Predicate) invalidPred =
      NS_JOIN(MODULE_CLOSURE_FUNC(Algorithm, Repeat, Predicate), Bind)(NULL, NULL);

  if (!Algorithm_Repeat_Predicate(0U, invalidPred))
    return 19;
  if (Algorithm_Repeat_Predicate(1U, invalidPred))
    return 20;
  if (!Algorithm_Repeat_Predicate(2U, pred) || predRemaining != 0)
    return 21;
  if (Algorithm_Repeat_Predicate(1U, pred))
    return 22;

  size_t threshold = 3U;
  MODULE_CLOSURE(Algorithm, Repeat, PredicateIndexed) indexedPred =
      NS_JOIN(MODULE_CLOSURE_FUNC(Algorithm, Repeat, PredicateIndexed), Bind)(
          indexed_below_threshold, &threshold);
  if (!Algorithm_Repeat_PredicateIndexed(3U, indexedPred))
    return 23;
  if (Algorithm_Repeat_PredicateIndexed(4U, indexedPred))
    return 24;

  /* ======================================================================== */
  /* Test Algorithm_Repeat: Bytes, Primitives & CString                       */
  /* ======================================================================== */

  const unsigned char pattern[2] = {0xAAU, 0x55U};
  unsigned char byteBuffer[6] = {0};
  if (Algorithm_Repeat_Bytes(byteBuffer, pattern, sizeof(pattern), 3U) !=
      STATUS_CONST(SUCCESS))
    return 25;
  for (size_t i = 0U; i < 3U; ++i) {
    if (byteBuffer[i * 2U] != 0xAAU || byteBuffer[i * 2U + 1U] != 0x55U)
      return 26;
  }
  if (Algorithm_Repeat_Bytes(NULL, NULL, 0U, 5U) != STATUS_CONST(SUCCESS))
    return 27;
  if (Algorithm_Repeat_Bytes(NULL, pattern, sizeof(pattern), 1U) !=
      STATUS_CONST(INVALID_ARGUMENT))
    return 28;
  /* Overlap detection */
  if (Algorithm_Repeat_Bytes(byteBuffer + 1U, byteBuffer, 2U, 2U) !=
      STATUS_CONST(INVALID_ARGUMENT))
    return 29;

  int intArray[4] = {0};
  if (Algorithm_Repeat_Int(intArray, 42, 4U) != STATUS_CONST(SUCCESS))
    return 30;
  for (size_t i = 0U; i < 4U; ++i) {
    if (intArray[i] != 42)
      return 31;
  }

  const char *strArray[3] = {NULL};
  if (Algorithm_Repeat_CString(strArray, "cosmeron", 3U) != STATUS_CONST(SUCCESS))
    return 32;
  for (size_t i = 0U; i < 3U; ++i) {
    if (strcmp(strArray[i], "cosmeron") != 0)
      return 33;
  }

  if (PACKAGE_CALL(Algorithm_Repeat, Int, intArray, 99, 4U) !=
      STATUS_CONST(SUCCESS))
    return 34;
  if (intArray[0] != 99 || intArray[3] != 99)
    return 35;

  /* ======================================================================== */
  /* Test TEMPLATE_REPEAT                                                     */
  /* ======================================================================== */

  AlgorithmTest_Point points[3] = {{0, 0}};
  AlgorithmTest_Point fillValue = {7, 14};
  if (Algorithm_Repeat_Point_Fill(points, fillValue, 3U) != STATUS_CONST(SUCCESS))
    return 36;
  for (size_t i = 0U; i < 3U; ++i) {
    if (points[i].x != 7 || points[i].y != 14)
      return 37;
  }

  /* Test MODULE_CALL on template */
  AlgorithmTest_Point fillValue2 = {8, 16};
  if (MODULE_CALL(Algorithm, Repeat, Point, Fill, (points, fillValue2, 3U)) !=
      STATUS_CONST(SUCCESS))
    return 37;
  if (points[0].x != 8 || points[2].y != 16)
    return 37;

  int baseVal = 5;
  MODULE_CLOSURE(Algorithm, Repeat, Point, Generator) gen =
      NS_JOIN(MODULE_CLOSURE_FUNC(Algorithm, Repeat, Point, Generator), Bind)(
          generate_constant_point, &baseVal);
  if (Algorithm_Repeat_Point_Generate(points, 3U, gen) != STATUS_CONST(SUCCESS))
    return 38;
  for (size_t i = 0U; i < 3U; ++i) {
    if (points[i].x != 5 || points[i].y != 10)
      return 39;
  }

  int offsetVal = 100;
  MODULE_CLOSURE(Algorithm, Repeat, Point, GeneratorIndexed) indexedGen =
      NS_JOIN(MODULE_CLOSURE_FUNC(Algorithm, Repeat, Point, GeneratorIndexed), Bind)(
          generate_indexed_point, &offsetVal);
  if (Algorithm_Repeat_Point_GenerateIndexed(points, 3U, indexedGen) !=
      STATUS_CONST(SUCCESS))
    return 40;
  for (size_t i = 0U; i < 3U; ++i) {
    if (points[i].x != (int)i + 100 || points[i].y != (int)(i * 10U))
      return 41;
  }

  return 0;
}
