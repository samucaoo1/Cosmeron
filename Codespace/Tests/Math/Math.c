#include "../../../Cosmeron/Modules/Math/Math.h"

#include <assert.h>
#include <stdio.h>

int main(void) {
  /* 1. Value: Clamp, Between, Max, Min */
  assert(MODULE_FUNC(Math, Clamp, Int32)(15, 0, 10) == 10);
  assert(MODULE_FUNC(Math, Clamp, Int32)(-5, 0, 10) == 0);
  assert(MODULE_FUNC(Math, Clamp, Int32)(5, 10, 0) == 5);

  assert(MODULE_FUNC(Math, IsBetween, Int32)(5, 0, 10));
  assert(!MODULE_FUNC(Math, IsBetween, Int32)(11, 0, 10));
  assert(MODULE_FUNC(Math, Between, Int32)(-1, 0, 10) ==
         CNS(Math, BELOW_RANGE));
  assert(MODULE_FUNC(Math, Between, Int32)(15, 0, 10) ==
         CNS(Math, ABOVE_RANGE));
  assert(MODULE_FUNC(Math, Between, Int32)(5, 0, 10) == CNS(Math, IN_RANGE));
  assert(MODULE_FUNC(Math, Between, Int32)(5, 10, 0) ==
         CNS(Math, INVALID_RANGE));

  assert(MODULE_FUNC(Math, Max, Int32)(3, 7) == 7);
  assert(MODULE_FUNC(Math, Min, Int32)(3, 7) == 3);

  /* 2. Arithmetic: Checked & Saturating */
  int32_t res = 0;
  assert(MODULE_FUNC(Math, Arithmetic, Add, Int32)(10, 20, &res) ==
         STATUS_CONST(SUCCESS));
  assert(res == 30);
  assert(MODULE_FUNC(Math, Arithmetic, Add, Int32)(INT32_MAX, 1, &res) ==
         STATUS_CONST(OUT_OF_RANGE));
  assert(MODULE_FUNC(Math, Arithmetic, Div, Int32)(10, 0, &res) ==
         STATUS_CONST(INVALID_ARGUMENT));
  assert(MODULE_FUNC(Math, Arithmetic, SaturatingAdd, Int32)(INT32_MAX, 10) ==
         INT32_MAX);
  assert(MODULE_FUNC(Math, Arithmetic, SaturatingSub, UInt32)(5U, 20U) == 0U);

  /* 3. Equation: Linear & Quadratic */
  double x = 0.0;
  MODULE_ENUM(Math, Solution) sol = CNS(Math, NONE);
  assert(MODULE_FUNC(Math, Equation, Linear, Float64)(2.0, -6.0, &x, &sol) ==
         STATUS_CONST(SUCCESS));
  assert(sol == CNS(Math, ONE));
  assert(x > 2.999999 && x < 3.000001);

  double r1 = 0.0;
  double r2 = 0.0;
  /* x^2 - 5x + 6 = 0 -> roots 2 and 3 */
  assert(MODULE_FUNC(Math, Equation, Quadratic, Float64)(
             1.0, -5.0, 6.0, &r1, &r2, &sol) == STATUS_CONST(SUCCESS));
  assert(sol == CNS(Math, TWO));
  assert(r1 > 1.999999 && r1 < 2.000001);
  assert(r2 > 2.999999 && r2 < 3.000001);

  printf("Math tests passed!\n");
  return 0;
}
