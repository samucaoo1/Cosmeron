#include "../../Cosmeron/Modules/Text/Text.h"

#include <assert.h>
#include <limits.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <wchar.h>

typedef struct TestPoint {
  int x;
  int y;
} TestPoint;

static OPSTATUS TestFormatPointAdapter(MODULE_TYPE(Text, Writer) *writer,
                                       const void *value,
                                       MODULE_TYPE(Text, View8) spec) {
  const TestPoint *point = (const TestPoint *)value;
  if (point == NULL) {
    return MODULE_FUNC(Text, Writer, WriteCStr)(writer, "(null)");
  }
  if (MODULE_FUNC(Text, View8, EqualCStr)(spec, "pair")) {
    return MODULE_FUNC(Text, Format, To)(writer, "%d:%d", point->x, point->y);
  }
  return MODULE_FUNC(Text, Format, To)(writer, "(%d, %d)", point->x, point->y);
}

typedef struct TestSinkBuf {
  char data[128];
  size_t len;
  bool failNext;
} TestSinkBuf;

static OPSTATUS TestDelegateSink(const char *data, size_t length,
                                 void *context) {
  TestSinkBuf *buf = (TestSinkBuf *)context;
  if (buf->failNext) {
    return STATUS_CONST(GENERIC_ERROR);
  }
  if (buf->len + length >= sizeof(buf->data)) {
    return STATUS_CONST(INSUFFICIENT_SPACE);
  }
  memcpy(buf->data + buf->len, data, length);
  buf->len += length;
  buf->data[buf->len] = '\0';
  return STATUS_CONST(SUCCESS);
}

static void TestFormatBufferVariadicHelper(char *buf, size_t cap,
                                           size_t *outWritten, const char *fmt,
                                           ...) {
  va_list args;
  va_start(args, fmt);
  assert(MODULE_FUNC(Text, Format, BufferVariadic)(
             buf, cap, outWritten, fmt, args) == STATUS_CONST(SUCCESS));
  va_end(args);
}

static void TestUnicode(void) {
  /* Codepoint */
  assert(MODULE_FUNC(Text, Codepoint, IsValid)(0x1F600U));
  assert(MODULE_FUNC(Text, Codepoint, IsValid)(0xD800U));
  assert(!MODULE_FUNC(Text, Codepoint, IsValid)(0x110000U));
  assert(!MODULE_FUNC(Text, Codepoint, IsScalar)(0xD800U));
  assert(MODULE_FUNC(Text, Codepoint, IsScalar)(0x1F600U));
  assert(MODULE_FUNC(Text, Codepoint, IsAscii)('A'));
  assert(!MODULE_FUNC(Text, Codepoint, IsAscii)(0x80U));
  assert(MODULE_FUNC(Text, Codepoint, IsControl)('\n'));
  assert(MODULE_FUNC(Text, Codepoint, IsWhitespace)(' '));
  assert(MODULE_FUNC(Text, Codepoint, IsWhitespace)(0x00A0U));
  assert(MODULE_FUNC(Text, Codepoint, IsWhitespace)(0x3000U));

  /* UTF-8 */
  unsigned char u8[8] = {0};
  size_t units = 0U;
  assert(MODULE_FUNC(Text, UTF8, EncodedLength)(0x1F642U, &units) ==
         STATUS_CONST(SUCCESS));
  assert(units == 4U);
  assert(MODULE_FUNC(Text, UTF8, Encode)(0x1F642U, u8, sizeof(u8), &units) ==
         STATUS_CONST(SUCCESS));
  assert(units == 4U);
  assert(MODULE_FUNC(Text, UTF8, Validate)(u8, units));

  MODULE_TYPE(Text, Char32) decoded = 0U;
  size_t consumed = 0U;
  assert(MODULE_FUNC(Text, UTF8, Decode)(u8, units, &decoded, &consumed) ==
         STATUS_CONST(SUCCESS));
  assert(decoded == 0x1F642U && consumed == 4U);

  /* Overlong & incomplete UTF-8 */
  const unsigned char overlong[2] = {0xC0U, 0xAFU};
  assert(MODULE_FUNC(Text, UTF8, Decode)(overlong, 2U, &decoded, &consumed) ==
         STATUS_CONST(INVALID_SEQUENCE));
  assert(MODULE_FUNC(Text, UTF8, Decode)(u8, 2U, &decoded, &consumed) ==
         STATUS_CONST(INCOMPLETE_SEQUENCE));

  const unsigned char mixed[] = {'A', 0xC3U, 0xA9U, 0xF0U, 0x9FU, 0x99U, 0x82U};
  size_t cpCount = 0U;
  assert(MODULE_FUNC(Text, UTF8, Count)(mixed, sizeof(mixed), &cpCount,
                                        &units) == STATUS_CONST(SUCCESS));
  assert(cpCount == 3U && units == sizeof(mixed));

  /* UTF-16 + transcoding */
  MODULE_TYPE(Text, Char16) u16[8] = {0};
  assert(MODULE_FUNC(Text, UTF16, EncodedLength)(0x1F642U, &units) ==
         STATUS_CONST(SUCCESS));
  assert(units == 2U);
  assert(MODULE_FUNC(Text, UTF16, Encode)(0x1F642U, u16, 8U, &units) ==
         STATUS_CONST(SUCCESS));
  assert(units == 2U);
  assert(MODULE_FUNC(Text, UTF16, Validate)(u16, 2U));
  assert(MODULE_FUNC(Text, UTF16, Decode)(u16, 2U, &decoded, &consumed) ==
         STATUS_CONST(SUCCESS));
  assert(decoded == 0x1F642U && consumed == 2U);

  const MODULE_TYPE(Text, Char16) loneHigh = 0xD800U;
  assert(!MODULE_FUNC(Text, UTF16, Validate)(&loneHigh, 1U));
  assert(MODULE_FUNC(Text, UTF16, Decode)(&loneHigh, 1U, &decoded,
                                          &consumed) ==
         STATUS_CONST(INCOMPLETE_SEQUENCE));

  size_t neededU16 = 0U;
  assert(MODULE_FUNC(Text, UTF16, FromUTF8)(mixed, sizeof(mixed), NULL, 0U,
                                            &neededU16) ==
         STATUS_CONST(SUCCESS));
  assert(neededU16 == 4U);
  assert(MODULE_FUNC(Text, UTF16, FromUTF8)(mixed, sizeof(mixed), u16, 8U,
                                            &units) == STATUS_CONST(SUCCESS));
  assert(units == 4U);
  assert(MODULE_FUNC(Text, UTF16, Count)(u16, units, &cpCount, &consumed) ==
         STATUS_CONST(SUCCESS));
  assert(cpCount == 3U && consumed == 4U);

  unsigned char roundTrip[16] = {0};
  size_t rtBytes = 0U;
  assert(MODULE_FUNC(Text, UTF16, ToUTF8)(u16, units, NULL, 0U, &rtBytes) ==
         STATUS_CONST(SUCCESS));
  assert(rtBytes == sizeof(mixed));
  assert(MODULE_FUNC(Text, UTF16, ToUTF8)(u16, units, roundTrip,
                                          sizeof(roundTrip), &rtBytes) ==
         STATUS_CONST(SUCCESS));
  assert(rtBytes == sizeof(mixed) && memcmp(roundTrip, mixed, rtBytes) == 0);
  assert(MODULE_FUNC(Text, UTF16, FromUTF8)(mixed, sizeof(mixed), u16, 2U,
                                            &units) ==
         STATUS_CONST(INSUFFICIENT_SPACE));

  /* Width */
  int w = 0;
  assert(MODULE_FUNC(Text, Width, Codepoint)('A', &w) == STATUS_CONST(SUCCESS));
  assert(w == 1);
  assert(MODULE_FUNC(Text, Width, Codepoint)(0x0301U, &w) ==
         STATUS_CONST(SUCCESS));
  assert(w == 0);
  assert(MODULE_FUNC(Text, Width, Codepoint)(0x1F600U, &w) ==
         STATUS_CONST(SUCCESS));
  assert(w == 2);
  assert(MODULE_FUNC(Text, Width, Codepoint)('\n', &w) ==
         STATUS_CONST(INVALID_SEQUENCE));

  size_t cols = 0U;
  assert(MODULE_FUNC(Text, Width, UTF8)(mixed, sizeof(mixed), &cols, &units) ==
         STATUS_CONST(SUCCESS));
  assert(cols == 4U && units == sizeof(mixed));
}

static void TestCString(void) {
  assert(MODULE_FUNC(Text, CString, Length)("Cosmeron") == 8U);
  assert(MODULE_FUNC(Text, CString, LengthBounded)("Cosmeron", 3U) == 3U);
  assert(MODULE_FUNC(Text, CString, IsEmpty)(""));
  assert(MODULE_FUNC(Text, CString, IsAscii)("Cosmeron"));
  assert(!MODULE_FUNC(Text, CString, IsAscii)("\xC3\xA9"));

  char buf[32] = {0};
  size_t len = 0U;
  assert(MODULE_FUNC(Text, CString, Copy)(buf, sizeof(buf), "Cos", &len) ==
         STATUS_CONST(SUCCESS));
  assert(len == 3U);
  assert(MODULE_FUNC(Text, CString, Concat)(buf, sizeof(buf), "meron", &len) ==
         STATUS_CONST(SUCCESS));
  assert(len == 8U && strcmp(buf, "Cosmeron") == 0);
  assert(MODULE_FUNC(Text, CString, CopyBounded)(buf, 4U, "Cosmeron", 3U,
                                                 &len) ==
         STATUS_CONST(SUCCESS));
  assert(len == 3U && strcmp(buf, "Cos") == 0);

  assert(MODULE_FUNC(Text, CString, CompareIgnoreCase)("abc", "ABC") ==
         COMPARISON_CONST(EQUAL));
  assert(MODULE_FUNC(Text, CString, EqualIgnoreCase)("Cosmeron", "COSMERON"));
  assert(MODULE_FUNC(Text, CString, StartsWith)("Cosmeron", "Cos"));
  assert(MODULE_FUNC(Text, CString, EndsWithIgnoreCase)("Cosmeron", "ERON"));

  size_t idx = 0U;
  assert(MODULE_FUNC(Text, CString, FindChar)("banana", 'a', &idx) ==
         STATUS_CONST(SUCCESS) && idx == 1U);
  assert(MODULE_FUNC(Text, CString, FindLastChar)("banana", 'a', &idx) ==
         STATUS_CONST(SUCCESS) && idx == 5U);
  assert(MODULE_FUNC(Text, CString, FindCStr)("Cosmeron C11", "C11", &idx) ==
         STATUS_CONST(SUCCESS) && idx == 9U);

  size_t span = 0U;
  assert(MODULE_FUNC(Text, CString, Span)("123abc", "0123456789", &span) ==
         STATUS_CONST(SUCCESS) && span == 3U);
  assert(MODULE_FUNC(Text, CString, ComplementSpan)("abc:123", ":", &span) ==
         STATUS_CONST(SUCCESS) && span == 3U);

  assert(MODULE_FUNC(Text, CString, Trim)("  hi \t", NULL, buf, sizeof(buf),
                                          &len) == STATUS_CONST(SUCCESS));
  assert(len == 2U && strcmp(buf, "hi") == 0);
  assert(MODULE_FUNC(Text, CString, TrimLeft)("..hi..", ".", buf, sizeof(buf),
                                              &len) == STATUS_CONST(SUCCESS));
  assert(strcmp(buf, "hi..") == 0);
  assert(MODULE_FUNC(Text, CString, TrimRight)("..hi..", ".", buf, sizeof(buf),
                                               &len) == STATUS_CONST(SUCCESS));
  assert(strcmp(buf, "..hi") == 0);

  size_t count = 0U;
  assert(MODULE_FUNC(Text, CString, CountChar)("banana", 'a', &count) ==
         STATUS_CONST(SUCCESS) && count == 3U);
  assert(MODULE_FUNC(Text, CString, ReplaceChar)(buf, '.', '_', &count) ==
         STATUS_CONST(SUCCESS) && count == 2U && strcmp(buf, "__hi") == 0);

  char *dup = NULL;
  assert(MODULE_FUNC(Text, CString, DuplicateBounded)("Cosmeron", 3U, &dup) ==
         STATUS_CONST(SUCCESS));
  assert(strcmp(dup, "Cos") == 0);
  MODULE_FUNC(Text, CString, Free)(dup);
}

static void TestViewAndString(void) {
  MODULE_TYPE(Text, View8) view =
      MODULE_FUNC(Text, View8, FromCStr)("  \xC2\xA0Hello\xE3\x80\x80 ");
  assert(MODULE_FUNC(Text, View8, Validate)(view));
  MODULE_TYPE(Text, View8) trimmed = MODULE_FUNC(Text, View8, Trim)(view);
  assert(MODULE_FUNC(Text, View8, EqualCStr)(trimmed, "Hello"));

  MODULE_TYPE(Text, View8) utfView =
      MODULE_FUNC(Text, View8, FromCStr)("A\xC3\xA9\xF0\x9F\x99\x82Z");
  size_t cps = 0U;
  assert(MODULE_FUNC(Text, View8, CountCodepoints)(utfView, &cps) ==
         STATUS_CONST(SUCCESS) && cps == 4U);
  MODULE_TYPE(Text, View8) sliced = MODULE_FUNC(Text, View8, Empty)();
  assert(MODULE_FUNC(Text, View8, SliceCodepoints)(utfView, 1U, 2U, &sliced) ==
         STATUS_CONST(SUCCESS));
  assert(MODULE_FUNC(Text, View8, EqualCStr)(sliced, "\xC3\xA9\xF0\x9F\x99\x82"));

  /* Uninitialized String8 + FromView must initialize properly */
  MODULE_TYPE(Text, String8) str8;
  assert(MODULE_FUNC(Text, String8, FromView)(
             &str8,
             MODULE_FUNC(Text, View8, FromCStr)(" \xC2\xA0" "Cosmeron\t")) ==
         STATUS_CONST(SUCCESS));
  assert(MODULE_FUNC(Text, String8, Trim)(&str8) == STATUS_CONST(SUCCESS));
  assert(MODULE_FUNC(Text, String8, EqualCStr)(&str8, "Cosmeron"));
  assert(MODULE_FUNC(Text, String8, AppendCodepoint)(&str8, 0x1F642U) ==
         STATUS_CONST(SUCCESS));
  assert(MODULE_FUNC(Text, String8, Validate)(&str8));
  assert(MODULE_FUNC(Text, String8, CountCodepoints)(&str8, &cps) ==
         STATUS_CONST(SUCCESS) && cps == 9U);

  size_t foundIdx = 0U;
  assert(MODULE_FUNC(Text, String8, FindCStr)(&str8, "meron", &foundIdx) ==
         STATUS_CONST(SUCCESS) && foundIdx == 3U);

  /* Format & AppendFormat directly on String8 (triggers capacity growth) */
  assert(MODULE_FUNC(Text, String8, Format)(
             &str8, "val=%04d, view=%v, ok=%t", 42,
             MODULE_FUNC(Text, View8, FromCStr)("abc"), true) ==
         STATUS_CONST(SUCCESS));
  assert(strcmp(MODULE_FUNC(Text, String8, CStr)(&str8),
                "val=0042, view=abc, ok=true") == 0);
  assert(MODULE_FUNC(Text, String8, AppendFormat)(
             &str8, " | long=%040d", 7) == STATUS_CONST(SUCCESS));
  assert(MODULE_FUNC(Text, String8, Length)(&str8) ==
         strlen("val=0042, view=abc, ok=true | long=") + 40U);

#if !defined(TEXT_DISABLE_FUNCTION_TABLE) &&                                   \
    !defined(MODULE_DISABLE_FUNCTION_TABLE)
  assert(OBJECT_CALL(&str8, Clear) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL(&str8, AppendCStr, "FT") == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL_0(&str8, Length) == 2U);
#endif

  assert(MODULE_FUNC(Text, String8, Destroy)(&str8) == STATUS_CONST(SUCCESS));

  /* String16 & String32 */
  const MODULE_TYPE(Text, Char16) c16[] = {'H', 'i', 0};
  MODULE_TYPE(Text, String16) str16 = {0};
  assert(MODULE_FUNC(Text, String16, FromCStr)(&str16, c16) ==
         STATUS_CONST(SUCCESS));
  assert(MODULE_FUNC(Text, String16, PushBack)(&str16, '!') ==
         STATUS_CONST(SUCCESS));
  assert(MODULE_FUNC(Text, String16, Length)(&str16) == 3U);
  assert(MODULE_FUNC(Text, String16, Destroy)(&str16) == STATUS_CONST(SUCCESS));

  const MODULE_TYPE(Text, Char32) c32[] = {'O', 'K', 0};
  MODULE_TYPE(Text, String32) str32 = {0};
  assert(MODULE_FUNC(Text, String32, FromCStr)(&str32, c32) ==
         STATUS_CONST(SUCCESS));
  assert(MODULE_FUNC(Text, String32, Length)(&str32) == 2U);
  assert(MODULE_FUNC(Text, String32, Destroy)(&str32) == STATUS_CONST(SUCCESS));
}

static void TestWriterAndFormat(void) {
  char buf[128] = {0};
  MODULE_TYPE(Text, Writer) w =
      MODULE_FUNC(Text, Writer, ToBuffer)(buf, sizeof(buf));
  assert(MODULE_FUNC(Text, Writer, WriteCStr)(&w, "n=") ==
         STATUS_CONST(SUCCESS));
  assert(MODULE_FUNC(Text, Writer, WriteInt)(&w, INTMAX_MIN, 10U, false) ==
         STATUS_CONST(SUCCESS));
  assert(MODULE_FUNC(Text, Writer, WriteCStr)(&w, " hex=") ==
         STATUS_CONST(SUCCESS));
  const unsigned char raw[3] = {0x0A, 0xFF, 0x10};
  assert(MODULE_FUNC(Text, Writer, WriteHex)(&w, raw, sizeof(raw), true) ==
         STATUS_CONST(SUCCESS));
  assert(MODULE_FUNC(Text, Writer, WriteCStr)(&w, " esc=") ==
         STATUS_CONST(SUCCESS));
  assert(MODULE_FUNC(Text, Writer, WriteEscaped)(&w, "a\nb\"c\x01", 6U,
                                                 true) ==
         STATUS_CONST(SUCCESS));
  assert(MODULE_FUNC(Text, Writer, Finish)(&w) == STATUS_CONST(SUCCESS));
  assert(strstr(buf, " hex=0AFF10 esc=\"a\\nb\\\"c\\x01\"") != NULL);

  /* Truncation */
  char small[8] = {0};
  MODULE_TYPE(Text, Writer) sw =
      MODULE_FUNC(Text, Writer, ToBuffer)(small, sizeof(small));
  assert(MODULE_FUNC(Text, Writer, WriteCStr)(&sw, "Cosmeron!") ==
         STATUS_CONST(SUCCESS));
  assert(MODULE_FUNC(Text, Writer, IsTruncated)(&sw));
  assert(MODULE_FUNC(Text, Writer, Written)(&sw) == 9U);
  assert(MODULE_FUNC(Text, Writer, Finish)(&sw) ==
         STATUS_CONST(INSUFFICIENT_SPACE));
  assert(strcmp(small, "Cosmero") == 0);

  /* Delegate sink */
  TestSinkBuf sinkBuf = {{0}, 0U, false};
  MODULE_TYPE(Text, Writer) cw = MODULE_FUNC(Text, Writer, ToDelegate)(
      NS_JOIN(MODULE_DELEGATE_FUNC(Text, WriterSink), Bind)(TestDelegateSink,
                                                            &sinkBuf));
  assert(MODULE_FUNC(Text, Format, To)(&cw, "%s:%U:%t", "hi",
                                       (MODULE_TYPE(Text, Char32))0x1F642U,
                                       false) == STATUS_CONST(SUCCESS));
  assert(MODULE_FUNC(Text, Writer, Finish)(&cw) == STATUS_CONST(SUCCESS));
  assert(strcmp(sinkBuf.data, "hi:\xF0\x9F\x99\x82:false") == 0);

  sinkBuf.failNext = true;
  assert(MODULE_FUNC(Text, Writer, WriteChar)(&cw, '!') ==
         STATUS_CONST(GENERIC_ERROR));
  assert(MODULE_FUNC(Text, Writer, Status)(&cw) == STATUS_CONST(GENERIC_ERROR));

  /* Format measurement + variadic */
  size_t needed = 0U;
  assert(MODULE_FUNC(Text, Format, Buffer)(
             NULL, 0U, &needed, "%+06d|%#x|%#b|%q|%#q", 42, 0x2A, 5U, "a\tb",
             "x\ny") == STATUS_CONST(SUCCESS));
  assert(needed > 0U);
  TestFormatBufferVariadicHelper(buf, sizeof(buf), &needed,
                                 "%*d|%w16d|%w64u|%td|%t", -5, 12,
                                 (int)(int16_t)-7, (uint64_t)999ULL,
                                 (ptrdiff_t)-42, true);
  assert(strcmp(buf, "12   |-7|999|-42|true") == 0);

  /* Wide string and large-width float */
  const wchar_t wstr[] = {L'A', (wchar_t)0x00E9, L'\0'};
  assert(MODULE_FUNC(Text, Format, Buffer)(buf, sizeof(buf), &needed,
                                           "%ls|%08.2f", wstr, 3.5) ==
         STATUS_CONST(SUCCESS));
  assert(strcmp(buf, "A\xC3\xA9|00003.50") == 0);

  /* Custom adapter %! and %(spec)! (Mode A) */
  TestPoint pt = {.x = 3, .y = 4};
  assert(MODULE_FUNC(Text, Format, Buffer)(
             buf, sizeof(buf), &needed, "p=%! s=%10(pair)!",
             TestFormatPointAdapter, &pt, TestFormatPointAdapter, &pt) ==
         STATUS_CONST(SUCCESS));
  assert(strcmp(buf, "p=(3, 4) s=       3:4") == 0);

  /* Trailing '%' and unclosed '%(' must fail safely */
  assert(MODULE_FUNC(Text, Format, Buffer)(buf, sizeof(buf), &needed, "bad%") ==
         STATUS_CONST(INVALID_ARGUMENT));
  assert(MODULE_FUNC(Text, Format, Buffer)(buf, sizeof(buf), &needed, "%(x") ==
         STATUS_CONST(INVALID_ARGUMENT));
}

static void TestBytesHuman(void) {
  char buf[64] = {0};
  size_t written = 0U;
  assert(MODULE_FUNC(Text, BytesHuman, Format)(buf, sizeof(buf), &written,
                                               1536ULL) ==
         STATUS_CONST(SUCCESS));
  assert(strcmp(buf, "1.50 KiB") == 0);

  assert(MODULE_FUNC(Text, BytesHuman, FormatEx)(
             buf, sizeof(buf), &written, 1500ULL,
             MODULE_NS(Text, BytesHuman, Unit, SI), 1U) ==
         STATUS_CONST(SUCCESS));
  assert(strcmp(buf, "1.5 KB") == 0);

  MODULE_TYPE(Text, Writer) w =
      MODULE_FUNC(Text, Writer, ToBuffer)(buf, sizeof(buf));
  assert(MODULE_FUNC(Text, BytesHuman, Write)(&w, 512ULL) ==
         STATUS_CONST(SUCCESS));
  assert(MODULE_FUNC(Text, Writer, Finish)(&w) == STATUS_CONST(SUCCESS));
  assert(strcmp(buf, "512 B") == 0);

  uint64_t bytes = 0U;
  assert(MODULE_FUNC(Text, BytesHuman, Parse)("1.500000000 TiB", &bytes) ==
         STATUS_CONST(SUCCESS));
  assert(bytes == 1649267441664ULL);
  assert(MODULE_FUNC(Text, BytesHuman, Parse)("1.500000000 EiB", &bytes) ==
         STATUS_CONST(SUCCESS));
  assert(bytes == 1729382256910270464ULL);
  assert(MODULE_FUNC(Text, BytesHuman, Parse)("20 EiB", &bytes) ==
         STATUS_CONST(ARITHMETIC_OVERFLOW));
  assert(MODULE_FUNC(Text, BytesHuman, Parse)("1. KiB", &bytes) ==
         STATUS_CONST(INVALID_ARGUMENT));
}

static void TestGrid(void) {
  const MODULE_TYPED(Tuple, Dual, UInt16) size = {.col = 4U, .row = 3U};
  const MODULE_TYPED(Tuple, Dual, UInt16) pos = {.col = 1U, .row = 2U};
  MODULE_TYPED(Text, Grid, Char8) grid = {0};
  assert(MODULE_FUNC(Text, Grid, Char8, Create)(&grid, size) ==
         STATUS_CONST(SUCCESS));

  MODULE_TYPE(Text, Attribute, Cell) attr =
      MODULE_FUNC(Text, Attribute, Default)();
  attr.foreground = MODULE_FUNC(Text, Attribute, Color, RGB)(10U, 20U, 30U);
  attr.flags = MODULE_NS(Text, Attribute, BOLD);
  attr.underline = MODULE_NS(Text, Attribute, Underline, SINGLE);
  assert(MODULE_FUNC(Text, Attribute, Color, Red)(attr.foreground) == 10U);
  assert(MODULE_FUNC(Text, Attribute, Color, Green)(attr.foreground) == 20U);
  assert(MODULE_FUNC(Text, Attribute, Color, Blue)(attr.foreground) == 30U);
  assert(MODULE_FUNC(Text, Attribute, Color, WindowsPalette)() != NULL);

  assert(MODULE_FUNC(Text, Grid, Char8, WriteCell)(&grid, pos, 'X', attr) ==
         STATUS_CONST(SUCCESS));
  MODULE_TYPE(Text, Char8) outCh = 0U;
  MODULE_TYPE(Text, Attribute, Cell) outAttr = {0};
  assert(MODULE_FUNC(Text, Grid, Char8, ReadCell)(&grid, pos, &outCh,
                                                  &outAttr) ==
         STATUS_CONST(SUCCESS));
  assert(outCh == 'X' && outAttr.foreground == attr.foreground &&
         outAttr.underline == MODULE_NS(Text, Attribute, Underline, SINGLE));

  /* Blit test */
  MODULE_TYPED(Text, Grid, Char8) stamp = {0};
  const MODULE_TYPED(Tuple, Dual, UInt16) stampSize = {.col = 2U, .row = 1U};
  assert(MODULE_FUNC(Text, Grid, Char8, Create)(&stamp, stampSize) ==
         STATUS_CONST(SUCCESS));
  assert(MODULE_FUNC(Text, Grid, Char8, WriteCell)(
             &stamp, (MODULE_TYPED(Tuple, Dual, UInt16)){.col = 0U, .row = 0U},
             'Y', attr) == STATUS_CONST(SUCCESS));
  assert(MODULE_FUNC(Text, Grid, Char8, Blit)(
             &grid, (MODULE_TYPED(Tuple, Dual, UInt16)){.col = 0U, .row = 2U},
             &stamp, 0U) == STATUS_CONST(SUCCESS));
  assert(MODULE_FUNC(Text, Grid, Char8, ReadCell)(
             &grid, (MODULE_TYPED(Tuple, Dual, UInt16)){.col = 0U, .row = 2U},
             &outCh, &outAttr) == STATUS_CONST(SUCCESS) &&
         outCh == 'Y');

  assert(MODULE_FUNC(Text, Grid, Char8, Destroy)(&stamp) ==
         STATUS_CONST(SUCCESS));
  assert(MODULE_FUNC(Text, Grid, Char8, Destroy)(&grid) ==
         STATUS_CONST(SUCCESS));
}

int main(void) {
  TestUnicode();
  TestCString();
  TestViewAndString();
  TestWriterAndFormat();
  TestBytesHuman();
  TestGrid();

  printf("Text tests passed!\n");
  return 0;
}
