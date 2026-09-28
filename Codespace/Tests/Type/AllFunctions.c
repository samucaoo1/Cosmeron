#include "../../Cosmeron/Modules/Type/TBigint.h"
#include "../../Cosmeron/Modules/Type/TBlock.h"
#include "../../Cosmeron/Modules/Type/TDecimal.h"

#include <string.h>

#define CHECK_STATUS(EXPR) do { if ((EXPR) != STATUS_CONST(SUCCESS)) return __LINE__; } while (0)
#define CHECK_TRUE(EXPR) do { if (!(EXPR)) return __LINE__; } while (0)

#define TEST_TBLOCK(SUFFIX)                                                    \
  do {                                                                         \
    TBlock(SUFFIX, a)                                                          \
    TBlock(SUFFIX, b)                                                          \
    bool bit = false;                                                          \
    CHECK_STATUS(TBLOCK_FUNC(SUFFIX, Clear)(&a));                              \
    CHECK_STATUS(TBLOCK_FUNC(SUFFIX, Clear)(&b));                              \
    a.limb[0] = 3U; b.limb[0] = 1U;                                           \
    CHECK_STATUS(TBLOCK_FUNC(SUFFIX, And)(&a, &b));                            \
    CHECK_TRUE(a.limb[0] == 1U);                                               \
    a.limb[0] = 1U; b.limb[0] = 2U;                                           \
    CHECK_STATUS(TBLOCK_FUNC(SUFFIX, Or)(&a, &b));                             \
    CHECK_TRUE(a.limb[0] == 3U);                                               \
    a.limb[0] = 1U; b.limb[0] = 3U;                                           \
    CHECK_STATUS(TBLOCK_FUNC(SUFFIX, Xor)(&a, &b));                            \
    CHECK_TRUE(a.limb[0] == 2U);                                               \
    a.limb[0] = 0U;                                                           \
    CHECK_STATUS(TBLOCK_FUNC(SUFFIX, Not)(&a));                                \
    CHECK_STATUS(TBLOCK_FUNC(SUFFIX, Not)(&a));                                \
    CHECK_TRUE(a.limb[0] == 0U);                                               \
    a.limb[0] = 1U;                                                           \
    CHECK_STATUS(TBLOCK_FUNC(SUFFIX, ShiftLeft)(&a, 1U));                      \
    CHECK_TRUE(a.limb[0] == 2U);                                               \
    CHECK_STATUS(TBLOCK_FUNC(SUFFIX, ShiftRight)(&a, 1U));                     \
    CHECK_TRUE(a.limb[0] == 1U);                                               \
    CHECK_STATUS(TBLOCK_FUNC(SUFFIX, RotateLeft)(&a, 1U));                     \
    CHECK_STATUS(TBLOCK_FUNC(SUFFIX, RotateRight)(&a, 1U));                    \
    CHECK_TRUE(a.limb[0] == 1U);                                               \
    CHECK_STATUS(TBLOCK_FUNC(SUFFIX, BitSet)(&a, 5U));                         \
    CHECK_STATUS(TBLOCK_FUNC(SUFFIX, BitCheck)(&a, 5U, &bit));                 \
    CHECK_TRUE(bit);                                                           \
    CHECK_STATUS(TBLOCK_FUNC(SUFFIX, BitClear)(&a, 5U));                       \
    CHECK_STATUS(TBLOCK_FUNC(SUFFIX, BitCheck)(&a, 5U, &bit));                 \
    CHECK_TRUE(!bit);                                                          \
  } while (0)

#define TEST_BIGLIKE(PREFIX, TYPEMACRO, DECLMACRO, SUFFIX)                    \
  do {                                                                         \
    DECLMACRO(SUFFIX, a)                                                       \
    DECLMACRO(SUFFIX, b)                                                       \
    DECLMACRO(SUFFIX, q)                                                       \
    DECLMACRO(SUFFIX, r)                                                       \
    CMPOUT cmp = COMPARISON_CONST(EQUAL);                           \
    bool bit = false;                                                          \
    char buffer[400];                                                          \
    CHECK_STATUS(PREFIX##_FUNC(SUFFIX, Init)(&a));                             \
    CHECK_STATUS(PREFIX##_FUNC(SUFFIX, Clear)(&b));                            \
    a.limb[0] = 5U; b.limb[0] = 3U;                                           \
    CHECK_STATUS(PREFIX##_FUNC(SUFFIX, Add)(&a, &b));                          \
    CHECK_TRUE(a.limb[0] == 8U);                                               \
    CHECK_STATUS(PREFIX##_FUNC(SUFFIX, Sub)(&a, &b));                          \
    CHECK_TRUE(a.limb[0] == 5U);                                               \
    CHECK_STATUS(PREFIX##_FUNC(SUFFIX, Mul)(&a, &b));                          \
    CHECK_TRUE(a.limb[0] == 15U);                                              \
    CHECK_STATUS(PREFIX##_FUNC(SUFFIX, Div)(&a, &b));                          \
    CHECK_TRUE(a.limb[0] == 5U);                                               \
    a.limb[0] = 17U;                                                          \
    CHECK_STATUS(PREFIX##_FUNC(SUFFIX, Mod)(&a, &b));                          \
    CHECK_TRUE(a.limb[0] == 2U);                                               \
    a.limb[0] = 17U;                                                          \
    CHECK_STATUS(PREFIX##_FUNC(SUFFIX, DivMod)(&a, &b, &q, &r));               \
    CHECK_TRUE(q.limb[0] == 5U && r.limb[0] == 2U);                            \
    CHECK_STATUS(PREFIX##_FUNC(SUFFIX, Increment)(&a));                        \
    CHECK_TRUE(a.limb[0] == 18U);                                              \
    CHECK_STATUS(PREFIX##_FUNC(SUFFIX, Decrement)(&a));                        \
    CHECK_TRUE(a.limb[0] == 17U);                                              \
    a.limb[0] = 3U; b.limb[0] = 1U;                                           \
    CHECK_STATUS(PREFIX##_FUNC(SUFFIX, And)(&a, &b));                          \
    CHECK_TRUE(a.limb[0] == 1U);                                               \
    a.limb[0] = 1U; b.limb[0] = 2U;                                           \
    CHECK_STATUS(PREFIX##_FUNC(SUFFIX, Or)(&a, &b));                           \
    CHECK_TRUE(a.limb[0] == 3U);                                               \
    a.limb[0] = 1U; b.limb[0] = 3U;                                           \
    CHECK_STATUS(PREFIX##_FUNC(SUFFIX, Xor)(&a, &b));                          \
    CHECK_TRUE(a.limb[0] == 2U);                                               \
    a.limb[0] = 0U;                                                           \
    CHECK_STATUS(PREFIX##_FUNC(SUFFIX, Not)(&a));                              \
    CHECK_STATUS(PREFIX##_FUNC(SUFFIX, Not)(&a));                              \
    CHECK_TRUE(a.limb[0] == 0U);                                               \
    a.limb[0] = 1U;                                                           \
    CHECK_STATUS(PREFIX##_FUNC(SUFFIX, ShiftLeft)(&a, 1U));                    \
    CHECK_TRUE(a.limb[0] == 2U);                                               \
    CHECK_STATUS(PREFIX##_FUNC(SUFFIX, ShiftRight)(&a, 1U));                   \
    CHECK_TRUE(a.limb[0] == 1U);                                               \
    CHECK_STATUS(PREFIX##_FUNC(SUFFIX, RotateLeft)(&a, 1U));                   \
    CHECK_STATUS(PREFIX##_FUNC(SUFFIX, RotateRight)(&a, 1U));                  \
    CHECK_TRUE(a.limb[0] == 1U);                                               \
    CHECK_STATUS(PREFIX##_FUNC(SUFFIX, BitSet)(&a, 7U));                       \
    CHECK_STATUS(PREFIX##_FUNC(SUFFIX, BitCheck)(&a, 7U, &bit));               \
    CHECK_TRUE(bit);                                                           \
    CHECK_STATUS(PREFIX##_FUNC(SUFFIX, BitClear)(&a, 7U));                     \
    CHECK_STATUS(PREFIX##_FUNC(SUFFIX, BitCheck)(&a, 7U, &bit));               \
    CHECK_TRUE(!bit);                                                          \
    a.limb[0] = 1U; b.limb[0] = 2U;                                           \
    CHECK_STATUS(PREFIX##_FUNC(SUFFIX, Compare)(&a, &b, &cmp));                \
    CHECK_TRUE(cmp == COMPARISON_CONST(LOWER));                                \
    CHECK_TRUE(!PREFIX##_FUNC(SUFFIX, Equal)(&a, &b));                         \
    CHECK_TRUE(PREFIX##_FUNC(SUFFIX, NotEqual)(&a, &b));                       \
    CHECK_TRUE(PREFIX##_FUNC(SUFFIX, LessThan)(&a, &b));                       \
    CHECK_TRUE(!PREFIX##_FUNC(SUFFIX, GreaterThan)(&a, &b));                   \
    CHECK_TRUE(PREFIX##_FUNC(SUFFIX, LessOrEqual)(&a, &b));                    \
    CHECK_TRUE(!PREFIX##_FUNC(SUFFIX, GreaterOrEqual)(&a, &b));                \
    CHECK_TRUE(!PREFIX##_FUNC(SUFFIX, IsZero)(&a));                            \
    a.limb[0] = 255U;                                                         \
    CHECK_STATUS(PREFIX##_FUNC(SUFFIX, ToCStringBase)(&a, buffer, sizeof(buffer), 16U)); \
    CHECK_TRUE(strcmp(buffer, "FF") == 0);                                     \
    CHECK_STATUS(PREFIX##_FUNC(SUFFIX, ToCString)(&a, buffer, sizeof(buffer))); \
    CHECK_TRUE(strcmp(buffer, "255") == 0);                                    \
  } while (0)

int main(void) {
  TEST_TBLOCK(128);
  TEST_TBLOCK(256);
  TEST_TBLOCK(512);
  TEST_TBLOCK(1024);

  TEST_BIGLIKE(TBIGINT, TBIGINT_TYPE, TBigint, 128);
  TEST_BIGLIKE(TBIGINT, TBIGINT_TYPE, TBigint, 256);
  TEST_BIGLIKE(TBIGINT, TBIGINT_TYPE, TBigint, 512);
  TEST_BIGLIKE(TBIGINT, TBIGINT_TYPE, TBigint, 1024);

  TEST_BIGLIKE(TDECIMAL, TDECIMAL_TYPE, TDecimal, 128);
  TEST_BIGLIKE(TDECIMAL, TDECIMAL_TYPE, TDecimal, 256);
  TEST_BIGLIKE(TDECIMAL, TDECIMAL_TYPE, TDecimal, 512);
  TEST_BIGLIKE(TDECIMAL, TDECIMAL_TYPE, TDecimal, 1024);

  return 0;
}
