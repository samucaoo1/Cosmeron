#include "../../Cosmeron/Modules/Bit/Bit.h"

#define CHECK(CONDITION)                                                       \
  do {                                                                         \
    if (CONDITION)                                                             \
      return __LINE__;                                                         \
  } while (0)

#define TEST_WIDTH(TYPE, SUFFIX, BITS, MAXV)                                  \
  do {                                                                         \
    TYPE value = 0;                                                            \
    TYPE index = 0;                                                            \
    TYPE field = 0;                                                            \
    CHECK(BIT_FUNC(SUFFIX, Set)(&value, 0U) != STATUS_CONST(SUCCESS));        \
    CHECK(value != (TYPE)1 || !BIT_FUNC(SUFFIX, Check)(value, 0U));           \
    CHECK(BIT_FUNC(SUFFIX, Clear)(&value, 0U) != STATUS_CONST(SUCCESS));      \
    CHECK(value != 0);                                                         \
    CHECK(BIT_FUNC(SUFFIX, Flip)(&value, 1U) != STATUS_CONST(SUCCESS));       \
    CHECK(value != (TYPE)2);                                                   \
    BIT_FUNC(SUFFIX, MaskSet)(&value, (TYPE)5);                               \
    CHECK(!BIT_FUNC(SUFFIX, MaskCheckAll)(value, (TYPE)7));                   \
    BIT_FUNC(SUFFIX, MaskClear)(&value, (TYPE)1);                             \
    CHECK(BIT_FUNC(SUFFIX, MaskCheckAny)(value, (TYPE)1));                    \
    BIT_FUNC(SUFFIX, MaskFlip)(&value, (TYPE)6);                              \
    CHECK(value != 0);                                                         \
    CHECK(BIT_FUNC(SUFFIX, Popcount)((TYPE)(MAXV)) != (BITS));                \
    CHECK(!BIT_FUNC(SUFFIX, IsPowerOfTwo)((TYPE)1));                           \
    CHECK(BIT_FUNC(SUFFIX, IsPowerOfTwo)((TYPE)3));                            \
    CHECK(BIT_FUNC(SUFFIX, IndexLSB)((TYPE)8, &index) !=                      \
          STATUS_CONST(SUCCESS));                                              \
    CHECK(index != (TYPE)3);                                                   \
    CHECK(BIT_FUNC(SUFFIX, IndexMSB)((TYPE)8, &index) !=                      \
          STATUS_CONST(SUCCESS));                                              \
    CHECK(index != (TYPE)3);                                                   \
    CHECK(BIT_FUNC(SUFFIX, IndexLSB)((TYPE)0, &index) !=                      \
          STATUS_CONST(NOT_FOUND));                                            \
    CHECK(BIT_FUNC(SUFFIX, RotateLeft)((TYPE)1, (TYPE)1) != (TYPE)2);         \
    CHECK(BIT_FUNC(SUFFIX, RotateRight)((TYPE)2, (TYPE)1) != (TYPE)1);        \
    value = (TYPE)0x3C;                                                        \
    CHECK(BIT_FUNC(SUFFIX, Extract)(value, 5U, 2U, &field) !=                 \
          STATUS_CONST(SUCCESS));                                              \
    CHECK(field != (TYPE)0xF);                                                 \
    CHECK(BIT_FUNC(SUFFIX, Insert)(&value, 3U, 0U, (TYPE)5) !=                \
          STATUS_CONST(SUCCESS));                                              \
    CHECK(BIT_FUNC(SUFFIX, Extract)(value, 3U, 0U, &field) !=                 \
          STATUS_CONST(SUCCESS));                                              \
    CHECK(field != (TYPE)5);                                                   \
    {                                                                          \
      TYPE old = value;                                                        \
      CHECK(BIT_FUNC(SUFFIX, Set)(&value, (BITS)) !=                          \
            STATUS_CONST(OUT_OF_RANGE));                                       \
      CHECK(value != old || BIT_FUNC(SUFFIX, Check)(value, (BITS)));          \
    }                                                                          \
  } while (0)

int main(void) {
  TEST_WIDTH(uint8_t, 8, 8U, UINT8_MAX);
  TEST_WIDTH(uint16_t, 16, 16U, UINT16_MAX);
  TEST_WIDTH(uint32_t, 32, 32U, UINT32_MAX);
  TEST_WIDTH(uint64_t, 64, 64U, UINT64_MAX);
  return 0;
}
