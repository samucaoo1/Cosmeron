#include "../../Cosmeron/Modules/Math/Math.h"

#include <stdio.h>

int main(void) {
  /* 1. Value clamping & range checks */
  int32_t clamped = MODULE_FUNC(Math, Clamp, Int32, Apply)(42, 0, 10);
  printf("Clamp(42, 0, 10) = %d\n", (int)clamped);

  /* 2. Checked & clamped arithmetic */
  int32_t sum = 0;
  OPSTATUS overflowStatus =
      MODULE_FUNC(Math, Arithmetic, Int32, Add)(INT32_MAX, 1, &sum);
  int32_t satSum = MODULE_FUNC(Math, ClampedArithmetic, Int32, ClampAdd)(
      INT32_MAX, 10, 0, INT32_MAX);
  printf("Checked Add overflow=%s, ClampedAdd=%d\n",
         MODULE_FUNC(Condition, Status, Name)(overflowStatus), (int)satSum);

  /* 3. Quadratic equation: x^2 - 5x + 6 = 0 */
  double r1 = 0.0;
  double r2 = 0.0;
  MODULE_ENUM(Math, Solution) sol = CNS(Math, NONE);
  if (MODULE_FUNC(Math, QuadraticEquation, Float64, Solve)(
          1.0, -5.0, 6.0, &r1, &r2, &sol) == STATUS_CONST(SUCCESS) &&
      sol == CNS(Math, TWO)) {
    printf("Quadratic roots: %.2f, %.2f\n", r1, r2);
  }

  return 0;
}
