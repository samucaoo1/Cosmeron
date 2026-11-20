#include "../../Cosmeron/Modules/Numeric/Numeric.h"

#include <stdio.h>

int main(void) {
  /* 1. Rational: 1/3 + 1/6 = 1/2 */
  MODULE_TYPED(Numeric, Rational, Int32) a, b, sum;
  (void)MODULE_FUNC(Numeric, Rational, Int32, Make)(1, 3, &a);
  (void)MODULE_FUNC(Numeric, Rational, Int32, Make)(1, 6, &b);
  (void)MODULE_FUNC(Numeric, Rational, Int32, Add)(a, b, &sum);
  printf("Rational 1/3 + 1/6 = %d/%d\n", (int)sum.numerator,
         (int)sum.denominator);

  /* 2. Complex: |3 + 4i| = 5 */
  MODULE_TYPED(Numeric, Complex, Float64) c =
      MODULE_FUNC(Numeric, Complex, Float64, Make)(3.0, 4.0);
  printf("Complex |3+4i| = %.2f\n",
         MODULE_FUNC(Numeric, Complex, Float64, Magnitude)(c));

  /* 3. Fixed-point Q16.16: 2.5 * 3 = 7.5 */
  MODULE_TYPED(Numeric, Fixed, Q16_16) f1 =
      MODULE_FUNC(Numeric, Fixed, Q16_16, FromFloat64)(2.5);
  MODULE_TYPED(Numeric, Fixed, Q16_16) f2 =
      MODULE_FUNC(Numeric, Fixed, Q16_16, FromInt)(3);
  MODULE_TYPED(Numeric, Fixed, Q16_16) fProd =
      MODULE_FUNC(Numeric, Fixed, Q16_16, Mul)(f1, f2);
  printf("Fixed 2.5 * 3 = %.2f\n",
         MODULE_FUNC(Numeric, Fixed, Q16_16, ToFloat64)(fProd));

  /* 4. Bigint128 */
  MODULE_TYPE(Numeric, Bigint128) biA = {0};
  MODULE_TYPE(Numeric, Bigint128) biB = {0};
  (void)MODULE_FUNC(Numeric, Bigint128, Init)(&biA);
  (void)MODULE_FUNC(Numeric, Bigint128, Init)(&biB);
  biA.limb[0] = 40ULL;
  biB.limb[0] = 2ULL;
  (void)OBJECT_CALL(&biA, Add, &biB);
  printf("Bigint128: %llu\n", (unsigned long long)biA.limb[0]);

  return 0;
}
