#include <assert.h>
#include <string.h>

#include "../../Cosmeron/Modules/Text/Text.h"

#define OK(x) assert((x) == STATUS_CONST(SUCCESS))

static void test_format_bytes_human(void) {
  char buf[64];
  size_t written = 0U;

  /* Basic format (IEC default) */
  OK(Text_Format_BytesHuman(buf, sizeof(buf), &written, 0U));
  assert(strcmp(buf, "0 B") == 0);
  assert(written == 3U);

  OK(Text_Format_BytesHuman(buf, sizeof(buf), &written, 512U));
  assert(strcmp(buf, "512 B") == 0);

  OK(Text_Format_BytesHuman(buf, sizeof(buf), &written, 1023U));
  assert(strcmp(buf, "1023 B") == 0);

  OK(Text_Format_BytesHuman(buf, sizeof(buf), &written, 1024U));
  assert(strcmp(buf, "1.00 KiB") == 0);

  OK(Text_Format_BytesHuman(buf, sizeof(buf), &written, 1536U));
  assert(strcmp(buf, "1.50 KiB") == 0);

  OK(Text_Format_BytesHuman(buf, sizeof(buf), &written, 1048576U));
  assert(strcmp(buf, "1.00 MiB") == 0);

  OK(Text_Format_BytesHuman(buf, sizeof(buf), &written, 1073741824ULL));
  assert(strcmp(buf, "1.00 GiB") == 0);

  /* SI standard (powers of 1000) */
  OK(Text_Format_BytesHumanEx(buf, sizeof(buf), &written, 1000U, TEXT_FORMAT_CONST(BYTE_UNIT_SI), 0U));
  assert(strcmp(buf, "1 KB") == 0);

  OK(Text_Format_BytesHumanEx(buf, sizeof(buf), &written, 2500000U, TEXT_FORMAT_CONST(BYTE_UNIT_SI), 1U));
  assert(strcmp(buf, "2.5 MB") == 0);

  /* Buffer too small */
  char small[4];
  assert(Text_Format_BytesHuman(small, sizeof(small), &written, 1048576U) == STATUS_CONST(INSUFFICIENT_SPACE));

  /* Invalid arguments */
  assert(Text_Format_BytesHuman(NULL, 10U, &written, 100U) == STATUS_CONST(INVALID_ARGUMENT));
  assert(Text_Format_BytesHuman(buf, 0U, &written, 100U) == STATUS_CONST(INVALID_ARGUMENT));
}

static void test_parse_bytes_human(void) {
  uint64_t bytes = 0U;

  /* Simple bytes */
  OK(Text_Parse_BytesHuman("0", &bytes));
  assert(bytes == 0U);

  OK(Text_Parse_BytesHuman("512 B", &bytes));
  assert(bytes == 512U);

  OK(Text_Parse_BytesHuman("1024", &bytes));
  assert(bytes == 1024U);

  /* IEC units */
  OK(Text_Parse_BytesHuman("1 KiB", &bytes));
  assert(bytes == 1024U);

  OK(Text_Parse_BytesHuman("1.5 KiB", &bytes));
  assert(bytes == 1536U);

  OK(Text_Parse_BytesHuman("1 MiB", &bytes));
  assert(bytes == 1048576ULL);

  OK(Text_Parse_BytesHuman("2 GiB", &bytes));
  assert(bytes == 2ULL * 1024ULL * 1024ULL * 1024ULL);

  /* SI units */
  OK(Text_Parse_BytesHuman("1 KB", &bytes));
  assert(bytes == 1000U);

  OK(Text_Parse_BytesHuman("2.5 MB", &bytes));
  assert(bytes == 2500000ULL);

  /* Flexible spacing and casing */
  OK(Text_Parse_BytesHuman("   64   mib   ", &bytes));
  assert(bytes == 64ULL * 1024ULL * 1024ULL);

  OK(Text_Parse_BytesHuman("10G", &bytes));
  assert(bytes == 10ULL * 1024ULL * 1024ULL * 1024ULL);

  /* Invalid arguments & parsing errors */
  assert(Text_Parse_BytesHuman(NULL, &bytes) == STATUS_CONST(INVALID_ARGUMENT));
  assert(Text_Parse_BytesHuman("1024", NULL) == STATUS_CONST(INVALID_ARGUMENT));
  assert(Text_Parse_BytesHuman("", &bytes) == STATUS_CONST(INVALID_ARGUMENT));
  assert(Text_Parse_BytesHuman("invalid", &bytes) == STATUS_CONST(INVALID_ARGUMENT));
  assert(Text_Parse_BytesHuman("100 XYZ", &bytes) == STATUS_CONST(INVALID_ARGUMENT));
  assert(Text_Parse_BytesHuman("100 MB trailing_garbage", &bytes) == STATUS_CONST(INVALID_ARGUMENT));

  /* Arithmetic overflow */
  assert(Text_Parse_BytesHuman("99999999999999999999999999999", &bytes) == STATUS_CONST(ARITHMETIC_OVERFLOW));
  assert(Text_Parse_BytesHuman("1000000000000 GiB", &bytes) == STATUS_CONST(ARITHMETIC_OVERFLOW));
}

#ifndef PACKAGE_DISABLE_FUNCTION_TABLE
static void test_package_and_function_table(void) {
  char buf[64];
  size_t written = 0U;
  uint64_t bytes = 0U;

  /* Direct package functions */
  OK(Text_BytesHuman_Format(buf, sizeof(buf), &written, 2048U));
  assert(strcmp(buf, "2.00 KiB") == 0);

  OK(Text_BytesHuman_Parse("2 KiB", &bytes));
  assert(bytes == 2048U);

  /* FunctionTable struct access */
  const Text_BytesHuman_FunctionTable *ft = Text_BytesHuman_GetFunctionTable();
  assert(ft != NULL);
  OK(ft->Format(buf, sizeof(buf), &written, 1024U));
  assert(strcmp(buf, "1.00 KiB") == 0);
  OK(ft->Parse("1 MiB", &bytes));
  assert(bytes == 1048576ULL);

  /* PACKAGE_CALL macro helper */
  OK(PACKAGE_CALL(Text_BytesHuman, Format, buf, sizeof(buf), &written, 4096U));
  assert(strcmp(buf, "4.00 KiB") == 0);
  OK(PACKAGE_CALL(Text_BytesHuman, Parse, "4 KiB", &bytes));
  assert(bytes == 4096U);
}
#endif

int main(void) {
  test_format_bytes_human();
  test_parse_bytes_human();
#ifndef PACKAGE_DISABLE_FUNCTION_TABLE
  test_package_and_function_table();
#endif
  return 0;
}

