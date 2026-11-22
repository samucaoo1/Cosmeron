#include "../../Cosmeron/Modules/Text/Text.h"

#include <stdio.h>

typedef struct ExampleCoord {
  int x;
  int y;
} ExampleCoord;

static OPSTATUS FormatExampleCoord(MODULE_TYPE(Text, Writer) *writer,
                                   const void *value,
                                   MODULE_TYPE(Text, View8) spec) {
  (void)spec;
  const ExampleCoord *coord = (const ExampleCoord *)value;
  return MODULE_FUNC(Text, Format, To)(writer, "(%d, %d)", coord->x, coord->y);
}

int main(void) {
  /* 1. Owned UTF-8 String + Writer */
  MODULE_TYPE(Text, String8) str = {0};
  (void)MODULE_FUNC(Text, String8, Create)(&str, 16U);

  MODULE_TYPE(Text, Writer) strWriter =
      MODULE_FUNC(Text, Writer, ToString)(&str);
  (void)MODULE_FUNC(Text, Writer, WriteCStr)(&strWriter, "Cos");
  (void)MODULE_FUNC(Text, Writer, WriteCStr)(&strWriter, "meron");
  (void)MODULE_FUNC(Text, Writer, WriteCodepoint)(&strWriter, '!');
  (void)MODULE_FUNC(Text, Writer, Finish)(&strWriter);

  size_t codepoints = 0U;
  (void)MODULE_FUNC(Text, String8, CountCodepoints)(&str, &codepoints);
  printf("String8=\"%s\" codepoints=%zu\n",
         MODULE_FUNC(Text, String8, CStr)(&str), codepoints);

  /* 2. Non-owning View */
  MODULE_TYPE(Text, View8) view = MODULE_FUNC(Text, String8, View)(&str);
  printf("View8 starts with \"Cos\": %s\n",
         MODULE_FUNC(Text, View8, StartsWith)(
             view, MODULE_FUNC(Text, View8, FromCStr)("Cos"))
             ? "true"
             : "false");

  /* 3. Format with native %v, %t, and %! adapter */
  ExampleCoord origin = {.x = 10, .y = 20};
  char out[96] = {0};
  size_t written = 0U;
  (void)MODULE_FUNC(Text, Format, Buffer)(
      out, sizeof(out), &written, "view=%v ok=%t pos=%!", view, true,
      FormatExampleCoord, &origin);
  printf("Formatted: %s\n", out);

  /* 4. Human-readable byte sizes */
  char sizeBuf[32] = {0};
  (void)MODULE_FUNC(Text, BytesHuman, Format)(sizeBuf, sizeof(sizeBuf),
                                               &written, 1536ULL);
  printf("BytesHuman: %s\n", sizeBuf);

  (void)MODULE_FUNC(Text, String8, Destroy)(&str);
  return 0;
}
