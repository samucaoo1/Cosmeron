#include "../../Cosmeron/Modules/Text/Text.h"

#include <assert.h>

static void test_palette_and_attributes(void) {
  const TEXT_GRID_COLOR_TYPE(RGB) *palette =
      TEXT_GRID_COLOR_FUNC(WindowsPalette)();
  TEXT_GRID_ATTRIBUTE_TYPE(Cell) cell = TEXT_GRID_ATTRIBUTE_FUNC(Default)();
  assert(TEXT_GRID_COLOR_RGB(0x12U, 0x34U, 0x56U) == UINT32_C(0x123456));
  assert(TEXT_GRID_COLOR_RED(UINT32_C(0x123456)) == 0x12U);
  assert(palette[TEXT_GRID_COLOR_NS(WINDOWS_INDEX_BRIGHT_RED)] ==
         TEXT_GRID_COLOR_NS(WINDOWS_BRIGHT_RED));
  assert((cell.flags & TEXT_GRID_ATTRIBUTE_NS(DEFAULT_FOREGROUND)) != 0U);
  assert((cell.flags & TEXT_GRID_ATTRIBUTE_NS(DEFAULT_BACKGROUND)) != 0U);
  assert(cell.underline == TEXT_GRID_UNDERLINE_NS(NONE));
}

static void test_char_variants(void) {
  TEXT_GRID_CHAR_TYPE(char) bytes = {0};
  TEXT_GRID_CHAR_TYPE(char16) words = {0};
  TDUAL_TYPE(uint16) size = {.col = 2U, .row = 2U};
  assert(TEXT_GRID_CHAR_FUNC(char, Create)(&bytes, size) == STATUS_NS(SUCCESS));
  assert(TEXT_GRID_CHAR_FUNC(char16, Create)(&words, size) ==
         STATUS_NS(SUCCESS));
  assert(bytes.data[3] == '\0' && words.data[3] == u'\0');
  assert(TEXT_GRID_CHAR_FUNC(char, Destroy)(&bytes) == STATUS_NS(SUCCESS));
  assert(TEXT_GRID_CHAR_FUNC(char16, Destroy)(&words) == STATUS_NS(SUCCESS));
}

static void test_grid_operations(void) {
  TEXT_GRID_TYPE(char32) grid = {0};
  TEXT_GRID_TYPE(char32) region = {0};
  TEXT_GRID_ATTRIBUTE_TYPE(Cell) red = TEXT_GRID_ATTRIBUTE_FUNC(Default)();
  TEXT_GRID_ATTRIBUTE_TYPE(Cell) transparent =
      TEXT_GRID_ATTRIBUTE_FUNC(Default)();
  TDUAL_TYPE(uint16) position = {.x = 1U, .y = 1U};
  TQUAD_TYPE(uint16) rectangle = {
      .left = 1U, .right = 2U, .top = 1U, .bottom = 2U};
  TText_Char32 character;
  TEXT_GRID_ATTRIBUTE_TYPE(Cell) attribute;

  red.foreground = TEXT_GRID_COLOR_NS(WINDOWS_BRIGHT_RED);
  red.background = TEXT_GRID_COLOR_NS(WINDOWS_DARK_BLUE);
  red.flags = TEXT_GRID_ATTRIBUTE_NS(BOLD);
  red.underline = TEXT_GRID_UNDERLINE_NS(DOUBLE);

  assert(TEXT_GRID_FUNC(char32, Create)(
             &grid, (TDUAL_TYPE(uint16)){.col = 4U, .row = 3U}) ==
         STATUS_NS(SUCCESS));
  assert(TEXT_GRID_FUNC(char32, IsValid)(&grid));
  assert(grid.chars.data[0] == U'\0');

  assert(TEXT_GRID_FUNC(char32, WriteCell)(&grid, position, ((TText_Char32)UINT32_C(0x754C)), red) ==
         STATUS_NS(SUCCESS));
  assert(TEXT_GRID_FUNC(char32, ReadCell)(&grid, position, &character,
                                          &attribute) == STATUS_NS(SUCCESS));
  assert(character == ((TText_Char32)UINT32_C(0x754C)) && attribute.foreground == red.foreground);

  assert(TEXT_GRID_FUNC(char32, FillRegion)(&grid, rectangle, U'X', red) ==
         STATUS_NS(SUCCESS));
  assert(TEXT_GRID_FUNC(char32, Read)(&grid, rectangle, &region) ==
         STATUS_NS(SUCCESS));
  assert(region.chars.size.col == 2U && region.chars.size.row == 2U);
  assert(region.chars.data[3] == U'X');

  assert(TEXT_GRID_FUNC(char32, Write)(
             &grid, (TDUAL_TYPE(uint16)){.x = 0U, .y = 0U}, &region) ==
         STATUS_NS(SUCCESS));
  assert(grid.chars.data[0] == U'X');

  transparent.foreground = TEXT_GRID_COLOR_NS(WINDOWS_WHITE);
  transparent.background = TEXT_GRID_COLOR_NS(WINDOWS_BRIGHT_GREEN);
  transparent.flags = TEXT_GRID_ATTRIBUTE_NS(TRANSPARENT_BACKGROUND);
  assert(TEXT_GRID_FUNC(char32, Fill)(&region, U'Z', transparent) ==
         STATUS_NS(SUCCESS));
  assert(TEXT_GRID_FUNC(char32, Blit)(
             &grid, (TDUAL_TYPE(uint16)){.x = 0U, .y = 0U}, &region) ==
         STATUS_NS(SUCCESS));
  assert(grid.chars.data[0] == U'Z');
  assert(grid.attributes.data[0].background == red.background);

  assert(TEXT_GRID_FUNC(char32, Resize)(
             &grid, (TDUAL_TYPE(uint16)){.col = 5U, .row = 4U}) ==
         STATUS_NS(SUCCESS));
  assert(grid.chars.data[0] == U'Z');
  assert(grid.chars.data[19] == U'\0');
  assert(TEXT_GRID_FUNC(char32, Destroy)(&region) == STATUS_NS(SUCCESS));
  assert(TEXT_GRID_FUNC(char32, Destroy)(&grid) == STATUS_NS(SUCCESS));
  assert(TEXT_GRID_FUNC(char32, IsEmpty)(&grid));
}

int main(void) {
  test_palette_and_attributes();
  test_char_variants();
  test_grid_operations();
  return 0;
}
