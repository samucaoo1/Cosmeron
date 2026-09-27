#include "../../Cosmeron/Modules/Bit/Bit.h"

#define CHECK(CONDITION) do { if (CONDITION) { return __LINE__; } } while (0)

#define TEST_WIDTH(TYPE, SUFFIX, BITS, MAXV) do {                              \
  TYPE value = 0;                                                              \
  BIT_FUNC(SUFFIX, Set)(&value, 0U);                                           \
  CHECK(value != (TYPE)1 || !BIT_FUNC(SUFFIX, Check)(&value, 0U));             \
  BIT_FUNC(SUFFIX, Clear)(&value, 0U);                                         \
  CHECK(value != 0);                                                           \
  BIT_FUNC(SUFFIX, Flip)(&value, 1U);                                          \
  CHECK(value != (TYPE)2);                                                     \
  BIT_FUNC(SUFFIX, MaskSet)(&value, (TYPE)5);                                  \
  CHECK(!BIT_FUNC(SUFFIX, MaskCheckAll)(&value, (TYPE)7));                     \
  BIT_FUNC(SUFFIX, MaskClear)(&value, (TYPE)1);                                \
  CHECK(BIT_FUNC(SUFFIX, MaskCheckAny)(&value, (TYPE)1));                      \
  BIT_FUNC(SUFFIX, MaskFlip)(&value, (TYPE)6);                                 \
  CHECK(value != 0);                                                           \
  CHECK(BIT_FUNC(SUFFIX, Popcount)((TYPE)(MAXV)) != (BITS));                   \
  CHECK(!BIT_FUNC(SUFFIX, IsPowerOfTwo)((TYPE)1));                             \
  CHECK(BIT_FUNC(SUFFIX, IsPowerOfTwo)((TYPE)3));                              \
  CHECK(BIT_FUNC(SUFFIX, IndexLSB)((TYPE)8) != (TYPE)3);                       \
  CHECK(BIT_FUNC(SUFFIX, IndexMSB)((TYPE)8) != (TYPE)3);                       \
  CHECK(BIT_FUNC(SUFFIX, IndexLSB)((TYPE)0) != (TYPE)(MAXV));                  \
  CHECK(BIT_FUNC(SUFFIX, RotateLeft)((TYPE)1, (TYPE)1) != (TYPE)2);            \
  CHECK(BIT_FUNC(SUFFIX, RotateRight)((TYPE)2, (TYPE)1) != (TYPE)1);           \
  value = (TYPE)0x3C;                                                          \
  CHECK(BIT_FUNC(SUFFIX, Extract)(&value, 5U, 2U) != (TYPE)0xF);               \
  BIT_FUNC(SUFFIX, Insert)(&value, 3U, 0U, (TYPE)5);                           \
  CHECK(BIT_FUNC(SUFFIX, Extract)(&value, 3U, 0U) != (TYPE)5);                 \
  { TYPE old = value; BIT_FUNC(SUFFIX, Set)(&value, (BITS));                   \
    CHECK(value != old || BIT_FUNC(SUFFIX, Check)(&value, (BITS))); }          \
} while (0)

int main(void) {
  TEST_WIDTH(uint8_t, 8, 8U, UINT8_MAX);
  TEST_WIDTH(uint16_t, 16, 16U, UINT16_MAX);
  TEST_WIDTH(uint32_t, 32, 32U, UINT32_MAX);
  TEST_WIDTH(uint64_t, 64, 64U, UINT64_MAX);
  return 0;
}
