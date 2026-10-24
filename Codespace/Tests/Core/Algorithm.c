#include "../../Cosmeron/Core/Algorithm/Comparison.h"

static CMPOUT compare_int_pointer(const void *left, const void *right, void *context) {
  (void)context;
  const int leftValue = *(const int *)left;
  const int rightValue = *(const int *)right;
  return COMPARISON_FUNC(Int)(leftValue, rightValue);
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

  /* Test FunctionTable table call */
  if (PACKAGE_CALL(ALGORITHM_NS(COMPARISON_MOD), Int, 1, 2) != COMPARISON_CONST(LOWER))
    return 8;
  if (PACKAGE_CALL(ALGORITHM_NS(COMPARISON_MOD), Float, 2.0f, 1.0f) != COMPARISON_CONST(HIGHER))
    return 9;

  return 0;
}
