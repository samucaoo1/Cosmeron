#include "../../../Cosmeron/Modules/Text/Text.h"

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
  MODULE_TYPE(Text, View, 8) view =
      MODULE_FUNC(Text, View, 8, FromCStr)("Hello");
  assert(MODULE_FUNC(Text, View, 8, Length)(view) == 5U);
  assert(MODULE_FUNC(Text, View, 8, StartsWith)(
      view, MODULE_FUNC(Text, View, 8, FromCStr)("He")));

  /* 4. Builder */
  MODULE_TYPE(Text, Builder) builder;
  assert(MODULE_FUNC(Text, Builder, Create)(&builder, 16U) ==
         STATUS_CONST(SUCCESS));
  assert(MODULE_FUNC(Text, Builder, AppendCStr)(&builder, "Cos") ==
         STATUS_CONST(SUCCESS));
  assert(MODULE_FUNC(Text, Builder, AppendCStr)(&builder, "meron") ==
         STATUS_CONST(SUCCESS));
  assert(strcmp(MODULE_FUNC(Text, Builder, CStr)(&builder), "Cosmeron") == 0);
  assert(MODULE_FUNC(Text, Builder, Destroy)(&builder) ==
         STATUS_CONST(SUCCESS));

  printf("Text tests passed!\n");
  return 0;
}
