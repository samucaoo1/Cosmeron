#include "../../Cosmeron/Modules/Text/Text.h"

#define CHECK_STATUS(EXPR) do { if ((EXPR) != STATUS_CONST(SUCCESS)) return __LINE__; } while (0)
#define CHECK_TRUE(EXPR) do { if (!(EXPR)) return __LINE__; } while (0)

#define TEST_CHAR_GRID(TYPE, SUFFIX, VALUE1, VALUE2)                            \
  do {                                                                         \
    TEXT_GRID_CHAR_TYPE(SUFFIX) grid = {0};                                    \
    TEXT_GRID_CHAR_TYPE(SUFFIX) clone = {0};                                   \
    TEXT_GRID_CHAR_TYPE(SUFFIX) region = {0};                                  \
    TYPE out = (TYPE)0;                                                        \
    TDUAL_TYPE(uint16) size = {.col = 2U, .row = 2U};                          \
    TDUAL_TYPE(uint16) pos = {.x = 1U, .y = 1U};                               \
    TQUAD_TYPE(uint16) rect = {.left = 0U, .right = 1U, .top = 0U, .bottom = 0U}; \
    CHECK_TRUE(TEXT_GRID_CHAR_FUNC(SUFFIX, IsEmpty)(&grid));                   \
    CHECK_TRUE(TEXT_GRID_CHAR_FUNC(SUFFIX, IsValid)(&grid));                   \
    CHECK_STATUS(TEXT_GRID_CHAR_FUNC(SUFFIX, Create)(&grid, size));            \
    CHECK_TRUE(TEXT_GRID_CHAR_FUNC(SUFFIX, IsValid)(&grid));                   \
    CHECK_STATUS(TEXT_GRID_CHAR_FUNC(SUFFIX, WriteCell)(&grid, pos, (TYPE)(VALUE1))); \
    CHECK_STATUS(TEXT_GRID_CHAR_FUNC(SUFFIX, ReadCell)(&grid, pos, &out));      \
    CHECK_TRUE(out == (TYPE)(VALUE1));                                         \
    CHECK_STATUS(TEXT_GRID_CHAR_FUNC(SUFFIX, Fill)(&grid, (TYPE)(VALUE2)));    \
    CHECK_STATUS(TEXT_GRID_CHAR_FUNC(SUFFIX, FillRegion)(&grid, rect, (TYPE)(VALUE1))); \
    CHECK_STATUS(TEXT_GRID_CHAR_FUNC(SUFFIX, Clone)(&clone, &grid));           \
    CHECK_STATUS(TEXT_GRID_CHAR_FUNC(SUFFIX, Read)(&grid, rect, &region));     \
    CHECK_STATUS(TEXT_GRID_CHAR_FUNC(SUFFIX, Write)(                           \
        &grid, (TDUAL_TYPE(uint16)){.x = 0U, .y = 1U}, &region));              \
    CHECK_STATUS(TEXT_GRID_CHAR_FUNC(SUFFIX, WriteRegion)(                     \
        &grid, (TDUAL_TYPE(uint16)){.x = 0U, .y = 0U}, &clone, rect));         \
    CHECK_STATUS(TEXT_GRID_CHAR_FUNC(SUFFIX, Resize)(                          \
        &grid, (TDUAL_TYPE(uint16)){.col = 3U, .row = 3U}));                   \
    CHECK_STATUS(TEXT_GRID_CHAR_FUNC(SUFFIX, Clear)(&grid));                   \
    CHECK_STATUS(TEXT_GRID_CHAR_FUNC(SUFFIX, Recreate)(                        \
        &grid, (TDUAL_TYPE(uint16)){.col = 1U, .row = 1U}));                   \
    CHECK_STATUS(TEXT_GRID_CHAR_FUNC(SUFFIX, Destroy)(&region));               \
    CHECK_STATUS(TEXT_GRID_CHAR_FUNC(SUFFIX, Destroy)(&clone));                \
    CHECK_STATUS(TEXT_GRID_CHAR_FUNC(SUFFIX, Destroy)(&grid));                 \
    CHECK_TRUE(TEXT_GRID_CHAR_FUNC(SUFFIX, IsEmpty)(&grid));                   \
  } while (0)

#define TEST_COMPOSITE_GRID(TYPE, SUFFIX, VALUE1, VALUE2)                      \
  do {                                                                         \
    TEXT_GRID_TYPE(SUFFIX) grid = {0};                                         \
    TEXT_GRID_TYPE(SUFFIX) clone = {0};                                        \
    TEXT_GRID_TYPE(SUFFIX) region = {0};                                       \
    TEXT_GRID_ATTRIBUTE_TYPE(Cell) cell = TEXT_GRID_ATTRIBUTE_FUNC(Default)(); \
    TEXT_GRID_ATTRIBUTE_TYPE(Cell) outCell;                                    \
    TYPE out = (TYPE)0;                                                        \
    TDUAL_TYPE(uint16) size = {.col = 2U, .row = 2U};                          \
    TDUAL_TYPE(uint16) pos = {.x = 1U, .y = 1U};                               \
    TQUAD_TYPE(uint16) rect = {.left = 0U, .right = 1U, .top = 0U, .bottom = 0U}; \
    cell.foreground = TEXT_GRID_COLOR_CONST(WINDOWS_BRIGHT_GREEN);                \
    CHECK_TRUE(TEXT_GRID_FUNC(SUFFIX, IsEmpty)(&grid));                        \
    CHECK_TRUE(TEXT_GRID_FUNC(SUFFIX, IsValid)(&grid));                        \
    CHECK_STATUS(TEXT_GRID_FUNC(SUFFIX, Create)(&grid, size));                 \
    CHECK_TRUE(TEXT_GRID_FUNC(SUFFIX, IsValid)(&grid));                        \
    CHECK_STATUS(TEXT_GRID_FUNC(SUFFIX, WriteCell)(                            \
        &grid, pos, (TYPE)(VALUE1), cell));                                    \
    CHECK_STATUS(TEXT_GRID_FUNC(SUFFIX, ReadCell)(&grid, pos, &out, &outCell));\
    CHECK_TRUE(out == (TYPE)(VALUE1));                                         \
    CHECK_STATUS(TEXT_GRID_FUNC(SUFFIX, Fill)(&grid, (TYPE)(VALUE2), cell));   \
    CHECK_STATUS(TEXT_GRID_FUNC(SUFFIX, FillRegion)(                           \
        &grid, rect, (TYPE)(VALUE1), cell));                                   \
    CHECK_STATUS(TEXT_GRID_FUNC(SUFFIX, Clone)(&clone, &grid));                \
    CHECK_STATUS(TEXT_GRID_FUNC(SUFFIX, Read)(&grid, rect, &region));          \
    CHECK_STATUS(TEXT_GRID_FUNC(SUFFIX, Write)(                                \
        &grid, (TDUAL_TYPE(uint16)){.x = 0U, .y = 1U}, &region));              \
    CHECK_STATUS(TEXT_GRID_FUNC(SUFFIX, WriteRegion)(                          \
        &grid, (TDUAL_TYPE(uint16)){.x = 0U, .y = 0U}, &clone, rect));         \
    CHECK_STATUS(TEXT_GRID_FUNC(SUFFIX, Blit)(                                 \
        &grid, (TDUAL_TYPE(uint16)){.x = 0U, .y = 0U}, &region));              \
    CHECK_STATUS(TEXT_GRID_FUNC(SUFFIX, BlitRegion)(                           \
        &grid, (TDUAL_TYPE(uint16)){.x = 0U, .y = 0U}, &clone, rect));         \
    CHECK_STATUS(TEXT_GRID_FUNC(SUFFIX, Resize)(                               \
        &grid, (TDUAL_TYPE(uint16)){.col = 3U, .row = 3U}));                   \
    CHECK_STATUS(TEXT_GRID_FUNC(SUFFIX, ClearWith)(&grid, cell));              \
    CHECK_STATUS(TEXT_GRID_FUNC(SUFFIX, Clear)(&grid));                        \
    CHECK_STATUS(TEXT_GRID_FUNC(SUFFIX, Recreate)(                             \
        &grid, (TDUAL_TYPE(uint16)){.col = 1U, .row = 1U}));                   \
    CHECK_STATUS(TEXT_GRID_FUNC(SUFFIX, Destroy)(&region));                    \
    CHECK_STATUS(TEXT_GRID_FUNC(SUFFIX, Destroy)(&clone));                     \
    CHECK_STATUS(TEXT_GRID_FUNC(SUFFIX, Destroy)(&grid));                      \
    CHECK_TRUE(TEXT_GRID_FUNC(SUFFIX, IsEmpty)(&grid));                        \
  } while (0)

static int test_attributes(void) {
  TEXT_GRID_ATTRIBUTE_TYPE(Grid) grid = {0};
  TEXT_GRID_ATTRIBUTE_TYPE(Grid) clone = {0};
  TEXT_GRID_ATTRIBUTE_TYPE(Grid) region = {0};
  TEXT_GRID_ATTRIBUTE_TYPE(Cell) cell = TEXT_GRID_ATTRIBUTE_FUNC(Default)();
  TEXT_GRID_ATTRIBUTE_TYPE(Cell) out;
  TDUAL_TYPE(uint16) size = {.col = 2U, .row = 2U};
  TDUAL_TYPE(uint16) pos = {.x = 1U, .y = 1U};
  TQUAD_TYPE(uint16) rect = {.left = 0U, .right = 1U, .top = 0U, .bottom = 0U};

  if (!TEXT_GRID_ATTRIBUTE_FUNC(IsEmpty)(&grid) ||
      !TEXT_GRID_ATTRIBUTE_FUNC(IsValid)(&grid))
    return __LINE__;
  if (TEXT_GRID_ATTRIBUTE_FUNC(Create)(&grid, size) != STATUS_CONST(SUCCESS))
    return __LINE__;
  cell.flags |= TEXT_GRID_ATTRIBUTE_CONST(BOLD);
  if (TEXT_GRID_ATTRIBUTE_FUNC(WriteCell)(&grid, pos, cell) != STATUS_CONST(SUCCESS))
    return __LINE__;
  if (TEXT_GRID_ATTRIBUTE_FUNC(ReadCell)(&grid, pos, &out) != STATUS_CONST(SUCCESS) ||
      (out.flags & TEXT_GRID_ATTRIBUTE_CONST(BOLD)) == 0U)
    return __LINE__;
  if (TEXT_GRID_ATTRIBUTE_FUNC(Fill)(&grid, cell) != STATUS_CONST(SUCCESS))
    return __LINE__;
  if (TEXT_GRID_ATTRIBUTE_FUNC(FillRegion)(&grid, rect, cell) != STATUS_CONST(SUCCESS))
    return __LINE__;
  if (TEXT_GRID_ATTRIBUTE_FUNC(Clone)(&clone, &grid) != STATUS_CONST(SUCCESS))
    return __LINE__;
  if (TEXT_GRID_ATTRIBUTE_FUNC(Read)(&grid, rect, &region) != STATUS_CONST(SUCCESS))
    return __LINE__;
  if (TEXT_GRID_ATTRIBUTE_FUNC(Write)(
          &grid, (TDUAL_TYPE(uint16)){.x = 0U, .y = 1U}, &region) != STATUS_CONST(SUCCESS))
    return __LINE__;
  if (TEXT_GRID_ATTRIBUTE_FUNC(WriteRegion)(
          &grid, (TDUAL_TYPE(uint16)){.x = 0U, .y = 0U}, &clone, rect) != STATUS_CONST(SUCCESS))
    return __LINE__;
  if (TEXT_GRID_ATTRIBUTE_FUNC(Resize)(
          &grid, (TDUAL_TYPE(uint16)){.col = 3U, .row = 3U}) != STATUS_CONST(SUCCESS))
    return __LINE__;
  if (TEXT_GRID_ATTRIBUTE_FUNC(ClearWith)(&grid, cell) != STATUS_CONST(SUCCESS))
    return __LINE__;
  if (TEXT_GRID_ATTRIBUTE_FUNC(Clear)(&grid) != STATUS_CONST(SUCCESS))
    return __LINE__;
  if (TEXT_GRID_ATTRIBUTE_FUNC(Recreate)(
          &grid, (TDUAL_TYPE(uint16)){.col = 1U, .row = 1U}) != STATUS_CONST(SUCCESS))
    return __LINE__;
  if (TEXT_GRID_ATTRIBUTE_FUNC(Destroy)(&region) != STATUS_CONST(SUCCESS) ||
      TEXT_GRID_ATTRIBUTE_FUNC(Destroy)(&clone) != STATUS_CONST(SUCCESS) ||
      TEXT_GRID_ATTRIBUTE_FUNC(Destroy)(&grid) != STATUS_CONST(SUCCESS))
    return __LINE__;
  return 0;
}

int main(void) {
  if (Text_UTF8_EncodedLength((TEXT_TYPE(TChar32))'A') != 1U ||
      Text_UTF8_EncodedLength((TEXT_TYPE(TChar32))UINT32_C(0x1F600)) != 4U)
    return 1;
  if (Text_UTF16_EncodedLength((TEXT_TYPE(TChar32))'A') != 1U ||
      Text_UTF16_EncodedLength((TEXT_TYPE(TChar32))UINT32_C(0x1F600)) != 2U)
    return 2;
  if (!Text_Codepoint_IsControl((TEXT_TYPE(TChar32))'\n'))
    return 3;
  if (test_attributes() != 0)
    return 4;

  TEST_CHAR_GRID(char, char, 'A', 'B');
  TEST_CHAR_GRID(TEXT_TYPE(TChar16), char16, 65U, 66U);
  TEST_CHAR_GRID(TEXT_TYPE(TChar32), char32, 65U, 66U);

  TEST_COMPOSITE_GRID(char, char, 'A', 'B');
  TEST_COMPOSITE_GRID(TEXT_TYPE(TChar16), char16, 65U, 66U);
  TEST_COMPOSITE_GRID(TEXT_TYPE(TChar32), char32, 65U, 66U);

  return 0;
}
