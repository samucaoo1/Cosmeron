# Text

The **Text** module supplies Unicode code point checks, UTF-8/UTF-16 conversion, approximate display-width measurement, and a **two-dimensional off-screen text canvas** with character and visual-attribute planes. It is organized as a C11, header-included API.

```c
#include "Cosmeron/Modules/Text/Text.h"
```

The aggregate header includes all Text packages. The individual headers may also be included directly.

**Scope:** The `Grid` functions operate on memory-backed cells. They do **not** write to `stdout`, control a terminal, read `stdin`, call `ncurses`, or flush/display the canvas. Terminal I/O and rendering backends belong to a separate layer.

---

# Overview

| Package | Header | Templates | Purpose |
| --- | --- | ---: | --- |
| [Codepoint](#codepoint-package) | `Unicode/Codepoint.h` | 5 | Classification of Unicode code point values. |
| [UTF8](#utf8-package) | `Encoding/UTF8.h` | 5 | UTF-8 code point encoding, strict decoding, validation and counting. |
| [UTF16](#utf16-package) | `Encoding/UTF16.h` | 4 | UTF-16 code unit encoding, strict decoding and validation. |
| [Width](#width-package) | `Unicode/Width.h` | 2 | Approximate Unicode display columns for codepoints and UTF-8 text. |
| [Attribute](#attribute-package) | `Grid/Attribute.h` | 17 | Grid-cell style/color metadata and an independently owned attribute plane. |
| [Color](#color-package) | `Grid/Attribute.h` | 1 | The standard 16-color Windows palette lookup. |
| [CharGrid](#chargrid-package) | `Grid/Char.h` | 15 | Three variants of character-only rectangular planes. |
| [Grid](#grid-package) | `Grid/Grid.h` | 18 | Three variants of composite character/attribute canvas grids. |

**67 public function templates** are declared across these packages. `CharGrid` and `Grid` each instantiate three concrete character-width variants: `char`, `char16`, and `char32`. Together, the module currently exposes **133 concrete functions**.

### Macro form

```c
TEXT_GRID_TYPE(char32) canvas = {0};
OPSTATUS status = TEXT_GRID_FUNC(char32, Create)(
    &canvas, (TDUAL_TYPE(uint16)){.col = 80, .row = 25});
```

### Direct form

```c
Text_Grid_TGrid_char32 canvas = {0};
OPSTATUS status = Text_Grid_char32_Create(
    &canvas, (Struct_TDual_uint16){.col = 80, .row = 25});
```

With the default namespace, both forms refer to the same generated type and function. The macro form automatically follows a custom `COSMERON_NAMESPACE`; the direct symbol spelling shown here assumes the default namespace.

---

# API reference

## Function summary

| Package | Function | Description |
| --- | --- | --- |
| Codepoint | [`IsValid`](#codepoint-isvalid) | Tests whether a code point is at most U+10FFFF; surrogate values also pass this check. |
| Codepoint | [`IsScalar`](#codepoint-isscalar) | Tests whether a code point is within Unicode range and is not a surrogate. |
| Codepoint | [`IsASCII`](#codepoint-isascii) | Checks whether a code point lies between U+0000 and U+007F. |
| Codepoint | [`IsControl`](#codepoint-iscontrol) | Tests C0 and C1 control ranges: U+0000..001F and U+007F..009F. |
| Codepoint | [`IsWhitespace`](#codepoint-iswhitespace) | Tests the whitespace ranges explicitly listed in the implementation. |
| UTF8 | [`EncodedLength`](#utf8-encodedlength) | Reports the number of bytes required to encode one Unicode scalar. |
| UTF8 | [`Encode`](#utf8-encode) | Encodes a Unicode scalar to 1–4 UTF-8 bytes in a caller-supplied buffer. |
| UTF8 | [`Decode`](#utf8-decode) | Decodes one UTF-8 sequence from a byte buffer and reports consumed bytes. |
| UTF8 | [`Validate`](#utf8-validate) | Returns whether all supplied bytes form a valid UTF-8 sequence. |
| UTF8 | [`Count`](#utf8-count) | Counts Unicode code points within a valid UTF-8 byte span. |
| UTF16 | [`EncodedLength`](#utf16-encodedlength) | Reports whether a Unicode scalar occupies one or two UTF-16 code units. |
| UTF16 | [`Encode`](#utf16-encode) | Encodes one Unicode scalar as one UTF-16 code unit or a surrogate pair. |
| UTF16 | [`Decode`](#utf16-decode) | Decodes one scalar from a UTF-16 code-unit span. |
| UTF16 | [`Validate`](#utf16-validate) | Checks an entire UTF-16 code-unit span for well-formed surrogate pairs. |
| Width | [`Codepoint`](#width-codepoint) | Estimates a code point's terminal width in columns: 0, 1 or 2. |
| Width | [`UTF8`](#width-utf8) | Decodes a UTF-8 span and sums the display widths of its code points. |
| Attribute | [`Default`](#attribute-default) | Attribute-grid operation. |
| Attribute | [`IsEmpty`](#attribute-isempty) | Checks whether the object is an initialized, empty grid with zero dimensions and no allocated storage. |
| Attribute | [`IsValid`](#attribute-isvalid) | Checks internal dimensions, data pointers and, for composite grids, agreement of both planes. |
| Attribute | [`Create`](#attribute-create) | Allocates storage for a previously empty grid at a requested column/row size. |
| Attribute | [`Destroy`](#attribute-destroy) | Frees owned grid storage and resets the object to an empty state. |
| Attribute | [`Resize`](#attribute-resize) | Resizes the grid, retaining the overlapping top-left portion and initializing new cells. |
| Attribute | [`Recreate`](#attribute-recreate) | Replaces an existing valid grid with a freshly initialized grid of the requested dimensions. |
| Attribute | [`Clone`](#attribute-clone) | Deep-copies the source grid into a valid destination, replacing its previous storage. |
| Attribute | [`Clear`](#attribute-clear) | Restores grid contents to default attributes, without changing dimensions. |
| Attribute | [`ClearWith`](#attribute-clearwith) | Clears using a caller-specified attribute cell. |
| Attribute | [`Fill`](#attribute-fill) | Fills all cells with one character and/or attribute value. |
| Attribute | [`ReadCell`](#attribute-readcell) | Reads the character or attribute stored at a zero-based cell position. |
| Attribute | [`WriteCell`](#attribute-writecell) | Writes a character or attribute at a zero-based cell position. |
| Attribute | [`FillRegion`](#attribute-fillregion) | Fills an inclusive rectangle with the requested attributes. |
| Attribute | [`Read`](#attribute-read) | Extracts an inclusive rectangular region into an independently owned destination grid. |
| Attribute | [`Write`](#attribute-write) | Overwrites a destination region using all of a source grid's cells. |
| Attribute | [`WriteRegion`](#attribute-writeregion) | Overwrites a destination region using an inclusive rectangle of the source. |
| Color | [`WindowsPalette`](#color-windowspalette) | Returns a pointer to a static table of 16 Windows-console RGB colors. |
| CharGrid | [`IsEmpty`](#chargrid-isempty) | Checks whether the object is an initialized, empty grid with zero dimensions and no allocated storage. |
| CharGrid | [`IsValid`](#chargrid-isvalid) | Checks internal dimensions, data pointers and, for composite grids, agreement of both planes. |
| CharGrid | [`Create`](#chargrid-create) | Allocates storage for a previously empty grid at a requested column/row size. |
| CharGrid | [`Destroy`](#chargrid-destroy) | Frees owned grid storage and resets the object to an empty state. |
| CharGrid | [`Resize`](#chargrid-resize) | Resizes the grid, retaining the overlapping top-left portion and initializing new cells. |
| CharGrid | [`Recreate`](#chargrid-recreate) | Replaces an existing valid grid with a freshly initialized grid of the requested dimensions. |
| CharGrid | [`Clone`](#chargrid-clone) | Deep-copies the source grid into a valid destination, replacing its previous storage. |
| CharGrid | [`Clear`](#chargrid-clear) | Restores grid contents to zeroed characters, without changing dimensions. |
| CharGrid | [`ReadCell`](#chargrid-readcell) | Reads the character or attribute stored at a zero-based cell position. |
| CharGrid | [`WriteCell`](#chargrid-writecell) | Writes a character or attribute at a zero-based cell position. |
| CharGrid | [`Fill`](#chargrid-fill) | Fills all cells with one character and/or attribute value. |
| CharGrid | [`FillRegion`](#chargrid-fillregion) | Fills an inclusive rectangle with the requested character. |
| CharGrid | [`Read`](#chargrid-read) | Extracts an inclusive rectangular region into an independently owned destination grid. |
| CharGrid | [`Write`](#chargrid-write) | Overwrites a destination region using all of a source grid's cells. |
| CharGrid | [`WriteRegion`](#chargrid-writeregion) | Overwrites a destination region using an inclusive rectangle of the source. |
| Grid | [`IsEmpty`](#grid-isempty) | Checks whether the object is an initialized, empty grid with zero dimensions and no allocated storage. |
| Grid | [`IsValid`](#grid-isvalid) | Checks internal dimensions, data pointers and, for composite grids, agreement of both planes. |
| Grid | [`Create`](#grid-create) | Allocates storage for a previously empty grid at a requested column/row size. |
| Grid | [`Destroy`](#grid-destroy) | Frees owned grid storage and resets the object to an empty state. |
| Grid | [`Resize`](#grid-resize) | Resizes the grid, retaining the overlapping top-left portion and initializing new cells. |
| Grid | [`Recreate`](#grid-recreate) | Replaces an existing valid grid with a freshly initialized grid of the requested dimensions. |
| Grid | [`Clone`](#grid-clone) | Deep-copies the source grid into a valid destination, replacing its previous storage. |
| Grid | [`Clear`](#grid-clear) | Restores grid contents to empty characters or default attributes, without changing dimensions. |
| Grid | [`ClearWith`](#grid-clearwith) | Clears using a caller-specified attribute cell. |
| Grid | [`ReadCell`](#grid-readcell) | Reads the character or attribute stored at a zero-based cell position. |
| Grid | [`WriteCell`](#grid-writecell) | Writes a character or attribute at a zero-based cell position. |
| Grid | [`Fill`](#grid-fill) | Fills all cells with one character and/or attribute value. |
| Grid | [`FillRegion`](#grid-fillregion) | Fills an inclusive rectangle with the requested character and/or attributes. |
| Grid | [`Read`](#grid-read) | Extracts an inclusive rectangular region into an independently owned destination grid. |
| Grid | [`Write`](#grid-write) | Overwrites a destination region using all of a source grid's cells. |
| Grid | [`WriteRegion`](#grid-writeregion) | Overwrites a destination region using an inclusive rectangle of the source. |
| Grid | [`Blit`](#grid-blit) | Composites all cells of a source over a destination, applying attribute transparency/inverse rules. |
| Grid | [`BlitRegion`](#grid-blitregion) | Composites a rectangular source region over the destination. |

---

# Core types and layout

| Type macro | Default C type | Meaning |
| --- | --- | --- |
| `TEXT_TYPE(TChar16)` | `Text_TChar16` | 16-bit character code unit (`uint16_t`) |
| `TEXT_TYPE(TChar32)` | `Text_TChar32` | 32-bit code point storage (`uint32_t`) |
| `TEXT_GRID_CHAR_TYPE(char)` | `Text_Grid_TCharGrid_char` | Character-only plane of native `char` |
| `TEXT_GRID_CHAR_TYPE(char16)` | `Text_Grid_TCharGrid_char16` | Character-only plane of 16-bit code units |
| `TEXT_GRID_CHAR_TYPE(char32)` | `Text_Grid_TCharGrid_char32` | Character-only plane of 32-bit code units |
| `TEXT_GRID_ATTRIBUTE_TYPE(Cell)` | `Text_Grid_Attribute_TCell` | Foreground/background, flags and underline style |
| `TEXT_GRID_ATTRIBUTE_TYPE(Grid)` | `Text_Grid_Attribute_TGrid` | Independently owned attribute array and dimensions |
| `TEXT_GRID_TYPE(char)` | `Text_Grid_TGrid_char` | Composite char/attribute canvas |
| `TEXT_GRID_TYPE(char16)` | `Text_Grid_TGrid_char16` | Composite 16-bit-code-unit/attribute canvas |
| `TEXT_GRID_TYPE(char32)` | `Text_Grid_TGrid_char32` | Composite 32-bit-code-unit/attribute canvas |
| `TEXT_GRID_COLOR_TYPE(RGB)` | `Text_Grid_Color_TRGB` | `uint32_t` holding a 24-bit RGB color |
| `TEXT_GRID_ATTRIBUTE_TYPE(Flags)` | `Text_Grid_Attribute_TFlags` | 16-bit flag storage |
| `TEXT_GRID_UNDERLINE_TYPE(Style)` | `Text_Grid_Underline_TStyle` | Underline style identifier |

A grid uses `TDUAL_TYPE(uint16)` dimensions: `.col` means width in cells and `.row` means height. The position `{.x, .y}` is **zero-based** and storage is row-major: `data[y * size.col + x]`. A rectangle uses `TQUAD_TYPE(uint16)` fields `.left`, `.right`, `.top`, `.bottom`, with **inclusive** right and bottom bounds.

## Grid storage

```c
TEXT_GRID_TYPE(char32) canvas = {0};
/* canvas.chars.data       : Text_TChar32 * */
/* canvas.attributes.data : Text_Grid_Attribute_TCell * */
/* canvas.chars.size and canvas.attributes.size must match. */
```

A character grid stores individual code units, **not** full Unicode grapheme clusters. A 32-bit code point can occupy zero, one or two terminal columns, but assigning it to one grid cell does not automatically expand or merge neighboring cells. UTF-8 sequences cannot be safely inserted directly as a single `char` grid cell.

---

# Attributes, colors and constants

`TEXT_GRID_ATTRIBUTE_TYPE(Cell)` holds 24-bit `foreground` and `background`, 13-bit `flags`, and 3-bit `underline` fields. The bit-field layout is C-implementation-dependent and should not be serialized or treated as a portable binary network format.

### Attribute flags

| Constant suffix | Meaning |
| --- | --- |
| `BOLD`, `FAINT` | Intensity-style flags |
| `ITALIC`, `BLINK`, `INVERSE`, `HIDDEN` | Presentation flags |
| `STRIKE`, `OVERLINE` | Decorations |
| `TRANSPARENT_BACKGROUND` | Allow composited background to show through |
| `CODE_UNIT_CONTINUATION` | Mark a continuation cell |
| `DEFAULT_FOREGROUND`, `DEFAULT_BACKGROUND` | Use default terminal colors when rendered |
| `RESERVED` | Reserved bit |

Access these as `TEXT_GRID_ATTRIBUTE_CONST(BOLD)`, `TEXT_GRID_ATTRIBUTE_CONST(TRANSPARENT_BACKGROUND)`, etc. The flags can be combined with bitwise OR. The current grid implementation does not enforce mutual exclusion between `BOLD` and `FAINT`.

### Underline styles

`TEXT_GRID_UNDERLINE_CONST(NONE)`, `SINGLE`, `DOUBLE`, `CURLY`, `DOTTED`, `DASHED`, `RESERVED_1` and `RESERVED_2`. Underline style identifies a requested decoration; actual rendering depends on an external display layer.

### RGB helpers and Windows palette

```c
uint32_t color = TEXT_GRID_COLOR_RGB(0x12, 0x34, 0x56);
uint8_t r = TEXT_GRID_COLOR_RED(color);
uint8_t g = TEXT_GRID_COLOR_GREEN(color);
uint8_t b = TEXT_GRID_COLOR_BLUE(color);
/* color == 0x123456, r == 0x12, g == 0x34, b == 0x56 */
```

These are **macros, not functions**. `TEXT_GRID_COLOR_CONST(RGB_MASK)` is 0x00FFFFFF. The header provides the 16 familiar Windows-console colors as `TEXT_GRID_COLOR_CONST(WINDOWS_...)`, corresponding palette indices `WINDOWS_INDEX_...`, and `TEXT_GRID_COLOR_CONST(WINDOWS_COUNT)` = 16. The WindowsPalette function exposes the read-only table.

---

# Error handling and ownership

Most conversion and grid operations return `OPSTATUS`, commonly `STATUS_CONST(SUCCESS)`, `INVALID_ARGUMENT`, `INVALID_SEQUENCE`, `INCOMPLETE_SEQUENCE`, `INSUFFICIENT_SPACE`, `OUT_OF_RANGE`, `ARITHMETIC_OVERFLOW` or `OUT_OF_MEMORY`, depending on the operation.

- **Conversion:** UTF-8/UTF-16 encoders write the requested code units only after validating scalars and capacity. Decoders set the code point and consumed length only on success. Nothing is NUL-terminated automatically.
- **Ownership:** `Create`, `Resize`, `Recreate`, `Clone` and `Read` allocate grid data. Call matching `Destroy` for each independently owned grid. Do not shallow-copy owning grids or free their data pointers separately.
- **Initialization:** start with `{0}`. Both dimensions must be zero for an empty grid; sizes where only one dimension is zero are rejected.
- **Bounds:** `Read`, `Write`, `FillRegion`, and `Blit` do not clip partial rectangles. Status distinguishes invalid rectangle ordering from outside-grid positions.
- **Grid updates:** ordinary `Write` replaces cell/attribute values. `Blit` composes attributes with transparency rules while still copying source glyphs.

---

# Codepoint package

Header: `Cosmeron/Modules/Text/Unicode/Codepoint.h`

Classification of Unicode code point values.



## Function summary

| Operation | Description |
| --- | --- |
| [`IsValid`](#codepoint-isvalid) | Tests whether a code point is at most U+10FFFF; surrogate values also pass this check. |
| [`IsScalar`](#codepoint-isscalar) | Tests whether a code point is within Unicode range and is not a surrogate. |
| [`IsASCII`](#codepoint-isascii) | Checks whether a code point lies between U+0000 and U+007F. |
| [`IsControl`](#codepoint-iscontrol) | Tests C0 and C1 control ranges: U+0000..001F and U+007F..009F. |
| [`IsWhitespace`](#codepoint-iswhitespace) | Tests the whitespace ranges explicitly listed in the implementation. |

---

# Codepoint IsValid

Tests whether a code point is at most U+10FFFF; surrogate values also pass this check.

### Syntax

#### Macro form

```c
bool TEXT_CODEPOINT_FUNC(IsValid)(TEXT_TYPE(TChar32) codepoint);
```

#### Direct form

```c
bool Text_Codepoint_IsValid(Text_TChar32 codepoint);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `codepoint` | `TEXT_TYPE(TChar32) codepoint` | Unicode scalar or code-point value, according to operation. |

---

### Return value

`bool`: true if the predicate is satisfied; false otherwise.

---

### Remarks

Only bounds are checked (<= U+10FFFF). Use IsScalar when the surrogate range U+D800..DFFF must be excluded.

---

### Example

```c
bool result = TEXT_CODEPOINT_FUNC(IsValid)((TEXT_TYPE(TChar32))0x0041U);
```

---

# Codepoint IsScalar

Tests whether a code point is within Unicode range and is not a surrogate.

### Syntax

#### Macro form

```c
bool TEXT_CODEPOINT_FUNC(IsScalar)(TEXT_TYPE(TChar32) codepoint);
```

#### Direct form

```c
bool Text_Codepoint_IsScalar(Text_TChar32 codepoint);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `codepoint` | `TEXT_TYPE(TChar32) codepoint` | Unicode scalar or code-point value, according to operation. |

---

### Return value

`bool`: true if the predicate is satisfied; false otherwise.

---

### Remarks

Rejects U+D800..DFFF and code points above U+10FFFF. Valid scalars may still be controls, zero-width marks or unassigned characters.

---

### Example

```c
bool result = TEXT_CODEPOINT_FUNC(IsScalar)((TEXT_TYPE(TChar32))0x0041U);
```

---

# Codepoint IsASCII

Checks whether a code point lies between U+0000 and U+007F.

### Syntax

#### Macro form

```c
bool TEXT_CODEPOINT_FUNC(IsASCII)(TEXT_TYPE(TChar32) codepoint);
```

#### Direct form

```c
bool Text_Codepoint_IsASCII(Text_TChar32 codepoint);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `codepoint` | `TEXT_TYPE(TChar32) codepoint` | Unicode scalar or code-point value, according to operation. |

---

### Return value

`bool`: true if the predicate is satisfied; false otherwise.

---

### Remarks

Includes ASCII controls and U+007F. ASCII membership does not imply a printable character.

---

### Example

```c
bool result = TEXT_CODEPOINT_FUNC(IsASCII)((TEXT_TYPE(TChar32))0x0041U);
```

---

# Codepoint IsControl

Tests C0 and C1 control ranges: U+0000..001F and U+007F..009F.

### Syntax

#### Macro form

```c
bool TEXT_CODEPOINT_FUNC(IsControl)(TEXT_TYPE(TChar32) codepoint);
```

#### Direct form

```c
bool Text_Codepoint_IsControl(Text_TChar32 codepoint);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `codepoint` | `TEXT_TYPE(TChar32) codepoint` | Unicode scalar or code-point value, according to operation. |

---

### Return value

`bool`: true if the predicate is satisfied; false otherwise.

---

### Remarks

Recognizes U+0000..001F and U+007F..009F, irrespective of display or terminal control semantics.

---

### Example

```c
bool result = TEXT_CODEPOINT_FUNC(IsControl)((TEXT_TYPE(TChar32))0x0041U);
```

---

# Codepoint IsWhitespace

Tests the whitespace ranges explicitly listed in the implementation.

### Syntax

#### Macro form

```c
bool TEXT_CODEPOINT_FUNC(IsWhitespace)(TEXT_TYPE(TChar32) codepoint);
```

#### Direct form

```c
bool Text_Codepoint_IsWhitespace(Text_TChar32 codepoint);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `codepoint` | `TEXT_TYPE(TChar32) codepoint` | Unicode scalar or code-point value, according to operation. |

---

### Return value

`bool`: true if the predicate is satisfied; false otherwise.

---

### Remarks

The recognized set is a hard-coded selection including TAB/LF/CR, space, NBSP and selected Unicode separators. Not a locale-aware or automatically updated Unicode property database.

---

### Example

```c
bool result = TEXT_CODEPOINT_FUNC(IsWhitespace)((TEXT_TYPE(TChar32))0x0041U);
```

---

# UTF8 package

Header: `Cosmeron/Modules/Text/Encoding/UTF8.h`

UTF-8 code point encoding, strict decoding, validation and counting.



## Function summary

| Operation | Description |
| --- | --- |
| [`EncodedLength`](#utf8-encodedlength) | Reports the number of bytes required to encode one Unicode scalar. |
| [`Encode`](#utf8-encode) | Encodes a Unicode scalar to 1–4 UTF-8 bytes in a caller-supplied buffer. |
| [`Decode`](#utf8-decode) | Decodes one UTF-8 sequence from a byte buffer and reports consumed bytes. |
| [`Validate`](#utf8-validate) | Returns whether all supplied bytes form a valid UTF-8 sequence. |
| [`Count`](#utf8-count) | Counts Unicode code points within a valid UTF-8 byte span. |

---

# UTF8 EncodedLength

Reports the number of bytes required to encode one Unicode scalar.

### Syntax

#### Macro form

```c
OPSTATUS TEXT_UTF8_FUNC(EncodedLength)(TEXT_TYPE(TChar32) codepoint, size_t *outUnits);
```

#### Direct form

```c
OPSTATUS Text_UTF8_EncodedLength(Text_TChar32 codepoint, size_t *outUnits);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `codepoint` | `TEXT_TYPE(TChar32) codepoint` | Unicode scalar or code-point value, according to operation. |
| `outUnits` | `size_t *outUnits` | Output count of encoded/consumed code units (bytes for UTF-8, uint16 units for UTF-16). |

---

### Return value

`OPSTATUS`: `STATUS_CONST(SUCCESS)` or a recoverable error such as invalid argument, encoding sequence, insufficient capacity, bounds or allocation failure.

---

### Remarks

Requires an output pointer and a Unicode scalar. The length counts bytes, excluding any NUL terminator. A surrogate or code point above U+10FFFF returns INVALID_SEQUENCE.

---

### Example

```c
size_t units = 0;
OPSTATUS status = TEXT_UTF8_FUNC(EncodedLength)((TEXT_TYPE(TChar32))0x1F600U, &units);
```

---

# UTF8 Encode

Encodes a Unicode scalar to 1–4 UTF-8 bytes in a caller-supplied buffer.

### Syntax

#### Macro form

```c
OPSTATUS TEXT_UTF8_FUNC(Encode)(TEXT_TYPE(TChar32) codepoint, unsigned char *output, size_t capacity, size_t *outUnits);
```

#### Direct form

```c
OPSTATUS Text_UTF8_Encode(Text_TChar32 codepoint, unsigned char *output, size_t capacity, size_t *outUnits);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `codepoint` | `TEXT_TYPE(TChar32) codepoint` | Unicode scalar or code-point value, according to operation. |
| `output` | `unsigned char *output` | Caller-owned destination for encoded code units. |
| `capacity` | `size_t capacity` | Capacity of the output array in the encoding's code units. |
| `outUnits` | `size_t *outUnits` | Output count of encoded/consumed code units (bytes for UTF-8, uint16 units for UTF-16). |

---

### Return value

`OPSTATUS`: `STATUS_CONST(SUCCESS)` or a recoverable error such as invalid argument, encoding sequence, insufficient capacity, bounds or allocation failure.

---

### Remarks

The caller supplies a byte buffer and capacity in bytes. Returns INSUFFICIENT_SPACE if the buffer cannot hold the complete encoded sequence; it writes no NUL terminator.

---

### Example

```c
unsigned char bytes[4]; size_t units = 0;
OPSTATUS status = TEXT_UTF8_FUNC(Encode)((TEXT_TYPE(TChar32))0x1F600U, bytes,
    sizeof bytes, &units);
```

---

# UTF8 Decode

Decodes one UTF-8 sequence from a byte buffer and reports consumed bytes.

### Syntax

#### Macro form

```c
OPSTATUS TEXT_UTF8_FUNC(Decode)(const unsigned char *input, size_t size, TEXT_TYPE(TChar32) *outCodepoint, size_t *outUnits);
```

#### Direct form

```c
OPSTATUS Text_UTF8_Decode(const unsigned char *input, size_t size, Text_TChar32 *outCodepoint, size_t *outUnits);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `input` | `const unsigned char *input` | Input code-unit span; counted by size, not necessarily NUL terminated. |
| `size` | `size_t size` | Length of input in bytes for UTF-8 or 16-bit units for UTF-16; grid size uses Struct_TDual_uint16. |
| `outCodepoint` | `TEXT_TYPE(TChar32) *outCodepoint` | Writable output Unicode scalar. |
| `outUnits` | `size_t *outUnits` | Output count of encoded/consumed code units (bytes for UTF-8, uint16 units for UTF-16). |

---

### Return value

`OPSTATUS`: `STATUS_CONST(SUCCESS)` or a recoverable error such as invalid argument, encoding sequence, insufficient capacity, bounds or allocation failure.

---

### Remarks

Decodes only the **first** sequence. A missing continuation returns INCOMPLETE_SEQUENCE; malformed leading bytes, overlong forms, invalid continuations, surrogates, and out-of-range values return INVALID_SEQUENCE. On failure, output values remain unchanged.

---

### Example

```c
const unsigned char bytes[] = {0xF0U, 0x9FU, 0x98U, 0x80U};
TEXT_TYPE(TChar32) scalar = 0; size_t units = 0;
OPSTATUS status = TEXT_UTF8_FUNC(Decode)(bytes, sizeof bytes, &scalar, &units);
```

---

# UTF8 Validate

Returns whether all supplied bytes form a valid UTF-8 sequence.

### Syntax

#### Macro form

```c
bool TEXT_UTF8_FUNC(Validate)(const unsigned char *input, size_t size);
```

#### Direct form

```c
bool Text_UTF8_Validate(const unsigned char *input, size_t size);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `input` | `const unsigned char *input` | Input code-unit span; counted by size, not necessarily NUL terminated. |
| `size` | `size_t size` | Length of input in bytes for UTF-8 or 16-bit units for UTF-16; grid size uses Struct_TDual_uint16. |

---

### Return value

`bool`: true if the predicate is satisfied; false otherwise.

---

### Remarks

Checks a byte span, not a NUL-terminated string. An empty span is valid, including NULL with size 0.

---

### Example

```c
const unsigned char bytes[] = {'H', 'i'};
bool valid = TEXT_UTF8_FUNC(Validate)(bytes, sizeof bytes);
```

---

# UTF8 Count

Counts Unicode code points within a valid UTF-8 byte span.

### Syntax

#### Macro form

```c
OPSTATUS TEXT_UTF8_FUNC(Count)(const unsigned char *input, size_t size, size_t *outCodepoints, size_t *outUnits);
```

#### Direct form

```c
OPSTATUS Text_UTF8_Count(const unsigned char *input, size_t size, size_t *outCodepoints, size_t *outUnits);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `input` | `const unsigned char *input` | Input code-unit span; counted by size, not necessarily NUL terminated. |
| `size` | `size_t size` | Length of input in bytes for UTF-8 or 16-bit units for UTF-16; grid size uses Struct_TDual_uint16. |
| `outCodepoints` | `size_t *outCodepoints` | Writable total decoded code-point count. |
| `outUnits` | `size_t *outUnits` | Output count of encoded/consumed code units (bytes for UTF-8, uint16 units for UTF-16). |

---

### Return value

`OPSTATUS`: `STATUS_CONST(SUCCESS)` or a recoverable error such as invalid argument, encoding sequence, insufficient capacity, bounds or allocation failure.

---

### Remarks

Returns both code point count and successfully consumed byte count only after complete success. Does not count grapheme clusters or display columns; accepts NULL input only when size is 0.

---

### Example

```c
const unsigned char bytes[] = {'H', 'i'};
size_t points = 0, units = 0;
OPSTATUS status = TEXT_UTF8_FUNC(Count)(bytes, sizeof bytes, &points, &units);
```

---

# UTF16 package

Header: `Cosmeron/Modules/Text/Encoding/UTF16.h`

UTF-16 code unit encoding, strict decoding and validation.



## Function summary

| Operation | Description |
| --- | --- |
| [`EncodedLength`](#utf16-encodedlength) | Reports whether a Unicode scalar occupies one or two UTF-16 code units. |
| [`Encode`](#utf16-encode) | Encodes one Unicode scalar as one UTF-16 code unit or a surrogate pair. |
| [`Decode`](#utf16-decode) | Decodes one scalar from a UTF-16 code-unit span. |
| [`Validate`](#utf16-validate) | Checks an entire UTF-16 code-unit span for well-formed surrogate pairs. |

---

# UTF16 EncodedLength

Reports whether a Unicode scalar occupies one or two UTF-16 code units.

### Syntax

#### Macro form

```c
OPSTATUS TEXT_UTF16_FUNC(EncodedLength)(TEXT_TYPE(TChar32) codepoint, size_t *outUnits);
```

#### Direct form

```c
OPSTATUS Text_UTF16_EncodedLength(Text_TChar32 codepoint, size_t *outUnits);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `codepoint` | `TEXT_TYPE(TChar32) codepoint` | Unicode scalar or code-point value, according to operation. |
| `outUnits` | `size_t *outUnits` | Output count of encoded/consumed code units (bytes for UTF-8, uint16 units for UTF-16). |

---

### Return value

`OPSTATUS`: `STATUS_CONST(SUCCESS)` or a recoverable error such as invalid argument, encoding sequence, insufficient capacity, bounds or allocation failure.

---

### Remarks

Validates Unicode scalar values; a surrogate alone is never a valid input scalar. Counts 16-bit units, **not bytes**.

---

### Example

```c
size_t units = 0;
OPSTATUS status = TEXT_UTF16_FUNC(EncodedLength)((TEXT_TYPE(TChar32))0x1F600U, &units);
```

---

# UTF16 Encode

Encodes one Unicode scalar as one UTF-16 code unit or a surrogate pair.

### Syntax

#### Macro form

```c
OPSTATUS TEXT_UTF16_FUNC(Encode)(TEXT_TYPE(TChar32) codepoint, TEXT_TYPE(TChar16) *output, size_t capacity, size_t *outUnits);
```

#### Direct form

```c
OPSTATUS Text_UTF16_Encode(Text_TChar32 codepoint, Text_TChar16 *output, size_t capacity, size_t *outUnits);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `codepoint` | `TEXT_TYPE(TChar32) codepoint` | Unicode scalar or code-point value, according to operation. |
| `output` | `TEXT_TYPE(TChar16) *output` | Caller-owned destination for encoded code units. |
| `capacity` | `size_t capacity` | Capacity of the output array in the encoding's code units. |
| `outUnits` | `size_t *outUnits` | Output count of encoded/consumed code units (bytes for UTF-8, uint16 units for UTF-16). |

---

### Return value

`OPSTATUS`: `STATUS_CONST(SUCCESS)` or a recoverable error such as invalid argument, encoding sequence, insufficient capacity, bounds or allocation failure.

---

### Remarks

Capacity and outUnits are in uint16_t code units; supplementary scalars use surrogate pairs. Does not add a string terminator, and insufficient capacity returns INSUFFICIENT_SPACE.

---

### Example

```c
TEXT_TYPE(TChar16) encoded[2]; size_t units = 0;
OPSTATUS status = TEXT_UTF16_FUNC(Encode)((TEXT_TYPE(TChar32))0x1F600U, encoded, 2, &units);
```

---

# UTF16 Decode

Decodes one scalar from a UTF-16 code-unit span.

### Syntax

#### Macro form

```c
OPSTATUS TEXT_UTF16_FUNC(Decode)(const TEXT_TYPE(TChar16) *input, size_t size, TEXT_TYPE(TChar32) *outCodepoint, size_t *outUnits);
```

#### Direct form

```c
OPSTATUS Text_UTF16_Decode(const Text_TChar16 *input, size_t size, Text_TChar32 *outCodepoint, size_t *outUnits);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `input` | `const TEXT_TYPE(TChar16) *input` | Input code-unit span; counted by size, not necessarily NUL terminated. |
| `size` | `size_t size` | Length of input in bytes for UTF-8 or 16-bit units for UTF-16; grid size uses Struct_TDual_uint16. |
| `outCodepoint` | `TEXT_TYPE(TChar32) *outCodepoint` | Writable output Unicode scalar. |
| `outUnits` | `size_t *outUnits` | Output count of encoded/consumed code units (bytes for UTF-8, uint16 units for UTF-16). |

---

### Return value

`OPSTATUS`: `STATUS_CONST(SUCCESS)` or a recoverable error such as invalid argument, encoding sequence, insufficient capacity, bounds or allocation failure.

---

### Remarks

An isolated low surrogate or invalid pair is INVALID_SEQUENCE. A high surrogate without a following unit is INCOMPLETE_SEQUENCE. Returns consumed 16-bit units on success.

---

### Example

```c
const TEXT_TYPE(TChar16) input[] = {0xD83DU, 0xDE00U};
TEXT_TYPE(TChar32) scalar = 0; size_t units = 0;
OPSTATUS status = TEXT_UTF16_FUNC(Decode)(input, 2, &scalar, &units);
```

---

# UTF16 Validate

Checks an entire UTF-16 code-unit span for well-formed surrogate pairs.

### Syntax

#### Macro form

```c
bool TEXT_UTF16_FUNC(Validate)(const TEXT_TYPE(TChar16) *input, size_t size);
```

#### Direct form

```c
bool Text_UTF16_Validate(const Text_TChar16 *input, size_t size);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `input` | `const TEXT_TYPE(TChar16) *input` | Input code-unit span; counted by size, not necessarily NUL terminated. |
| `size` | `size_t size` | Length of input in bytes for UTF-8 or 16-bit units for UTF-16; grid size uses Struct_TDual_uint16. |

---

### Return value

`bool`: true if the predicate is satisfied; false otherwise.

---

### Remarks

Checks all input code units, including surrogate-pair ordering. An empty span is valid; NULL is accepted only for size 0.

---

### Example

```c
const TEXT_TYPE(TChar16) input[] = {0xD83DU, 0xDE00U};
bool valid = TEXT_UTF16_FUNC(Validate)(input, 2);
```

---

# Width package

Header: `Cosmeron/Modules/Text/Unicode/Width.h`

Approximate Unicode display columns for codepoints and UTF-8 text.



## Function summary

| Operation | Description |
| --- | --- |
| [`Codepoint`](#width-codepoint) | Estimates a code point's terminal width in columns: 0, 1 or 2. |
| [`UTF8`](#width-utf8) | Decodes a UTF-8 span and sums the display widths of its code points. |

---

# Width Codepoint

Estimates a code point's terminal width in columns: 0, 1 or 2.

### Syntax

#### Macro form

```c
OPSTATUS TEXT_WIDTH_FUNC(Codepoint)(TEXT_TYPE(TChar32) codepoint, int *outWidth);
```

#### Direct form

```c
OPSTATUS Text_Width_Codepoint(Text_TChar32 codepoint, int *outWidth);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `codepoint` | `TEXT_TYPE(TChar32) codepoint` | Unicode scalar or code-point value, according to operation. |
| `outWidth` | `int *outWidth` | Writable width result, 0, 1 or 2. |

---

### Return value

`OPSTATUS`: `STATUS_CONST(SUCCESS)` or a recoverable error such as invalid argument, encoding sequence, insufficient capacity, bounds or allocation failure.

---

### Remarks

Rejects non-scalars and control code points as INVALID_SEQUENCE. Returns 0 for selected combining and zero-width scalar ranges, 2 for hard-coded wide ranges (including many CJK/emoji), and 1 otherwise. **NUL (U+0000) is a control code and returns INVALID_SEQUENCE**, not width zero. Ambiguous-width, grapheme clusters and terminal-specific rendering are not handled.

---

### Example

```c
int columns = 0;
OPSTATUS status = TEXT_WIDTH_FUNC(Codepoint)((TEXT_TYPE(TChar32))0x754CU, &columns);
```

---

# Width UTF8

Decodes a UTF-8 span and sums the display widths of its code points.

### Syntax

#### Macro form

```c
OPSTATUS TEXT_WIDTH_FUNC(UTF8)(const unsigned char *input, size_t size, size_t *outColumns, size_t *outUnits);
```

#### Direct form

```c
OPSTATUS Text_Width_UTF8(const unsigned char *input, size_t size, size_t *outColumns, size_t *outUnits);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `input` | `const unsigned char *input` | Input code-unit span; counted by size, not necessarily NUL terminated. |
| `size` | `size_t size` | Length of input in bytes for UTF-8 or 16-bit units for UTF-16; grid size uses Struct_TDual_uint16. |
| `outColumns` | `size_t *outColumns` | Writable total display-column result. |
| `outUnits` | `size_t *outUnits` | Output count of encoded/consumed code units (bytes for UTF-8, uint16 units for UTF-16). |

---

### Return value

`OPSTATUS`: `STATUS_CONST(SUCCESS)` or a recoverable error such as invalid argument, encoding sequence, insufficient capacity, bounds or allocation failure.

---

### Remarks

A successful call returns total columns and consumed UTF-8 bytes. The function rejects malformed encodings **and control code points**. It simply sums code point widths, so ZWJ emoji sequences and grapheme clusters may differ from rendered width.

---

### Example

```c
const unsigned char utf8[] = {0xE7U, 0x95U, 0x8CU};
size_t columns = 0, units = 0;
OPSTATUS status = TEXT_WIDTH_FUNC(UTF8)(utf8, sizeof utf8, &columns, &units);
```

---

# Attribute package

Header: `Cosmeron/Modules/Text/Grid/Attribute.h`

Grid-cell style/color metadata and an independently owned attribute plane.



## Function summary

| Operation | Description |
| --- | --- |
| [`Default`](#attribute-default) | Attribute-grid operation. |
| [`IsEmpty`](#attribute-isempty) | Checks whether the object is an initialized, empty grid with zero dimensions and no allocated storage. |
| [`IsValid`](#attribute-isvalid) | Checks internal dimensions, data pointers and, for composite grids, agreement of both planes. |
| [`Create`](#attribute-create) | Allocates storage for a previously empty grid at a requested column/row size. |
| [`Destroy`](#attribute-destroy) | Frees owned grid storage and resets the object to an empty state. |
| [`Resize`](#attribute-resize) | Resizes the grid, retaining the overlapping top-left portion and initializing new cells. |
| [`Recreate`](#attribute-recreate) | Replaces an existing valid grid with a freshly initialized grid of the requested dimensions. |
| [`Clone`](#attribute-clone) | Deep-copies the source grid into a valid destination, replacing its previous storage. |
| [`Clear`](#attribute-clear) | Restores grid contents to default attributes, without changing dimensions. |
| [`ClearWith`](#attribute-clearwith) | Clears using a caller-specified attribute cell. |
| [`Fill`](#attribute-fill) | Fills all cells with one character and/or attribute value. |
| [`ReadCell`](#attribute-readcell) | Reads the character or attribute stored at a zero-based cell position. |
| [`WriteCell`](#attribute-writecell) | Writes a character or attribute at a zero-based cell position. |
| [`FillRegion`](#attribute-fillregion) | Fills an inclusive rectangle with the requested attributes. |
| [`Read`](#attribute-read) | Extracts an inclusive rectangular region into an independently owned destination grid. |
| [`Write`](#attribute-write) | Overwrites a destination region using all of a source grid's cells. |
| [`WriteRegion`](#attribute-writeregion) | Overwrites a destination region using an inclusive rectangle of the source. |

---

# Attribute Default

Attribute-grid operation.

### Syntax

#### Macro form

```c
TEXT_GRID_ATTRIBUTE_TYPE(Cell) TEXT_GRID_ATTRIBUTE_FUNC(Default)(void);
```

#### Direct form

```c
Text_Grid_Attribute_TCell Text_Grid_Attribute_Default(void);
```

---

### Parameters

None.

---

### Return value

`Text_Grid_Attribute_TCell`: default attribute value returned by copy.

---

### Remarks

All positions use `x`/`.col` for columns and `y`/`.row` for rows. Region bounds (`left`, `right`, `top`, `bottom`) are **inclusive**, with `left <= right` and `top <= bottom`. Violations are INVALID_ARGUMENT or OUT_OF_RANGE.

---

### Example

```c
TEXT_GRID_ATTRIBUTE_TYPE(Cell) defaults = TEXT_GRID_ATTRIBUTE_FUNC(Default)();
```

---

# Attribute IsEmpty

Checks whether the object is an initialized, empty grid with zero dimensions and no allocated storage.

### Syntax

#### Macro form

```c
bool TEXT_GRID_ATTRIBUTE_FUNC(IsEmpty)(const TEXT_GRID_ATTRIBUTE_TYPE(Grid) *grid);
```

#### Direct form

```c
bool Text_Grid_Attribute_IsEmpty(const Text_Grid_Attribute_TGrid *grid);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `grid` | `const TEXT_GRID_ATTRIBUTE_TYPE(Grid) *grid` | Pointer to a correctly initialized grid. |

---

### Return value

`bool`: true if the predicate is satisfied; false otherwise.

---

### Remarks

A zero-initialized `{0}` grid is valid and empty; a nonempty grid is valid only when its storage and dimensions are consistent. A NULL pointer returns false.

---

### Example

```c
bool result = TEXT_GRID_ATTRIBUTE_FUNC(IsEmpty)(&grid);
```

---

# Attribute IsValid

Checks internal dimensions, data pointers and, for composite grids, agreement of both planes.

### Syntax

#### Macro form

```c
bool TEXT_GRID_ATTRIBUTE_FUNC(IsValid)(const TEXT_GRID_ATTRIBUTE_TYPE(Grid) *grid);
```

#### Direct form

```c
bool Text_Grid_Attribute_IsValid(const Text_Grid_Attribute_TGrid *grid);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `grid` | `const TEXT_GRID_ATTRIBUTE_TYPE(Grid) *grid` | Pointer to a correctly initialized grid. |

---

### Return value

`bool`: true if the predicate is satisfied; false otherwise.

---

### Remarks

A zero-initialized `{0}` grid is valid and empty; a nonempty grid is valid only when its storage and dimensions are consistent. A NULL pointer returns false.

---

### Example

```c
bool result = TEXT_GRID_ATTRIBUTE_FUNC(IsValid)(&grid);
```

---

# Attribute Create

Allocates storage for a previously empty grid at a requested column/row size.

### Syntax

#### Macro form

```c
OPSTATUS TEXT_GRID_ATTRIBUTE_FUNC(Create)(TEXT_GRID_ATTRIBUTE_TYPE(Grid) *grid, TDUAL_TYPE(uint16) size);
```

#### Direct form

```c
OPSTATUS Text_Grid_Attribute_Create(Text_Grid_Attribute_TGrid *grid, Struct_TDual_uint16 size);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `grid` | `TEXT_GRID_ATTRIBUTE_TYPE(Grid) *grid` | Pointer to a correctly initialized grid. |
| `size` | `TDUAL_TYPE(uint16) size` | Length of input in bytes for UTF-8 or 16-bit units for UTF-16; grid size uses Struct_TDual_uint16. |

---

### Return value

`OPSTATUS`: `STATUS_CONST(SUCCESS)` or a recoverable error such as invalid argument, encoding sequence, insufficient capacity, bounds or allocation failure.

---

### Remarks

Requires a zero-initialized or destroyed **empty** destination. Both dimensions may be 0; a 0-by-N or N-by-0 size is rejected. New character cells are zero, new attribute cells use Default().

---

### Example

```c
TEXT_GRID_ATTRIBUTE_TYPE(Grid) grid = {0};
TDUAL_TYPE(uint16) size = {.col = 4, .row = 3};
OPSTATUS status = TEXT_GRID_ATTRIBUTE_FUNC(Create)(&grid, size);
```

---

# Attribute Destroy

Frees owned grid storage and resets the object to an empty state.

### Syntax

#### Macro form

```c
OPSTATUS TEXT_GRID_ATTRIBUTE_FUNC(Destroy)(TEXT_GRID_ATTRIBUTE_TYPE(Grid) *grid);
```

#### Direct form

```c
OPSTATUS Text_Grid_Attribute_Destroy(Text_Grid_Attribute_TGrid *grid);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `grid` | `TEXT_GRID_ATTRIBUTE_TYPE(Grid) *grid` | Pointer to a correctly initialized grid. |

---

### Return value

`OPSTATUS`: `STATUS_CONST(SUCCESS)` or a recoverable error such as invalid argument, encoding sequence, insufficient capacity, bounds or allocation failure.

---

### Remarks

Releases owned buffers. NULL returns INVALID_ARGUMENT; destroying an already empty initialized object succeeds.

---

### Example

```c
OPSTATUS status = TEXT_GRID_ATTRIBUTE_FUNC(Destroy)(&grid);
```

---

# Attribute Resize

Resizes the grid, retaining the overlapping top-left portion and initializing new cells.

### Syntax

#### Macro form

```c
OPSTATUS TEXT_GRID_ATTRIBUTE_FUNC(Resize)(TEXT_GRID_ATTRIBUTE_TYPE(Grid) *grid, TDUAL_TYPE(uint16) size);
```

#### Direct form

```c
OPSTATUS Text_Grid_Attribute_Resize(Text_Grid_Attribute_TGrid *grid, Struct_TDual_uint16 size);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `grid` | `TEXT_GRID_ATTRIBUTE_TYPE(Grid) *grid` | Pointer to a correctly initialized grid. |
| `size` | `TDUAL_TYPE(uint16) size` | Length of input in bytes for UTF-8 or 16-bit units for UTF-16; grid size uses Struct_TDual_uint16. |

---

### Return value

`OPSTATUS`: `STATUS_CONST(SUCCESS)` or a recoverable error such as invalid argument, encoding sequence, insufficient capacity, bounds or allocation failure.

---

### Remarks

Allocates a new grid and preserves the overlapping **top-left** area. New cells are zero/default attributes; allocation failures leave the original intact.

---

### Example

```c
TDUAL_TYPE(uint16) size = {.col = 4, .row = 3};
OPSTATUS status = TEXT_GRID_ATTRIBUTE_FUNC(Resize)(&grid, size);
```

---

# Attribute Recreate

Replaces an existing valid grid with a freshly initialized grid of the requested dimensions.

### Syntax

#### Macro form

```c
OPSTATUS TEXT_GRID_ATTRIBUTE_FUNC(Recreate)(TEXT_GRID_ATTRIBUTE_TYPE(Grid) *grid, TDUAL_TYPE(uint16) size);
```

#### Direct form

```c
OPSTATUS Text_Grid_Attribute_Recreate(Text_Grid_Attribute_TGrid *grid, Struct_TDual_uint16 size);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `grid` | `TEXT_GRID_ATTRIBUTE_TYPE(Grid) *grid` | Pointer to a correctly initialized grid. |
| `size` | `TDUAL_TYPE(uint16) size` | Length of input in bytes for UTF-8 or 16-bit units for UTF-16; grid size uses Struct_TDual_uint16. |

---

### Return value

`OPSTATUS`: `STATUS_CONST(SUCCESS)` or a recoverable error such as invalid argument, encoding sequence, insufficient capacity, bounds or allocation failure.

---

### Remarks

Allocates fresh storage of the new size, discarding old contents only after successful creation. New cells are zero/default attributes.

---

### Example

```c
TDUAL_TYPE(uint16) size = {.col = 4, .row = 3};
OPSTATUS status = TEXT_GRID_ATTRIBUTE_FUNC(Recreate)(&grid, size);
```

---

# Attribute Clone

Deep-copies the source grid into a valid destination, replacing its previous storage.

### Syntax

#### Macro form

```c
OPSTATUS TEXT_GRID_ATTRIBUTE_FUNC(Clone)(TEXT_GRID_ATTRIBUTE_TYPE(Grid) *destination, const TEXT_GRID_ATTRIBUTE_TYPE(Grid) *source);
```

#### Direct form

```c
OPSTATUS Text_Grid_Attribute_Clone(Text_Grid_Attribute_TGrid *destination, const Text_Grid_Attribute_TGrid *source);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `destination` | `TEXT_GRID_ATTRIBUTE_TYPE(Grid) *destination` | Writable grid object; must be structurally valid, including zero-initialized empty form. |
| `source` | `const TEXT_GRID_ATTRIBUTE_TYPE(Grid) *source` | Read-only source grid with valid storage. |

---

### Return value

`OPSTATUS`: `STATUS_CONST(SUCCESS)` or a recoverable error such as invalid argument, encoding sequence, insufficient capacity, bounds or allocation failure.

---

### Remarks

Allocates a deep copy, replacing destination only on success. Existing source and destination must both be valid (an empty `{0}` destination is valid).

---

### Example

```c
/* source and destination: already valid TEXT_GRID_ATTRIBUTE_TYPE(Grid) objects. */
OPSTATUS status = TEXT_GRID_ATTRIBUTE_FUNC(Clone)(&destination, &source);
```

---

# Attribute Clear

Restores grid contents to default attributes, without changing dimensions.

### Syntax

#### Macro form

```c
OPSTATUS TEXT_GRID_ATTRIBUTE_FUNC(Clear)(TEXT_GRID_ATTRIBUTE_TYPE(Grid) *grid);
```

#### Direct form

```c
OPSTATUS Text_Grid_Attribute_Clear(Text_Grid_Attribute_TGrid *grid);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `grid` | `TEXT_GRID_ATTRIBUTE_TYPE(Grid) *grid` | Pointer to a correctly initialized grid. |

---

### Return value

`OPSTATUS`: `STATUS_CONST(SUCCESS)` or a recoverable error such as invalid argument, encoding sequence, insufficient capacity, bounds or allocation failure.

---

### Remarks

Resets every attribute to Attribute_Default, preserving the grid dimensions.

---

### Example

```c
OPSTATUS status = TEXT_GRID_ATTRIBUTE_FUNC(Clear)(&grid);
```

---

# Attribute ClearWith

Clears using a caller-specified attribute cell.

### Syntax

#### Macro form

```c
OPSTATUS TEXT_GRID_ATTRIBUTE_FUNC(ClearWith)(TEXT_GRID_ATTRIBUTE_TYPE(Grid) *grid, TEXT_GRID_ATTRIBUTE_TYPE(Cell) attribute);
```

#### Direct form

```c
OPSTATUS Text_Grid_Attribute_ClearWith(Text_Grid_Attribute_TGrid *grid, Text_Grid_Attribute_TCell attribute);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `grid` | `TEXT_GRID_ATTRIBUTE_TYPE(Grid) *grid` | Pointer to a correctly initialized grid. |
| `attribute` | `TEXT_GRID_ATTRIBUTE_TYPE(Cell) attribute` | Attribute cell value, or writable attribute pointer on ReadCell. |

---

### Return value

`OPSTATUS`: `STATUS_CONST(SUCCESS)` or a recoverable error such as invalid argument, encoding sequence, insufficient capacity, bounds or allocation failure.

---

### Remarks

Replaces all attribute cells with the provided attribute, preserving allocation.

---

### Example

```c
TEXT_GRID_ATTRIBUTE_TYPE(Cell) cell = TEXT_GRID_ATTRIBUTE_FUNC(Default)();
OPSTATUS status = TEXT_GRID_ATTRIBUTE_FUNC(ClearWith)(&grid, cell);
```

---

# Attribute Fill

Fills all cells with one character and/or attribute value.

### Syntax

#### Macro form

```c
OPSTATUS TEXT_GRID_ATTRIBUTE_FUNC(Fill)(TEXT_GRID_ATTRIBUTE_TYPE(Grid) *grid, TEXT_GRID_ATTRIBUTE_TYPE(Cell) attribute);
```

#### Direct form

```c
OPSTATUS Text_Grid_Attribute_Fill(Text_Grid_Attribute_TGrid *grid, Text_Grid_Attribute_TCell attribute);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `grid` | `TEXT_GRID_ATTRIBUTE_TYPE(Grid) *grid` | Pointer to a correctly initialized grid. |
| `attribute` | `TEXT_GRID_ATTRIBUTE_TYPE(Cell) attribute` | Attribute cell value, or writable attribute pointer on ReadCell. |

---

### Return value

`OPSTATUS`: `STATUS_CONST(SUCCESS)` or a recoverable error such as invalid argument, encoding sequence, insufficient capacity, bounds or allocation failure.

---

### Remarks

Equivalent to ClearWith: fills all attribute cells.

---

### Example

```c
TEXT_GRID_ATTRIBUTE_TYPE(Cell) cell = TEXT_GRID_ATTRIBUTE_FUNC(Default)();
OPSTATUS status = TEXT_GRID_ATTRIBUTE_FUNC(Fill)(&grid, cell);
```

---

# Attribute ReadCell

Reads the character or attribute stored at a zero-based cell position.

### Syntax

#### Macro form

```c
OPSTATUS TEXT_GRID_ATTRIBUTE_FUNC(ReadCell)(const TEXT_GRID_ATTRIBUTE_TYPE(Grid) *grid, TDUAL_TYPE(uint16) position, TEXT_GRID_ATTRIBUTE_TYPE(Cell) *attribute);
```

#### Direct form

```c
OPSTATUS Text_Grid_Attribute_ReadCell(const Text_Grid_Attribute_TGrid *grid, Struct_TDual_uint16 position, Text_Grid_Attribute_TCell *attribute);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `grid` | `const TEXT_GRID_ATTRIBUTE_TYPE(Grid) *grid` | Pointer to a correctly initialized grid. |
| `position` | `TDUAL_TYPE(uint16) position` | Zero-based (x,y) position, Struct TDual_uint16. |
| `attribute` | `TEXT_GRID_ATTRIBUTE_TYPE(Cell) *attribute` | Attribute cell value, or writable attribute pointer on ReadCell. |

---

### Return value

`OPSTATUS`: `STATUS_CONST(SUCCESS)` or a recoverable error such as invalid argument, encoding sequence, insufficient capacity, bounds or allocation failure.

---

### Remarks

Access uses `data[y * columns + x]`; positions must lie inside the allocated dimensions. Out-of-bounds calls return OUT_OF_RANGE. Reads use required typed output pointers.

---

### Example

```c
TDUAL_TYPE(uint16) position = {.x = 1, .y = 1};
TEXT_GRID_ATTRIBUTE_TYPE(Cell) cell = TEXT_GRID_ATTRIBUTE_FUNC(Default)();
OPSTATUS status = TEXT_GRID_ATTRIBUTE_FUNC(ReadCell)(&grid, position, &cell);
```

---

# Attribute WriteCell

Writes a character or attribute at a zero-based cell position.

### Syntax

#### Macro form

```c
OPSTATUS TEXT_GRID_ATTRIBUTE_FUNC(WriteCell)(TEXT_GRID_ATTRIBUTE_TYPE(Grid) *grid, TDUAL_TYPE(uint16) position, TEXT_GRID_ATTRIBUTE_TYPE(Cell) attribute);
```

#### Direct form

```c
OPSTATUS Text_Grid_Attribute_WriteCell(Text_Grid_Attribute_TGrid *grid, Struct_TDual_uint16 position, Text_Grid_Attribute_TCell attribute);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `grid` | `TEXT_GRID_ATTRIBUTE_TYPE(Grid) *grid` | Pointer to a correctly initialized grid. |
| `position` | `TDUAL_TYPE(uint16) position` | Zero-based (x,y) position, Struct TDual_uint16. |
| `attribute` | `TEXT_GRID_ATTRIBUTE_TYPE(Cell) attribute` | Attribute cell value, or writable attribute pointer on ReadCell. |

---

### Return value

`OPSTATUS`: `STATUS_CONST(SUCCESS)` or a recoverable error such as invalid argument, encoding sequence, insufficient capacity, bounds or allocation failure.

---

### Remarks

Access uses `data[y * columns + x]`; positions must lie inside the allocated dimensions. Out-of-bounds calls return OUT_OF_RANGE. Reads use required typed output pointers.

---

### Example

```c
TDUAL_TYPE(uint16) position = {.x = 1, .y = 1};
TEXT_GRID_ATTRIBUTE_TYPE(Cell) cell = TEXT_GRID_ATTRIBUTE_FUNC(Default)();
OPSTATUS status = TEXT_GRID_ATTRIBUTE_FUNC(WriteCell)(&grid, position, cell);
```

---

# Attribute FillRegion

Fills an inclusive rectangle with the requested attributes.

### Syntax

#### Macro form

```c
OPSTATUS TEXT_GRID_ATTRIBUTE_FUNC(FillRegion)(TEXT_GRID_ATTRIBUTE_TYPE(Grid) *grid, TQUAD_TYPE(uint16) region, TEXT_GRID_ATTRIBUTE_TYPE(Cell) attribute);
```

#### Direct form

```c
OPSTATUS Text_Grid_Attribute_FillRegion(Text_Grid_Attribute_TGrid *grid, Struct_TQuad_uint16 region, Text_Grid_Attribute_TCell attribute);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `grid` | `TEXT_GRID_ATTRIBUTE_TYPE(Grid) *grid` | Pointer to a correctly initialized grid. |
| `region` | `TQUAD_TYPE(uint16) region` | Inclusive rectangle (left/right/top/bottom), Struct TQuad_uint16. |
| `attribute` | `TEXT_GRID_ATTRIBUTE_TYPE(Cell) attribute` | Attribute cell value, or writable attribute pointer on ReadCell. |

---

### Return value

`OPSTATUS`: `STATUS_CONST(SUCCESS)` or a recoverable error such as invalid argument, encoding sequence, insufficient capacity, bounds or allocation failure.

---

### Remarks

The rectangle is inclusive; invalid ordering returns INVALID_ARGUMENT, and out-of-bounds coordinates return OUT_OF_RANGE. No clipping is performed.

---

### Example

```c
TDUAL_TYPE(uint16) position = {.x = 1, .y = 1};
TQUAD_TYPE(uint16) region = {.left = 0, .right = 1, .top = 0, .bottom = 1};
TEXT_GRID_ATTRIBUTE_TYPE(Cell) cell = TEXT_GRID_ATTRIBUTE_FUNC(Default)();
OPSTATUS status = TEXT_GRID_ATTRIBUTE_FUNC(FillRegion)(&grid, region, cell);
```

---

# Attribute Read

Extracts an inclusive rectangular region into an independently owned destination grid.

### Syntax

#### Macro form

```c
OPSTATUS TEXT_GRID_ATTRIBUTE_FUNC(Read)(const TEXT_GRID_ATTRIBUTE_TYPE(Grid) *source, TQUAD_TYPE(uint16) region, TEXT_GRID_ATTRIBUTE_TYPE(Grid) *destination);
```

#### Direct form

```c
OPSTATUS Text_Grid_Attribute_Read(const Text_Grid_Attribute_TGrid *source, Struct_TQuad_uint16 region, Text_Grid_Attribute_TGrid *destination);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `source` | `const TEXT_GRID_ATTRIBUTE_TYPE(Grid) *source` | Read-only source grid with valid storage. |
| `region` | `TQUAD_TYPE(uint16) region` | Inclusive rectangle (left/right/top/bottom), Struct TQuad_uint16. |
| `destination` | `TEXT_GRID_ATTRIBUTE_TYPE(Grid) *destination` | Writable grid object; must be structurally valid, including zero-initialized empty form. |

---

### Return value

`OPSTATUS`: `STATUS_CONST(SUCCESS)` or a recoverable error such as invalid argument, encoding sequence, insufficient capacity, bounds or allocation failure.

---

### Remarks

The selected rectangle is copied to newly allocated temporary storage, then replaces the **valid** destination on success. The destination may be an empty `{0}` grid. No automatic clipping.

---

### Example

```c
TQUAD_TYPE(uint16) region = {.left = 0, .right = 1, .top = 0, .bottom = 1};
/* source and destination: already valid TEXT_GRID_ATTRIBUTE_TYPE(Grid) objects. */
OPSTATUS status = TEXT_GRID_ATTRIBUTE_FUNC(Read)(&source, region, &destination);
```

---

# Attribute Write

Overwrites a destination region using all of a source grid's cells.

### Syntax

#### Macro form

```c
OPSTATUS TEXT_GRID_ATTRIBUTE_FUNC(Write)(TEXT_GRID_ATTRIBUTE_TYPE(Grid) *destination, TDUAL_TYPE(uint16) destinationPosition, const TEXT_GRID_ATTRIBUTE_TYPE(Grid) *source);
```

#### Direct form

```c
OPSTATUS Text_Grid_Attribute_Write(Text_Grid_Attribute_TGrid *destination, Struct_TDual_uint16 destinationPosition, const Text_Grid_Attribute_TGrid *source);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `destination` | `TEXT_GRID_ATTRIBUTE_TYPE(Grid) *destination` | Writable grid object; must be structurally valid, including zero-initialized empty form. |
| `destinationPosition` | `TDUAL_TYPE(uint16) destinationPosition` | Zero-based (x,y) destination top-left corner. |
| `source` | `const TEXT_GRID_ATTRIBUTE_TYPE(Grid) *source` | Read-only source grid with valid storage. |

---

### Return value

`OPSTATUS`: `STATUS_CONST(SUCCESS)` or a recoverable error such as invalid argument, encoding sequence, insufficient capacity, bounds or allocation failure.

---

### Remarks

Copies source cells to destination, replacing their contents and attributes where applicable. Writes use temporary source snapshots to support overlapping/self-copy. Source and destination must already be valid; the write must fit exactly.

---

### Example

```c
TDUAL_TYPE(uint16) position = {.x = 1, .y = 1};
/* source and destination: already valid TEXT_GRID_ATTRIBUTE_TYPE(Grid) objects. */
OPSTATUS status = TEXT_GRID_ATTRIBUTE_FUNC(Write)(&destination, position, &source);
```

---

# Attribute WriteRegion

Overwrites a destination region using an inclusive rectangle of the source.

### Syntax

#### Macro form

```c
OPSTATUS TEXT_GRID_ATTRIBUTE_FUNC(WriteRegion)(TEXT_GRID_ATTRIBUTE_TYPE(Grid) *destination, TDUAL_TYPE(uint16) destinationPosition, const TEXT_GRID_ATTRIBUTE_TYPE(Grid) *source, TQUAD_TYPE(uint16) sourceRegion);
```

#### Direct form

```c
OPSTATUS Text_Grid_Attribute_WriteRegion(Text_Grid_Attribute_TGrid *destination, Struct_TDual_uint16 destinationPosition, const Text_Grid_Attribute_TGrid *source, Struct_TQuad_uint16 sourceRegion);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `destination` | `TEXT_GRID_ATTRIBUTE_TYPE(Grid) *destination` | Writable grid object; must be structurally valid, including zero-initialized empty form. |
| `destinationPosition` | `TDUAL_TYPE(uint16) destinationPosition` | Zero-based (x,y) destination top-left corner. |
| `source` | `const TEXT_GRID_ATTRIBUTE_TYPE(Grid) *source` | Read-only source grid with valid storage. |
| `sourceRegion` | `TQUAD_TYPE(uint16) sourceRegion` | Inclusive source rectangle, Struct TQuad_uint16. |

---

### Return value

`OPSTATUS`: `STATUS_CONST(SUCCESS)` or a recoverable error such as invalid argument, encoding sequence, insufficient capacity, bounds or allocation failure.

---

### Remarks

Copies source cells to destination, replacing their contents and attributes where applicable. Writes use temporary source snapshots to support overlapping/self-copy. Source and destination must already be valid; the write must fit exactly.

---

### Example

```c
TQUAD_TYPE(uint16) region = {.left = 0, .right = 1, .top = 0, .bottom = 1};
TDUAL_TYPE(uint16) position = {.x = 1, .y = 1};
/* source and destination: already valid TEXT_GRID_ATTRIBUTE_TYPE(Grid) objects. */
OPSTATUS status = TEXT_GRID_ATTRIBUTE_FUNC(WriteRegion)(&destination, position, &source, region);
```

---

# Color package

Header: `Cosmeron/Modules/Text/Grid/Attribute.h`

The standard 16-color Windows palette lookup.



## Function summary

| Operation | Description |
| --- | --- |
| [`WindowsPalette`](#color-windowspalette) | Returns a pointer to a static table of 16 Windows-console RGB colors. |

---

# Color WindowsPalette

Returns a pointer to a static table of 16 Windows-console RGB colors.

### Syntax

#### Macro form

```c
const TEXT_GRID_COLOR_TYPE(RGB) * TEXT_GRID_COLOR_FUNC(WindowsPalette)(void);
```

#### Direct form

```c
const Text_Grid_Color_TRGB * Text_Grid_Color_WindowsPalette(void);
```

---

### Parameters

None.

---

### Return value

`const Text_Grid_Color_TRGB *`: pointer to read-only, static 16-entry palette.

---

### Remarks

Returns an internal static 16-element RGB array ordered like the WINDOWS_INDEX_* enum. Do not free or write through the returned const pointer.

---

### Example

```c
const TEXT_GRID_COLOR_TYPE(RGB) *palette = TEXT_GRID_COLOR_FUNC(WindowsPalette)();
uint32_t red = palette[TEXT_GRID_COLOR_CONST(WINDOWS_INDEX_BRIGHT_RED)];
```

---

# CharGrid package

Header: `Cosmeron/Modules/Text/Grid/Char.h`

Three variants of character-only rectangular planes.

The signatures below show the `char32` specialization, using `TEXT_TYPE(TChar32)` characters. Equivalent operations are generated for `char` and `char16`; replace the suffix, its type macro and character C type together.

## Function summary

| Operation | Description |
| --- | --- |
| [`IsEmpty`](#chargrid-isempty) | Checks whether the object is an initialized, empty grid with zero dimensions and no allocated storage. |
| [`IsValid`](#chargrid-isvalid) | Checks internal dimensions, data pointers and, for composite grids, agreement of both planes. |
| [`Create`](#chargrid-create) | Allocates storage for a previously empty grid at a requested column/row size. |
| [`Destroy`](#chargrid-destroy) | Frees owned grid storage and resets the object to an empty state. |
| [`Resize`](#chargrid-resize) | Resizes the grid, retaining the overlapping top-left portion and initializing new cells. |
| [`Recreate`](#chargrid-recreate) | Replaces an existing valid grid with a freshly initialized grid of the requested dimensions. |
| [`Clone`](#chargrid-clone) | Deep-copies the source grid into a valid destination, replacing its previous storage. |
| [`Clear`](#chargrid-clear) | Restores grid contents to zeroed characters, without changing dimensions. |
| [`ReadCell`](#chargrid-readcell) | Reads the character or attribute stored at a zero-based cell position. |
| [`WriteCell`](#chargrid-writecell) | Writes a character or attribute at a zero-based cell position. |
| [`Fill`](#chargrid-fill) | Fills all cells with one character and/or attribute value. |
| [`FillRegion`](#chargrid-fillregion) | Fills an inclusive rectangle with the requested character. |
| [`Read`](#chargrid-read) | Extracts an inclusive rectangular region into an independently owned destination grid. |
| [`Write`](#chargrid-write) | Overwrites a destination region using all of a source grid's cells. |
| [`WriteRegion`](#chargrid-writeregion) | Overwrites a destination region using an inclusive rectangle of the source. |

---

# CharGrid IsEmpty

Checks whether the object is an initialized, empty grid with zero dimensions and no allocated storage.

### Syntax

#### Macro form

```c
bool TEXT_GRID_CHAR_FUNC(char32, IsEmpty)(const TEXT_GRID_CHAR_TYPE(char32) *grid);
```

#### Direct form

```c
bool Text_Grid_Char_char32_IsEmpty(const Text_Grid_TCharGrid_char32 *grid);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `grid` | `const TEXT_GRID_CHAR_TYPE(char32) *grid` | Pointer to a correctly initialized grid. |

---

### Return value

`bool`: true if the predicate is satisfied; false otherwise.

---

### Remarks

A zero-initialized `{0}` grid is valid and empty; a nonempty grid is valid only when its storage and dimensions are consistent. A NULL pointer returns false.

---

### Example

```c
bool result = TEXT_GRID_CHAR_FUNC(char32, IsEmpty)(&grid);
```

---

# CharGrid IsValid

Checks internal dimensions, data pointers and, for composite grids, agreement of both planes.

### Syntax

#### Macro form

```c
bool TEXT_GRID_CHAR_FUNC(char32, IsValid)(const TEXT_GRID_CHAR_TYPE(char32) *grid);
```

#### Direct form

```c
bool Text_Grid_Char_char32_IsValid(const Text_Grid_TCharGrid_char32 *grid);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `grid` | `const TEXT_GRID_CHAR_TYPE(char32) *grid` | Pointer to a correctly initialized grid. |

---

### Return value

`bool`: true if the predicate is satisfied; false otherwise.

---

### Remarks

A zero-initialized `{0}` grid is valid and empty; a nonempty grid is valid only when its storage and dimensions are consistent. A NULL pointer returns false.

---

### Example

```c
bool result = TEXT_GRID_CHAR_FUNC(char32, IsValid)(&grid);
```

---

# CharGrid Create

Allocates storage for a previously empty grid at a requested column/row size.

### Syntax

#### Macro form

```c
OPSTATUS TEXT_GRID_CHAR_FUNC(char32, Create)(TEXT_GRID_CHAR_TYPE(char32) *grid, TDUAL_TYPE(uint16) size);
```

#### Direct form

```c
OPSTATUS Text_Grid_Char_char32_Create(Text_Grid_TCharGrid_char32 *grid, Struct_TDual_uint16 size);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `grid` | `TEXT_GRID_CHAR_TYPE(char32) *grid` | Pointer to a correctly initialized grid. |
| `size` | `TDUAL_TYPE(uint16) size` | Length of input in bytes for UTF-8 or 16-bit units for UTF-16; grid size uses Struct_TDual_uint16. |

---

### Return value

`OPSTATUS`: `STATUS_CONST(SUCCESS)` or a recoverable error such as invalid argument, encoding sequence, insufficient capacity, bounds or allocation failure.

---

### Remarks

Requires a zero-initialized or destroyed **empty** destination. Both dimensions may be 0; a 0-by-N or N-by-0 size is rejected. New character cells are zero, new attribute cells use Default().

---

### Example

```c
TEXT_GRID_CHAR_TYPE(char32) grid = {0};
TDUAL_TYPE(uint16) size = {.col = 4, .row = 3};
OPSTATUS status = TEXT_GRID_CHAR_FUNC(char32, Create)(&grid, size);
```

---

# CharGrid Destroy

Frees owned grid storage and resets the object to an empty state.

### Syntax

#### Macro form

```c
OPSTATUS TEXT_GRID_CHAR_FUNC(char32, Destroy)(TEXT_GRID_CHAR_TYPE(char32) *grid);
```

#### Direct form

```c
OPSTATUS Text_Grid_Char_char32_Destroy(Text_Grid_TCharGrid_char32 *grid);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `grid` | `TEXT_GRID_CHAR_TYPE(char32) *grid` | Pointer to a correctly initialized grid. |

---

### Return value

`OPSTATUS`: `STATUS_CONST(SUCCESS)` or a recoverable error such as invalid argument, encoding sequence, insufficient capacity, bounds or allocation failure.

---

### Remarks

Releases owned buffers. NULL returns INVALID_ARGUMENT; destroying an already empty initialized object succeeds.

---

### Example

```c
OPSTATUS status = TEXT_GRID_CHAR_FUNC(char32, Destroy)(&grid);
```

---

# CharGrid Resize

Resizes the grid, retaining the overlapping top-left portion and initializing new cells.

### Syntax

#### Macro form

```c
OPSTATUS TEXT_GRID_CHAR_FUNC(char32, Resize)(TEXT_GRID_CHAR_TYPE(char32) *grid, TDUAL_TYPE(uint16) size);
```

#### Direct form

```c
OPSTATUS Text_Grid_Char_char32_Resize(Text_Grid_TCharGrid_char32 *grid, Struct_TDual_uint16 size);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `grid` | `TEXT_GRID_CHAR_TYPE(char32) *grid` | Pointer to a correctly initialized grid. |
| `size` | `TDUAL_TYPE(uint16) size` | Length of input in bytes for UTF-8 or 16-bit units for UTF-16; grid size uses Struct_TDual_uint16. |

---

### Return value

`OPSTATUS`: `STATUS_CONST(SUCCESS)` or a recoverable error such as invalid argument, encoding sequence, insufficient capacity, bounds or allocation failure.

---

### Remarks

Allocates a new grid and preserves the overlapping **top-left** area. New cells are zero/default attributes; allocation failures leave the original intact.

---

### Example

```c
TDUAL_TYPE(uint16) size = {.col = 4, .row = 3};
OPSTATUS status = TEXT_GRID_CHAR_FUNC(char32, Resize)(&grid, size);
```

---

# CharGrid Recreate

Replaces an existing valid grid with a freshly initialized grid of the requested dimensions.

### Syntax

#### Macro form

```c
OPSTATUS TEXT_GRID_CHAR_FUNC(char32, Recreate)(TEXT_GRID_CHAR_TYPE(char32) *grid, TDUAL_TYPE(uint16) size);
```

#### Direct form

```c
OPSTATUS Text_Grid_Char_char32_Recreate(Text_Grid_TCharGrid_char32 *grid, Struct_TDual_uint16 size);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `grid` | `TEXT_GRID_CHAR_TYPE(char32) *grid` | Pointer to a correctly initialized grid. |
| `size` | `TDUAL_TYPE(uint16) size` | Length of input in bytes for UTF-8 or 16-bit units for UTF-16; grid size uses Struct_TDual_uint16. |

---

### Return value

`OPSTATUS`: `STATUS_CONST(SUCCESS)` or a recoverable error such as invalid argument, encoding sequence, insufficient capacity, bounds or allocation failure.

---

### Remarks

Allocates fresh storage of the new size, discarding old contents only after successful creation. New cells are zero/default attributes.

---

### Example

```c
TDUAL_TYPE(uint16) size = {.col = 4, .row = 3};
OPSTATUS status = TEXT_GRID_CHAR_FUNC(char32, Recreate)(&grid, size);
```

---

# CharGrid Clone

Deep-copies the source grid into a valid destination, replacing its previous storage.

### Syntax

#### Macro form

```c
OPSTATUS TEXT_GRID_CHAR_FUNC(char32, Clone)(TEXT_GRID_CHAR_TYPE(char32) *destination, const TEXT_GRID_CHAR_TYPE(char32) *source);
```

#### Direct form

```c
OPSTATUS Text_Grid_Char_char32_Clone(Text_Grid_TCharGrid_char32 *destination, const Text_Grid_TCharGrid_char32 *source);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `destination` | `TEXT_GRID_CHAR_TYPE(char32) *destination` | Writable grid object; must be structurally valid, including zero-initialized empty form. |
| `source` | `const TEXT_GRID_CHAR_TYPE(char32) *source` | Read-only source grid with valid storage. |

---

### Return value

`OPSTATUS`: `STATUS_CONST(SUCCESS)` or a recoverable error such as invalid argument, encoding sequence, insufficient capacity, bounds or allocation failure.

---

### Remarks

Allocates a deep copy, replacing destination only on success. Existing source and destination must both be valid (an empty `{0}` destination is valid).

---

### Example

```c
/* source and destination: already valid TEXT_GRID_CHAR_TYPE(char32) objects. */
OPSTATUS status = TEXT_GRID_CHAR_FUNC(char32, Clone)(&destination, &source);
```

---

# CharGrid Clear

Restores grid contents to zeroed characters, without changing dimensions.

### Syntax

#### Macro form

```c
OPSTATUS TEXT_GRID_CHAR_FUNC(char32, Clear)(TEXT_GRID_CHAR_TYPE(char32) *grid);
```

#### Direct form

```c
OPSTATUS Text_Grid_Char_char32_Clear(Text_Grid_TCharGrid_char32 *grid);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `grid` | `TEXT_GRID_CHAR_TYPE(char32) *grid` | Pointer to a correctly initialized grid. |

---

### Return value

`OPSTATUS`: `STATUS_CONST(SUCCESS)` or a recoverable error such as invalid argument, encoding sequence, insufficient capacity, bounds or allocation failure.

---

### Remarks

Sets all code units to zero without shrinking the grid.

---

### Example

```c
OPSTATUS status = TEXT_GRID_CHAR_FUNC(char32, Clear)(&grid);
```

---

# CharGrid ReadCell

Reads the character or attribute stored at a zero-based cell position.

### Syntax

#### Macro form

```c
OPSTATUS TEXT_GRID_CHAR_FUNC(char32, ReadCell)(const TEXT_GRID_CHAR_TYPE(char32) *grid, TDUAL_TYPE(uint16) position, TEXT_TYPE(TChar32) *character);
```

#### Direct form

```c
OPSTATUS Text_Grid_Char_char32_ReadCell(const Text_Grid_TCharGrid_char32 *grid, Struct_TDual_uint16 position, Text_TChar32 *character);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `grid` | `const TEXT_GRID_CHAR_TYPE(char32) *grid` | Pointer to a correctly initialized grid. |
| `position` | `TDUAL_TYPE(uint16) position` | Zero-based (x,y) position, Struct TDual_uint16. |
| `character` | `TEXT_TYPE(TChar32) *character` | A single character code unit, or pointer output on read; not an entire Unicode text string. |

---

### Return value

`OPSTATUS`: `STATUS_CONST(SUCCESS)` or a recoverable error such as invalid argument, encoding sequence, insufficient capacity, bounds or allocation failure.

---

### Remarks

Access uses `data[y * columns + x]`; positions must lie inside the allocated dimensions. Out-of-bounds calls return OUT_OF_RANGE. Reads use required typed output pointers.

---

### Example

```c
TDUAL_TYPE(uint16) position = {.x = 1, .y = 1};
TEXT_TYPE(TChar32) character = U'X';
OPSTATUS status = TEXT_GRID_CHAR_FUNC(char32, ReadCell)(&grid, position, &character);
```

---

# CharGrid WriteCell

Writes a character or attribute at a zero-based cell position.

### Syntax

#### Macro form

```c
OPSTATUS TEXT_GRID_CHAR_FUNC(char32, WriteCell)(TEXT_GRID_CHAR_TYPE(char32) *grid, TDUAL_TYPE(uint16) position, TEXT_TYPE(TChar32) character);
```

#### Direct form

```c
OPSTATUS Text_Grid_Char_char32_WriteCell(Text_Grid_TCharGrid_char32 *grid, Struct_TDual_uint16 position, Text_TChar32 character);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `grid` | `TEXT_GRID_CHAR_TYPE(char32) *grid` | Pointer to a correctly initialized grid. |
| `position` | `TDUAL_TYPE(uint16) position` | Zero-based (x,y) position, Struct TDual_uint16. |
| `character` | `TEXT_TYPE(TChar32) character` | A single character code unit, or pointer output on read; not an entire Unicode text string. |

---

### Return value

`OPSTATUS`: `STATUS_CONST(SUCCESS)` or a recoverable error such as invalid argument, encoding sequence, insufficient capacity, bounds or allocation failure.

---

### Remarks

Access uses `data[y * columns + x]`; positions must lie inside the allocated dimensions. Out-of-bounds calls return OUT_OF_RANGE. Reads use required typed output pointers.

---

### Example

```c
TDUAL_TYPE(uint16) position = {.x = 1, .y = 1};
TEXT_TYPE(TChar32) character = U'X';
OPSTATUS status = TEXT_GRID_CHAR_FUNC(char32, WriteCell)(&grid, position, character);
```

---

# CharGrid Fill

Fills all cells with one character and/or attribute value.

### Syntax

#### Macro form

```c
OPSTATUS TEXT_GRID_CHAR_FUNC(char32, Fill)(TEXT_GRID_CHAR_TYPE(char32) *grid, TEXT_TYPE(TChar32) character);
```

#### Direct form

```c
OPSTATUS Text_Grid_Char_char32_Fill(Text_Grid_TCharGrid_char32 *grid, Text_TChar32 character);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `grid` | `TEXT_GRID_CHAR_TYPE(char32) *grid` | Pointer to a correctly initialized grid. |
| `character` | `TEXT_TYPE(TChar32) character` | A single character code unit, or pointer output on read; not an entire Unicode text string. |

---

### Return value

`OPSTATUS`: `STATUS_CONST(SUCCESS)` or a recoverable error such as invalid argument, encoding sequence, insufficient capacity, bounds or allocation failure.

---

### Remarks

Sets every stored code unit to the supplied value; no UTF decoding, width measurement or wrapping.

---

### Example

```c
TEXT_TYPE(TChar32) character = U'X';
OPSTATUS status = TEXT_GRID_CHAR_FUNC(char32, Fill)(&grid, character);
```

---

# CharGrid FillRegion

Fills an inclusive rectangle with the requested character.

### Syntax

#### Macro form

```c
OPSTATUS TEXT_GRID_CHAR_FUNC(char32, FillRegion)(TEXT_GRID_CHAR_TYPE(char32) *grid, TQUAD_TYPE(uint16) region, TEXT_TYPE(TChar32) character);
```

#### Direct form

```c
OPSTATUS Text_Grid_Char_char32_FillRegion(Text_Grid_TCharGrid_char32 *grid, Struct_TQuad_uint16 region, Text_TChar32 character);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `grid` | `TEXT_GRID_CHAR_TYPE(char32) *grid` | Pointer to a correctly initialized grid. |
| `region` | `TQUAD_TYPE(uint16) region` | Inclusive rectangle (left/right/top/bottom), Struct TQuad_uint16. |
| `character` | `TEXT_TYPE(TChar32) character` | A single character code unit, or pointer output on read; not an entire Unicode text string. |

---

### Return value

`OPSTATUS`: `STATUS_CONST(SUCCESS)` or a recoverable error such as invalid argument, encoding sequence, insufficient capacity, bounds or allocation failure.

---

### Remarks

The rectangle is inclusive; invalid ordering returns INVALID_ARGUMENT, and out-of-bounds coordinates return OUT_OF_RANGE. No clipping is performed.

---

### Example

```c
TDUAL_TYPE(uint16) position = {.x = 1, .y = 1};
TQUAD_TYPE(uint16) region = {.left = 0, .right = 1, .top = 0, .bottom = 1};
TEXT_TYPE(TChar32) character = U'X';
OPSTATUS status = TEXT_GRID_CHAR_FUNC(char32, FillRegion)(&grid, region, character);
```

---

# CharGrid Read

Extracts an inclusive rectangular region into an independently owned destination grid.

### Syntax

#### Macro form

```c
OPSTATUS TEXT_GRID_CHAR_FUNC(char32, Read)(const TEXT_GRID_CHAR_TYPE(char32) *source, TQUAD_TYPE(uint16) region, TEXT_GRID_CHAR_TYPE(char32) *destination);
```

#### Direct form

```c
OPSTATUS Text_Grid_Char_char32_Read(const Text_Grid_TCharGrid_char32 *source, Struct_TQuad_uint16 region, Text_Grid_TCharGrid_char32 *destination);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `source` | `const TEXT_GRID_CHAR_TYPE(char32) *source` | Read-only source grid with valid storage. |
| `region` | `TQUAD_TYPE(uint16) region` | Inclusive rectangle (left/right/top/bottom), Struct TQuad_uint16. |
| `destination` | `TEXT_GRID_CHAR_TYPE(char32) *destination` | Writable grid object; must be structurally valid, including zero-initialized empty form. |

---

### Return value

`OPSTATUS`: `STATUS_CONST(SUCCESS)` or a recoverable error such as invalid argument, encoding sequence, insufficient capacity, bounds or allocation failure.

---

### Remarks

The selected rectangle is copied to newly allocated temporary storage, then replaces the **valid** destination on success. The destination may be an empty `{0}` grid. No automatic clipping.

---

### Example

```c
TQUAD_TYPE(uint16) region = {.left = 0, .right = 1, .top = 0, .bottom = 1};
/* source and destination: already valid TEXT_GRID_CHAR_TYPE(char32) objects. */
OPSTATUS status = TEXT_GRID_CHAR_FUNC(char32, Read)(&source, region, &destination);
```

---

# CharGrid Write

Overwrites a destination region using all of a source grid's cells.

### Syntax

#### Macro form

```c
OPSTATUS TEXT_GRID_CHAR_FUNC(char32, Write)(TEXT_GRID_CHAR_TYPE(char32) *destination, TDUAL_TYPE(uint16) destinationPosition, const TEXT_GRID_CHAR_TYPE(char32) *source);
```

#### Direct form

```c
OPSTATUS Text_Grid_Char_char32_Write(Text_Grid_TCharGrid_char32 *destination, Struct_TDual_uint16 destinationPosition, const Text_Grid_TCharGrid_char32 *source);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `destination` | `TEXT_GRID_CHAR_TYPE(char32) *destination` | Writable grid object; must be structurally valid, including zero-initialized empty form. |
| `destinationPosition` | `TDUAL_TYPE(uint16) destinationPosition` | Zero-based (x,y) destination top-left corner. |
| `source` | `const TEXT_GRID_CHAR_TYPE(char32) *source` | Read-only source grid with valid storage. |

---

### Return value

`OPSTATUS`: `STATUS_CONST(SUCCESS)` or a recoverable error such as invalid argument, encoding sequence, insufficient capacity, bounds or allocation failure.

---

### Remarks

Copies source cells to destination, replacing their contents and attributes where applicable. Writes use temporary source snapshots to support overlapping/self-copy. Source and destination must already be valid; the write must fit exactly.

---

### Example

```c
TDUAL_TYPE(uint16) position = {.x = 1, .y = 1};
/* source and destination: already valid TEXT_GRID_CHAR_TYPE(char32) objects. */
OPSTATUS status = TEXT_GRID_CHAR_FUNC(char32, Write)(&destination, position, &source);
```

---

# CharGrid WriteRegion

Overwrites a destination region using an inclusive rectangle of the source.

### Syntax

#### Macro form

```c
OPSTATUS TEXT_GRID_CHAR_FUNC(char32, WriteRegion)(TEXT_GRID_CHAR_TYPE(char32) *destination, TDUAL_TYPE(uint16) destinationPosition, const TEXT_GRID_CHAR_TYPE(char32) *source, TQUAD_TYPE(uint16) sourceRegion);
```

#### Direct form

```c
OPSTATUS Text_Grid_Char_char32_WriteRegion(Text_Grid_TCharGrid_char32 *destination, Struct_TDual_uint16 destinationPosition, const Text_Grid_TCharGrid_char32 *source, Struct_TQuad_uint16 sourceRegion);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `destination` | `TEXT_GRID_CHAR_TYPE(char32) *destination` | Writable grid object; must be structurally valid, including zero-initialized empty form. |
| `destinationPosition` | `TDUAL_TYPE(uint16) destinationPosition` | Zero-based (x,y) destination top-left corner. |
| `source` | `const TEXT_GRID_CHAR_TYPE(char32) *source` | Read-only source grid with valid storage. |
| `sourceRegion` | `TQUAD_TYPE(uint16) sourceRegion` | Inclusive source rectangle, Struct TQuad_uint16. |

---

### Return value

`OPSTATUS`: `STATUS_CONST(SUCCESS)` or a recoverable error such as invalid argument, encoding sequence, insufficient capacity, bounds or allocation failure.

---

### Remarks

Copies source cells to destination, replacing their contents and attributes where applicable. Writes use temporary source snapshots to support overlapping/self-copy. Source and destination must already be valid; the write must fit exactly.

---

### Example

```c
TQUAD_TYPE(uint16) region = {.left = 0, .right = 1, .top = 0, .bottom = 1};
TDUAL_TYPE(uint16) position = {.x = 1, .y = 1};
/* source and destination: already valid TEXT_GRID_CHAR_TYPE(char32) objects. */
OPSTATUS status = TEXT_GRID_CHAR_FUNC(char32, WriteRegion)(&destination, position, &source, region);
```

---

# Grid package

Header: `Cosmeron/Modules/Text/Grid/Grid.h`

Three variants of composite character/attribute canvas grids.

The signatures below show the `char32` specialization, using `TEXT_TYPE(TChar32)` characters. Equivalent operations are generated for `char` and `char16`; replace the suffix, its type macro and character C type together.

## Function summary

| Operation | Description |
| --- | --- |
| [`IsEmpty`](#grid-isempty) | Checks whether the object is an initialized, empty grid with zero dimensions and no allocated storage. |
| [`IsValid`](#grid-isvalid) | Checks internal dimensions, data pointers and, for composite grids, agreement of both planes. |
| [`Create`](#grid-create) | Allocates storage for a previously empty grid at a requested column/row size. |
| [`Destroy`](#grid-destroy) | Frees owned grid storage and resets the object to an empty state. |
| [`Resize`](#grid-resize) | Resizes the grid, retaining the overlapping top-left portion and initializing new cells. |
| [`Recreate`](#grid-recreate) | Replaces an existing valid grid with a freshly initialized grid of the requested dimensions. |
| [`Clone`](#grid-clone) | Deep-copies the source grid into a valid destination, replacing its previous storage. |
| [`Clear`](#grid-clear) | Restores grid contents to empty characters or default attributes, without changing dimensions. |
| [`ClearWith`](#grid-clearwith) | Clears using a caller-specified attribute cell. |
| [`ReadCell`](#grid-readcell) | Reads the character or attribute stored at a zero-based cell position. |
| [`WriteCell`](#grid-writecell) | Writes a character or attribute at a zero-based cell position. |
| [`Fill`](#grid-fill) | Fills all cells with one character and/or attribute value. |
| [`FillRegion`](#grid-fillregion) | Fills an inclusive rectangle with the requested character and/or attributes. |
| [`Read`](#grid-read) | Extracts an inclusive rectangular region into an independently owned destination grid. |
| [`Write`](#grid-write) | Overwrites a destination region using all of a source grid's cells. |
| [`WriteRegion`](#grid-writeregion) | Overwrites a destination region using an inclusive rectangle of the source. |
| [`Blit`](#grid-blit) | Composites all cells of a source over a destination, applying attribute transparency/inverse rules. |
| [`BlitRegion`](#grid-blitregion) | Composites a rectangular source region over the destination. |

---

# Grid IsEmpty

Checks whether the object is an initialized, empty grid with zero dimensions and no allocated storage.

### Syntax

#### Macro form

```c
bool TEXT_GRID_FUNC(char32, IsEmpty)(const TEXT_GRID_TYPE(char32) *grid);
```

#### Direct form

```c
bool Text_Grid_char32_IsEmpty(const Text_Grid_TGrid_char32 *grid);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `grid` | `const TEXT_GRID_TYPE(char32) *grid` | Pointer to a correctly initialized grid. |

---

### Return value

`bool`: true if the predicate is satisfied; false otherwise.

---

### Remarks

A zero-initialized `{0}` grid is valid and empty; a nonempty grid is valid only when its storage and dimensions are consistent. A NULL pointer returns false.

---

### Example

```c
bool result = TEXT_GRID_FUNC(char32, IsEmpty)(&grid);
```

---

# Grid IsValid

Checks internal dimensions, data pointers and, for composite grids, agreement of both planes.

### Syntax

#### Macro form

```c
bool TEXT_GRID_FUNC(char32, IsValid)(const TEXT_GRID_TYPE(char32) *grid);
```

#### Direct form

```c
bool Text_Grid_char32_IsValid(const Text_Grid_TGrid_char32 *grid);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `grid` | `const TEXT_GRID_TYPE(char32) *grid` | Pointer to a correctly initialized grid. |

---

### Return value

`bool`: true if the predicate is satisfied; false otherwise.

---

### Remarks

A zero-initialized `{0}` grid is valid and empty; a nonempty grid is valid only when its storage and dimensions are consistent. A NULL pointer returns false.

---

### Example

```c
bool result = TEXT_GRID_FUNC(char32, IsValid)(&grid);
```

---

# Grid Create

Allocates storage for a previously empty grid at a requested column/row size.

### Syntax

#### Macro form

```c
OPSTATUS TEXT_GRID_FUNC(char32, Create)(TEXT_GRID_TYPE(char32) *grid, TDUAL_TYPE(uint16) size);
```

#### Direct form

```c
OPSTATUS Text_Grid_char32_Create(Text_Grid_TGrid_char32 *grid, Struct_TDual_uint16 size);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `grid` | `TEXT_GRID_TYPE(char32) *grid` | Pointer to a correctly initialized grid. |
| `size` | `TDUAL_TYPE(uint16) size` | Length of input in bytes for UTF-8 or 16-bit units for UTF-16; grid size uses Struct_TDual_uint16. |

---

### Return value

`OPSTATUS`: `STATUS_CONST(SUCCESS)` or a recoverable error such as invalid argument, encoding sequence, insufficient capacity, bounds or allocation failure.

---

### Remarks

Requires a zero-initialized or destroyed **empty** destination. Both dimensions may be 0; a 0-by-N or N-by-0 size is rejected. New character cells are zero, new attribute cells use Default().

---

### Example

```c
TEXT_GRID_TYPE(char32) grid = {0};
TDUAL_TYPE(uint16) size = {.col = 4, .row = 3};
OPSTATUS status = TEXT_GRID_FUNC(char32, Create)(&grid, size);
```

---

# Grid Destroy

Frees owned grid storage and resets the object to an empty state.

### Syntax

#### Macro form

```c
OPSTATUS TEXT_GRID_FUNC(char32, Destroy)(TEXT_GRID_TYPE(char32) *grid);
```

#### Direct form

```c
OPSTATUS Text_Grid_char32_Destroy(Text_Grid_TGrid_char32 *grid);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `grid` | `TEXT_GRID_TYPE(char32) *grid` | Pointer to a correctly initialized grid. |

---

### Return value

`OPSTATUS`: `STATUS_CONST(SUCCESS)` or a recoverable error such as invalid argument, encoding sequence, insufficient capacity, bounds or allocation failure.

---

### Remarks

Releases owned buffers. NULL returns INVALID_ARGUMENT; destroying an already empty initialized object succeeds.

---

### Example

```c
OPSTATUS status = TEXT_GRID_FUNC(char32, Destroy)(&grid);
```

---

# Grid Resize

Resizes the grid, retaining the overlapping top-left portion and initializing new cells.

### Syntax

#### Macro form

```c
OPSTATUS TEXT_GRID_FUNC(char32, Resize)(TEXT_GRID_TYPE(char32) *grid, TDUAL_TYPE(uint16) size);
```

#### Direct form

```c
OPSTATUS Text_Grid_char32_Resize(Text_Grid_TGrid_char32 *grid, Struct_TDual_uint16 size);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `grid` | `TEXT_GRID_TYPE(char32) *grid` | Pointer to a correctly initialized grid. |
| `size` | `TDUAL_TYPE(uint16) size` | Length of input in bytes for UTF-8 or 16-bit units for UTF-16; grid size uses Struct_TDual_uint16. |

---

### Return value

`OPSTATUS`: `STATUS_CONST(SUCCESS)` or a recoverable error such as invalid argument, encoding sequence, insufficient capacity, bounds or allocation failure.

---

### Remarks

Allocates a new grid and preserves the overlapping **top-left** area. New cells are zero/default attributes; allocation failures leave the original intact.

---

### Example

```c
TDUAL_TYPE(uint16) size = {.col = 4, .row = 3};
OPSTATUS status = TEXT_GRID_FUNC(char32, Resize)(&grid, size);
```

---

# Grid Recreate

Replaces an existing valid grid with a freshly initialized grid of the requested dimensions.

### Syntax

#### Macro form

```c
OPSTATUS TEXT_GRID_FUNC(char32, Recreate)(TEXT_GRID_TYPE(char32) *grid, TDUAL_TYPE(uint16) size);
```

#### Direct form

```c
OPSTATUS Text_Grid_char32_Recreate(Text_Grid_TGrid_char32 *grid, Struct_TDual_uint16 size);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `grid` | `TEXT_GRID_TYPE(char32) *grid` | Pointer to a correctly initialized grid. |
| `size` | `TDUAL_TYPE(uint16) size` | Length of input in bytes for UTF-8 or 16-bit units for UTF-16; grid size uses Struct_TDual_uint16. |

---

### Return value

`OPSTATUS`: `STATUS_CONST(SUCCESS)` or a recoverable error such as invalid argument, encoding sequence, insufficient capacity, bounds or allocation failure.

---

### Remarks

Allocates fresh storage of the new size, discarding old contents only after successful creation. New cells are zero/default attributes.

---

### Example

```c
TDUAL_TYPE(uint16) size = {.col = 4, .row = 3};
OPSTATUS status = TEXT_GRID_FUNC(char32, Recreate)(&grid, size);
```

---

# Grid Clone

Deep-copies the source grid into a valid destination, replacing its previous storage.

### Syntax

#### Macro form

```c
OPSTATUS TEXT_GRID_FUNC(char32, Clone)(TEXT_GRID_TYPE(char32) *destination, const TEXT_GRID_TYPE(char32) *source);
```

#### Direct form

```c
OPSTATUS Text_Grid_char32_Clone(Text_Grid_TGrid_char32 *destination, const Text_Grid_TGrid_char32 *source);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `destination` | `TEXT_GRID_TYPE(char32) *destination` | Writable grid object; must be structurally valid, including zero-initialized empty form. |
| `source` | `const TEXT_GRID_TYPE(char32) *source` | Read-only source grid with valid storage. |

---

### Return value

`OPSTATUS`: `STATUS_CONST(SUCCESS)` or a recoverable error such as invalid argument, encoding sequence, insufficient capacity, bounds or allocation failure.

---

### Remarks

Allocates a deep copy, replacing destination only on success. Existing source and destination must both be valid (an empty `{0}` destination is valid).

---

### Example

```c
/* source and destination: already valid TEXT_GRID_TYPE(char32) objects. */
OPSTATUS status = TEXT_GRID_FUNC(char32, Clone)(&destination, &source);
```

---

# Grid Clear

Restores grid contents to empty characters or default attributes, without changing dimensions.

### Syntax

#### Macro form

```c
OPSTATUS TEXT_GRID_FUNC(char32, Clear)(TEXT_GRID_TYPE(char32) *grid);
```

#### Direct form

```c
OPSTATUS Text_Grid_char32_Clear(Text_Grid_TGrid_char32 *grid);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `grid` | `TEXT_GRID_TYPE(char32) *grid` | Pointer to a correctly initialized grid. |

---

### Return value

`OPSTATUS`: `STATUS_CONST(SUCCESS)` or a recoverable error such as invalid argument, encoding sequence, insufficient capacity, bounds or allocation failure.

---

### Remarks

Zeros characters and resets attributes to Attribute_Default without changing dimensions.

---

### Example

```c
OPSTATUS status = TEXT_GRID_FUNC(char32, Clear)(&grid);
```

---

# Grid ClearWith

Clears using a caller-specified attribute cell.

### Syntax

#### Macro form

```c
OPSTATUS TEXT_GRID_FUNC(char32, ClearWith)(TEXT_GRID_TYPE(char32) *grid, TEXT_GRID_ATTRIBUTE_TYPE(Cell) attribute);
```

#### Direct form

```c
OPSTATUS Text_Grid_char32_ClearWith(Text_Grid_TGrid_char32 *grid, Text_Grid_Attribute_TCell attribute);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `grid` | `TEXT_GRID_TYPE(char32) *grid` | Pointer to a correctly initialized grid. |
| `attribute` | `TEXT_GRID_ATTRIBUTE_TYPE(Cell) attribute` | Attribute cell value, or writable attribute pointer on ReadCell. |

---

### Return value

`OPSTATUS`: `STATUS_CONST(SUCCESS)` or a recoverable error such as invalid argument, encoding sequence, insufficient capacity, bounds or allocation failure.

---

### Remarks

Clears characters to zero and sets all attribute cells to the provided value.

---

### Example

```c
TEXT_GRID_ATTRIBUTE_TYPE(Cell) cell = TEXT_GRID_ATTRIBUTE_FUNC(Default)();
OPSTATUS status = TEXT_GRID_FUNC(char32, ClearWith)(&grid, cell);
```

---

# Grid ReadCell

Reads the character or attribute stored at a zero-based cell position.

### Syntax

#### Macro form

```c
OPSTATUS TEXT_GRID_FUNC(char32, ReadCell)(const TEXT_GRID_TYPE(char32) *grid, TDUAL_TYPE(uint16) position, TEXT_TYPE(TChar32) *character, TEXT_GRID_ATTRIBUTE_TYPE(Cell) *attribute);
```

#### Direct form

```c
OPSTATUS Text_Grid_char32_ReadCell(const Text_Grid_TGrid_char32 *grid, Struct_TDual_uint16 position, Text_TChar32 *character, Text_Grid_Attribute_TCell *attribute);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `grid` | `const TEXT_GRID_TYPE(char32) *grid` | Pointer to a correctly initialized grid. |
| `position` | `TDUAL_TYPE(uint16) position` | Zero-based (x,y) position, Struct TDual_uint16. |
| `character` | `TEXT_TYPE(TChar32) *character` | A single character code unit, or pointer output on read; not an entire Unicode text string. |
| `attribute` | `TEXT_GRID_ATTRIBUTE_TYPE(Cell) *attribute` | Attribute cell value, or writable attribute pointer on ReadCell. |

---

### Return value

`OPSTATUS`: `STATUS_CONST(SUCCESS)` or a recoverable error such as invalid argument, encoding sequence, insufficient capacity, bounds or allocation failure.

---

### Remarks

Access uses `data[y * columns + x]`; positions must lie inside the allocated dimensions. Out-of-bounds calls return OUT_OF_RANGE. Reads use required typed output pointers.

---

### Example

```c
TDUAL_TYPE(uint16) position = {.x = 1, .y = 1};
TEXT_GRID_ATTRIBUTE_TYPE(Cell) cell = TEXT_GRID_ATTRIBUTE_FUNC(Default)();
TEXT_TYPE(TChar32) character = U'X';
/* Both outputs must have writable storage. */
OPSTATUS status = TEXT_GRID_FUNC(char32, ReadCell)(&grid, position, &character, &cell);
```

---

# Grid WriteCell

Writes a character or attribute at a zero-based cell position.

### Syntax

#### Macro form

```c
OPSTATUS TEXT_GRID_FUNC(char32, WriteCell)(TEXT_GRID_TYPE(char32) *grid, TDUAL_TYPE(uint16) position, TEXT_TYPE(TChar32) character, TEXT_GRID_ATTRIBUTE_TYPE(Cell) attribute);
```

#### Direct form

```c
OPSTATUS Text_Grid_char32_WriteCell(Text_Grid_TGrid_char32 *grid, Struct_TDual_uint16 position, Text_TChar32 character, Text_Grid_Attribute_TCell attribute);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `grid` | `TEXT_GRID_TYPE(char32) *grid` | Pointer to a correctly initialized grid. |
| `position` | `TDUAL_TYPE(uint16) position` | Zero-based (x,y) position, Struct TDual_uint16. |
| `character` | `TEXT_TYPE(TChar32) character` | A single character code unit, or pointer output on read; not an entire Unicode text string. |
| `attribute` | `TEXT_GRID_ATTRIBUTE_TYPE(Cell) attribute` | Attribute cell value, or writable attribute pointer on ReadCell. |

---

### Return value

`OPSTATUS`: `STATUS_CONST(SUCCESS)` or a recoverable error such as invalid argument, encoding sequence, insufficient capacity, bounds or allocation failure.

---

### Remarks

Access uses `data[y * columns + x]`; positions must lie inside the allocated dimensions. Out-of-bounds calls return OUT_OF_RANGE. Reads use required typed output pointers.

---

### Example

```c
TDUAL_TYPE(uint16) position = {.x = 1, .y = 1};
TEXT_GRID_ATTRIBUTE_TYPE(Cell) cell = TEXT_GRID_ATTRIBUTE_FUNC(Default)();
TEXT_TYPE(TChar32) character = U'X';
OPSTATUS status = TEXT_GRID_FUNC(char32, WriteCell)(&grid, position, character, cell);
```

---

# Grid Fill

Fills all cells with one character and/or attribute value.

### Syntax

#### Macro form

```c
OPSTATUS TEXT_GRID_FUNC(char32, Fill)(TEXT_GRID_TYPE(char32) *grid, TEXT_TYPE(TChar32) character, TEXT_GRID_ATTRIBUTE_TYPE(Cell) attribute);
```

#### Direct form

```c
OPSTATUS Text_Grid_char32_Fill(Text_Grid_TGrid_char32 *grid, Text_TChar32 character, Text_Grid_Attribute_TCell attribute);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `grid` | `TEXT_GRID_TYPE(char32) *grid` | Pointer to a correctly initialized grid. |
| `character` | `TEXT_TYPE(TChar32) character` | A single character code unit, or pointer output on read; not an entire Unicode text string. |
| `attribute` | `TEXT_GRID_ATTRIBUTE_TYPE(Cell) attribute` | Attribute cell value, or writable attribute pointer on ReadCell. |

---

### Return value

`OPSTATUS`: `STATUS_CONST(SUCCESS)` or a recoverable error such as invalid argument, encoding sequence, insufficient capacity, bounds or allocation failure.

---

### Remarks

Sets every character and attribute cell to the corresponding arguments.

---

### Example

```c
TEXT_GRID_ATTRIBUTE_TYPE(Cell) cell = TEXT_GRID_ATTRIBUTE_FUNC(Default)();
TEXT_TYPE(TChar32) character = U'X';
OPSTATUS status = TEXT_GRID_FUNC(char32, Fill)(&grid, character, cell);
```

---

# Grid FillRegion

Fills an inclusive rectangle with the requested character and/or attributes.

### Syntax

#### Macro form

```c
OPSTATUS TEXT_GRID_FUNC(char32, FillRegion)(TEXT_GRID_TYPE(char32) *grid, TQUAD_TYPE(uint16) region, TEXT_TYPE(TChar32) character, TEXT_GRID_ATTRIBUTE_TYPE(Cell) attribute);
```

#### Direct form

```c
OPSTATUS Text_Grid_char32_FillRegion(Text_Grid_TGrid_char32 *grid, Struct_TQuad_uint16 region, Text_TChar32 character, Text_Grid_Attribute_TCell attribute);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `grid` | `TEXT_GRID_TYPE(char32) *grid` | Pointer to a correctly initialized grid. |
| `region` | `TQUAD_TYPE(uint16) region` | Inclusive rectangle (left/right/top/bottom), Struct TQuad_uint16. |
| `character` | `TEXT_TYPE(TChar32) character` | A single character code unit, or pointer output on read; not an entire Unicode text string. |
| `attribute` | `TEXT_GRID_ATTRIBUTE_TYPE(Cell) attribute` | Attribute cell value, or writable attribute pointer on ReadCell. |

---

### Return value

`OPSTATUS`: `STATUS_CONST(SUCCESS)` or a recoverable error such as invalid argument, encoding sequence, insufficient capacity, bounds or allocation failure.

---

### Remarks

The rectangle is inclusive; invalid ordering returns INVALID_ARGUMENT, and out-of-bounds coordinates return OUT_OF_RANGE. No clipping is performed.

---

### Example

```c
TDUAL_TYPE(uint16) position = {.x = 1, .y = 1};
TQUAD_TYPE(uint16) region = {.left = 0, .right = 1, .top = 0, .bottom = 1};
TEXT_GRID_ATTRIBUTE_TYPE(Cell) cell = TEXT_GRID_ATTRIBUTE_FUNC(Default)();
TEXT_TYPE(TChar32) character = U'X';
OPSTATUS status = TEXT_GRID_FUNC(char32, FillRegion)(&grid, region, character, cell);
```

---

# Grid Read

Extracts an inclusive rectangular region into an independently owned destination grid.

### Syntax

#### Macro form

```c
OPSTATUS TEXT_GRID_FUNC(char32, Read)(const TEXT_GRID_TYPE(char32) *source, TQUAD_TYPE(uint16) region, TEXT_GRID_TYPE(char32) *destination);
```

#### Direct form

```c
OPSTATUS Text_Grid_char32_Read(const Text_Grid_TGrid_char32 *source, Struct_TQuad_uint16 region, Text_Grid_TGrid_char32 *destination);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `source` | `const TEXT_GRID_TYPE(char32) *source` | Read-only source grid with valid storage. |
| `region` | `TQUAD_TYPE(uint16) region` | Inclusive rectangle (left/right/top/bottom), Struct TQuad_uint16. |
| `destination` | `TEXT_GRID_TYPE(char32) *destination` | Writable grid object; must be structurally valid, including zero-initialized empty form. |

---

### Return value

`OPSTATUS`: `STATUS_CONST(SUCCESS)` or a recoverable error such as invalid argument, encoding sequence, insufficient capacity, bounds or allocation failure.

---

### Remarks

The selected rectangle is copied to newly allocated temporary storage, then replaces the **valid** destination on success. The destination may be an empty `{0}` grid. No automatic clipping.

---

### Example

```c
TQUAD_TYPE(uint16) region = {.left = 0, .right = 1, .top = 0, .bottom = 1};
/* source and destination: already valid TEXT_GRID_TYPE(char32) objects. */
OPSTATUS status = TEXT_GRID_FUNC(char32, Read)(&source, region, &destination);
```

---

# Grid Write

Overwrites a destination region using all of a source grid's cells.

### Syntax

#### Macro form

```c
OPSTATUS TEXT_GRID_FUNC(char32, Write)(TEXT_GRID_TYPE(char32) *destination, TDUAL_TYPE(uint16) destinationPosition, const TEXT_GRID_TYPE(char32) *source);
```

#### Direct form

```c
OPSTATUS Text_Grid_char32_Write(Text_Grid_TGrid_char32 *destination, Struct_TDual_uint16 destinationPosition, const Text_Grid_TGrid_char32 *source);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `destination` | `TEXT_GRID_TYPE(char32) *destination` | Writable grid object; must be structurally valid, including zero-initialized empty form. |
| `destinationPosition` | `TDUAL_TYPE(uint16) destinationPosition` | Zero-based (x,y) destination top-left corner. |
| `source` | `const TEXT_GRID_TYPE(char32) *source` | Read-only source grid with valid storage. |

---

### Return value

`OPSTATUS`: `STATUS_CONST(SUCCESS)` or a recoverable error such as invalid argument, encoding sequence, insufficient capacity, bounds or allocation failure.

---

### Remarks

Copies source cells to destination, replacing their contents and attributes where applicable. Writes use temporary source snapshots to support overlapping/self-copy. Source and destination must already be valid; the write must fit exactly.

---

### Example

```c
TDUAL_TYPE(uint16) position = {.x = 1, .y = 1};
/* source and destination: already valid TEXT_GRID_TYPE(char32) objects. */
OPSTATUS status = TEXT_GRID_FUNC(char32, Write)(&destination, position, &source);
```

---

# Grid WriteRegion

Overwrites a destination region using an inclusive rectangle of the source.

### Syntax

#### Macro form

```c
OPSTATUS TEXT_GRID_FUNC(char32, WriteRegion)(TEXT_GRID_TYPE(char32) *destination, TDUAL_TYPE(uint16) destinationPosition, const TEXT_GRID_TYPE(char32) *source, TQUAD_TYPE(uint16) sourceRegion);
```

#### Direct form

```c
OPSTATUS Text_Grid_char32_WriteRegion(Text_Grid_TGrid_char32 *destination, Struct_TDual_uint16 destinationPosition, const Text_Grid_TGrid_char32 *source, Struct_TQuad_uint16 sourceRegion);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `destination` | `TEXT_GRID_TYPE(char32) *destination` | Writable grid object; must be structurally valid, including zero-initialized empty form. |
| `destinationPosition` | `TDUAL_TYPE(uint16) destinationPosition` | Zero-based (x,y) destination top-left corner. |
| `source` | `const TEXT_GRID_TYPE(char32) *source` | Read-only source grid with valid storage. |
| `sourceRegion` | `TQUAD_TYPE(uint16) sourceRegion` | Inclusive source rectangle, Struct TQuad_uint16. |

---

### Return value

`OPSTATUS`: `STATUS_CONST(SUCCESS)` or a recoverable error such as invalid argument, encoding sequence, insufficient capacity, bounds or allocation failure.

---

### Remarks

Copies source cells to destination, replacing their contents and attributes where applicable. Writes use temporary source snapshots to support overlapping/self-copy. Source and destination must already be valid; the write must fit exactly.

---

### Example

```c
TQUAD_TYPE(uint16) region = {.left = 0, .right = 1, .top = 0, .bottom = 1};
TDUAL_TYPE(uint16) position = {.x = 1, .y = 1};
/* source and destination: already valid TEXT_GRID_TYPE(char32) objects. */
OPSTATUS status = TEXT_GRID_FUNC(char32, WriteRegion)(&destination, position, &source, region);
```

---

# Grid Blit

Composites all cells of a source over a destination, applying attribute transparency/inverse rules.

### Syntax

#### Macro form

```c
OPSTATUS TEXT_GRID_FUNC(char32, Blit)(TEXT_GRID_TYPE(char32) *destination, TDUAL_TYPE(uint16) destinationPosition, const TEXT_GRID_TYPE(char32) *source);
```

#### Direct form

```c
OPSTATUS Text_Grid_char32_Blit(Text_Grid_TGrid_char32 *destination, Struct_TDual_uint16 destinationPosition, const Text_Grid_TGrid_char32 *source);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `destination` | `TEXT_GRID_TYPE(char32) *destination` | Writable grid object; must be structurally valid, including zero-initialized empty form. |
| `destinationPosition` | `TDUAL_TYPE(uint16) destinationPosition` | Zero-based (x,y) destination top-left corner. |
| `source` | `const TEXT_GRID_TYPE(char32) *source` | Read-only source grid with valid storage. |

---

### Return value

`OPSTATUS`: `STATUS_CONST(SUCCESS)` or a recoverable error such as invalid argument, encoding sequence, insufficient capacity, bounds or allocation failure.

---

### Remarks

Unlike Write, this **always copies the source character** while compositing the attribute. INVERSE swaps foreground/background **within each affected attribute**, including its default-color flags, before composition. If source has TRANSPARENT_BACKGROUND, the destination's background/default-background state is preserved; this flag is not transparent-glyph alpha. No clipping.

---

### Example

```c
TDUAL_TYPE(uint16) position = {.x = 1, .y = 1};
/* source and destination: already valid TEXT_GRID_TYPE(char32) objects. */
OPSTATUS status = TEXT_GRID_FUNC(char32, Blit)(&destination, position, &source);
```

---

# Grid BlitRegion

Composites a rectangular source region over the destination.

### Syntax

#### Macro form

```c
OPSTATUS TEXT_GRID_FUNC(char32, BlitRegion)(TEXT_GRID_TYPE(char32) *destination, TDUAL_TYPE(uint16) destinationPosition, const TEXT_GRID_TYPE(char32) *source, TQUAD_TYPE(uint16) sourceRegion);
```

#### Direct form

```c
OPSTATUS Text_Grid_char32_BlitRegion(Text_Grid_TGrid_char32 *destination, Struct_TDual_uint16 destinationPosition, const Text_Grid_TGrid_char32 *source, Struct_TQuad_uint16 sourceRegion);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `destination` | `TEXT_GRID_TYPE(char32) *destination` | Writable grid object; must be structurally valid, including zero-initialized empty form. |
| `destinationPosition` | `TDUAL_TYPE(uint16) destinationPosition` | Zero-based (x,y) destination top-left corner. |
| `source` | `const TEXT_GRID_TYPE(char32) *source` | Read-only source grid with valid storage. |
| `sourceRegion` | `TQUAD_TYPE(uint16) sourceRegion` | Inclusive source rectangle, Struct TQuad_uint16. |

---

### Return value

`OPSTATUS`: `STATUS_CONST(SUCCESS)` or a recoverable error such as invalid argument, encoding sequence, insufficient capacity, bounds or allocation failure.

---

### Remarks

Unlike Write, this **always copies the source character** while compositing the attribute. INVERSE swaps source/destination foreground/background into resolved attributes. If source has TRANSPARENT_BACKGROUND, the destination's background/default-background state is preserved; this flag is not transparent-glyph alpha. No clipping.

---

### Example

```c
TQUAD_TYPE(uint16) region = {.left = 0, .right = 1, .top = 0, .bottom = 1};
TDUAL_TYPE(uint16) position = {.x = 1, .y = 1};
/* source and destination: already valid TEXT_GRID_TYPE(char32) objects. */
OPSTATUS status = TEXT_GRID_FUNC(char32, BlitRegion)(&destination, position, &source, region);
```

---

# Complete examples

## Encode and decode one Unicode scalar

```c
#include "Cosmeron/Modules/Text/Text.h"

int main(void) {
    unsigned char encoded[4] = {0};
    TEXT_TYPE(TChar32) decoded = 0;
    size_t encodedBytes = 0, consumedBytes = 0;

    OPSTATUS status = TEXT_UTF8_FUNC(Encode)(
        (TEXT_TYPE(TChar32))0x1F600U,
        encoded, sizeof encoded, &encodedBytes);
    if (status != STATUS_CONST(SUCCESS))
        return 1;

    status = TEXT_UTF8_FUNC(Decode)(
        encoded, encodedBytes, &decoded, &consumedBytes);
    return status == STATUS_CONST(SUCCESS) &&
           decoded == (TEXT_TYPE(TChar32))0x1F600U &&
           consumedBytes == 4 ? 0 : 2;
}
```

## Draw to a memory-backed character/attribute canvas

```c
#include "Cosmeron/Modules/Text/Text.h"

int main(void) {
    TEXT_GRID_TYPE(char32) canvas = {0};
    TEXT_GRID_ATTRIBUTE_TYPE(Cell) style =
        TEXT_GRID_ATTRIBUTE_FUNC(Default)();
    TDUAL_TYPE(uint16) size = {.col = 20, .row = 6};
    TDUAL_TYPE(uint16) position = {.x = 3, .y = 2};
    TEXT_TYPE(TChar32) actual = 0;
    TEXT_GRID_ATTRIBUTE_TYPE(Cell) actualStyle;

    style.flags = TEXT_GRID_ATTRIBUTE_CONST(BOLD);
    style.foreground = TEXT_GRID_COLOR_RGB(0, 255, 128);
    style.background = TEXT_GRID_COLOR_CONST(WINDOWS_BLACK);

    OPSTATUS status = TEXT_GRID_FUNC(char32, Create)(&canvas, size);
    if (status != STATUS_CONST(SUCCESS))
        return 1;

    status = TEXT_GRID_FUNC(char32, WriteCell)(
        &canvas, position, U'X', style);
    if (status == STATUS_CONST(SUCCESS))
        status = TEXT_GRID_FUNC(char32, ReadCell)(
            &canvas, position, &actual, &actualStyle);

    int ok = status == STATUS_CONST(SUCCESS) &&
             actual == U'X' && actualStyle.foreground == style.foreground;

    TEXT_GRID_FUNC(char32, Destroy)(&canvas);
    /* No stdout/terminal output occurs. A renderer is a separate step. */
    return ok ? 0 : 2;
}
```

## Composite a transparent-background overlay

```c
#include "Cosmeron/Modules/Text/Text.h"

int main(void) {
    TEXT_GRID_TYPE(char32) background = {0};
    TEXT_GRID_TYPE(char32) overlay = {0};
    TDUAL_TYPE(uint16) size = {.col = 2, .row = 1};
    TDUAL_TYPE(uint16) origin = {.x = 0, .y = 0};
    TEXT_GRID_ATTRIBUTE_TYPE(Cell) base =
        TEXT_GRID_ATTRIBUTE_FUNC(Default)();
    TEXT_GRID_ATTRIBUTE_TYPE(Cell) top =
        TEXT_GRID_ATTRIBUTE_FUNC(Default)();

    base.flags = 0;
    base.background = TEXT_GRID_COLOR_RGB(5, 10, 30);
    top.flags = TEXT_GRID_ATTRIBUTE_CONST(TRANSPARENT_BACKGROUND);
    top.foreground = TEXT_GRID_COLOR_RGB(250, 250, 250);

    OPSTATUS status = TEXT_GRID_FUNC(char32, Create)(&background, size);
    if (status != STATUS_CONST(SUCCESS)) return 1;
    status = TEXT_GRID_FUNC(char32, Create)(&overlay, size);
    if (status != STATUS_CONST(SUCCESS)) {
        TEXT_GRID_FUNC(char32, Destroy)(&background);
        return 2;
    }

    status = TEXT_GRID_FUNC(char32, Fill)(&background, U'.', base);
    if (status == STATUS_CONST(SUCCESS))
        status = TEXT_GRID_FUNC(char32, Fill)(&overlay, U'X', top);
    if (status == STATUS_CONST(SUCCESS))
        status = TEXT_GRID_FUNC(char32, Blit)(
            &background, origin, &overlay);

    int ok = status == STATUS_CONST(SUCCESS) &&
             background.chars.data[0] == U'X' &&
             background.attributes.data[0].background == base.background;

    TEXT_GRID_FUNC(char32, Destroy)(&overlay);
    TEXT_GRID_FUNC(char32, Destroy)(&background);
    return ok ? 0 : 3;
}
```

---

# Grid vs. terminal I/O

`TEXT_GRID_FUNC(...)` is a **canvas manipulation interface**, not a terminal session. The current public headers have no `PrintXY`, `CPrintf`, `Update`, `Flush`, `ReadKey`, stdin/stdout redirection, terminal cursor movement, ANSI/Win32 console backend, raycasting or pseudo-3D renderer. Such features are distinct responsibilities and must not be documented as already implemented in Text.

`Blit` composes **attributes** rather than acting as alpha blending for characters. In particular, `TRANSPARENT_BACKGROUND` preserves the destination background but **does not preserve the destination glyph**; the source glyph replaces it. `INVERSE` is resolved before composition, swapping the foreground/background and their default flags.

The code stores a `CODE_UNIT_CONTINUATION` flag, but no high-level grapheme-placement algorithm automatically sets or interprets it. Display-width functions measure scalars or sum scalars in UTF-8, and neither handles a full grapheme cluster composition model.

---

# Build and portability

The Text tests use C11 and cover Unicode validation, UTF-8/UTF-16 round-trips, errors, width classification, color palette, character grid variants, attribute grids, composite grids, region copies, Blit, custom namespace and aggregated header inclusion.

```sh
make -C Codespace/Tests/Text strict
```

That Makefile builds test executables with both GCC and Clang under `-Wall -Wextra -Wpedantic -Werror` and has no explicit user link libraries. The grid implementations use core memory allocation utilities; allocation failure returns `OPSTATUS` instead of silently taking ownership of a failed result.

The module does not currently implement Unicode normalization, grapheme segmentation, bidirectional text layout, complete emoji/ZWJ terminal width handling, a runtime Unicode database or a terminal I/O backend.

---

# Notes

- This reference is derived from **67 header prototype templates**, instantiated as **133 concrete functions** across `char`, `char16`, and `char32` where applicable.
- `TEXT_GRID_COLOR_RGB` and channel extraction operations are **macros**, not separate functions.
- Source grid sizes use `TDUAL_TYPE(uint16)`; rectangular regions use `TQUAD_TYPE(uint16)` from the Struct module.
- The documents describe the implementation on `audio-module`; test sources have been inspected, but the examples have not been separately compiled in this documentation step.
