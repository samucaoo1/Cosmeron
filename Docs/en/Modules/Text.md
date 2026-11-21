# Text

The **Text** module provides Unicode classification and transcoding, C-string utilities, non-owning views, mutable owned strings, decoupled streaming formatting, and memory-backed character/attribute grids. It is a C11 API organized into packages, pure POD structures, and objects.

## Headers

~~~c
#include "Cosmeron/Modules/Text/Text.h"
~~~

The aggregate includes the following public areas:

| Header | Facility |
| --- | --- |
| `Unicode/Codepoint.h` | Unicode code point classification. |
| `Unicode/UTF8.h`, `Unicode/UTF16.h` | UTF-8 and UTF-16 encoding, decoding, validation, counting, and transcoding. |
| `Unicode/Width.h` | Approximate display-column width. |
| `CString/CString.h` | Stateless NUL-terminated C-string operations. |
| `View/View.h` | Non-owning typed string views (`View8`, `View16`, `View32`). |
| `String/String.h` | Owned mutable strings (`String8`, `String16`, `String32`) and UTF-8 helpers. |
| `Format/Writer.h` | Decoupled text sink (`Counter`, `ToBuffer`, `ToClosure`, `ToString`) and typed writers. |
| `Format/Format.h` | Safe bounded and writer-based formatting with custom `%!` adapters. |
| `Format/BytesHuman.h` | Human-readable byte size formatting and parsing. |
| `Grid/Attribute.h`, `Grid/Char.h`, `Grid/Grid.h` | Attribute planes, color helpers, and memory-backed text grids. |

---

# Overview

Text separates low-level Unicode and C-string primitives from non-owning views, owned mutable strings, decoupled output formatting (`Writer` + `Format` + `BytesHuman`), and off-screen character/attribute grids. None of these facilities perform terminal I/O or depend on external modules such as `Tuple` or `Numeric` for formatting.

---

# API reference

## Unicode and encodings

### Codepoint

Header: `Unicode/Codepoint.h`. Package: `Text_Codepoint`.

The package exposes predicates under the `Is` family (`VARIANT`):

| Function | Meaning |
| --- | --- |
| `IsValid(codepoint)` | Checks whether the value is within U+0000..U+10FFFF. |
| `IsScalar(codepoint)` | Checks whether the value is a Unicode scalar (not a surrogate). |
| `IsAscii(codepoint)` | Checks whether the value is in U+0000..U+007F. |
| `IsControl(codepoint)` | Checks C0 and C1 control ranges. |
| `IsWhitespace(codepoint)` | Checks the Unicode whitespace ranges implemented by the module. |

~~~c
bool scalar = MODULE_FUNC(Text, Codepoint, IsScalar)(codepoint);
~~~

Use `IsScalar` before encoding. `IsValid` alone permits surrogate code points, which are not Unicode scalar values.

### UTF8

Header: `Unicode/UTF8.h`. Input and output buffers use `unsigned char` bytes.

| Function | Contract |
| --- | --- |
| `EncodedLength(codepoint, outUnits)` | Reports the number of UTF-8 bytes required. |
| `Encode(codepoint, output, capacity, outUnits)` | Encodes one scalar into a caller-provided byte buffer. |
| `Decode(input, size, outCodepoint, outUnits)` | Decodes one sequence and reports consumed bytes. |
| `Validate(input, size)` | Returns whether the complete byte span is valid UTF-8. |
| `Count(input, size, outCodepoints, outUnits)` | Counts code points and reports consumed units. |

~~~c
unsigned char encoded[4];
size_t written = 0U;

OPSTATUS status = MODULE_FUNC(Text, UTF8, Encode)(
    (MODULE_TYPE(Text, Char32))0x1F642,
    encoded, sizeof(encoded), &written);
~~~

### UTF16

Header: `Unicode/UTF16.h`. Input and output use `MODULE_TYPE(Text, Char16)` code units.

| Function | Contract |
| --- | --- |
| `EncodedLength(codepoint, outUnits)` | Reports one or two UTF-16 code units. |
| `Encode(codepoint, output, capacity, outUnits)` | Encodes one scalar into a caller-provided UTF-16 buffer. |
| `Decode(input, size, outCodepoint, outUnits)` | Decodes one code point and reports consumed code units. |
| `Validate(input, size)` | Validates a complete UTF-16 code-unit span. |
| `Count(input, size, outCodepoints, outUnits)` | Counts code points and reports consumed code units. |
| `FromUTF8(input, size, output, capacity, outUnits)` | Transcodes UTF-8 bytes to UTF-16 code units (pass `NULL, 0U` to measure). |
| `ToUTF8(input, size, output, capacity, outBytes)` | Transcodes UTF-16 code units to UTF-8 bytes (pass `NULL, 0U` to measure). |

### Display width

Header: `Unicode/Width.h`. Package: `Text_Width`.

| Function | Contract |
| --- | --- |
| `Codepoint(codepoint, outWidth)` | Reports the display width (`0`, `1`, or `2`) of one code point. |
| `UTF8(input, size, outColumns, outUnits)` | Measures the display columns of a UTF-8 span and reports consumed bytes. |

---

## C-string utilities, Views, and Strings

### CString

Header: `CString/CString.h`. Package: `Text_CString`.

| Group | Methods |
| --- | --- |
| Length | `Length`, `LengthBounded` |
| Classification | `IsEmpty`, `IsAscii`, `IsAsciiBounded` |
| Copy and concat | `Copy`, `CopyBounded`, `Concat`, `ConcatBounded` |
| Comparison | `Compare`, `CompareBounded`, `CompareIgnoreCase`, `CompareIgnoreCaseBounded` |
| Equality | `Equal`, `EqualBounded`, `EqualIgnoreCase`, `EqualIgnoreCaseBounded` |
| Prefix/suffix | `StartsWith`, `StartsWithBounded`, `StartsWithIgnoreCase`, `EndsWith`, `EndsWithBounded`, `EndsWithIgnoreCase` |
| Search | `FindChar`, `FindCharBounded`, `FindLastChar`, `FindLastCharBounded`, `FindCStr`, `FindCStrBounded` |
| Span | `Span`, `ComplementSpan` |
| Trim | `Trim`, `TrimLeft`, `TrimRight` |
| Count/replace | `CountChar`, `CountCharBounded`, `ReplaceChar`, `ReplaceCharBounded` |
| Allocation | `Duplicate`, `DuplicateBounded`, `Free` |

Prefer bounded operations whenever the available extent is known and the input may lack a terminator inside that extent.

### View

Header: `View/View.h`.

The generated non-owning types are `MODULE_TYPE(Text, View8)`, `View16`, and `View32`. Lengths and indexes are code units unless a UTF-8 code-point helper (`View8_*Codepoints`, `View8_Validate`, `View8_Trim*`) is used.

| Function | Description |
| --- | --- |
| `Create(data, length)`, `FromCStr(cstr)`, `Empty()` | Construct views without allocating. |
| `Length`, `IsEmpty`, `Data`, `At(index, outValue)` | Inspect view contents. |
| `Slice(start, count)`, `Prefix(count)`, `Suffix(count)` | Return subviews. |
| `Compare(left, right)`, `Equal`, `EqualCStr` | Compare views. |
| `Find`, `FindView`, `FindCStr`, `StartsWith*`, `EndsWith*` | Search and test prefixes/suffixes. |
| `View8_Validate`, `View8_CountCodepoints`, `View8_NextCodepoint`, `View8_Trim*`, `View8_SliceCodepoints` | UTF-8 code-point operations on `View8`. |

### Owned strings

Header: `String/String.h`. Generates `String8` (`char`), `String16` (`Text_Char16`), and `String32` (`Text_Char32`).

| Group | Methods |
| --- | --- |
| Lifecycle | `Create(initialCapacity)`, `Destroy()`, `Clone(src)` |
| Capacity | `Reserve(requiredCapacity)`, `ShrinkToFit()` |
| Construction | `FromBuffer(buffer, count)`, `FromCStr(cstr)`, `FromView(view)` |
| Mutation | `Clear()`, `AppendBuffer(buffer, count)`, `AppendCStr(cstr)`, `AppendString(other)`, `AppendView(view)`, `PushBack(value)`, `PopBack(outChar)`, `SetAt(index, value)`, `Insert(index, buffer, count)`, `InsertChar(index, value)`, `Erase(index, count)` |
| Access | `Length()`, `Capacity()`, `IsEmpty()`, `CStr()`, `At(index, outChar)` |
| Views and slicing | `View()`, `Slice(start, count)`, `Substr(start, count, outSubstr)` |
| Comparison | `Compare(other)`, `Equal(other)`, `EqualCStr(cstr)`, `EqualView(view)`, `StartsWith(prefix)`, `EndsWith(suffix)` |
| Search | `Find(needle, outIndex)`, `FindCStr(needle, outIndex)` |
| UTF-8 (`String8_*`) | `AppendCodepoint`, `CountCodepoints`, `Validate`, `Trim`, `TrimLeft`, `TrimRight`, `Format`, `FormatVariadic`, `AppendFormat`, `AppendFormatVariadic`, `SliceCodepoints` |

`String8` also exposes `MODULE_FUNC(Text, Writer, ToString)(&str)` so `Writer` and `Format` write directly into a dynamically growing `String8` in a single pass.

~~~c
MODULE_TYPE(Text, String8) text = {0};
if (MODULE_FUNC(Text, String8, Create)(&text, 16U) == STATUS_CONST(SUCCESS)) {
    (void)MODULE_FUNC(Text, String8, Format)(&text, "Hello %s!", "Cosmeron");
    (void)MODULE_FUNC(Text, String8, Destroy)(&text);
}
~~~

---

## Formatting

### Writer

Header: `Format/Writer.h`. Type: `MODULE_TYPE(Text, Writer)`. Package: `Text_Writer`.

`Writer` decouples text generation from the output destination:

| Constructor | Destination |
| --- | --- |
| `ToBuffer(buffer, capacity)` | Caller-owned `char` buffer (always NUL-terminated; tracks truncation). |
| `Counter()` | Measurement-only mode (counts written bytes without storing). |
| `ToClosure(sink)` | Any `MODULE_CLOSURE(Text, WriterSink)` target. |
| `ToString(str)` | Declared in `String/String.h`; appends directly into a `String8`. |

Typed write primitives: `WriteBuffer`, `WriteChar`, `WriteCodepoint`, `WriteRepeat`, `WriteCStr`, `WriteView`, `WriteBool`, `WriteInt`, `WriteUInt`, `WriteFloat`, `WriteHex`, `WriteEscaped`, plus inspection (`Written`, `IsTruncated`, `Status`) and `Finish`.

### Format

Header: `Format/Format.h`. Package: `Text_Format`.

| Function | Contract |
| --- | --- |
| `To(writer, format, ...)` / `ToVariadic(writer, format, args)` | Writes formatted output to any `Text_Writer`. |
| `Buffer(buffer, capacity, outWritten, format, ...)` / `BufferVariadic(...)` | Safe bounded formatter to a buffer (`NULL, 0U` measures required bytes). |

Supported specifiers include standard C11/C23 specifiers (`%d`/`%i`, `%u`, `%o`, `%x`/`%X`, `%b`/`%B`, `%f`/`%F`/`%e`/`%E`/`%g`/`%G`/`%a`/`%A`, `%c`/`%lc`, `%s`/`%ls`, `%p`, `%%`, width/precision/`*`, `hh`/`h`/`l`/`ll`/`j`/`z`/`t`/`L`/`w8`/`w16`/`w32`/`w64`) plus native Cosmeron extensions:
- `%v`: `MODULE_TYPE(Text, View8)` by value.
- `%U`: `MODULE_TYPE(Text, Char32)` encoded as UTF-8.
- `%t`: `bool` (`true` / `false`).
- `%q` / `%#q`: escaped C-string (quoted or unquoted).
- `%!` / `%(spec)!`: custom adapter pair `(MODULE_CALLBACK(Text, Format, Adapter) adapter, const void *value)` so external types format cleanly without coupling `Text` to other modules.

### Human-readable byte sizes

Header: `Format/BytesHuman.h`. Package: `Text_BytesHuman`. Enum: `MODULE_ENUM(Text, BytesHuman, Unit)` (`IEC`, `SI`).

| Function | Description |
| --- | --- |
| `Write(writer, bytes)` / `WriteEx(writer, bytes, unitStandard, precision)` | Writes formatted size to a `Text_Writer`. |
| `Format(buffer, capacity, outWritten, bytes)` / `FormatEx(...)` | Bounded formatting to a caller buffer. |
| `Parse(text, outBytes)` | Parses a textual byte-size value into `uint64_t` with overflow protection. |

---

## Character and attribute grids

### Scope and layout

Grid operations work on off-screen memory-backed cells (`MODULE_TYPED(Tuple, Dual, UInt16)` for sizes/positions and `MODULE_TYPED(Tuple, Quad, UInt16)` for regions). They do not write to stdout or drive a terminal backend.

- `Grid/Attribute.h`: exposes `MODULE_TYPE(Text, Attribute, Cell)`, `MODULE_TYPE(Text, Attribute, Grid)`, `MODULE_ENUM(Text, Attribute, Flag)`, `MODULE_ENUM(Text, Attribute, Underline)`, `Attribute_*` grid operations, and `Attribute_Color` (`SPACE(Color, ...)` providing `RGB`, `Red`, `Green`, `Blue`, and `WindowsPalette`).
- `Grid/Char.h`: generates `MODULE_TYPED(Text, Char, Char8)`, `Char16`, and `Char32`.
- `Grid/Grid.h`: generates `MODULE_TYPED(Text, Grid, Char8)`, `Char16`, and `Char32`, combining character and attribute planes with `Blit` / `BlitRegion`.

---

# Contracts and ownership rules

- Buffer-writing encoders, formatters, grid operations, and string operations return `OPSTATUS`; check it before reading output parameters.
- UTF-8 lengths are bytes; UTF-16 lengths are code units; code-point counts are Unicode scalar counts.
- `View` is non-owning; `String` and `Grid` own their backing allocations and must be destroyed.
- Pointers returned by `Data`, `CStr`, and `WindowsPalette` are borrowed.

## Related modules

- [Tuple](Tuple.md) — grid dimensions and regions.
- [Memory](Memory.md) — allocation and byte operations.
- [Algorithm](Algorithm.md) — comparison results used by text views and C strings.
- [Module](Module.md) — packages, objects, closures, and typed specializations.

## Source headers

- `Codespace/Cosmeron/Modules/Text/Text.h`
- `Codespace/Cosmeron/Modules/Text/Unicode/Codepoint.h`
- `Codespace/Cosmeron/Modules/Text/Unicode/UTF8.h`
- `Codespace/Cosmeron/Modules/Text/Unicode/UTF16.h`
- `Codespace/Cosmeron/Modules/Text/Unicode/Width.h`
- `Codespace/Cosmeron/Modules/Text/CString/CString.h`
- `Codespace/Cosmeron/Modules/Text/View/View.def`
- `Codespace/Cosmeron/Modules/Text/String/String.def`
- `Codespace/Cosmeron/Modules/Text/Format/Writer.h`
- `Codespace/Cosmeron/Modules/Text/Format/Format.h`
- `Codespace/Cosmeron/Modules/Text/Format/BytesHuman.h`
- `Codespace/Cosmeron/Modules/Text/Grid/Attribute.h`
- `Codespace/Cosmeron/Modules/Text/Grid/Char.def`
- `Codespace/Cosmeron/Modules/Text/Grid/Grid.def`
