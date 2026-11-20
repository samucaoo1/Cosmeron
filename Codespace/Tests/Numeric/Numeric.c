#include "../../Cosmeron/Modules/Numeric/Numeric.h"

#include <assert.h>
#include <stdio.h>

int main(void) {
  /* 1. Rational: 1/3 + 1/6 = 1/2 */
  MODULE_TYPED(Numeric, Rational, Int32) r1, r2, rSum, rDiv;
  assert(MODULE_FUNC(Numeric, Rational, Int32, Make)(1, 3, &r1) ==
         STATUS_CONST(SUCCESS));
  assert(MODULE_FUNC(Numeric, Rational, Int32, Make)(1, 6, &r2) ==
         STATUS_CONST(SUCCESS));
  assert(MODULE_FUNC(Numeric, Rational, Int32, Add)(r1, r2, &rSum) ==
         STATUS_CONST(SUCCESS));
  assert(rSum.numerator == 1 && rSum.denominator == 2);

  MODULE_TYPED(Numeric, Rational, Int32) rZero = {.numerator = 0,
                                                  .denominator = 1};
  assert(MODULE_FUNC(Numeric, Rational, Int32, Div)(r1, rZero, &rDiv) ==
         STATUS_CONST(INVALID_ARGUMENT));

  /* 2. Complex: (3 + 4i) -> magnitude 5 */
  MODULE_TYPED(Numeric, Complex, Float64) c1 =
      MODULE_FUNC(Numeric, Complex, Float64, Make)(3.0, 4.0);
  double mag = MODULE_FUNC(Numeric, Complex, Float64, Magnitude)(c1);
  assert(mag > 4.999999 && mag < 5.000001);

  MODULE_TYPED(Numeric, Complex, Float64) c2 =
      MODULE_FUNC(Numeric, Complex, Float64, Make)(1.0, -2.0);
  MODULE_TYPED(Numeric, Complex, Float64) cProd =
      MODULE_FUNC(Numeric, Complex, Float64, Mul)(c1, c2);
  /* (3+4i)(1-2i) = 11 - 2i */
  assert(cProd.real > 10.999999 && cProd.real < 11.000001);
  assert(cProd.imag > -2.000001 && cProd.imag < -1.999999);

  /* 3. Fixed: Q16_16 (2.5 * 3.0 = 7.5) */
  MODULE_TYPED(Numeric, Fixed, Q16_16) f1 =
      MODULE_FUNC(Numeric, Fixed, Q16_16, FromFloat64)(2.5);
  MODULE_TYPED(Numeric, Fixed, Q16_16) f2 =
      MODULE_FUNC(Numeric, Fixed, Q16_16, FromInt)(3);
  MODULE_TYPED(Numeric, Fixed, Q16_16) fProd =
      MODULE_FUNC(Numeric, Fixed, Q16_16, Mul)(f1, f2);
  double fVal = MODULE_FUNC(Numeric, Fixed, Q16_16, ToFloat64)(fProd);
  assert(fVal > 7.4999 && fVal < 7.5001);

  /* 4. Bigint128 & Decimal128 (glued width identifiers) */
  MODULE_TYPE(Numeric, Bigint128) biA = {0};
  MODULE_TYPE(Numeric, Bigint128) biB = {0};
  assert(MODULE_FUNC(Numeric, Bigint128, Init)(&biA) == STATUS_CONST(SUCCESS));
  assert(MODULE_FUNC(Numeric, Bigint128, Init)(&biB) == STATUS_CONST(SUCCESS));
  biA.limb[0] = 40ULL;
  biB.limb[0] = 2ULL;
  assert(OBJECT_CALL(&biA, Add, &biB) == STATUS_CONST(SUCCESS));
  assert(biA.limb[0] == 42ULL);

  MODULE_TYPE(Numeric, Decimal128) decA = {0};
  MODULE_TYPE(Numeric, Decimal128) decB = {0};
  assert(MODULE_FUNC(Numeric, Decimal128, Init)(&decA) == STATUS_CONST(SUCCESS));
  assert(MODULE_FUNC(Numeric, Decimal128, Init)(&decB) == STATUS_CONST(SUCCESS));
  decA.limb[0] = 50ULL;
  decB.limb[0] = 8ULL;
  assert(OBJECT_CALL(&decA, Sub, &decB) == STATUS_CONST(SUCCESS));
  assert(decA.limb[0] == 42ULL);

  printf("Numeric tests passed!\n");
  return 0;
}
