#include "../../Cosmeron/Modules/Text/Text.h"
#include <assert.h>
#include <stdarg.h>
#include <string.h>

#define CHECK_STATUS(EXPR)                                                     \
  do {                                                                         \
    if ((EXPR) != STATUS_CONST(SUCCESS)) return __LINE__;                      \
  } while (0)

#define CHECK_STR(ACTUAL, EXPECTED)                                            \
  do {                                                                         \
    if (strcmp((ACTUAL), (EXPECTED)) != 0) return __LINE__;                    \
  } while (0)

static OPSTATUS helper_vformat(char *buf, size_t cap, size_t *outW, const char *fmt, ...) {
  OPSTATUS status;
  va_list args;
  va_start(args, fmt);
  status = Text_Format_Variadic(buf, cap, outW, fmt, args);
  va_end(args);
  return status;
}

static OPSTATUS helper_vformat_unsafe(char *buf, size_t *outW, const char *fmt, ...) {
  OPSTATUS status;
  va_list args;
  va_start(args, fmt);
  status = Text_Format_Unsafe_Variadic(buf, outW, fmt, args);
  va_end(args);
  return status;
}

static int test_c23_standard_integers(void) {
  char buf[128];
  size_t written = 0U;

  CHECK_STATUS(Text_Format(buf, sizeof(buf), &written, "%d", 42));
  CHECK_STR(buf, "42");
  if (written != 2U) return __LINE__;

  CHECK_STATUS(Text_Format(buf, sizeof(buf), &written, "%+d", 42));
  CHECK_STR(buf, "+42");

  CHECK_STATUS(Text_Format(buf, sizeof(buf), &written, "% d", 42));
  CHECK_STR(buf, " 42");

  CHECK_STATUS(Text_Format(buf, sizeof(buf), &written, "%d", -42));
  CHECK_STR(buf, "-42");

  CHECK_STATUS(Text_Format(buf, sizeof(buf), &written, "%05d", 42));
  CHECK_STR(buf, "00042");

  CHECK_STATUS(Text_Format(buf, sizeof(buf), &written, "%-5d", 42));
  CHECK_STR(buf, "42   ");

  CHECK_STATUS(Text_Format(buf, sizeof(buf), &written, "%5d", 42));
  CHECK_STR(buf, "   42");

  CHECK_STATUS(Text_Format(buf, sizeof(buf), &written, "%.4d", 42));
  CHECK_STR(buf, "0042");

  CHECK_STATUS(Text_Format(buf, sizeof(buf), &written, "%u", 100U));
  CHECK_STR(buf, "100");

  CHECK_STATUS(Text_Format(buf, sizeof(buf), &written, "%o", 63U));
  CHECK_STR(buf, "77");

  CHECK_STATUS(Text_Format(buf, sizeof(buf), &written, "%#o", 63U));
  CHECK_STR(buf, "077");

  CHECK_STATUS(Text_Format(buf, sizeof(buf), &written, "%x", 0x2AU));
  CHECK_STR(buf, "2a");

  CHECK_STATUS(Text_Format(buf, sizeof(buf), &written, "%X", 0x2AU));
  CHECK_STR(buf, "2A");

  CHECK_STATUS(Text_Format(buf, sizeof(buf), &written, "%#x", 0x2AU));
  CHECK_STR(buf, "0x2a");

  CHECK_STATUS(Text_Format(buf, sizeof(buf), &written, "%#X", 0x2AU));
  CHECK_STR(buf, "0X2A");

  /* C23 binary format */
  CHECK_STATUS(Text_Format(buf, sizeof(buf), &written, "%b", 10U));
  CHECK_STR(buf, "1010");

  CHECK_STATUS(Text_Format(buf, sizeof(buf), &written, "%B", 10U));
  CHECK_STR(buf, "1010");

  CHECK_STATUS(Text_Format(buf, sizeof(buf), &written, "%#b", 10U));
  CHECK_STR(buf, "0b1010");

  CHECK_STATUS(Text_Format(buf, sizeof(buf), &written, "%#B", 10U));
  CHECK_STR(buf, "0B1010");

  /* Dynamic width and precision */
  CHECK_STATUS(Text_Format(buf, sizeof(buf), &written, "%*.*d", 6, 4, 42));
  CHECK_STR(buf, "  0042");

  /* Length modifiers */
  CHECK_STATUS(Text_Format(buf, sizeof(buf), &written, "%lld", 1234567890123LL));
  CHECK_STR(buf, "1234567890123");

  CHECK_STATUS(Text_Format(buf, sizeof(buf), &written, "%zu", (size_t)999U));
  CHECK_STR(buf, "999");

  return 0;
}

static int test_c23_standard_strings_and_chars(void) {
  char buf[128];
  size_t written = 0U;

  CHECK_STATUS(Text_Format(buf, sizeof(buf), &written, "%c", 'Z'));
  CHECK_STR(buf, "Z");

  CHECK_STATUS(Text_Format(buf, sizeof(buf), &written, "%3c", 'Z'));
  CHECK_STR(buf, "  Z");

  CHECK_STATUS(Text_Format(buf, sizeof(buf), &written, "%-3c", 'Z'));
  CHECK_STR(buf, "Z  ");

  CHECK_STATUS(Text_Format(buf, sizeof(buf), &written, "%s", "hello"));
  CHECK_STR(buf, "hello");

  CHECK_STATUS(Text_Format(buf, sizeof(buf), &written, "%8s", "hello"));
  CHECK_STR(buf, "   hello");

  CHECK_STATUS(Text_Format(buf, sizeof(buf), &written, "%-8s", "hello"));
  CHECK_STR(buf, "hello   ");

  CHECK_STATUS(Text_Format(buf, sizeof(buf), &written, "%.3s", "hello"));
  CHECK_STR(buf, "hel");

  CHECK_STATUS(Text_Format(buf, sizeof(buf), &written, "%s", (const char *)NULL));
  CHECK_STR(buf, "(null)");

  CHECK_STATUS(Text_Format(buf, sizeof(buf), &written, "%%"));
  CHECK_STR(buf, "%");

  int nVal = 0;
  CHECK_STATUS(Text_Format(buf, sizeof(buf), &written, "abc%n", &nVal));
  if (nVal != 3) return __LINE__;

  return 0;
}

static int test_c23_standard_floats(void) {
  char buf[128];
  size_t written = 0U;

  CHECK_STATUS(Text_Format(buf, sizeof(buf), &written, "%.2f", 3.14159));
  CHECK_STR(buf, "3.14");

  CHECK_STATUS(Text_Format(buf, sizeof(buf), &written, "%g", 2.5));
  CHECK_STR(buf, "2.5");

  return 0;
}

static int test_struct_formatting(void) {
  char buf[128];
  size_t written = 0U;

  /* TDual with default template {x, y} */
  Struct_TDual_int32 d1 = {.x = 10, .y = 20};
  CHECK_STATUS(Text_Format(buf, sizeof(buf), &written, "%TDual", &d1));
  CHECK_STR(buf, "{10, 20}");

  /* TDual with custom format string */
  CHECK_STATUS(Text_Format(buf, sizeof(buf), &written, "%(%d, %d)TDual", &d1));
  CHECK_STR(buf, "10, 20");

  CHECK_STATUS(Text_Format(buf, sizeof(buf), &written, "%(x=%d, y=%d)TDual", &d1));
  CHECK_STR(buf, "x=10, y=20");

  CHECK_STATUS(Text_Format(buf, sizeof(buf), &written, "%([%d : %d])TDual", &d1));
  CHECK_STR(buf, "[10 : 20]");

  /* TDual with x,y tokens */
  CHECK_STATUS(Text_Format(buf, sizeof(buf), &written, "%(x, y)TDual", &d1));
  CHECK_STR(buf, "10, 20");

  CHECK_STATUS(Text_Format(buf, sizeof(buf), &written, "%(x,y)TDual", &d1));
  CHECK_STR(buf, "10,20");

  /* TDual with double specialization */
  Struct_TDual_double d2 = {.x = 1.5, .y = 2.5};
  CHECK_STATUS(Text_Format(buf, sizeof(buf), &written, "%TDual_double", &d2));
  CHECK_STR(buf, "{1.5, 2.5}");

  CHECK_STATUS(Text_Format(buf, sizeof(buf), &written, "%(%.2f; %.2f)TDual", &d2));
  CHECK_STR(buf, "1.50; 2.50");

  /* TTriple with default template {x, y, z} */
  Struct_TTriple_int32 t1 = {.x = 1, .y = 2, .z = 3};
  CHECK_STATUS(Text_Format(buf, sizeof(buf), &written, "%TTriple", &t1));
  CHECK_STR(buf, "{1, 2, 3}");

  CHECK_STATUS(Text_Format(buf, sizeof(buf), &written, "%(%d / %d / %d)TTriple", &t1));
  CHECK_STR(buf, "1 / 2 / 3");

  /* TQuad */
  Struct_TQuad_int32 q1 = {.x = 5, .y = 6, .z = 7, .w = 8};
  CHECK_STATUS(Text_Format(buf, sizeof(buf), &written, "%TQuad", &q1));
  CHECK_STR(buf, "{5, 6, 7, 8}");

  /* TPair */
  Struct_TPair_int32 p1 = {.x = 100, .y = 200};
  CHECK_STATUS(Text_Format(buf, sizeof(buf), &written, "%TPair", &p1));
  CHECK_STR(buf, "{100, 200}");

  /* TPenta */
  TPENTA_TYPE(int32) penta1 = {.value1 = 1, .value2 = 2, .value3 = 3, .value4 = 4, .value5 = 5};
  CHECK_STATUS(Text_Format(buf, sizeof(buf), &written, "%TPenta", &penta1));
  CHECK_STR(buf, "{1, 2, 3, 4, 5}");

  /* Outer width alignment */
  CHECK_STATUS(Text_Format(buf, sizeof(buf), &written, "%-12TDual!", &d1));
  CHECK_STR(buf, "{10, 20}    !");

  return 0;
}

static int test_type_formatting(void) {
  char buf[128];
  size_t written = 0U;

  /* TBigint default 128-bit */
  TBIGINT_TYPE(128) big128 = {0};
  big128.limb[0] = 42U;
  CHECK_STATUS(Text_Format(buf, sizeof(buf), &written, "%TBigint", &big128));
  CHECK_STR(buf, "42");

  /* TBigint hex */
  CHECK_STATUS(Text_Format(buf, sizeof(buf), &written, "%(x)TBigint", &big128));
  CHECK_STR(buf, "2a");

  CHECK_STATUS(Text_Format(buf, sizeof(buf), &written, "%(X)TBigint", &big128));
  CHECK_STR(buf, "2A");

  CHECK_STATUS(Text_Format(buf, sizeof(buf), &written, "%#(x)TBigint", &big128));
  CHECK_STR(buf, "0x2a");

  /* TBigint binary */
  CHECK_STATUS(Text_Format(buf, sizeof(buf), &written, "%(b)TBigint", &big128));
  CHECK_STR(buf, "101010");

  CHECK_STATUS(Text_Format(buf, sizeof(buf), &written, "%#(b)TBigint", &big128));
  CHECK_STR(buf, "0b101010");

  /* TBigint256 */
  TBIGINT_TYPE(256) big256 = {0};
  big256.limb[0] = 255U;
  CHECK_STATUS(Text_Format(buf, sizeof(buf), &written, "%TBigint256", &big256));
  CHECK_STR(buf, "255");

  /* TDecimal 128-bit */
  TDECIMAL_TYPE(128) dec128 = {0};
  dec128.limb[0] = 100U;
  CHECK_STATUS(Text_Format(buf, sizeof(buf), &written, "%TDecimal", &dec128));
  CHECK_STR(buf, "100");

  return 0;
}

static int test_safety_and_va(void) {
  char buf[16];
  size_t written = 0U;

  /* Null format check */
  if (Text_Format(buf, sizeof(buf), &written, NULL) != STATUS_CONST(INVALID_ARGUMENT))
    return __LINE__;

  /* Null buffer with capacity > 0 check */
  if (Text_Format(NULL, 10U, &written, "%d", 1) != STATUS_CONST(INVALID_ARGUMENT))
    return __LINE__;

  /* Buffer length measurement with NULL buffer and 0 capacity */
  CHECK_STATUS(Text_Format(NULL, 0U, &written, "length=%d", 12345));
  if (written != 12U) return __LINE__;

  /* Truncation behavior on small capacity */
  char smallBuf[6];
  OPSTATUS status = Text_Format(smallBuf, sizeof(smallBuf), &written, "1234567890");
  if (status != STATUS_CONST(INSUFFICIENT_SPACE)) return __LINE__;
  if (written != 10U) return __LINE__;
  CHECK_STR(smallBuf, "12345");

  /* Text_Format_Unsafe */
  char unsafeBuf[64];
  CHECK_STATUS(Text_Format_Unsafe(unsafeBuf, &written, "val=%d", 777));
  CHECK_STR(unsafeBuf, "val=777");

  /* Text_Format_Variadic */
  CHECK_STATUS(helper_vformat(buf, sizeof(buf), &written, "v=%d", 88));
  CHECK_STR(buf, "v=88");

  /* Text_Format_Unsafe_Variadic */
  CHECK_STATUS(helper_vformat_unsafe(unsafeBuf, &written, "vu=%d", 99));
  CHECK_STR(unsafeBuf, "vu=99");

  return 0;
}

int main(void) {
  if (test_c23_standard_integers() != 0) return 1;
  if (test_c23_standard_strings_and_chars() != 0) return 2;
  if (test_c23_standard_floats() != 0) return 3;
  if (test_struct_formatting() != 0) return 4;
  if (test_type_formatting() != 0) return 5;
  if (test_safety_and_va() != 0) return 6;
  return 0;
}
