#include "../../Cosmeron/Modules/Type/Fundamental.h"
#include "../../Cosmeron/Modules/Type/TBigint.h"
#include "../../Cosmeron/Modules/Type/TBlock.h"
#include "../../Cosmeron/Modules/Type/TDecimal.h"
#include <string.h>

#define TEST_TBLOCK_BOUNDARIES(SUFFIX, WIDTH, ERROR_BASE)                      \
  do {                                                                          \
    TBlock(SUFFIX, bits);                                                       \
    const void *api = (const void *)bits.api;                                   \
    bool value = false;                                                         \
    if (TBLOCK_FUNC(SUFFIX, BitSet)(&bits, (WIDTH) - 1U) !=                     \
            STATUS_CONST(SUCCESS))                                              \
      return (ERROR_BASE);                                                      \
    if (TBLOCK_FUNC(SUFFIX, BitCheck)(&bits, (WIDTH) - 1U, &value) !=           \
            STATUS_CONST(SUCCESS) ||                                            \
        !value || (const void *)bits.api != api)                                \
      return (ERROR_BASE) + 1;                                                  \
    if (TBLOCK_FUNC(SUFFIX, BitSet)(&bits, (WIDTH)) !=                          \
        STATUS_CONST(OUT_OF_RANGE))                                             \
      return (ERROR_BASE) + 2;                                                  \
    if (TBLOCK_FUNC(SUFFIX, ShiftRight)(&bits, (WIDTH) - 1U) !=                 \
            STATUS_CONST(SUCCESS) ||                                            \
        bits.limb[0] != 1U || (const void *)bits.api != api)                    \
      return (ERROR_BASE) + 3;                                                  \
    if (TBLOCK_FUNC(SUFFIX, RotateLeft)(&bits, (WIDTH)) !=                      \
            STATUS_CONST(SUCCESS) ||                                            \
        bits.limb[0] != 1U || (const void *)bits.api != api)                    \
      return (ERROR_BASE) + 4;                                                  \
    if (TBLOCK_FUNC(SUFFIX, Clear)(&bits) != STATUS_CONST(SUCCESS) ||           \
        (const void *)bits.api != api)                                          \
      return (ERROR_BASE) + 5;                                                  \
  } while (0)

#define TEST_TBIGINT_ARITHMETIC_BOUNDARIES(SUFFIX, WIDTH, ERROR_BASE)          \
  do {                                                                          \
    TBIGINT_TYPE(SUFFIX) maximum = {0};                                         \
    TBIGINT_TYPE(SUFFIX) one = {0};                                             \
    TBIGINT_TYPE(SUFFIX) two = {0};                                             \
    for (size_t i = 0; i < (WIDTH) / 64U; ++i)                                 \
      maximum.limb[i] = UINT64_MAX;                                             \
    one.limb[0] = 1U;                                                           \
    two.limb[0] = 2U;                                                           \
    TBIGINT_TYPE(SUFFIX) original = maximum;                                    \
    if (TBIGINT_FUNC(SUFFIX, Add)(&maximum, &one) !=                            \
        STATUS_CONST(ARITHMETIC_OVERFLOW))                                      \
      return (ERROR_BASE);                                                      \
    if (!TBIGINT_FUNC(SUFFIX, Equal)(&maximum, &original))                      \
      return (ERROR_BASE) + 1;                                                  \
    if (TBIGINT_FUNC(SUFFIX, Increment)(&maximum) !=                            \
        STATUS_CONST(ARITHMETIC_OVERFLOW))                                      \
      return (ERROR_BASE) + 2;                                                  \
    if (!TBIGINT_FUNC(SUFFIX, Equal)(&maximum, &original))                      \
      return (ERROR_BASE) + 3;                                                  \
    TBIGINT_TYPE(SUFFIX) zero = {0};                                            \
    if (TBIGINT_FUNC(SUFFIX, Sub)(&zero, &one) !=                               \
        STATUS_CONST(ARITHMETIC_OVERFLOW))                                      \
      return (ERROR_BASE) + 4;                                                  \
    if (!TBIGINT_FUNC(SUFFIX, IsZero)(&zero))                                   \
      return (ERROR_BASE) + 5;                                                  \
    if (TBIGINT_FUNC(SUFFIX, Decrement)(&zero) !=                               \
        STATUS_CONST(ARITHMETIC_OVERFLOW))                                      \
      return (ERROR_BASE) + 6;                                                  \
    if (!TBIGINT_FUNC(SUFFIX, IsZero)(&zero))                                   \
      return (ERROR_BASE) + 7;                                                  \
    TBIGINT_TYPE(SUFFIX) high = {0};                                            \
    high.limb[(WIDTH) / 64U - 1U] = UINT64_C(0x8000000000000000);              \
    original = high;                                                            \
    if (TBIGINT_FUNC(SUFFIX, Mul)(&high, &two) !=                               \
        STATUS_CONST(ARITHMETIC_OVERFLOW))                                      \
      return (ERROR_BASE) + 8;                                                  \
    if (!TBIGINT_FUNC(SUFFIX, Equal)(&high, &original))                         \
      return (ERROR_BASE) + 9;                                                  \
  } while (0)

#define TEST_TBIGINT_SHIFT_BOUNDARIES(SUFFIX, WIDTH, ERROR_BASE)               \
  do {                                                                          \
    TBigint(SUFFIX, value);                                                      \
    const void *api = (const void *)value.api;                                  \
    value.limb[0] = 1U;                                                         \
    if (TBIGINT_FUNC(SUFFIX, ShiftLeft)(&value, 0U) != STATUS_CONST(SUCCESS) || \
        value.limb[0] != 1U || (const void *)value.api != api)                  \
      return (ERROR_BASE);                                                      \
    if (TBIGINT_FUNC(SUFFIX, Clear)(&value) != STATUS_CONST(SUCCESS))           \
      return (ERROR_BASE) + 1;                                                  \
    value.limb[0] = 1U;                                                         \
    if (TBIGINT_FUNC(SUFFIX, ShiftLeft)(&value, (WIDTH) - 1U) !=                \
            STATUS_CONST(SUCCESS) ||                                            \
        value.limb[(WIDTH) / 64U - 1U] != UINT64_C(0x8000000000000000) ||      \
        (const void *)value.api != api)                                         \
      return (ERROR_BASE) + 2;                                                  \
    if (TBIGINT_FUNC(SUFFIX, Clear)(&value) != STATUS_CONST(SUCCESS))           \
      return (ERROR_BASE) + 3;                                                  \
    value.limb[0] = 1U;                                                         \
    if (TBIGINT_FUNC(SUFFIX, ShiftLeft)(&value, (WIDTH)) !=                     \
            STATUS_CONST(SUCCESS) ||                                            \
        !TBIGINT_FUNC(SUFFIX, IsZero)(&value) || (const void *)value.api != api)\
      return (ERROR_BASE) + 4;                                                  \
    value.limb[0] = 1U;                                                         \
    if (TBIGINT_FUNC(SUFFIX, ShiftLeft)(&value, (WIDTH) + 1U) !=                \
            STATUS_CONST(SUCCESS) ||                                            \
        !TBIGINT_FUNC(SUFFIX, IsZero)(&value) || (const void *)value.api != api)\
      return (ERROR_BASE) + 5;                                                  \
    if (TBIGINT_FUNC(SUFFIX, Clear)(&value) != STATUS_CONST(SUCCESS))           \
      return (ERROR_BASE) + 6;                                                  \
    value.limb[(WIDTH) / 64U - 1U] = UINT64_C(0x8000000000000000);             \
    if (TBIGINT_FUNC(SUFFIX, ShiftRight)(&value, (WIDTH) - 1U) !=               \
            STATUS_CONST(SUCCESS) ||                                            \
        value.limb[0] != 1U || (const void *)value.api != api)                  \
      return (ERROR_BASE) + 7;                                                  \
  } while (0)

int main(void) {
  Char8 c8 = 0;
  Char16 c16 = 0;
  Char32 c32 = 0;
  TBigint(128, bigint);
  TBlock(128, block);
  TDecimal(128, decimal);

  if (sizeof(c8) != 1U || sizeof(c16) != 2U || sizeof(c32) != 4U)
    return 1;

  if (TBIGINT_FUNC(128, Clear)(&bigint) != STATUS_CONST(SUCCESS))
    return 2;
  if (TBLOCK_FUNC(128, Clear)(&block) != STATUS_CONST(SUCCESS))
    return 3;
  if (TDECIMAL_FUNC(128, Clear)(&decimal) != STATUS_CONST(SUCCESS))
    return 4;

  bigint.limb[0] = 6U;
  {
    TBIGINT_TYPE(128) multiplier = {0};
    multiplier.limb[0] = 7U;
    if (TBIGINT_FUNC(128, Mul)(&bigint, &multiplier) != STATUS_CONST(SUCCESS))
      return 5;
    if (bigint.limb[0] != 42U || bigint.limb[1] != 0U)
      return 6;
  }

  decimal.limb[0] = 9U;
  {
    TDECIMAL_TYPE(128) multiplier = {0};
    multiplier.limb[0] = 5U;
    if (TDECIMAL_FUNC(128, Mul)(&decimal, &multiplier) != STATUS_CONST(SUCCESS))
      return 7;
    if (decimal.limb[0] != 45U || decimal.limb[1] != 0U)
      return 8;
  }

  bigint.limb[0] = 100U;
  {
    TBIGINT_TYPE(128) divisor = {0};
    divisor.limb[0] = 9U;
    if (TBIGINT_FUNC(128, Div)(&bigint, &divisor) != STATUS_CONST(SUCCESS))
      return 9;
    if (bigint.limb[0] != 11U)
      return 10;
    bigint.limb[0] = 100U;
    if (TBIGINT_FUNC(128, Mod)(&bigint, &divisor) != STATUS_CONST(SUCCESS))
      return 11;
    if (bigint.limb[0] != 1U)
      return 12;
    divisor.limb[0] = 0U;
    if (TBIGINT_FUNC(128, Div)(&bigint, &divisor) != STATUS_CONST(DIVISION_BY_ZERO))
      return 13;
  }

  {
    TBIGINT_TYPE(128) maximum = {0};
    TBIGINT_TYPE(128) one = {0};
    maximum.limb[0] = UINT64_MAX;
    maximum.limb[1] = UINT64_MAX;
    one.limb[0] = 1U;
    TBIGINT_TYPE(128) original = maximum;
    if (TBIGINT_FUNC(128, Add)(&maximum, &one) != STATUS_CONST(ARITHMETIC_OVERFLOW))
      return 14;
    if (!TBIGINT_FUNC(128, Equal)(&maximum, &original))
      return 15;
    if (TBIGINT_FUNC(128, Increment)(&maximum) != STATUS_CONST(ARITHMETIC_OVERFLOW))
      return 16;
    if (!TBIGINT_FUNC(128, Equal)(&maximum, &original))
      return 17;

    TBIGINT_TYPE(128) zero = {0};
    if (TBIGINT_FUNC(128, Sub)(&zero, &one) != STATUS_CONST(ARITHMETIC_OVERFLOW))
      return 18;
    if (!TBIGINT_FUNC(128, IsZero)(&zero))
      return 19;
    if (TBIGINT_FUNC(128, Decrement)(&zero) != STATUS_CONST(ARITHMETIC_OVERFLOW))
      return 20;
    if (!TBIGINT_FUNC(128, IsZero)(&zero))
      return 21;
  }

  {
    TBIGINT_TYPE(128) high = {0};
    TBIGINT_TYPE(128) two = {0};
    high.limb[1] = UINT64_C(0x8000000000000000);
    two.limb[0] = 2U;
    TBIGINT_TYPE(128) original = high;
    if (TBIGINT_FUNC(128, Mul)(&high, &two) != STATUS_CONST(ARITHMETIC_OVERFLOW))
      return 22;
    if (!TBIGINT_FUNC(128, Equal)(&high, &original))
      return 23;
  }

  {
    char buffer[64];
    TBIGINT_TYPE(128) value = {0};
    if (TBIGINT_FUNC(128, ToCString)(&value, buffer, sizeof(buffer)) != STATUS_CONST(SUCCESS))
      return 24;
    if (strcmp(buffer, "0") != 0)
      return 25;

    value.limb[0] = 42U;
    if (TBIGINT_FUNC(128, ToCString)(&value, buffer, sizeof(buffer)) != STATUS_CONST(SUCCESS))
      return 26;
    if (strcmp(buffer, "42") != 0)
      return 27;

    value.limb[0] = 0U;
    value.limb[1] = 1U;
    if (TBIGINT_FUNC(128, ToCString)(&value, buffer, sizeof(buffer)) != STATUS_CONST(SUCCESS))
      return 28;
    if (strcmp(buffer, "18446744073709551616") != 0)
      return 29;

    char tiny[2] = {'X', '\0'};
    if (TBIGINT_FUNC(128, ToCString)(&value, tiny, sizeof(tiny)) != STATUS_CONST(OUT_OF_RANGE))
      return 30;
  }

  {
    char buffer[140];
    TBIGINT_TYPE(128) value = {0};
    value.limb[0] = 255U;
    if (TBIGINT_FUNC(128, ToCStringBase)(&value, buffer, sizeof(buffer), 2U) != STATUS_CONST(SUCCESS) ||
        strcmp(buffer, "11111111") != 0)
      return 31;
    if (TBIGINT_FUNC(128, ToCStringBase)(&value, buffer, sizeof(buffer), 16U) != STATUS_CONST(SUCCESS) ||
        strcmp(buffer, "FF") != 0)
      return 32;
    if (TBIGINT_FUNC(128, ToCStringBase)(&value, buffer, sizeof(buffer), 36U) != STATUS_CONST(SUCCESS) ||
        strcmp(buffer, "73") != 0)
      return 33;
    if (TBIGINT_FUNC(128, ToCStringBase)(&value, buffer, sizeof(buffer), 1U) != STATUS_CONST(INVALID_ARGUMENT))
      return 34;
    if (TBIGINT_FUNC(128, ToCStringBase)(&value, buffer, sizeof(buffer), 37U) != STATUS_CONST(INVALID_ARGUMENT))
      return 35;
  }

  {
    TBLOCK_TYPE(128) bits = {0};
    bool bitValue = false;
    if (TBLOCK_FUNC(128, BitSet)(&bits, 127U) != STATUS_CONST(SUCCESS))
      return 36;
    if (TBLOCK_FUNC(128, BitCheck)(&bits, 127U, &bitValue) != STATUS_CONST(SUCCESS) || !bitValue)
      return 37;
    if (TBLOCK_FUNC(128, BitClear)(&bits, 127U) != STATUS_CONST(SUCCESS))
      return 38;
    if (TBLOCK_FUNC(128, BitCheck)(&bits, 127U, &bitValue) != STATUS_CONST(SUCCESS) || bitValue)
      return 39;
    if (TBLOCK_FUNC(128, BitSet)(&bits, 128U) != STATUS_CONST(OUT_OF_RANGE))
      return 40;
    if (TBLOCK_FUNC(128, BitCheck)(&bits, 128U, &bitValue) != STATUS_CONST(OUT_OF_RANGE))
      return 41;
    if (TBLOCK_FUNC(128, BitCheck)(&bits, 0U, NULL) != STATUS_CONST(INVALID_ARGUMENT))
      return 42;
  }

  {
    TBIGINT_TYPE(128) left = {0};
    TBIGINT_TYPE(128) right = {0};
    CMPOUT comparison;
    left.limb[0] = 1U;
    right.limb[0] = 2U;
    if (TBIGINT_FUNC(128, Compare)(&left, &right, &comparison) != STATUS_CONST(SUCCESS) ||
        comparison != COMPARISON_CONST(LOWER))
      return 43;
    if (TBIGINT_FUNC(128, Compare)(&right, &left, &comparison) != STATUS_CONST(SUCCESS) ||
        comparison != COMPARISON_CONST(HIGHER))
      return 44;
    right = left;
    if (TBIGINT_FUNC(128, Compare)(&left, &right, &comparison) != STATUS_CONST(SUCCESS) ||
        comparison != COMPARISON_CONST(EQUAL))
      return 45;
    if (TBIGINT_FUNC(128, Compare)(NULL, &right, &comparison) != STATUS_CONST(INVALID_ARGUMENT))
      return 46;
  }

  {
    TBigint(128, viaApi);
    if (!viaApi.api || viaApi.api->Clear(&viaApi) != STATUS_CONST(SUCCESS))
      return 47;
    viaApi.limb[0] = 255U;
    char buffer[8];
    if (viaApi.api->ToCStringBase(&viaApi, buffer, sizeof(buffer), 16U) !=
            STATUS_CONST(SUCCESS) ||
        strcmp(buffer, "FF") != 0)
      return 48;

    TBIGINT_TYPE(128) divisor = {0};
    TBigint(128, quotient);
    TBigint(128, remainder);
    viaApi.limb[0] = 100U;
    divisor.limb[0] = 9U;
    if (viaApi.api->DivMod(&viaApi, &divisor, &quotient, &remainder) !=
            STATUS_CONST(SUCCESS) ||
        quotient.limb[0] != 11U || remainder.limb[0] != 1U)
      return 49;
  }

  {
    TBIGINT_TYPE(128) dividend = {0};
    TBIGINT_TYPE(128) divisor = {0};
    TBigint(128, quotient);
    TBigint(128, remainder);
    const void *quotientApi = (const void *)quotient.api;
    const void *remainderApi = (const void *)remainder.api;
    dividend.limb[0] = UINT64_MAX;
    dividend.limb[1] = UINT64_MAX;
    divisor.limb[0] = 1U;
    divisor.limb[1] = UINT64_C(0x8000000000000000);
    if (TBIGINT_FUNC(128, DivMod)(&dividend, &divisor, &quotient, &remainder) !=
        STATUS_CONST(SUCCESS))
      return 49;
    if (quotient.limb[0] != 1U || quotient.limb[1] != 0U)
      return 50;
    if (remainder.limb[0] != UINT64_C(0xFFFFFFFFFFFFFFFE) ||
        remainder.limb[1] != UINT64_C(0x7FFFFFFFFFFFFFFF))
      return 51;
    if ((const void *)quotient.api != quotientApi ||
        (const void *)remainder.api != remainderApi)
      return 52;
  }

  {
    TBIGINT_TYPE(128) dividend = {0};
    TBIGINT_TYPE(128) divisor = {0};
    TBIGINT_TYPE(128) sameOutput = {0};
    dividend.limb[0] = 100U;
    divisor.limb[0] = 9U;
    if (TBIGINT_FUNC(128, DivMod)(&dividend, &divisor, &sameOutput, &sameOutput) !=
        STATUS_CONST(INVALID_ARGUMENT))
      return 50;
  }

  {
    TDecimal(128, dividend);
    TDECIMAL_TYPE(128) divisor = {0};
    TDecimal(128, quotient);
    TDecimal(128, remainder);
    const void *quotientApi = (const void *)quotient.api;
    const void *remainderApi = (const void *)remainder.api;
    dividend.limb[0] = 100U;
    divisor.limb[0] = 9U;
    if (dividend.api->DivMod(&dividend, &divisor, &quotient, &remainder) !=
            STATUS_CONST(SUCCESS) ||
        quotient.limb[0] != 11U || remainder.limb[0] != 1U)
      return 51;
    if ((const void *)quotient.api != quotientApi ||
        (const void *)remainder.api != remainderApi)
      return 52;
  }

  TEST_TBLOCK_BOUNDARIES(128, 128U, 60);
  TEST_TBLOCK_BOUNDARIES(256, 256U, 70);
  TEST_TBLOCK_BOUNDARIES(512, 512U, 80);
  TEST_TBLOCK_BOUNDARIES(1024, 1024U, 90);

  TEST_TBIGINT_ARITHMETIC_BOUNDARIES(128, 128U, 100);
  TEST_TBIGINT_ARITHMETIC_BOUNDARIES(256, 256U, 110);
  TEST_TBIGINT_ARITHMETIC_BOUNDARIES(512, 512U, 120);
  TEST_TBIGINT_ARITHMETIC_BOUNDARIES(1024, 1024U, 130);

  TEST_TBIGINT_SHIFT_BOUNDARIES(128, 128U, 140);
  TEST_TBIGINT_SHIFT_BOUNDARIES(256, 256U, 150);
  TEST_TBIGINT_SHIFT_BOUNDARIES(512, 512U, 160);
  TEST_TBIGINT_SHIFT_BOUNDARIES(1024, 1024U, 170);

  return 0;
}
