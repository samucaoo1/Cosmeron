#include "../../Cosmeron/Modules/Text/Text.h"

#include <stdio.h>

int main(void) {
  /* 1. String Builder -> Owned UTF-8 String */
  MODULE_TYPE(Text, Builder) builder;
  (void)MODULE_FUNC(Text, Builder, Create)(&builder, 16U);
  (void)MODULE_FUNC(Text, Builder, AppendCStr)(&builder, "Cos");
  (void)MODULE_FUNC(Text, Builder, AppendCStr)(&builder, "meron");

  MODULE_TYPE(Text, String8) str;
  (void)MODULE_FUNC(Text, Builder, BuildString)(&builder, &str);
  (void)MODULE_FUNC(Text, String8, AppendCodepoint)(&str, '!');

  size_t codepoints = 0U;
  (void)MODULE_FUNC(Text, String8, CountCodepoints)(&str, &codepoints);
  printf("String8=\"%s\" codepoints=%zu\n",
         MODULE_FUNC(Text, String8, CStr)(&str), codepoints);

  /* 2. Non-owning View */
  MODULE_TYPE(Text, View8) view =
      MODULE_FUNC(Text, View8, FromCStr)(MODULE_FUNC(Text, String8, CStr)(&str));
  printf("View8 starts with \"Cos\": %s\n",
         MODULE_FUNC(Text, View8, StartsWith)(
             view, MODULE_FUNC(Text, View8, FromCStr)("Cos"))
             ? "true"
             : "false");

  (void)MODULE_FUNC(Text, String8, Destroy)(&str);
  (void)MODULE_FUNC(Text, Builder, Destroy)(&builder);

  /* 3. Format */
  char out[64] = {0};
  size_t written = 0U;
  (void)MODULE_FUNC(Text, Format, Buffer)(out, sizeof(out), &written,
                                          "Hello, %s (%d)", "Cosmeron", 11);
  printf("Formatted: %s\n", out);

  return 0;
}
