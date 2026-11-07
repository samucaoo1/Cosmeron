#include "../../Cosmeron/Modules/Text/Text.h"
#include <assert.h>
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

static int test_view8_basics(void) {
  TEXT_VIEW_TYPE(8) empty = Text_View8_Empty();
  CHECK_TRUE(Text_View8_IsEmpty(empty));
  CHECK_TRUE(Text_View8_Length(empty) == 0U);
  CHECK_TRUE(Text_View8_Data(empty) == NULL);

  const char *sample = "Hello, Cosmeron!";
  TEXT_VIEW_TYPE(8) v = Text_View8_FromCStr(sample);
  CHECK_TRUE(!Text_View8_IsEmpty(v));
  CHECK_TRUE(Text_View8_Length(v) == 16U);
  CHECK_TRUE(Text_View8_Data(v) == sample);

  const char *ch = NULL;
  CHECK_STATUS(Text_View8_At(v, 0U, &ch));
  CHECK_TRUE(*ch == 'H');
  CHECK_STATUS(Text_View8_At(v, 15U, &ch));
  CHECK_TRUE(*ch == '!');
  CHECK_TRUE(Text_View8_At(v, 16U, &ch) == STATUS_CONST(OUT_OF_RANGE));

  TEXT_VIEW_TYPE(8) hello = Text_View8_Slice(v, 0U, 5U);
  CHECK_TRUE(Text_View8_Length(hello) == 5U);
  CHECK_TRUE(Text_View8_EqualCStr(hello, "Hello"));

  TEXT_VIEW_TYPE(8) prefix = Text_View8_Prefix(v, 5U);
  CHECK_TRUE(Text_View8_Equal(prefix, hello));

  TEXT_VIEW_TYPE(8) suffix = Text_View8_Suffix(v, 9U);
  CHECK_TRUE(Text_View8_EqualCStr(suffix, "Cosmeron!"));

  TEXT_VIEW_TYPE(8) clamped = Text_View8_Slice(v, 7U, 100U);
  CHECK_TRUE(Text_View8_EqualCStr(clamped, "Cosmeron!"));

  TEXT_VIEW_TYPE(8) outOfRange = Text_View8_Slice(v, 20U, 5U);
  CHECK_TRUE(Text_View8_IsEmpty(outOfRange));

  return 0;
}

static int test_view8_comparison_and_search(void) {
  TEXT_VIEW_TYPE(8) a = Text_View8_FromCStr("apple");
  TEXT_VIEW_TYPE(8) b = Text_View8_FromCStr("banana");
  TEXT_VIEW_TYPE(8) a2 = Text_View8_FromCStr("apple");

  CHECK_TRUE(Text_View8_Compare(a, b) == COMPARISON_LOWER_CONST);
  CHECK_TRUE(Text_View8_Compare(b, a) == COMPARISON_HIGHER_CONST);
  CHECK_TRUE(Text_View8_Compare(a, a2) == COMPARISON_EQUAL_CONST);
  CHECK_TRUE(Text_View8_Equal(a, a2));
  CHECK_TRUE(!Text_View8_Equal(a, b));

  TEXT_VIEW_TYPE(8) text = Text_View8_FromCStr("abracadabra");
  CHECK_TRUE(Text_View8_StartsWithCStr(text, "abra"));
  CHECK_TRUE(Text_View8_EndsWithCStr(text, "dabra"));
  CHECK_TRUE(!Text_View8_StartsWithCStr(text, "cad"));
  CHECK_TRUE(!Text_View8_EndsWithCStr(text, "cad"));

  size_t idx = 0U;
  CHECK_STATUS(Text_View8_Find(text, 'c', 0U, &idx));
  CHECK_TRUE(idx == 4U);

  CHECK_STATUS(Text_View8_FindCStr(text, "cada", 0U, &idx));
  CHECK_TRUE(idx == 4U);

  CHECK_TRUE(Text_View8_FindCStr(text, "nope", 0U, &idx) ==
             STATUS_CONST(NOT_FOUND));

  return 0;
}

static int test_view8_utf8(void) {
  /* Valid UTF-8 with ASCII and multi-byte characters: "Olá, mundo! 🌍" */
  const char *utf8Str = "Ol\xC3\xA1, mundo! \xF0\x9F\x8C\x8D";
  TEXT_VIEW_TYPE(8) v = Text_View8_FromCStr(utf8Str);

  CHECK_TRUE(Text_View8_Validate(v));

  size_t count = 0U;
  CHECK_STATUS(Text_View8_CountCodepoints(v, &count));
  /* 'O', 'l', 'á' (U+00E1), ',', ' ', 'm', 'u', 'n', 'd', 'o', '!', ' ', '🌍' (U+1F30D) => 13 codepoints */
  CHECK_TRUE(count == 13U);

  TEXT_VIEW_TYPE(8) cursor = v;
  TEXT_TYPE(TChar32) cp = 0U;
  CHECK_STATUS(Text_View8_NextCodepoint(&cursor, &cp));
  CHECK_TRUE(cp == (TEXT_TYPE(TChar32))'O');
  CHECK_STATUS(Text_View8_NextCodepoint(&cursor, &cp));
  CHECK_TRUE(cp == (TEXT_TYPE(TChar32))'l');
  CHECK_STATUS(Text_View8_NextCodepoint(&cursor, &cp));
  CHECK_TRUE(cp == (TEXT_TYPE(TChar32))UINT32_C(0x00E1)); /* 'á' */

  /* Slicing by codepoints: slice "Olá" */
  TEXT_VIEW_TYPE(8) sub;
  CHECK_STATUS(Text_View8_SliceCodepoints(v, 0U, 3U, &sub));
  CHECK_TRUE(Text_View8_EqualCStr(sub, "Ol\xC3\xA1"));

  /* Slicing the emoji: index 12, count 1 */
  CHECK_STATUS(Text_View8_SliceCodepoints(v, 12U, 1U, &sub));
  CHECK_TRUE(Text_View8_EqualCStr(sub, "\xF0\x9F\x8C\x8D"));

  /* Trimming with ASCII and Unicode whitespace */
  const char *padded = "  \t\n  Hello World!  \r\n  ";
  TEXT_VIEW_TYPE(8) paddedView = Text_View8_FromCStr(padded);
  TEXT_VIEW_TYPE(8) trimmed = Text_View8_Trim(paddedView);
  CHECK_TRUE(Text_View8_EqualCStr(trimmed, "Hello World!"));

  TEXT_VIEW_TYPE(8) trimmedLeft = Text_View8_TrimLeft(paddedView);
  CHECK_TRUE(Text_View8_StartsWithCStr(trimmedLeft, "Hello World!"));

  TEXT_VIEW_TYPE(8) trimmedRight = Text_View8_TrimRight(paddedView);
  CHECK_TRUE(Text_View8_EndsWithCStr(trimmedRight, "Hello World!"));

  return 0;
}

static int test_view16_and_32(void) {
  const TEXT_TYPE(TChar16) raw16[] = {65U, 66U, 67U, 0U};
  TEXT_VIEW_TYPE(16) v16 = Text_View16_FromCStr(raw16);
  CHECK_TRUE(Text_View16_Length(v16) == 3U);
  CHECK_TRUE(!Text_View16_IsEmpty(v16));

  const TEXT_TYPE(TChar16) *c16 = NULL;
  CHECK_STATUS(Text_View16_At(v16, 1U, &c16));
  CHECK_TRUE(*c16 == 66U);

  TEXT_VIEW_TYPE(16) s16 = Text_View16_Slice(v16, 1U, 2U);
  CHECK_TRUE(Text_View16_Length(s16) == 2U);

  const TEXT_TYPE(TChar32) raw32[] = {1000U, 2000U, 3000U, 0U};
  TEXT_VIEW_TYPE(32) v32 = Text_View32_FromCStr(raw32);
  CHECK_TRUE(Text_View32_Length(v32) == 3U);
  CHECK_TRUE(!Text_View32_IsEmpty(v32));

  size_t idx32 = 0U;
  CHECK_STATUS(Text_View32_Find(v32, 2000U, 0U, &idx32));
  CHECK_TRUE(idx32 == 1U);

  return 0;
}

static int test_string_view_bridge(void) {
  TEXT_STRING_TYPE(8) str;
  CHECK_STATUS(Text_String8_Init(&str));
  CHECK_STATUS(Text_String8_FromCStr(&str, "Cosmeron Text View Bridge"));

  /* String -> View */
  TEXT_VIEW_TYPE(8) view = Text_String8_View(&str);
  CHECK_TRUE(Text_View8_Length(view) == Text_String8_Length(&str));
  CHECK_TRUE(Text_String8_EqualView(&str, view));

  /* String -> Slice View */
  TEXT_VIEW_TYPE(8) slice = Text_String8_Slice(&str, 9U, 4U);
  CHECK_TRUE(Text_View8_EqualCStr(slice, "Text"));

  /* View -> String */
  TEXT_STRING_TYPE(8) target;
  CHECK_STATUS(Text_String8_Init(&target));
  CHECK_STATUS(Text_String8_FromView(&target, slice));
  CHECK_TRUE(Text_String8_EqualCStr(&target, "Text"));

  /* Append View */
  TEXT_VIEW_TYPE(8) space = Text_View8_FromCStr(" ");
  TEXT_VIEW_TYPE(8) bridge = Text_View8_FromCStr("Bridge");
  CHECK_STATUS(Text_String8_AppendView(&target, space));
  CHECK_STATUS(Text_String8_AppendView(&target, bridge));
  CHECK_TRUE(Text_String8_EqualCStr(&target, "Text Bridge"));

  /* String8 SliceCodepoints */
  TEXT_STRING_TYPE(8) utf8Str;
  CHECK_STATUS(Text_String8_Init(&utf8Str));
  CHECK_STATUS(Text_String8_FromCStr(&utf8Str, "Caf\xC3\xA9 de Paris"));
  TEXT_VIEW_TYPE(8) cafeView;
  CHECK_STATUS(Text_String8_SliceCodepoints(&utf8Str, 0U, 4U, &cafeView));
  CHECK_TRUE(Text_View8_EqualCStr(cafeView, "Caf\xC3\xA9"));

  Text_String8_Destroy(&utf8Str);
  Text_String8_Destroy(&target);
  Text_String8_Destroy(&str);
  return 0;
}

int main(void) {
  if (test_view8_basics() != 0) return 1;
  if (test_view8_comparison_and_search() != 0) return 2;
  if (test_view8_utf8() != 0) return 3;
  if (test_view16_and_32() != 0) return 4;
  if (test_string_view_bridge() != 0) return 5;
  return 0;
}
