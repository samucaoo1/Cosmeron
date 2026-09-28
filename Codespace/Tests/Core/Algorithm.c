#include "../../Cosmeron/Core/Algorithm/Comparison.h"

static CMPOUT compare_int_pointer(const void *left,
                                             const void *right) {
  const int leftValue = *(const int *)left;
  const int rightValue = *(const int *)right;
  return Comparison_Compare(int, leftValue, rightValue);
}

int main(void) {
  const unsigned char lowerBytes[] = {1, 2};
  const unsigned char higherBytes[] = {1, 3};
  int left = 4;
  int right = 9;

  if (Comparison_Compare(int, 1, 2) != COMPARISON_CONST(LOWER))
    return 1;
  if (Comparison_Compare(double, 3.0, 3.0) != COMPARISON_CONST(EQUAL))
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

  if (COMPARISON_FUNC(Invoke)(&left, &right, compare_int_pointer) !=
      COMPARISON_CONST(LOWER))
    return 7;

  return 0;
}
