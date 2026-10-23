#include <assert.h>
#include <string.h>

#include "../../Cosmeron/Modules/Text/Text.h"

#define OK(x) assert((x) == STATUS_CONST(SUCCESS))

static void test_length_and_predicates(void) {
  assert(Text_CString_Length(NULL) == 0U);
  assert(Text_CString_Length("") == 0U);
  assert(Text_CString_Length("Cosmeron") == 8U);

  assert(Text_CString_LengthBounded(NULL, 10U) == 0U);
  assert(Text_CString_LengthBounded("Cosmeron", 4U) == 4U);
  assert(Text_CString_LengthBounded("Cosmeron", 20U) == 8U);

  assert(Text_CString_IsEmpty(NULL) == true);
  assert(Text_CString_IsEmpty("") == true);
  assert(Text_CString_IsEmpty("a") == false);

  assert(Text_CString_IsAscii(NULL) == false);
  assert(Text_CString_IsAscii("") == true);
  assert(Text_CString_IsAscii("Hello World 123!") == true);
  assert(Text_CString_IsAscii("Olá") == false);

  assert(Text_CString_IsAsciiBounded("Hello\x80World", 5U) == true);
  assert(Text_CString_IsAsciiBounded("Hello\x80World", 6U) == false);
}

static void test_copy_and_concat(void) {
  char buf[16];
  size_t written = 0U;

  assert(Text_CString_Copy(NULL, 10U, "abc", &written) == STATUS_CONST(INVALID_ARGUMENT));
  assert(Text_CString_Copy(buf, 0U, "abc", &written) == STATUS_CONST(INVALID_ARGUMENT));
  assert(Text_CString_Copy(buf, sizeof(buf), NULL, &written) == STATUS_CONST(INVALID_ARGUMENT));

  OK(Text_CString_Copy(buf, sizeof(buf), "Hello", &written));
  assert(written == 5U);
  assert(strcmp(buf, "Hello") == 0);

  /* Insufficient space for copy */
  char small[4];
  assert(Text_CString_Copy(small, sizeof(small), "Hello", &written) == STATUS_CONST(INSUFFICIENT_SPACE));
  assert(small[sizeof(small) - 1U] == '\0');
  assert(strcmp(small, "Hel") == 0);

  /* Bounded copy */
  OK(Text_CString_CopyBounded(buf, sizeof(buf), "Hello World", 5U, &written));
  assert(written == 5U);
  assert(strcmp(buf, "Hello") == 0);

  /* Concat */
  OK(Text_CString_Concat(buf, sizeof(buf), " World", &written));
  assert(written == 11U);
  assert(strcmp(buf, "Hello World") == 0);

  /* Concat overflow */
  assert(Text_CString_Concat(buf, sizeof(buf), " Extremely Long String", &written) == STATUS_CONST(INSUFFICIENT_SPACE));
  assert(buf[sizeof(buf) - 1U] == '\0');

  /* Bounded concat */
  OK(Text_CString_Copy(buf, sizeof(buf), "Cosm", &written));
  OK(Text_CString_ConcatBounded(buf, sizeof(buf), "eronArchitecture", 4U, &written));
  assert(written == 8U);
  assert(strcmp(buf, "Cosmeron") == 0);
}

static void test_compare_and_equal(void) {
  assert(Text_CString_Compare(NULL, NULL) == COMPARISON_EQUAL_CONST);
  assert(Text_CString_Compare(NULL, "a") == COMPARISON_LOWER_CONST);
  assert(Text_CString_Compare("a", NULL) == COMPARISON_HIGHER_CONST);
  assert(Text_CString_Compare("abc", "abc") == COMPARISON_EQUAL_CONST);
  assert(Text_CString_Compare("abc", "abd") == COMPARISON_LOWER_CONST);
  assert(Text_CString_Compare("abd", "abc") == COMPARISON_HIGHER_CONST);

  assert(Text_CString_CompareBounded("abcX", "abcY", 3U) == COMPARISON_EQUAL_CONST);
  assert(Text_CString_CompareBounded("abcX", "abcY", 4U) == COMPARISON_LOWER_CONST);

  assert(Text_CString_CompareIgnoreCase("Cosmeron", "COSMERON") == COMPARISON_EQUAL_CONST);
  assert(Text_CString_CompareIgnoreCase("Alpha", "beta") == COMPARISON_LOWER_CONST);
  assert(Text_CString_CompareIgnoreCaseBounded("Alpha123", "ALPHA999", 5U) == COMPARISON_EQUAL_CONST);

  assert(Text_CString_Equal("test", "test") == true);
  assert(Text_CString_Equal("test", "Test") == false);
  assert(Text_CString_EqualIgnoreCase("test", "TEST") == true);
  assert(Text_CString_EqualBounded("test1", "test2", 4U) == true);
  assert(Text_CString_EqualIgnoreCaseBounded("TEST1", "test2", 4U) == true);
}

static void test_starts_and_ends_with(void) {
  assert(Text_CString_StartsWith("Cosmeron", "Cos") == true);
  assert(Text_CString_StartsWith("Cosmeron", "cos") == false);
  assert(Text_CString_StartsWithIgnoreCase("Cosmeron", "cos") == true);
  assert(Text_CString_StartsWith("Cosmeron", "CosmeronExtended") == false);

  assert(Text_CString_StartsWithBounded("Cosmeron", 3U, "Cos") == true);
  assert(Text_CString_StartsWithBounded("Cosmeron", 2U, "Cos") == false);

  assert(Text_CString_EndsWith("Cosmeron", "eron") == true);
  assert(Text_CString_EndsWith("Cosmeron", "ERON") == false);
  assert(Text_CString_EndsWithIgnoreCase("Cosmeron", "ERON") == true);
  assert(Text_CString_EndsWith("Cosmeron", "SuperCosmeron") == false);

  assert(Text_CString_EndsWithBounded("Cosmeron", 4U, "sm") == true);
}

static void test_find_and_search(void) {
  size_t idx = 0U;

  OK(Text_CString_FindChar("Cosmeron", 'm', &idx));
  assert(idx == 3U);
  assert(Text_CString_FindChar("Cosmeron", 'z', &idx) == STATUS_CONST(NOT_FOUND));

  assert(Text_CString_FindCharBounded("Cosmeron", 3U, 'm', &idx) == STATUS_CONST(NOT_FOUND));
  OK(Text_CString_FindCharBounded("Cosmeron", 4U, 'm', &idx));
  assert(idx == 3U);

  OK(Text_CString_FindLastChar("banana", 'a', &idx));
  assert(idx == 5U);
  OK(Text_CString_FindLastCharBounded("banana", 4U, 'a', &idx));
  assert(idx == 3U);

  OK(Text_CString_FindSubstring("hello cosmeron world", "cosmeron", &idx));
  assert(idx == 6U);
  assert(Text_CString_FindSubstring("hello", "world", &idx) == STATUS_CONST(NOT_FOUND));
  assert(Text_CString_FindSubstringBounded("hello cosmeron world", 10U, "cosmeron", &idx) == STATUS_CONST(NOT_FOUND));
  OK(Text_CString_FindSubstringBounded("hello cosmeron world", 14U, "cosmeron", &idx));
  assert(idx == 6U);

  /* Span and ComplementSpan */
  size_t spanLen = 0U;
  OK(Text_CString_Span("12345abc", "0123456789", &spanLen));
  assert(spanLen == 5U);

  OK(Text_CString_ComplementSpan("hello, world", ",;", &spanLen));
  assert(spanLen == 5U);
}

static void test_trim_and_manipulation(void) {
  char out[32];
  size_t len = 0U;

  OK(Text_CString_Trim("  \t\n  hello world  \r\n ", NULL, out, sizeof(out), &len));
  assert(len == 11U);
  assert(strcmp(out, "hello world") == 0);

  OK(Text_CString_TrimStart("###title###", "#", out, sizeof(out), &len));
  assert(strcmp(out, "title###") == 0);

  OK(Text_CString_TrimEnd("###title###", "#", out, sizeof(out), &len));
  assert(strcmp(out, "###title") == 0);

  /* Count and Replace */
  size_t count = 0U;
  OK(Text_CString_CountChar("abracadabra", 'a', &count));
  assert(count == 5U);

  char mutableStr[] = "foo-bar-baz";
  OK(Text_CString_ReplaceChar(mutableStr, '-', '_', &count));
  assert(count == 2U);
  assert(strcmp(mutableStr, "foo_bar_baz") == 0);

  /* Duplicate and Free */
  char *dup = NULL;
  OK(Text_CString_Duplicate("Cosmeron C11", &dup));
  assert(dup != NULL);
  assert(strcmp(dup, "Cosmeron C11") == 0);
  Text_CString_Free(dup);

  OK(Text_CString_DuplicateBounded("Cosmeron C11", 8U, &dup));
  assert(dup != NULL);
  assert(strcmp(dup, "Cosmeron") == 0);
  Text_CString_Free(dup);
}

#ifndef PACKAGE_DISABLE_FUNCTION_TABLE
static void test_package_and_function_table(void) {
  const Text_CString_FunctionTable *api = Text_CString_GetFunctionTable();
  assert(api != NULL);
  assert(api->Length("Cosmeron") == 8U);
  assert(api->IsEmpty(""));
  assert(api->Equal("abc", "abc"));
  assert(PACKAGE_CALL(Text_CString, Length, "Hello World") == 11U);
  assert(PACKAGE_CALL(Text_CString, EqualIgnoreCase, "foo", "FOO") == true);
}
#endif

int main(void) {
  test_length_and_predicates();
  test_copy_and_concat();
  test_compare_and_equal();
  test_starts_and_ends_with();
  test_find_and_search();
  test_trim_and_manipulation();
#ifndef PACKAGE_DISABLE_FUNCTION_TABLE
  test_package_and_function_table();
#endif
  return 0;
}

