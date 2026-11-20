#include "../../Cosmeron/Modules/Text/Text.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

int main(void) {
  /* 1. Unicode Codepoint & Width */
  assert(MODULE_FUNC(Text, Codepoint, IsValid)(0x1F600U));
  assert(!MODULE_FUNC(Text, Codepoint, IsScalar)(0xD800U));
  assert(MODULE_FUNC(Text, Codepoint, IsASCII)('A'));
  int cpWidth = 0;
  assert(MODULE_FUNC(Text, Width, Codepoint)('A', &cpWidth) ==
         STATUS_CONST(SUCCESS));
  assert(cpWidth == 1);

  /* 2. CString */
  assert(MODULE_FUNC(Text, CString, Length)("Cosmeron") == 8U);
  assert(MODULE_FUNC(Text, CString, IsAscii)("Cosmeron"));

  /* 3. View */
  MODULE_TYPE(Text, View8) view =
      MODULE_FUNC(Text, View8, FromCStr)("Hello");
  assert(MODULE_FUNC(Text, View8, Length)(view) == 5U);
  assert(MODULE_FUNC(Text, View8, StartsWith)(
      view, MODULE_FUNC(Text, View8, FromCStr)("He")));

  /* 4. Builder & String8 */
  MODULE_TYPE(Text, Builder) builder;
  assert(MODULE_FUNC(Text, Builder, Create)(&builder, 16U) ==
         STATUS_CONST(SUCCESS));
  assert(MODULE_FUNC(Text, Builder, AppendCStr)(&builder, "Cos") ==
         STATUS_CONST(SUCCESS));
  assert(MODULE_FUNC(Text, Builder, AppendCStr)(&builder, "meron") ==
         STATUS_CONST(SUCCESS));
  assert(strcmp(MODULE_FUNC(Text, Builder, CStr)(&builder), "Cosmeron") == 0);

  MODULE_TYPE(Text, String8) str8;
  assert(MODULE_FUNC(Text, Builder, BuildString)(&builder, &str8) ==
         STATUS_CONST(SUCCESS));
  assert(MODULE_FUNC(Text, String8, Validate)(&str8));
  assert(MODULE_FUNC(Text, String8, AppendCodepoint)(&str8, '!') ==
         STATUS_CONST(SUCCESS));
  size_t cpCount = 0U;
  assert(MODULE_FUNC(Text, String8, CountCodepoints)(&str8, &cpCount) ==
         STATUS_CONST(SUCCESS));
  assert(cpCount == 9U);
  assert(strcmp(MODULE_FUNC(Text, String8, CStr)(&str8), "Cosmeron!") == 0);
  assert(MODULE_FUNC(Text, String8, Destroy)(&str8) == STATUS_CONST(SUCCESS));
  assert(MODULE_FUNC(Text, Builder, Destroy)(&builder) ==
         STATUS_CONST(SUCCESS));

  /* 5. Attributed (TText_Attributed) */
  MODULE_TYPE(Text, Attributed) attr;
  assert(MODULE_FUNC(Text, Attributed, Create)(&attr, 16U) ==
         STATUS_CONST(SUCCESS));
  assert(MODULE_FUNC(Text, Attributed, SetText)(&attr, "Hi", 2U) ==
         STATUS_CONST(SUCCESS));
  assert(strcmp(MODULE_FUNC(Text, Attributed, Text)(&attr), "Hi") == 0);
  assert(MODULE_FUNC(Text, Attributed, Destroy)(&attr) ==
         STATUS_CONST(SUCCESS));

  /* 6. Format with %TBigint128 & %TDecimal128 */
  MODULE_TYPE(Numeric, Bigint128) bi = {0};
  MODULE_FUNC(Numeric, Bigint128, Init)(&bi);
  bi.limb[0] = 42ULL;
  MODULE_TYPE(Numeric, Decimal128) dec = {0};
  MODULE_FUNC(Numeric, Decimal128, Init)(&dec);
  dec.limb[0] = 99ULL;
  char fmtBuf[64] = {0};
  size_t fmtWritten = 0U;
  assert(MODULE_FUNC(Text, Format, Buffer)(
             fmtBuf, sizeof(fmtBuf), &fmtWritten, "%TBigint128:%TDecimal128",
             &bi, &dec) == STATUS_CONST(SUCCESS));
  assert(strcmp(fmtBuf, "42:99") == 0);

  printf("Text tests passed!\n");
  return 0;
}
