#include "../../Cosmeron/Modules/Text/Text.h"
#include <assert.h>
#include <stdarg.h>
#include <string.h>

#define CHECK_STATUS(EXPR)                                                     \
  do {                                                                         \
    if ((EXPR) != STATUS_CONST(SUCCESS)) return __LINE__;                      \
  } while (0)

#define CHECK_TRUE(EXPR)                                                       \
  do {                                                                         \
    if (!(EXPR)) return __LINE__;                                              \
  } while (0)

#define CHECK_STR(ACTUAL, EXPECTED)                                            \
  do {                                                                         \
    if (strcmp((ACTUAL), (EXPECTED)) != 0) return __LINE__;                    \
  } while (0)

static OPSTATUS helper_vformat_string(TEXT_STRING_TYPE(8) * str,
                                      const char *fmt, ...) {
  OPSTATUS status;
  va_list args;
  va_start(args, fmt);
  status = Text_String8_Format_Variadic(str, fmt, args);
  va_end(args);
  return status;
}

static OPSTATUS helper_vappend_format_string(TEXT_STRING_TYPE(8) * str,
                                             const char *fmt, ...) {
  OPSTATUS status;
  va_list args;
  va_start(args, fmt);
  status = Text_String8_AppendFormat_Variadic(str, fmt, args);
  va_end(args);
  return status;
}

static int test_string8_lifecycle_and_basics(void) {
  TEXT_STRING_TYPE(8) s;
  CHECK_STATUS(Text_String8_Init(&s));
  CHECK_TRUE(Text_String8_IsEmpty(&s));
  CHECK_TRUE(Text_String8_Length(&s) == 0U);
  CHECK_TRUE(Text_String8_Capacity(&s) == 0U);

  const char *cstr = NULL;
  CHECK_STATUS(Text_String8_CStr(&s, &cstr));
  CHECK_STR(cstr, "");

  CHECK_STATUS(Text_String8_Reserve(&s, 32U));
  CHECK_TRUE(Text_String8_Capacity(&s) >= 32U);
  CHECK_TRUE(Text_String8_IsEmpty(&s));

  CHECK_STATUS(Text_String8_FromCStr(&s, "Hello, Cosmeron!"));
  CHECK_TRUE(!Text_String8_IsEmpty(&s));
  CHECK_TRUE(Text_String8_Length(&s) == 16U);
  CHECK_STATUS(Text_String8_CStr(&s, &cstr));
  CHECK_STR(cstr, "Hello, Cosmeron!");

  CHECK_STATUS(Text_String8_ShrinkToFit(&s));
  CHECK_TRUE(Text_String8_Capacity(&s) == 16U);
  CHECK_STATUS(Text_String8_CStr(&s, &cstr));
  CHECK_STR(cstr, "Hello, Cosmeron!");

  Text_String8_Clear(&s);
  CHECK_TRUE(Text_String8_IsEmpty(&s));
  CHECK_TRUE(Text_String8_Length(&s) == 0U);
  CHECK_STATUS(Text_String8_CStr(&s, &cstr));
  CHECK_STR(cstr, "");

  Text_String8_Destroy(&s);
  CHECK_TRUE(Text_String8_IsEmpty(&s));
  CHECK_TRUE(Text_String8_Capacity(&s) == 0U);

  return 0;
}

static int test_string8_mutation_and_access(void) {
  TEXT_STRING_TYPE(8) s;
  CHECK_STATUS(Text_String8_Init(&s));

  CHECK_STATUS(Text_String8_PushBack(&s, 'H'));
  CHECK_STATUS(Text_String8_PushBack(&s, 'e'));
  CHECK_STATUS(Text_String8_PushBack(&s, 'y'));
  CHECK_TRUE(Text_String8_Length(&s) == 3U);
  CHECK_TRUE(Text_String8_EqualCStr(&s, "Hey"));

  char popped = '\0';
  CHECK_STATUS(Text_String8_PopBack(&s, &popped));
  CHECK_TRUE(popped == 'y');
  CHECK_TRUE(Text_String8_Length(&s) == 2U);
  CHECK_TRUE(Text_String8_EqualCStr(&s, "He"));

  CHECK_STATUS(Text_String8_Append(&s, "llo World"));
  CHECK_TRUE(Text_String8_EqualCStr(&s, "Hello World"));

  char *charPtr = NULL;
  CHECK_STATUS(Text_String8_At(&s, 1U, &charPtr));
  CHECK_TRUE(charPtr != NULL && *charPtr == 'e');

  CHECK_STATUS(Text_String8_InsertChar(&s, 5U, ','));
  CHECK_TRUE(Text_String8_EqualCStr(&s, "Hello, World"));

  CHECK_STATUS(Text_String8_Insert(&s, 6U, " Beautiful"));
  CHECK_TRUE(Text_String8_EqualCStr(&s, "Hello, Beautiful World"));

  CHECK_STATUS(Text_String8_Erase(&s, 5U, 11U));
  CHECK_TRUE(Text_String8_EqualCStr(&s, "Hello World"));

  TEXT_STRING_TYPE(8) other;
  CHECK_STATUS(Text_String8_Init(&other));
  CHECK_STATUS(Text_String8_FromCStr(&other, "!!!"));
  CHECK_STATUS(Text_String8_AppendString(&s, &other));
  CHECK_TRUE(Text_String8_EqualCStr(&s, "Hello World!!!"));

  TEXT_STRING_TYPE(8) copyStr;
  CHECK_STATUS(Text_String8_Init(&copyStr));
  CHECK_STATUS(Text_String8_Copy(&copyStr, &s));
  CHECK_TRUE(Text_String8_Equal(&copyStr, &s));

  Text_String8_Destroy(&s);
  Text_String8_Destroy(&other);
  Text_String8_Destroy(&copyStr);
  return 0;
}

static int test_string8_algorithms_and_compare(void) {
  TEXT_STRING_TYPE(8) a;
  TEXT_STRING_TYPE(8) b;
  CHECK_STATUS(Text_String8_Init(&a));
  CHECK_STATUS(Text_String8_Init(&b));

  CHECK_STATUS(Text_String8_FromCStr(&a, "apple"));
  CHECK_STATUS(Text_String8_FromCStr(&b, "banana"));

  CMPOUT cmp;
  CHECK_STATUS(Text_String8_Compare(&a, &b, &cmp));
  CHECK_TRUE(cmp == COMPARISON_LOWER_CONST);

  CHECK_STATUS(Text_String8_Compare(&b, &a, &cmp));
  CHECK_TRUE(cmp == COMPARISON_HIGHER_CONST);

  CHECK_STATUS(Text_String8_FromCStr(&b, "apple"));
  CHECK_STATUS(Text_String8_Compare(&a, &b, &cmp));
  CHECK_TRUE(cmp == COMPARISON_EQUAL_CONST);
  CHECK_TRUE(Text_String8_Equal(&a, &b));

  CHECK_STATUS(Text_String8_FromCStr(&a, "Cosmeron Text Engine"));
  CHECK_TRUE(Text_String8_StartsWith(&a, "Cosmeron"));
  CHECK_TRUE(!Text_String8_StartsWith(&a, "Engine"));
  CHECK_TRUE(Text_String8_EndsWith(&a, "Engine"));
  CHECK_TRUE(!Text_String8_EndsWith(&a, "Cosmeron"));

  size_t idx = 0U;
  CHECK_STATUS(Text_String8_Find(&a, 'T', 0U, &idx));
  CHECK_TRUE(idx == 9U);

  CHECK_STATUS(Text_String8_FindStr(&a, "Text", 0U, &idx));
  CHECK_TRUE(idx == 9U);

  if (Text_String8_FindStr(&a, "NotFound", 0U, &idx) != STATUS_CONST(NOT_FOUND)) {
    return __LINE__;
  }

  TEXT_STRING_TYPE(8) sub;
  CHECK_STATUS(Text_String8_Init(&sub));
  CHECK_STATUS(Text_String8_Substr(&a, 9U, 4U, &sub));
  CHECK_TRUE(Text_String8_EqualCStr(&sub, "Text"));

  Text_String8_Destroy(&a);
  Text_String8_Destroy(&b);
  Text_String8_Destroy(&sub);
  return 0;
}

static int test_string8_utf8_and_format(void) {
  TEXT_STRING_TYPE(8) s;
  CHECK_STATUS(Text_String8_Init(&s));

  /* UTF-8 Codepoint appending: 'A' (1 byte), 'é' (2 bytes), '€' (3 bytes), '🚀' (4 bytes) */
  CHECK_STATUS(Text_String8_AppendCodepoint(&s, (TEXT_TYPE(TChar32))'A'));
  CHECK_STATUS(Text_String8_AppendCodepoint(&s, (TEXT_TYPE(TChar32))0x00E9));
  CHECK_STATUS(Text_String8_AppendCodepoint(&s, (TEXT_TYPE(TChar32))0x20AC));
  CHECK_STATUS(Text_String8_AppendCodepoint(&s, (TEXT_TYPE(TChar32))0x1F680));

  CHECK_TRUE(Text_String8_Validate(&s));

  size_t codepoints = 0U;
  CHECK_STATUS(Text_String8_CountCodepoints(&s, &codepoints));
  CHECK_TRUE(codepoints == 4U);
  CHECK_TRUE(Text_String8_Length(&s) == (1U + 2U + 3U + 4U));

  /* Trim testing */
  TEXT_STRING_TYPE(8) trimStr;
  CHECK_STATUS(Text_String8_Init(&trimStr));
  CHECK_STATUS(Text_String8_FromCStr(&trimStr, "  \t\n  Hello World! \r\n "));
  CHECK_STATUS(Text_String8_Trim(&trimStr));
  CHECK_TRUE(Text_String8_EqualCStr(&trimStr, "Hello World!"));
  Text_String8_Destroy(&trimStr);

  /* Formatting integration */
  CHECK_STATUS(Text_String8_Format(&s, "Format %d: %s", 42, "Cosmeron"));
  CHECK_TRUE(Text_String8_EqualCStr(&s, "Format 42: Cosmeron"));

  CHECK_STATUS(Text_String8_AppendFormat(&s, " (score = %.1f)", 99.5));
  CHECK_TRUE(Text_String8_EqualCStr(&s, "Format 42: Cosmeron (score = 99.5)"));

  /* Variadic format helper */
  CHECK_STATUS(helper_vformat_string(&s, "VFormat %s", "OK"));
  CHECK_TRUE(Text_String8_EqualCStr(&s, "VFormat OK"));

  CHECK_STATUS(helper_vappend_format_string(&s, " + %d", 100));
  CHECK_TRUE(Text_String8_EqualCStr(&s, "VFormat OK + 100"));

  Text_String8_Destroy(&s);
  return 0;
}

static int test_string16_and_32(void) {
  /* TString16 */
  TEXT_STRING_TYPE(16) s16;
  CHECK_STATUS(Text_String16_Init(&s16));
  TEXT_TYPE(TChar16) base16[] = {'a', 'b', 'c', 0};
  TEXT_TYPE(TChar16) needle16[] = {'b', 'c', 0};
  CHECK_STATUS(Text_String16_FromCStr(&s16, base16));
  CHECK_TRUE(Text_String16_Length(&s16) == 3U);
  size_t idx16 = 0U;
  CHECK_STATUS(Text_String16_FindStr(&s16, needle16, 0U, &idx16));
  CHECK_TRUE(idx16 == 1U);

  TEXT_STRING_TYPE(16) other16;
  CHECK_STATUS(Text_String16_Init(&other16));
  CHECK_STATUS(Text_String16_FromCStr(&other16, base16));
  CMPOUT cmp16;
  CHECK_STATUS(Text_String16_Compare(&s16, &other16, &cmp16));
  CHECK_TRUE(cmp16 == COMPARISON_EQUAL_CONST);
  CHECK_TRUE(Text_String16_Equal(&s16, &other16));
  Text_String16_Destroy(&s16);
  Text_String16_Destroy(&other16);

  /* TString32 */
  TEXT_STRING_TYPE(32) s32;
  CHECK_STATUS(Text_String32_Init(&s32));
  TEXT_TYPE(TChar32) base32[] = {0x1F680, 0x1F681, 0};
  CHECK_STATUS(Text_String32_FromCStr(&s32, base32));
  CHECK_TRUE(Text_String32_Length(&s32) == 2U);
  size_t idx32 = 0U;
  CHECK_STATUS(Text_String32_Find(&s32, 0x1F681, 0U, &idx32));
  CHECK_TRUE(idx32 == 1U);
  Text_String32_Destroy(&s32);

  return 0;
}

int main(void) {
  if (test_string8_lifecycle_and_basics() != 0) return 1;
  if (test_string8_mutation_and_access() != 0) return 2;
  if (test_string8_algorithms_and_compare() != 0) return 3;
  if (test_string8_utf8_and_format() != 0) return 4;
  if (test_string16_and_32() != 0) return 5;
  return 0;
}
