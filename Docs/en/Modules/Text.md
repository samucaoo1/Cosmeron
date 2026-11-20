# Text

The **Text** module provides Unicode classification and encoding, non-owning views, mutable strings, C-string helpers, formatting, attributed text and memory-backed character grids. It is a C11 API organized into packages, typed structures and objects.

## Headers

~~~c
#include "Cosmeron/Modules/Text/Text.h"
~~~

The aggregate includes the following public areas:

| Header | Facility |
| --- | --- |
| `Unicode/Codepoint.h` | Unicode code point classification. |
| `Encoding/UTF8.h`, `Encoding/UTF16.h` | UTF-8 and UTF-16 encoding, decoding and validation. |
| `Unicode/Width.h` | Approximate display-column width. |
| `View/View.h` | Non-owning typed string views. |
| `String/String.h` | Owned mutable strings and UTF-8 string helpers. |
| `String/CString/CString.h` | C-string operations. |
| `String/Builder/Builder.h` | Incremental UTF-8 string construction. |
| `Format/Format.h`, `Format/BytesHuman.h` | Bounded formatting and byte-size text conversion. |
| `Grid/Attribute.h`, `Grid/Char.h`, `Grid/Grid.h` | Attribute planes and memory-backed text grids. |
| `Attributed/Attributed.h` | UTF-8 text with styled spans. |

---

# Unicode and encodings

## Codepoint

Header: `Unicode/Codepoint.h`. Package: `Text_Codepoint`.

The package exposes predicates under `Is`:

| Function | Meaning |
| --- | --- |
| `Valid(codepoint)` | Checks whether the value is within U+0000..U+10FFFF. |
| `Scalar(codepoint)` | Checks whether the value is a Unicode scalar (not a surrogate). |
| `ASCII(codepoint)` | Checks whether the value is in U+0000..U+007F. |
| `Control(codepoint)` | Checks C0 and C1 control ranges. |
| `Whitespace(codepoint)` | Checks the whitespace ranges explicitly implemented by the module. |

~~~c
bool scalar = MODULE_FUNC(Text, Codepoint, Is, Scalar)(codepoint);
~~~

Use `Scalar` before encoding. `Valid` alone permits surrogate code points, which are not Unicode scalar values.

## UTF8

Header: `Encoding/UTF8.h`. Input and output buffers use `unsigned char` bytes.

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

The output buffer capacity is in bytes. Decoding receives the available byte count and writes the decoded code point and consumed byte count through output parameters. Check every status before reading output values.

## UTF16

Header: `Encoding/UTF16.h`. Input and output use `MODULE_TYPE(Text, Char16)` code units.

| Function | Contract |
| --- | --- |
| `EncodedLength(codepoint, outUnits)` | Reports one or two UTF-16 code units. |
| `Encode(codepoint, output, capacity, outUnits)` | Encodes one scalar into a caller-provided UTF-16 buffer. |
| `Decode(input, size, outCodepoint, outUnits)` | Decodes one code point and reports consumed code units. |
| `Validate(input, size)` | Validates a complete UTF-16 code-unit span. |

UTF-16 capacity and consumed-size values are measured in code units, not bytes. A supplementary scalar requires a surrogate pair.

## Display width

Header: `Unicode/Width.h`. Package: `Text_Width`.

| Function | Contract |
| --- | --- |
| `Codepoint(codepoint, outWidth)` | Reports the display width of one code point. |
| `UTF8(input, size, outColumns, outUnits)` | Measures the display columns of a UTF-8 span and reports consumed bytes. |

The width calculation is a display-oriented estimate, not a full shaping or terminal-layout engine. Combining marks and wide characters can affect the number of columns differently from the number of encoded code units.

---

# Views and strings

## View

Header: `View/View.h`.

The generated non-owning types are `MODULE_TYPE(Text, View8)`, `View16` and `View32`. They point to existing storage and carry an explicit length.

| Function | Description |
| --- | --- |
| `Create(data, length)` | Creates a view over a pointer and code-unit count. |
| `FromCStr(cstr)` | Creates a view from a terminated string. |
| `Empty()` | Returns an empty view. |
| `Length`, `IsEmpty`, `Data` | Inspects the view. |
| `At(index, outValue)` | Reads one code unit with bounds checking. |
| `Slice(start, count)`, `Prefix(count)`, `Suffix(count)` | Returns a subview. |
| `Compare(left, right)`, `Equal` | Compares views. |
| `Find` | Searches for a code unit, another view or a C string. |
| `StartsWith`, `EndsWith` | Tests a prefix or suffix. |

A view does not allocate, copy, or append a terminator. It is valid only while its underlying storage remains valid and unmoved. Lengths and indexes are code units unless a specifically code-point-aware operation is used.

## Owned strings

Header: `String/String.h`. The aggregate generates `String8`, `String16` and `String32`. All three use the same generated API; the element type differs:

| Type | Element type | Length unit |
| --- | --- | --- |
| `String8` | `Text_Char8` (`uint8_t`) | Bytes/code units |
| `String16` | `Text_Char16` (`uint16_t`) | 16-bit code units |
| `String32` | `Text_Char32` (`uint32_t`) | 32-bit code units |

The generated object methods are:

| Group | Methods | Behavior |
| --- | --- | --- |
| Lifecycle | `Create(initialCapacity)`, `Destroy()`, `Clone(source)` | Creates, releases or clones owned storage. |
| Capacity | `Reserve(requiredCapacity)`, `ShrinkToFit()` | Grows or reduces capacity. |
| Construction | `FromBuffer(buffer, count)`, `FromCStr(cstr)`, `FromView(view)` | Initializes from code units, a terminated string or a view. |
| Mutation | `Clear()`, `AppendBuffer(buffer, count)`, `Append(cstr)`, `AppendString(other)`, `AppendView(view)`, `PushBack(value)`, `PopBack(outValue)`, `SetAt(index, value)`, `Insert(index, buffer, count)`, `InsertChar(index, value)`, `Erase(index, count)` | Updates the stored code-unit sequence. |
| Access | `Length()`, `Capacity()`, `IsEmpty()`, `CStr()`, `At(index, outValue)` | Reads size, capacity, emptiness, data or one code unit. |
| Views and slicing | `View()`, `Slice(start, count)`, `Substr(start, count, outSubstr)` | Creates a borrowed view or a new owned substring. |
| Comparison | `Compare(other)`, `Equal(other)`, `Equal.CStr(cstr)`, `Equal.View(view)`, `StartsWith(prefix)`, `EndsWith(suffix)` | Compares content or tests prefixes/suffixes. |
| Search | `Find(needle, outIndex)`, `Find.Str(needle, outIndex)` | Finds a code unit or a terminated string. |

`String` stores a trailing zero code unit but its reported length excludes that terminator. Lengths and indexes are code units, not Unicode scalar counts. For `String8`, a code unit is one UTF-8 byte; operations such as `At`, `SetAt`, `Erase` and `Slice` do not promise code-point boundaries.

~~~c
MODULE_TYPE(Text, String8) text = {0};

OPSTATUS status = OBJECT_CALL(&text, Create, 16U);
if (status == STATUS_CONST(SUCCESS)) {
    status = OBJECT_CALL(&text, Append, "Cosmeron");
}
if (status == STATUS_CONST(SUCCESS)) {
    size_t length = OBJECT_CALL_0(&text, Length);
    const char *data = OBJECT_CALL_0(&text, CStr);
    (void)length;
    (void)data;
}
(void)OBJECT_CALL_0(&text, Destroy);
~~~

The string owns its backing allocation. A pointer returned by `CStr` is borrowed and can be invalidated by mutation, capacity changes or destruction. Use `View`/ `Slice` when a non-owning range is sufficient and keep the owner alive while the view is used.

## C-string utilities

Header: `String/CString/CString.h`. Package: `Text_CString`.

The package provides:

| Group | Methods |
| --- | --- |
| Length | `Length`, `Length.Bounded` |
| Classification | `IsEmpty`, `IsAscii`, `IsAscii.Bounded` |
| Copy and concat | `Copy`, `Copy.Bounded`, `Concat`, `Concat.Bounded` |
| Comparison | `Compare`, `Compare.Bounded`, `Compare.IgnoreCase`, `Compare.IgnoreCaseBounded` |
| Equality | `Equal`, `Equal.Bounded`, `Equal.IgnoreCase`, `Equal.IgnoreCaseBounded` |
| Prefix/suffix | `StartsWith`, `StartsWith.Bounded`, `StartsWith.IgnoreCase`, `EndsWith`, `EndsWith.Bounded`, `EndsWith.IgnoreCase` |
| Search | `FindChar`, `FindLastChar`, `FindSubstring` and their bounded variants |
| Span | `Span`, `ComplementSpan` |
| Trim | `Trim`, `Trim.Start`, `Trim.End` |
| Count/replace | `CountChar`, `CountChar.Bounded`, `ReplaceChar`, `ReplaceChar.Bounded` |
| Allocation | `Duplicate`, `Duplicate.Bounded`, `Free` |

Prefer bounded operations whenever the available extent is known and the input may not contain a terminator inside that extent. `Copy` and `Concat` receive the destination capacity. `Duplicate` allocates a new string; release it with `Free`.

## String Builder

Header: `String/Builder/Builder.h`. Type: `MODULE_TYPE(Text, Builder)`.

The Builder is a dynamically growing UTF-8 buffer designed for incremental composition.

| Method | Description |
| --- | --- |
| `Create(initialCapacity)`, `Destroy` | Creates/releases the backing allocation. |
| `Clear`, `Reserve(newCapacity)`, `ShrinkToFit` | Controls contents and capacity. |
| `Append.Char` | Appends one byte character. |
| `Append.CStr` | Appends a C string. |
| `Append.Buffer` | Appends a sized byte buffer. |
| `Append.View` | Appends a UTF-8 view. |
| `Append.Codepoint` | Appends one Unicode code point. |
| `Append.Format`, `Append.FormatVariadic` | Appends formatted output. |
| `BuildString(outString)` | Materializes the contents as an owned String8. |
| `Length`, `Capacity`, `IsEmpty`, `CStr`, `View` | Observes the current contents. |

The Builder owns its buffer. `CStr` and `View` return borrowed references and must not be retained across an operation that may reallocate the buffer or after destruction.

---

# Formatting

## Format

Header: `Format/Format.h`. Package: `Text_Format`.

| Function | Contract |
| --- | --- |
| `Variadic(buffer, capacity, outWritten, format, arguments)` | Bounded formatter receiving a `va_list`. |
| `Variadic.Unsafe(buffer, outWritten, format, arguments)` | Unbounded formatter; caller must guarantee sufficient space. |
| `Buffer(buffer, capacity, outWritten, format, ...)` | Safe variadic convenience wrapper. |
| `Unsafe(buffer, outWritten, format, ...)` | Unsafe variadic convenience wrapper. |

Prefer `Buffer` or `Variadic`. The unsafe forms intentionally omit the output-capacity argument and must only be used when the destination size is guaranteed.

## Human-readable byte sizes

Header: `Format/BytesHuman.h`. Package: `Text_BytesHuman`.

| Function | Description |
| --- | --- |
| `Format(buffer, capacity, outWritten, bytes)` | Formats a byte count using the default unit standard and precision. |
| `Format.Ex(buffer, capacity, outWritten, bytes, unitStandard, precision)` | Selects IEC/SI units and precision. |
| `Parse(text, outBytes)` | Parses a textual byte-size value into `uint64_t`. |

The configuration enum defines IEC base 1024, SI base 1000, default precision 2 and maximum precision 6. The formatter writes into a caller-owned buffer and reports the number of bytes written.

---

# Character and attribute grids

## Scope and layout

Grid operations work on memory-backed cells. The module does **not** write to stdout, control a terminal, read stdin, call ncurses or display a canvas. Rendering and terminal input belong to an application/backend layer.

The character variants use `Char8`, `Char16` and `Char32`. Grid dimensions and positions use `MODULE_TYPED(Tuple, Dual, UInt16)`; rectangular regions use `MODULE_TYPED(Tuple, Quad, UInt16)`.

## Character-only grid

Header: `Grid/Char.h`. Generated object types follow `MODULE_TYPED(Text, Char, Char8)`, `Char16` and `Char32`.

The Char grid stores only characters. Its object methods include `Create(size)`, `Destroy`, `Resize(size)`, `Recreate(size)`, `Clone(source)`, `Clear`, `ReadCell(position, outCharacter)`, `WriteCell(position, character)`, `Fill(character)`, `FillRegion(region, character)`, `Read(region, destination)` and `Write`/`Write.Region`. `IsValid` checks the object state.

## Composite text grid

Header: `Grid/Grid.h`. Generated object types are `MODULE_TYPED(Text, Grid, Char8)`, `Char16` and `Char32`.

A composite grid stores a character plane together with an attribute plane. Its methods include `Create`, `Destroy`, `Resize`, `Recreate`, `Clone`, `Clear`, `Clear.With`, `ReadCell`, `WriteCell`, `Fill`, `FillRegion`, `Read`, `Write`, `WriteRegion` and `Blit`/`Blit.Region`.

The grid owns its backing planes. A resize/recreate/destroy can invalidate previously borrowed cell or data pointers. Use the status return from all mutating operations.

## Attribute plane and colors

Header: `Grid/Attribute.h`.

The attribute API exposes `MODULE_TYPE(Text, Attribute, Cell)` and `MODULE_TYPE(Text, Attribute, Grid)`. Attribute flags include bold, faint, italic, blink, inverse, hidden, strike, overline, transparent background, code-unit continuation and default foreground/background.

The `Attribute` package exposes `Default`, `IsEmpty`, `IsValid`, `Create`, `Destroy`, `Resize`, `Recreate`, `Clone`, `Clear`, `Clear.With`, `Fill`, `ReadCell`, `WriteCell`, `FillRegion`, `Read`, `Write` and `Write.Region`.

`Grid_Color` provides `RGB(red, green, blue)`, `Red(color)`, `Green(color)`, `Blue(color)` and `WindowsPalette()`. The Windows palette is a static 16-color table; it is a color utility, not a terminal backend.

---

# Attributed text

Header: `Attributed/Attributed.h`. Type: `MODULE_TYPE(Text, Attributed)`.

Attributed text stores UTF-8 content with associated attribute spans.

| Method | Description |
| --- | --- |
| `Create(initialCapacity)`, `Destroy` | Creates/releases text and span storage. |
| `Clear` | Clears text and spans. |
| `Clear.Spans` | Removes style spans while retaining text. |
| `SetText(text, length)`, `AppendText(text, length)` | Replaces or appends a sized UTF-8 byte span. |
| `AddSpan(start, length, attribute)` | Associates an attribute cell with a text range. |
| `GetAttributeAt(index, outAttribute)` | Retrieves the attribute at a byte index. |
| `RenderAnsi(outBuffer, capacity, outWritten)` | Renders ANSI escape sequences into a caller-owned buffer. |
| `Text`, `Length`, `SpanCount`, `Spans` | Inspects the current contents. |

The text and span arrays are managed by the Attributed object. The returned text/spans pointers are borrowed. `RenderAnsi` only produces a string; it does not print it or configure the terminal.

---

# Error handling, ownership and safe use

- Buffer-writing encoders, formatters, grid operations and string operations return `OPSTATUS`; check it before reading outputs.
- UTF-8 lengths are bytes; UTF-16 lengths are code units; code-point counts are neither.
- A View is non-owning. A String and Builder own their buffers.
- Grid and Attributed objects own their backing storage and must be destroyed.
- Pointers returned by `Data`, `CStr`, `Text` and `Spans` are borrowed and may be invalidated by mutation or destruction.
- The grid is an off-screen memory structure, not a terminal UI, renderer or input subsystem.

## Related modules

- [Tuple](Tuple.md) — grid dimensions and regions.
- [Memory](Memory.md) — allocation and byte operations.
- [Algorithm](Algorithm.md) — comparison results used by text views.
- [Module](Module.md) — generated packages, objects and typed specializations.

## Source headers

- `Codespace/Cosmeron/Modules/Text/Text.h`
- `Codespace/Cosmeron/Modules/Text/Unicode/Codepoint.h`
- `Codespace/Cosmeron/Modules/Text/Unicode/Width.h`
- `Codespace/Cosmeron/Modules/Text/Encoding/UTF8.h`
- `Codespace/Cosmeron/Modules/Text/Encoding/UTF16.h`
- `Codespace/Cosmeron/Modules/Text/View/View.def`
- `Codespace/Cosmeron/Modules/Text/String/String.def`
- `Codespace/Cosmeron/Modules/Text/String/CString/CString.h`
- `Codespace/Cosmeron/Modules/Text/String/Builder/Builder.h`
- `Codespace/Cosmeron/Modules/Text/Format/Format.h`
- `Codespace/Cosmeron/Modules/Text/Grid/Attribute.h`
- `Codespace/Cosmeron/Modules/Text/Grid/Char.def`
- `Codespace/Cosmeron/Modules/Text/Grid/Grid.def`
- `Codespace/Cosmeron/Modules/Text/Attributed/Attributed.h`
