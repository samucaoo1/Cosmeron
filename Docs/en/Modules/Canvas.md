# Canvas — retained terminal graphics

**Implementation:** `Codespace/Cosmeron/Modules/Graphics/Canvas/Canvas.h`.
`Canvas` is part of Graphics and shares the Terminal namespace, but does not
write to stdout until `Canvas_Update` (or its variants).

## Basic example (namespace macros)

```c
#include "Cosmeron/Modules/Graphics/Canvas/Canvas.h"
int main(void) {
  TERMINAL_TYPE(TCanvas) screen = {0};
  TDUAL_TYPE(uint16) size = {.col=80,.row=24};
  if (TERMINAL_FUNC(Canvas_Create)(&screen,size)!=STATUS_CONST(SUCCESS))
    return 1;
  TERMINAL_FUNC(Canvas_PrintAt)(
    &screen,(TDUAL_TYPE(uint16)){.x=10,.y=2},"HP: %u",100u);
  (void)TERMINAL_FUNC(Canvas_Update)(&screen);
  TERMINAL_FUNC(Canvas_Free)(&screen);
  return 0;
}
```

With the default namespace the same functions are `Terminal_Canvas_Create`,
`Terminal_Canvas_PrintAt`, `Terminal_Canvas_Update`, and `Terminal_Canvas_Free`.

## Operations

| Domain | Operations |
|---|---|
| Lifecycle | `Canvas_Create`, `Canvas_Free`, `Canvas_Resize`, `Canvas_GetSize` |
| Text | `Canvas_Print`, `Canvas_PrintLn`, `Canvas_PrintAt`, `Canvas_PrintStyled`, `Canvas_PrintStyledLn`, `Canvas_PrintStyledAt`, `Canvas_Write`, `Canvas_WriteLine`, `Canvas_PutChar` |
| Drawing | `Canvas_Get`, `Canvas_Set`, `Canvas_Clear`, `Canvas_Fill`, `Canvas_FillRegion` |
| Cursor and style | `Canvas_Cursor_GetPosition`, `Canvas_Cursor_SetPosition`, `Canvas_Cursor_Move`, `Canvas_Style_Set`, `Canvas_Style_Reset`, `Canvas_Attribute_Set` |
| Composition | `Canvas_Copy`, `Canvas_Crop`, `Canvas_View`, `Canvas_Blit`, `Canvas_BlitRegion` |
| Presentation | `Canvas_Update`, `Canvas_UpdateAt`, `Canvas_UpdateTo`, `Canvas_UpdateAtTo` |

## Ownership and views

A `Canvas_Create` result is an owning `Text_Grid_TGrid_char32` surface.
`Canvas_Copy` and `Canvas_Crop` are deep copies. `Canvas_View` is a
borrowed view over the same backing grid, with translated coordinates and
independent cursor/style state. Views of views are supported. `Canvas_Free`
is safe to call on views but does not free the owner.

**Important:** Resizing or freeing the owner invalidates its borrowed views.
Free views or stop using them before the owner is resized or destroyed.
Always zero-initialize a `TCanvas` before first `Create`, `Copy`, `Crop`
or `View`.

The `TQuad_uint16` region uses **inclusive** edges (`left`, `right`,
`top`, `bottom`). Views clip operations to their local extents. Blits
snapshot their source first, supporting self-blit and overlapping views.
Attributes use the existing Text/Grid composition rules, including
transparent backgrounds.

## UTF-8 and presentation

Text is validated with Text/UTF8, characters use Text/Unicode Width, and
double-column glyphs occupy a leading cell and a
`TEXT_GRID_ATTRIBUTE_CONST(CODE_UNIT_CONTINUATION)` cell.
Combining marks are explicitly not supported yet because there is no grapheme
composition layer. Embedded unsupported or malformed sequences fail with
`OPSTATUS`; they do not get silently replaced.

The canvas is always redrawn on a requested `Update`; there is no stale
cached frame hiding a prior direct `Terminal_Print` call.
Presentation requires interactive stdout. A redirected output stream returns
`NOT_AVAILABLE` rather than receiving ANSI escape sequences.
`UpdateAtTo` currently supports the borrowed process stdout terminal;
arbitrary custom backend handles are not implemented.

Cross-platform CI runs this header with GCC, Clang and MSVC, but terminal
appearance and all platform-specific interactive key sequences still require
manual terminal testing.


## Differential presentation

Use `TERMINAL_FUNC(Canvas_UpdateDiff)(current,previous)` or
`TERMINAL_FUNC(Canvas_UpdateDiffAt)(current,previous,position)`
to emit only changed cells. Both arguments must be valid canvases, and
`previous` is a caller-owned snapshot of the previous successful frame.
When sizes differ the implementation falls back to a full redraw.

Unlike the normal full `Update`, diff drawing assumes nothing has
modified the displayed surface outside this API. A direct
`Terminal_Print`, a terminal resize or another application writing to the
same terminal requires a full redraw and refreshing the snapshot.

This design deliberately avoids an invisible persistent renderer cache.

---

# API reference

Canvas functions manage a retained character-and-attribute grid. An owned Canvas allocates Text/Grid; a view borrows the owner's storage and becomes invalid after resize/free. The signatures below follow the public headers. `TERMINAL_FUNC(Name)` resolves to `Terminal_Name` with the default namespace, including the `Canvas_*` family. Usage fragments assume external objects are initialized where necessary.

## Function summary

| Function | Return | Purpose |
| --- | --- | --- |
| `TERMINAL_FUNC(Canvas_Create)` | `OPSTATUS` | Executes Canvas_Create operation. |
| `TERMINAL_FUNC(Canvas_Free)` | `void` | Executes Canvas_Free operation. |
| `TERMINAL_FUNC(Canvas_Resize)` | `OPSTATUS` | Executes Canvas_Resize operation. |
| `TERMINAL_FUNC(Canvas_GetSize)` | `OPSTATUS` | Executes Canvas_GetSize operation. |
| `TERMINAL_FUNC(Canvas_Print)` | `OPSTATUS` | Executes Canvas_Print operation. |
| `TERMINAL_FUNC(Canvas_PrintLn)` | `OPSTATUS` | Executes Canvas_PrintLn operation. |
| `TERMINAL_FUNC(Canvas_PrintAt)` | `OPSTATUS` | Executes Canvas_PrintAt operation. |
| `TERMINAL_FUNC(Canvas_PrintStyled)` | `OPSTATUS` | Executes Canvas_PrintStyled operation. |
| `TERMINAL_FUNC(Canvas_PrintStyledLn)` | `OPSTATUS` | Executes Canvas_PrintStyledLn operation. |
| `TERMINAL_FUNC(Canvas_PrintStyledAt)` | `OPSTATUS` | Executes Canvas_PrintStyledAt operation. |
| `TERMINAL_FUNC(Canvas_Write)` | `OPSTATUS` | Executes Canvas_Write operation. |
| `TERMINAL_FUNC(Canvas_WriteLine)` | `OPSTATUS` | Executes Canvas_WriteLine operation. |
| `TERMINAL_FUNC(Canvas_PutChar)` | `OPSTATUS` | Executes Canvas_PutChar operation. |
| `TERMINAL_FUNC(Canvas_Cursor_GetPosition)` | `OPSTATUS` | Executes Canvas_Cursor_GetPosition operation. |
| `TERMINAL_FUNC(Canvas_Cursor_SetPosition)` | `OPSTATUS` | Executes Canvas_Cursor_SetPosition operation. |
| `TERMINAL_FUNC(Canvas_Cursor_Move)` | `OPSTATUS` | Executes Canvas_Cursor_Move operation. |
| `TERMINAL_FUNC(Canvas_Style_Set)` | `void` | Executes Canvas_Style_Set operation. |
| `TERMINAL_FUNC(Canvas_Style_Reset)` | `void` | Executes Canvas_Style_Reset operation. |
| `TERMINAL_FUNC(Canvas_Attribute_Set)` | `void` | Executes Canvas_Attribute_Set operation. |
| `TERMINAL_FUNC(Canvas_Get)` | `OPSTATUS` | Executes Canvas_Get operation. |
| `TERMINAL_FUNC(Canvas_Set)` | `OPSTATUS` | Executes Canvas_Set operation. |
| `TERMINAL_FUNC(Canvas_Clear)` | `void` | Executes Canvas_Clear operation. |
| `TERMINAL_FUNC(Canvas_Fill)` | `OPSTATUS` | Executes Canvas_Fill operation. |
| `TERMINAL_FUNC(Canvas_FillRegion)` | `OPSTATUS` | Executes Canvas_FillRegion operation. |
| `TERMINAL_FUNC(Canvas_Copy)` | `OPSTATUS` | Executes Canvas_Copy operation. |
| `TERMINAL_FUNC(Canvas_Crop)` | `OPSTATUS` | Executes Canvas_Crop operation. |
| `TERMINAL_FUNC(Canvas_View)` | `OPSTATUS` | Executes Canvas_View operation. |
| `TERMINAL_FUNC(Canvas_Blit)` | `OPSTATUS` | Executes Canvas_Blit operation. |
| `TERMINAL_FUNC(Canvas_BlitRegion)` | `OPSTATUS` | Executes Canvas_BlitRegion operation. |
| `TERMINAL_FUNC(Canvas_Update)` | `OPSTATUS` | Executes Canvas_Update operation. |
| `TERMINAL_FUNC(Canvas_UpdateAt)` | `OPSTATUS` | Executes Canvas_UpdateAt operation. |
| `TERMINAL_FUNC(Canvas_UpdateTo)` | `OPSTATUS` | Executes Canvas_UpdateTo operation. |
| `TERMINAL_FUNC(Canvas_UpdateAtTo)` | `OPSTATUS` | Executes Canvas_UpdateAtTo operation. |
| `TERMINAL_FUNC(Canvas_UpdateDiff)` | `OPSTATUS` | Executes Canvas_UpdateDiff operation. |
| `TERMINAL_FUNC(Canvas_UpdateDiffAt)` | `OPSTATUS` | Executes Canvas_UpdateDiffAt operation. |

# Canvas_Create

Performs Canvas_Create.

### Syntax

#### Macro form

```c
OPSTATUS TERMINAL_FUNC(Canvas_Create)(TERMINAL_TYPE(TCanvas) *outCanvas, TDUAL_TYPE(uint16) size);
```

#### Direct form

```c
OPSTATUS Terminal_Canvas_Create(Terminal_TCanvas *outCanvas, TDual_uint16 size);
```

### Parameters

- `outCanvas`: zero-initialized output Canvas that will own its storage when created/copied.
- `size`: Canvas size or byte count as declared.

### Return value

`OPSTATUS`: SUCCESS if completed; errors include invalid pointers, out-of-range coordinates, unsupported console capabilities and I/O failures as applicable.

### Remarks

Canvas changes are retained in memory until Update. Views borrow their owner's grid; resize/free of owner invalidates borrowed views.

### Example

```c
/* Exemplo isolado: handles e Canvas citados precisam estar inicializados. */
TERMINAL_TYPE(TCanvas) copy = {0};
OPSTATUS status = TERMINAL_FUNC(Canvas_Create)(&copy, (TDUAL_TYPE(uint16)){.x=80,.y=24});
```

# Canvas_Free

Performs Canvas_Free.

### Syntax

#### Macro form

```c
void TERMINAL_FUNC(Canvas_Free)(TERMINAL_TYPE(TCanvas) *canvas);
```

#### Direct form

```c
void Terminal_Canvas_Free(Terminal_TCanvas *canvas);
```

### Parameters

- `canvas`: Canvas initialized/owned or borrowed according to the operation.

### Return value

Does not return a value.

### Remarks

Canvas changes are retained in memory until Update. Views borrow their owner's grid; resize/free of owner invalidates borrowed views.

### Example

```c
/* Exemplo isolado: handles e Canvas citados precisam estar inicializados. */
TERMINAL_FUNC(Canvas_Free)(&canvas);
```

# Canvas_Resize

Performs Canvas_Resize.

### Syntax

#### Macro form

```c
OPSTATUS TERMINAL_FUNC(Canvas_Resize)(TERMINAL_TYPE(TCanvas) *canvas, TDUAL_TYPE(uint16) size);
```

#### Direct form

```c
OPSTATUS Terminal_Canvas_Resize(Terminal_TCanvas *canvas, TDual_uint16 size);
```

### Parameters

- `canvas`: Canvas initialized/owned or borrowed according to the operation.
- `size`: Canvas size or byte count as declared.

### Return value

`OPSTATUS`: SUCCESS if completed; errors include invalid pointers, out-of-range coordinates, unsupported console capabilities and I/O failures as applicable.

### Remarks

Canvas changes are retained in memory until Update. Views borrow their owner's grid; resize/free of owner invalidates borrowed views.

### Example

```c
/* Exemplo isolado: handles e Canvas citados precisam estar inicializados. */
OPSTATUS status = TERMINAL_FUNC(Canvas_Resize)(&canvas, (TDUAL_TYPE(uint16)){.x=80,.y=24});
```

# Canvas_GetSize

Performs Canvas_GetSize.

### Syntax

#### Macro form

```c
OPSTATUS TERMINAL_FUNC(Canvas_GetSize)(const TERMINAL_TYPE(TCanvas) *canvas, TDUAL_TYPE(uint16) *outSize);
```

#### Direct form

```c
OPSTATUS Terminal_Canvas_GetSize(const Terminal_TCanvas *canvas, TDual_uint16 *outSize);
```

### Parameters

- `canvas`: Canvas initialized/owned or borrowed according to the operation.
- `outSize`: output width and height.

### Return value

`OPSTATUS`: SUCCESS if completed; errors include invalid pointers, out-of-range coordinates, unsupported console capabilities and I/O failures as applicable.

### Remarks

Canvas changes are retained in memory until Update. Views borrow their owner's grid; resize/free of owner invalidates borrowed views.

### Example

```c
/* Exemplo isolado: handles e Canvas citados precisam estar inicializados. */
TDUAL_TYPE(uint16) coords = {0};
OPSTATUS status = TERMINAL_FUNC(Canvas_GetSize)(&canvas, &coords);
```

# Canvas_Print

Performs Canvas_Print.

### Syntax

#### Macro form

```c
OPSTATUS TERMINAL_FUNC(Canvas_Print)(TERMINAL_TYPE(TCanvas) *canvas, const char *format, ...);
```

#### Direct form

```c
OPSTATUS Terminal_Canvas_Print(Terminal_TCanvas *canvas, const char *format, ...);
```

### Parameters

- `canvas`: Canvas initialized/owned or borrowed according to the operation.
- `format`: printf-style format string; remaining arguments must match conversions.
- `...`: additional format arguments matching `format`.

### Return value

`OPSTATUS`: SUCCESS if completed; errors include invalid pointers, out-of-range coordinates, unsupported console capabilities and I/O failures as applicable.

### Remarks

Canvas changes are retained in memory until Update. Views borrow their owner's grid; resize/free of owner invalidates borrowed views. This is a C varargs call: format specifiers and argument types must match.

### Example

```c
/* Exemplo isolado: handles e Canvas citados precisam estar inicializados. */
OPSTATUS status = TERMINAL_FUNC(Canvas_Print)(&canvas, "%s", "hello");
```

# Canvas_PrintLn

Performs Canvas_PrintLn.

### Syntax

#### Macro form

```c
OPSTATUS TERMINAL_FUNC(Canvas_PrintLn)(TERMINAL_TYPE(TCanvas) *canvas, const char *format, ...);
```

#### Direct form

```c
OPSTATUS Terminal_Canvas_PrintLn(Terminal_TCanvas *canvas, const char *format, ...);
```

### Parameters

- `canvas`: Canvas initialized/owned or borrowed according to the operation.
- `format`: printf-style format string; remaining arguments must match conversions.
- `...`: additional format arguments matching `format`.

### Return value

`OPSTATUS`: SUCCESS if completed; errors include invalid pointers, out-of-range coordinates, unsupported console capabilities and I/O failures as applicable.

### Remarks

Canvas changes are retained in memory until Update. Views borrow their owner's grid; resize/free of owner invalidates borrowed views. This is a C varargs call: format specifiers and argument types must match.

### Example

```c
/* Exemplo isolado: handles e Canvas citados precisam estar inicializados. */
OPSTATUS status = TERMINAL_FUNC(Canvas_PrintLn)(&canvas, "%s", "hello");
```

# Canvas_PrintAt

Performs Canvas_PrintAt.

### Syntax

#### Macro form

```c
OPSTATUS TERMINAL_FUNC(Canvas_PrintAt)(TERMINAL_TYPE(TCanvas) *canvas, TDUAL_TYPE(uint16) position, const char *format, ...);
```

#### Direct form

```c
OPSTATUS Terminal_Canvas_PrintAt(Terminal_TCanvas *canvas, TDual_uint16 position, const char *format, ...);
```

### Parameters

- `canvas`: Canvas initialized/owned or borrowed according to the operation.
- `position`: cell position in zero-based coordinates.
- `format`: printf-style format string; remaining arguments must match conversions.
- `...`: additional format arguments matching `format`.

### Return value

`OPSTATUS`: SUCCESS if completed; errors include invalid pointers, out-of-range coordinates, unsupported console capabilities and I/O failures as applicable.

### Remarks

Canvas changes are retained in memory until Update. Views borrow their owner's grid; resize/free of owner invalidates borrowed views. This is a C varargs call: format specifiers and argument types must match.

### Example

```c
/* Exemplo isolado: handles e Canvas citados precisam estar inicializados. */
OPSTATUS status = TERMINAL_FUNC(Canvas_PrintAt)(&canvas, (TDUAL_TYPE(uint16)){.x=2,.y=2}, "%s", "hello");
```

# Canvas_PrintStyled

Performs Canvas_PrintStyled.

### Syntax

#### Macro form

```c
OPSTATUS TERMINAL_FUNC(Canvas_PrintStyled)(TERMINAL_TYPE(TCanvas) *canvas, TEXT_GRID_ATTRIBUTE_TYPE(Cell) style, const char *format, ...);
```

#### Direct form

```c
OPSTATUS Terminal_Canvas_PrintStyled(Terminal_TCanvas *canvas, Text_Grid_Attribute_Cell style, const char *format, ...);
```

### Parameters

- `canvas`: Canvas initialized/owned or borrowed according to the operation.
- `style`: Text/Grid cell attribute, not a terminal-global style.
- `format`: printf-style format string; remaining arguments must match conversions.
- `...`: additional format arguments matching `format`.

### Return value

`OPSTATUS`: SUCCESS if completed; errors include invalid pointers, out-of-range coordinates, unsupported console capabilities and I/O failures as applicable.

### Remarks

Canvas changes are retained in memory until Update. Views borrow their owner's grid; resize/free of owner invalidates borrowed views. This is a C varargs call: format specifiers and argument types must match.

### Example

```c
/* Exemplo isolado: handles e Canvas citados precisam estar inicializados. */
OPSTATUS status = TERMINAL_FUNC(Canvas_PrintStyled)(&canvas, TEXT_GRID_ATTRIBUTE_FUNC(Default)(), "%s", "hello");
```

# Canvas_PrintStyledLn

Performs Canvas_PrintStyledLn.

### Syntax

#### Macro form

```c
OPSTATUS TERMINAL_FUNC(Canvas_PrintStyledLn)(TERMINAL_TYPE(TCanvas) *canvas, TEXT_GRID_ATTRIBUTE_TYPE(Cell) style, const char *format, ...);
```

#### Direct form

```c
OPSTATUS Terminal_Canvas_PrintStyledLn(Terminal_TCanvas *canvas, Text_Grid_Attribute_Cell style, const char *format, ...);
```

### Parameters

- `canvas`: Canvas initialized/owned or borrowed according to the operation.
- `style`: Text/Grid cell attribute, not a terminal-global style.
- `format`: printf-style format string; remaining arguments must match conversions.
- `...`: additional format arguments matching `format`.

### Return value

`OPSTATUS`: SUCCESS if completed; errors include invalid pointers, out-of-range coordinates, unsupported console capabilities and I/O failures as applicable.

### Remarks

Canvas changes are retained in memory until Update. Views borrow their owner's grid; resize/free of owner invalidates borrowed views. This is a C varargs call: format specifiers and argument types must match.

### Example

```c
/* Exemplo isolado: handles e Canvas citados precisam estar inicializados. */
OPSTATUS status = TERMINAL_FUNC(Canvas_PrintStyledLn)(&canvas, TEXT_GRID_ATTRIBUTE_FUNC(Default)(), "%s", "hello");
```

# Canvas_PrintStyledAt

Performs Canvas_PrintStyledAt.

### Syntax

#### Macro form

```c
OPSTATUS TERMINAL_FUNC(Canvas_PrintStyledAt)(TERMINAL_TYPE(TCanvas) *canvas, TDUAL_TYPE(uint16) position, TEXT_GRID_ATTRIBUTE_TYPE(Cell) style, const char *format, ...);
```

#### Direct form

```c
OPSTATUS Terminal_Canvas_PrintStyledAt(Terminal_TCanvas *canvas, TDual_uint16 position, Text_Grid_Attribute_Cell style, const char *format, ...);
```

### Parameters

- `canvas`: Canvas initialized/owned or borrowed according to the operation.
- `position`: cell position in zero-based coordinates.
- `style`: Text/Grid cell attribute, not a terminal-global style.
- `format`: printf-style format string; remaining arguments must match conversions.
- `...`: additional format arguments matching `format`.

### Return value

`OPSTATUS`: SUCCESS if completed; errors include invalid pointers, out-of-range coordinates, unsupported console capabilities and I/O failures as applicable.

### Remarks

Canvas changes are retained in memory until Update. Views borrow their owner's grid; resize/free of owner invalidates borrowed views. This is a C varargs call: format specifiers and argument types must match.

### Example

```c
/* Exemplo isolado: handles e Canvas citados precisam estar inicializados. */
OPSTATUS status = TERMINAL_FUNC(Canvas_PrintStyledAt)(&canvas, (TDUAL_TYPE(uint16)){.x=2,.y=2}, TEXT_GRID_ATTRIBUTE_FUNC(Default)(), "%s", "hello");
```

# Canvas_Write

Performs Canvas_Write.

### Syntax

#### Macro form

```c
OPSTATUS TERMINAL_FUNC(Canvas_Write)(TERMINAL_TYPE(TCanvas) *canvas, const char *utf8Text, size_t byteLength);
```

#### Direct form

```c
OPSTATUS Terminal_Canvas_Write(Terminal_TCanvas *canvas, const char *utf8Text, size_t byteLength);
```

### Parameters

- `canvas`: Canvas initialized/owned or borrowed according to the operation.
- `utf8Text`: UTF-8 source bytes.
- `byteLength`: UTF-8 byte length.

### Return value

`OPSTATUS`: SUCCESS if completed; errors include invalid pointers, out-of-range coordinates, unsupported console capabilities and I/O failures as applicable.

### Remarks

Canvas changes are retained in memory until Update. Views borrow their owner's grid; resize/free of owner invalidates borrowed views.

### Example

```c
/* Exemplo isolado: handles e Canvas citados precisam estar inicializados. */
OPSTATUS status = TERMINAL_FUNC(Canvas_Write)(&canvas, "hello", 5);
```

# Canvas_WriteLine

Performs Canvas_WriteLine.

### Syntax

#### Macro form

```c
OPSTATUS TERMINAL_FUNC(Canvas_WriteLine)(TERMINAL_TYPE(TCanvas) *canvas, const char *text);
```

#### Direct form

```c
OPSTATUS Terminal_Canvas_WriteLine(Terminal_TCanvas *canvas, const char *text);
```

### Parameters

- `canvas`: Canvas initialized/owned or borrowed according to the operation.
- `text`: null-terminated input text.

### Return value

`OPSTATUS`: SUCCESS if completed; errors include invalid pointers, out-of-range coordinates, unsupported console capabilities and I/O failures as applicable.

### Remarks

Canvas changes are retained in memory until Update. Views borrow their owner's grid; resize/free of owner invalidates borrowed views.

### Example

```c
/* Exemplo isolado: handles e Canvas citados precisam estar inicializados. */
OPSTATUS status = TERMINAL_FUNC(Canvas_WriteLine)(&canvas, "hello");
```

# Canvas_PutChar

Performs Canvas_PutChar.

### Syntax

#### Macro form

```c
OPSTATUS TERMINAL_FUNC(Canvas_PutChar)(TERMINAL_TYPE(TCanvas) *canvas, uint32_t codepoint);
```

#### Direct form

```c
OPSTATUS Terminal_Canvas_PutChar(Terminal_TCanvas *canvas, uint32_t codepoint);
```

### Parameters

- `canvas`: Canvas initialized/owned or borrowed according to the operation.
- `codepoint`: one Unicode scalar value.

### Return value

`OPSTATUS`: SUCCESS if completed; errors include invalid pointers, out-of-range coordinates, unsupported console capabilities and I/O failures as applicable.

### Remarks

Canvas changes are retained in memory until Update. Views borrow their owner's grid; resize/free of owner invalidates borrowed views.

### Example

```c
/* Exemplo isolado: handles e Canvas citados precisam estar inicializados. */
OPSTATUS status = TERMINAL_FUNC(Canvas_PutChar)(&canvas, '#');
```

# Canvas_Cursor_GetPosition

Performs Canvas_Cursor_GetPosition.

### Syntax

#### Macro form

```c
OPSTATUS TERMINAL_FUNC(Canvas_Cursor_GetPosition)(const TERMINAL_TYPE(TCanvas) *canvas, TDUAL_TYPE(uint16) *outPosition);
```

#### Direct form

```c
OPSTATUS Terminal_Canvas_Cursor_GetPosition(const Terminal_TCanvas *canvas, TDual_uint16 *outPosition);
```

### Parameters

- `canvas`: Canvas initialized/owned or borrowed according to the operation.
- `outPosition`: receives cursor position.

### Return value

`OPSTATUS`: SUCCESS if completed; errors include invalid pointers, out-of-range coordinates, unsupported console capabilities and I/O failures as applicable.

### Remarks

Canvas changes are retained in memory until Update. Views borrow their owner's grid; resize/free of owner invalidates borrowed views.

### Example

```c
/* Exemplo isolado: handles e Canvas citados precisam estar inicializados. */
TDUAL_TYPE(uint16) coords = {0};
OPSTATUS status = TERMINAL_FUNC(Canvas_Cursor_GetPosition)(&canvas, &coords);
```

# Canvas_Cursor_SetPosition

Performs Canvas_Cursor_SetPosition.

### Syntax

#### Macro form

```c
OPSTATUS TERMINAL_FUNC(Canvas_Cursor_SetPosition)(TERMINAL_TYPE(TCanvas) *canvas, TDUAL_TYPE(uint16) position);
```

#### Direct form

```c
OPSTATUS Terminal_Canvas_Cursor_SetPosition(Terminal_TCanvas *canvas, TDual_uint16 position);
```

### Parameters

- `canvas`: Canvas initialized/owned or borrowed according to the operation.
- `position`: cell position in zero-based coordinates.

### Return value

`OPSTATUS`: SUCCESS if completed; errors include invalid pointers, out-of-range coordinates, unsupported console capabilities and I/O failures as applicable.

### Remarks

Canvas changes are retained in memory until Update. Views borrow their owner's grid; resize/free of owner invalidates borrowed views.

### Example

```c
/* Exemplo isolado: handles e Canvas citados precisam estar inicializados. */
OPSTATUS status = TERMINAL_FUNC(Canvas_Cursor_SetPosition)(&canvas, (TDUAL_TYPE(uint16)){.x=2,.y=2});
```

# Canvas_Cursor_Move

Performs Canvas_Cursor_Move.

### Syntax

#### Macro form

```c
OPSTATUS TERMINAL_FUNC(Canvas_Cursor_Move)(TERMINAL_TYPE(TCanvas) *canvas, int32_t deltaX, int32_t deltaY);
```

#### Direct form

```c
OPSTATUS Terminal_Canvas_Cursor_Move(Terminal_TCanvas *canvas, int32_t deltaX, int32_t deltaY);
```

### Parameters

- `canvas`: Canvas initialized/owned or borrowed according to the operation.
- `deltaX`: signed cursor displacement horizontally.
- `deltaY`: signed cursor displacement vertically.

### Return value

`OPSTATUS`: SUCCESS if completed; errors include invalid pointers, out-of-range coordinates, unsupported console capabilities and I/O failures as applicable.

### Remarks

Canvas changes are retained in memory until Update. Views borrow their owner's grid; resize/free of owner invalidates borrowed views.

### Example

```c
/* Exemplo isolado: handles e Canvas citados precisam estar inicializados. */
OPSTATUS status = TERMINAL_FUNC(Canvas_Cursor_Move)(&canvas, 1, 0);
```

# Canvas_Style_Set

Performs Canvas_Style_Set.

### Syntax

#### Macro form

```c
void TERMINAL_FUNC(Canvas_Style_Set)(TERMINAL_TYPE(TCanvas) *canvas, TEXT_GRID_ATTRIBUTE_TYPE(Cell) style);
```

#### Direct form

```c
void Terminal_Canvas_Style_Set(Terminal_TCanvas *canvas, Text_Grid_Attribute_Cell style);
```

### Parameters

- `canvas`: Canvas initialized/owned or borrowed according to the operation.
- `style`: Text/Grid cell attribute, not a terminal-global style.

### Return value

Does not return a value.

### Remarks

Canvas changes are retained in memory until Update. Views borrow their owner's grid; resize/free of owner invalidates borrowed views.

### Example

```c
/* Exemplo isolado: handles e Canvas citados precisam estar inicializados. */
TERMINAL_FUNC(Canvas_Style_Set)(&canvas, TEXT_GRID_ATTRIBUTE_FUNC(Default)());
```

# Canvas_Style_Reset

Performs Canvas_Style_Reset.

### Syntax

#### Macro form

```c
void TERMINAL_FUNC(Canvas_Style_Reset)(TERMINAL_TYPE(TCanvas) *canvas);
```

#### Direct form

```c
void Terminal_Canvas_Style_Reset(Terminal_TCanvas *canvas);
```

### Parameters

- `canvas`: Canvas initialized/owned or borrowed according to the operation.

### Return value

Does not return a value.

### Remarks

Canvas changes are retained in memory until Update. Views borrow their owner's grid; resize/free of owner invalidates borrowed views.

### Example

```c
/* Exemplo isolado: handles e Canvas citados precisam estar inicializados. */
TERMINAL_FUNC(Canvas_Style_Reset)(&canvas);
```

# Canvas_Attribute_Set

Performs Canvas_Attribute_Set.

### Syntax

#### Macro form

```c
void TERMINAL_FUNC(Canvas_Attribute_Set)(TERMINAL_TYPE(TCanvas) *canvas, TEXT_GRID_ATTRIBUTE_TYPE(Flags) flags);
```

#### Direct form

```c
void Terminal_Canvas_Attribute_Set(Terminal_TCanvas *canvas, Text_Grid_Attribute_Flags flags);
```

### Parameters

- `canvas`: Canvas initialized/owned or borrowed according to the operation.
- `flags`: attribute flags.

### Return value

Does not return a value.

### Remarks

Canvas changes are retained in memory until Update. Views borrow their owner's grid; resize/free of owner invalidates borrowed views.

### Example

```c
/* Exemplo isolado: handles e Canvas citados precisam estar inicializados. */
TERMINAL_FUNC(Canvas_Attribute_Set)(&canvas, flags);
```

# Canvas_Get

Performs Canvas_Get.

### Syntax

#### Macro form

```c
OPSTATUS TERMINAL_FUNC(Canvas_Get)(const TERMINAL_TYPE(TCanvas) *canvas, TDUAL_TYPE(uint16) position, uint32_t *outCodepoint, TEXT_GRID_ATTRIBUTE_TYPE(Cell) *outAttribute);
```

#### Direct form

```c
OPSTATUS Terminal_Canvas_Get(const Terminal_TCanvas *canvas, TDual_uint16 position, uint32_t *outCodepoint, Text_Grid_Attribute_Cell *outAttribute);
```

### Parameters

- `canvas`: Canvas initialized/owned or borrowed according to the operation.
- `position`: cell position in zero-based coordinates.
- `outCodepoint`: receives Unicode codepoint read.
- `outAttribute`: receives cell attributes.

### Return value

`OPSTATUS`: SUCCESS if completed; errors include invalid pointers, out-of-range coordinates, unsupported console capabilities and I/O failures as applicable.

### Remarks

Canvas changes are retained in memory until Update. Views borrow their owner's grid; resize/free of owner invalidates borrowed views.

### Example

```c
/* Exemplo isolado: handles e Canvas citados precisam estar inicializados. */
uint32_t codepoint = 0;
TEXT_GRID_ATTRIBUTE_TYPE(Cell) attribute = {0};
OPSTATUS status = TERMINAL_FUNC(Canvas_Get)(&canvas, (TDUAL_TYPE(uint16)){.x=2,.y=2}, &codepoint, &attribute);
```

# Canvas_Set

Performs Canvas_Set.

### Syntax

#### Macro form

```c
OPSTATUS TERMINAL_FUNC(Canvas_Set)(TERMINAL_TYPE(TCanvas) *canvas, TDUAL_TYPE(uint16) position, uint32_t codepoint, TEXT_GRID_ATTRIBUTE_TYPE(Cell) attribute);
```

#### Direct form

```c
OPSTATUS Terminal_Canvas_Set(Terminal_TCanvas *canvas, TDual_uint16 position, uint32_t codepoint, Text_Grid_Attribute_Cell attribute);
```

### Parameters

- `canvas`: Canvas initialized/owned or borrowed according to the operation.
- `position`: cell position in zero-based coordinates.
- `codepoint`: one Unicode scalar value.
- `attribute`: Text/Grid cell attribute to store.

### Return value

`OPSTATUS`: SUCCESS if completed; errors include invalid pointers, out-of-range coordinates, unsupported console capabilities and I/O failures as applicable.

### Remarks

Canvas changes are retained in memory until Update. Views borrow their owner's grid; resize/free of owner invalidates borrowed views.

### Example

```c
/* Exemplo isolado: handles e Canvas citados precisam estar inicializados. */
OPSTATUS status = TERMINAL_FUNC(Canvas_Set)(&canvas, (TDUAL_TYPE(uint16)){.x=2,.y=2}, '#', TEXT_GRID_ATTRIBUTE_FUNC(Default)());
```

# Canvas_Clear

Performs Canvas_Clear.

### Syntax

#### Macro form

```c
void TERMINAL_FUNC(Canvas_Clear)(TERMINAL_TYPE(TCanvas) *canvas);
```

#### Direct form

```c
void Terminal_Canvas_Clear(Terminal_TCanvas *canvas);
```

### Parameters

- `canvas`: Canvas initialized/owned or borrowed according to the operation.

### Return value

Does not return a value.

### Remarks

Canvas changes are retained in memory until Update. Views borrow their owner's grid; resize/free of owner invalidates borrowed views.

### Example

```c
/* Exemplo isolado: handles e Canvas citados precisam estar inicializados. */
TERMINAL_FUNC(Canvas_Clear)(&canvas);
```

# Canvas_Fill

Performs Canvas_Fill.

### Syntax

#### Macro form

```c
OPSTATUS TERMINAL_FUNC(Canvas_Fill)(TERMINAL_TYPE(TCanvas) *canvas, uint32_t codepoint, TEXT_GRID_ATTRIBUTE_TYPE(Cell) attribute);
```

#### Direct form

```c
OPSTATUS Terminal_Canvas_Fill(Terminal_TCanvas *canvas, uint32_t codepoint, Text_Grid_Attribute_Cell attribute);
```

### Parameters

- `canvas`: Canvas initialized/owned or borrowed according to the operation.
- `codepoint`: one Unicode scalar value.
- `attribute`: Text/Grid cell attribute to store.

### Return value

`OPSTATUS`: SUCCESS if completed; errors include invalid pointers, out-of-range coordinates, unsupported console capabilities and I/O failures as applicable.

### Remarks

Canvas changes are retained in memory until Update. Views borrow their owner's grid; resize/free of owner invalidates borrowed views.

### Example

```c
/* Exemplo isolado: handles e Canvas citados precisam estar inicializados. */
OPSTATUS status = TERMINAL_FUNC(Canvas_Fill)(&canvas, '#', TEXT_GRID_ATTRIBUTE_FUNC(Default)());
```

# Canvas_FillRegion

Performs Canvas_FillRegion.

### Syntax

#### Macro form

```c
OPSTATUS TERMINAL_FUNC(Canvas_FillRegion)(TERMINAL_TYPE(TCanvas) *canvas, TQUAD_TYPE(uint16) region, uint32_t codepoint, TEXT_GRID_ATTRIBUTE_TYPE(Cell) attribute);
```

#### Direct form

```c
OPSTATUS Terminal_Canvas_FillRegion(Terminal_TCanvas *canvas, TQuad_uint16 region, uint32_t codepoint, Text_Grid_Attribute_Cell attribute);
```

### Parameters

- `canvas`: Canvas initialized/owned or borrowed according to the operation.
- `region`: Canvas-relative rectangular region.
- `codepoint`: one Unicode scalar value.
- `attribute`: Text/Grid cell attribute to store.

### Return value

`OPSTATUS`: SUCCESS if completed; errors include invalid pointers, out-of-range coordinates, unsupported console capabilities and I/O failures as applicable.

### Remarks

Canvas changes are retained in memory until Update. Views borrow their owner's grid; resize/free of owner invalidates borrowed views.

### Example

```c
/* Exemplo isolado: handles e Canvas citados precisam estar inicializados. */
OPSTATUS status = TERMINAL_FUNC(Canvas_FillRegion)(&canvas, region, '#', TEXT_GRID_ATTRIBUTE_FUNC(Default)());
```

# Canvas_Copy

Performs Canvas_Copy.

### Syntax

#### Macro form

```c
OPSTATUS TERMINAL_FUNC(Canvas_Copy)(const TERMINAL_TYPE(TCanvas) *source, TERMINAL_TYPE(TCanvas) *outCanvas);
```

#### Direct form

```c
OPSTATUS Terminal_Canvas_Copy(const Terminal_TCanvas *source, Terminal_TCanvas *outCanvas);
```

### Parameters

- `source`: borrowed source Canvas.
- `outCanvas`: zero-initialized output Canvas that will own its storage when created/copied.

### Return value

`OPSTATUS`: SUCCESS if completed; errors include invalid pointers, out-of-range coordinates, unsupported console capabilities and I/O failures as applicable.

### Remarks

Canvas changes are retained in memory until Update. Views borrow their owner's grid; resize/free of owner invalidates borrowed views.

### Example

```c
/* Exemplo isolado: handles e Canvas citados precisam estar inicializados. */
TERMINAL_TYPE(TCanvas) copy = {0};
OPSTATUS status = TERMINAL_FUNC(Canvas_Copy)(&source, &copy);
```

# Canvas_Crop

Performs Canvas_Crop.

### Syntax

#### Macro form

```c
OPSTATUS TERMINAL_FUNC(Canvas_Crop)(const TERMINAL_TYPE(TCanvas) *source, TQUAD_TYPE(uint16) region, TERMINAL_TYPE(TCanvas) *outCanvas);
```

#### Direct form

```c
OPSTATUS Terminal_Canvas_Crop(const Terminal_TCanvas *source, TQuad_uint16 region, Terminal_TCanvas *outCanvas);
```

### Parameters

- `source`: borrowed source Canvas.
- `region`: Canvas-relative rectangular region.
- `outCanvas`: zero-initialized output Canvas that will own its storage when created/copied.

### Return value

`OPSTATUS`: SUCCESS if completed; errors include invalid pointers, out-of-range coordinates, unsupported console capabilities and I/O failures as applicable.

### Remarks

Canvas changes are retained in memory until Update. Views borrow their owner's grid; resize/free of owner invalidates borrowed views.

### Example

```c
/* Exemplo isolado: handles e Canvas citados precisam estar inicializados. */
TERMINAL_TYPE(TCanvas) copy = {0};
OPSTATUS status = TERMINAL_FUNC(Canvas_Crop)(&source, region, &copy);
```

# Canvas_View

Performs Canvas_View.

### Syntax

#### Macro form

```c
OPSTATUS TERMINAL_FUNC(Canvas_View)(TERMINAL_TYPE(TCanvas) *source, TQUAD_TYPE(uint16) region, TERMINAL_TYPE(TCanvas) *outView);
```

#### Direct form

```c
OPSTATUS Terminal_Canvas_View(Terminal_TCanvas *source, TQuad_uint16 region, Terminal_TCanvas *outView);
```

### Parameters

- `source`: borrowed source Canvas.
- `region`: Canvas-relative rectangular region.
- `outView`: zero-initialized borrowed view output.

### Return value

`OPSTATUS`: SUCCESS if completed; errors include invalid pointers, out-of-range coordinates, unsupported console capabilities and I/O failures as applicable.

### Remarks

Canvas changes are retained in memory until Update. Views borrow their owner's grid; resize/free of owner invalidates borrowed views.

### Example

```c
/* Exemplo isolado: handles e Canvas citados precisam estar inicializados. */
TERMINAL_TYPE(TCanvas) view = {0};
OPSTATUS status = TERMINAL_FUNC(Canvas_View)(&source, region, &view);
```

# Canvas_Blit

Performs Canvas_Blit.

### Syntax

#### Macro form

```c
OPSTATUS TERMINAL_FUNC(Canvas_Blit)(TERMINAL_TYPE(TCanvas) *destination, TDUAL_TYPE(uint16) position, const TERMINAL_TYPE(TCanvas) *source);
```

#### Direct form

```c
OPSTATUS Terminal_Canvas_Blit(Terminal_TCanvas *destination, TDual_uint16 position, const Terminal_TCanvas *source);
```

### Parameters

- `destination`: destination Canvas for composition.
- `position`: cell position in zero-based coordinates.
- `source`: borrowed source Canvas.

### Return value

`OPSTATUS`: SUCCESS if completed; errors include invalid pointers, out-of-range coordinates, unsupported console capabilities and I/O failures as applicable.

### Remarks

Canvas changes are retained in memory until Update. Views borrow their owner's grid; resize/free of owner invalidates borrowed views.

### Example

```c
/* Exemplo isolado: handles e Canvas citados precisam estar inicializados. */
OPSTATUS status = TERMINAL_FUNC(Canvas_Blit)(&destination, (TDUAL_TYPE(uint16)){.x=2,.y=2}, &source);
```

# Canvas_BlitRegion

Performs Canvas_BlitRegion.

### Syntax

#### Macro form

```c
OPSTATUS TERMINAL_FUNC(Canvas_BlitRegion)(TERMINAL_TYPE(TCanvas) *destination, TDUAL_TYPE(uint16) position, const TERMINAL_TYPE(TCanvas) *source, TQUAD_TYPE(uint16) region);
```

#### Direct form

```c
OPSTATUS Terminal_Canvas_BlitRegion(Terminal_TCanvas *destination, TDual_uint16 position, const Terminal_TCanvas *source, TQuad_uint16 region);
```

### Parameters

- `destination`: destination Canvas for composition.
- `position`: cell position in zero-based coordinates.
- `source`: borrowed source Canvas.
- `region`: Canvas-relative rectangular region.

### Return value

`OPSTATUS`: SUCCESS if completed; errors include invalid pointers, out-of-range coordinates, unsupported console capabilities and I/O failures as applicable.

### Remarks

Canvas changes are retained in memory until Update. Views borrow their owner's grid; resize/free of owner invalidates borrowed views.

### Example

```c
/* Exemplo isolado: handles e Canvas citados precisam estar inicializados. */
OPSTATUS status = TERMINAL_FUNC(Canvas_BlitRegion)(&destination, (TDUAL_TYPE(uint16)){.x=2,.y=2}, &source, region);
```

# Canvas_Update

Performs Canvas_Update.

### Syntax

#### Macro form

```c
OPSTATUS TERMINAL_FUNC(Canvas_Update)(const TERMINAL_TYPE(TCanvas) *canvas);
```

#### Direct form

```c
OPSTATUS Terminal_Canvas_Update(const Terminal_TCanvas *canvas);
```

### Parameters

- `canvas`: Canvas initialized/owned or borrowed according to the operation.

### Return value

`OPSTATUS`: SUCCESS if completed; errors include invalid pointers, out-of-range coordinates, unsupported console capabilities and I/O failures as applicable.

### Remarks

Canvas changes are retained in memory until Update. Views borrow their owner's grid; resize/free of owner invalidates borrowed views.

### Example

```c
/* Exemplo isolado: handles e Canvas citados precisam estar inicializados. */
OPSTATUS status = TERMINAL_FUNC(Canvas_Update)(&canvas);
```

# Canvas_UpdateAt

Performs Canvas_UpdateAt.

### Syntax

#### Macro form

```c
OPSTATUS TERMINAL_FUNC(Canvas_UpdateAt)(const TERMINAL_TYPE(TCanvas) *canvas, TDUAL_TYPE(uint16) position);
```

#### Direct form

```c
OPSTATUS Terminal_Canvas_UpdateAt(const Terminal_TCanvas *canvas, TDual_uint16 position);
```

### Parameters

- `canvas`: Canvas initialized/owned or borrowed according to the operation.
- `position`: cell position in zero-based coordinates.

### Return value

`OPSTATUS`: SUCCESS if completed; errors include invalid pointers, out-of-range coordinates, unsupported console capabilities and I/O failures as applicable.

### Remarks

Canvas changes are retained in memory until Update. Views borrow their owner's grid; resize/free of owner invalidates borrowed views.

### Example

```c
/* Exemplo isolado: handles e Canvas citados precisam estar inicializados. */
OPSTATUS status = TERMINAL_FUNC(Canvas_UpdateAt)(&canvas, (TDUAL_TYPE(uint16)){.x=2,.y=2});
```

# Canvas_UpdateTo

Performs Canvas_UpdateTo.

### Syntax

#### Macro form

```c
OPSTATUS TERMINAL_FUNC(Canvas_UpdateTo)(const TERMINAL_TYPE(TCanvas) *canvas, TERMINAL_TYPE(TTerminal) *terminal);
```

#### Direct form

```c
OPSTATUS Terminal_Canvas_UpdateTo(const Terminal_TCanvas *canvas, Terminal_TTerminal *terminal);
```

### Parameters

- `canvas`: Canvas initialized/owned or borrowed according to the operation.
- `terminal`: target terminal handle for explicit presentation.

### Return value

`OPSTATUS`: SUCCESS if completed; errors include invalid pointers, out-of-range coordinates, unsupported console capabilities and I/O failures as applicable.

### Remarks

Canvas changes are retained in memory until Update. Views borrow their owner's grid; resize/free of owner invalidates borrowed views.

### Example

```c
/* Exemplo isolado: handles e Canvas citados precisam estar inicializados. */
OPSTATUS status = TERMINAL_FUNC(Canvas_UpdateTo)(&canvas, terminal);
```

# Canvas_UpdateAtTo

Performs Canvas_UpdateAtTo.

### Syntax

#### Macro form

```c
OPSTATUS TERMINAL_FUNC(Canvas_UpdateAtTo)(const TERMINAL_TYPE(TCanvas) *canvas, TERMINAL_TYPE(TTerminal) *terminal, TDUAL_TYPE(uint16) position);
```

#### Direct form

```c
OPSTATUS Terminal_Canvas_UpdateAtTo(const Terminal_TCanvas *canvas, Terminal_TTerminal *terminal, TDual_uint16 position);
```

### Parameters

- `canvas`: Canvas initialized/owned or borrowed according to the operation.
- `terminal`: target terminal handle for explicit presentation.
- `position`: cell position in zero-based coordinates.

### Return value

`OPSTATUS`: SUCCESS if completed; errors include invalid pointers, out-of-range coordinates, unsupported console capabilities and I/O failures as applicable.

### Remarks

Canvas changes are retained in memory until Update. Views borrow their owner's grid; resize/free of owner invalidates borrowed views.

### Example

```c
/* Exemplo isolado: handles e Canvas citados precisam estar inicializados. */
OPSTATUS status = TERMINAL_FUNC(Canvas_UpdateAtTo)(&canvas, terminal, (TDUAL_TYPE(uint16)){.x=2,.y=2});
```

# Canvas_UpdateDiff

Performs Canvas_UpdateDiff.

### Syntax

#### Macro form

```c
OPSTATUS TERMINAL_FUNC(Canvas_UpdateDiff)(const TERMINAL_TYPE(TCanvas) *canvas, const TERMINAL_TYPE(TCanvas) *previous);
```

#### Direct form

```c
OPSTATUS Terminal_Canvas_UpdateDiff(const Terminal_TCanvas *canvas, const Terminal_TCanvas *previous);
```

### Parameters

- `canvas`: Canvas initialized/owned or borrowed according to the operation.
- `previous`: previous Canvas frame for differential presentation.

### Return value

`OPSTATUS`: SUCCESS if completed; errors include invalid pointers, out-of-range coordinates, unsupported console capabilities and I/O failures as applicable.

### Remarks

Canvas changes are retained in memory until Update. Views borrow their owner's grid; resize/free of owner invalidates borrowed views. Differential presentation compares logical cell content and does not update the previous snapshot automatically.

### Example

```c
/* Exemplo isolado: handles e Canvas citados precisam estar inicializados. */
OPSTATUS status = TERMINAL_FUNC(Canvas_UpdateDiff)(&canvas, &previous);
```

# Canvas_UpdateDiffAt

Performs Canvas_UpdateDiffAt.

### Syntax

#### Macro form

```c
OPSTATUS TERMINAL_FUNC(Canvas_UpdateDiffAt)(const TERMINAL_TYPE(TCanvas) *canvas, const TERMINAL_TYPE(TCanvas) *previous, TDUAL_TYPE(uint16) position);
```

#### Direct form

```c
OPSTATUS Terminal_Canvas_UpdateDiffAt(const Terminal_TCanvas *canvas, const Terminal_TCanvas *previous, TDual_uint16 position);
```

### Parameters

- `canvas`: Canvas initialized/owned or borrowed according to the operation.
- `previous`: previous Canvas frame for differential presentation.
- `position`: cell position in zero-based coordinates.

### Return value

`OPSTATUS`: SUCCESS if completed; errors include invalid pointers, out-of-range coordinates, unsupported console capabilities and I/O failures as applicable.

### Remarks

Canvas changes are retained in memory until Update. Views borrow their owner's grid; resize/free of owner invalidates borrowed views. Differential presentation compares logical cell content and does not update the previous snapshot automatically.

### Example

```c
/* Exemplo isolado: handles e Canvas citados precisam estar inicializados. */
OPSTATUS status = TERMINAL_FUNC(Canvas_UpdateDiffAt)(&canvas, &previous, (TDUAL_TYPE(uint16)){.x=2,.y=2});
```

# See also

- [UI](UI.md), [GUI](GUI.md), [Text](Text.md), [Struct](Struct.md) and [Bit](Bit.md).
