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
      CHECK(BIT_FUNC(SUFFIX, Set)(&value, (BITS)) !=                           \
            STATUS_CONST(OUT_OF_RANGE));                                       \
      CHECK(value != old || BIT_FUNC(SUFFIX, Check)(value, (BITS)));          \
    }                                                                          \
    /* Null argument checks */                                                 \
    CHECK(BIT_FUNC(SUFFIX, Set)(NULL, 0U) != STATUS_CONST(INVALID_ARGUMENT));  \
    CHECK(BIT_FUNC(SUFFIX, Clear)(NULL, 0U) !=                                \
          STATUS_CONST(INVALID_ARGUMENT));                                     \
    CHECK(BIT_FUNC(SUFFIX, Flip)(NULL, 0U) != STATUS_CONST(INVALID_ARGUMENT)); \
    CHECK(BIT_FUNC(SUFFIX, IndexLSB)((TYPE)1, NULL) !=                         \
          STATUS_CONST(INVALID_ARGUMENT));                                     \
    CHECK(BIT_FUNC(SUFFIX, IndexMSB)((TYPE)1, NULL) !=                         \
          STATUS_CONST(INVALID_ARGUMENT));                                     \
    CHECK(BIT_FUNC(SUFFIX, Extract)((TYPE)1, 0U, 0U, NULL) !=                 \
          STATUS_CONST(INVALID_ARGUMENT));                                     \
    CHECK(BIT_FUNC(SUFFIX, Insert)(NULL, 0U, 0U, (TYPE)1) !=                   \
          STATUS_CONST(INVALID_ARGUMENT));                                     \
    /* Out of range checks */                                                  \
    CHECK(BIT_FUNC(SUFFIX, Extract)((TYPE)1, 0U, 1U, &field) !=               \
          STATUS_CONST(OUT_OF_RANGE));                                         \
    CHECK(BIT_FUNC(SUFFIX, Insert)(&value, 0U, 1U, (TYPE)1) !=                \
          STATUS_CONST(OUT_OF_RANGE));                                         \
    /* FunctionTable verification */                                           \
    const struct NS(BIT_PKG(SUFFIX), FunctionTable) *ft =                    \
        NS(BIT_PKG(SUFFIX), GetFunctionTable)();                             \
    CHECK(ft == NULL);                                                         \
    CHECK(ft->Set != &BIT_FUNC(SUFFIX, Set));                                  \
    CHECK(ft->Check != &BIT_FUNC(SUFFIX, Check));                              \
    CHECK(PACKAGE_CALL(BIT_PKG(SUFFIX), Check, (TYPE)0, 0U) != false);        \
  } while (0)

#define TEST_WIDTH_ROW(ROW)                                                   \
  TEST_WIDTH(BIT_WIDTH_GET(TYPE, ROW), BIT_WIDTH_GET(SUFFIX, ROW),             \
             BIT_WIDTH_GET(BITS, ROW), BIT_WIDTH_GET(MAX, ROW));

int main(void) {
  BIT_WIDTH_TABLE(TEST_WIDTH_ROW)
  return 0;
}
