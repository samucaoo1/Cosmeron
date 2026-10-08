# Canvas — retained textual surface

**Design only.** A Canvas is a text-based 2D drawing surface independent from terminal state and not a native WinCon screen buffer. All drawing modifies its in-memory cells. Presentation is **explicit** via `Update`.

## Existing implementations to reuse

- `../../Struct/TDual.h`: positions and dimensions: `TDUAL_TYPE(uint16)`, fields `x/y` or `col/row`.
- `../../Struct/TQuad.h`: rectangles `TQUAD_TYPE(uint16)` with `left/top/right/bottom`. This is an **inclusive-coordinate** rectangle, matching `Text/Grid`; no new `Rect` type.
- `../../Text/Grid/Grid.h`: `TEXT_GRID_TYPE(char32)` storage, cell access, fill, clone, blit.
- `../../Text/Grid/Attribute.h`: `TEXT_GRID_ATTRIBUTE_TYPE(Cell)` for style/cell attributes, `TEXT_GRID_ATTRIBUTE_TYPE(Flags)` and `TEXT_GRID_COLOR_TYPE(RGB)`.
- `../../Text/Encoding/UTF8.h`, `../../Text/Unicode/Width.h` for decoding/terminal widths.
- Do **not** create new `Cell`, `Color`, `Style`, `Position` or `Size` structs. Canvas itself is a new **view/ownership/context descriptor**, not a duplicate grid implementation.

### Shape and memory model

~~~text
TCanvas (owner) -> TEXT_GRID_TYPE(char32) cell+attribute storage
  |-- TCanvas (borrowed view: origin, extent, stride)
  |     +-- TCanvas (nested borrowed view)
  +-- TCanvas (another view)
~~~

`TCanvas` contains the drawing cursor, current cell style, clip region and storage/view metadata. A canvas **owns** its backing storage only if created with `Canvas_Create` (or cloned/cropped); `Canvas_View` returns a borrowed descriptor that must not free the underlying storage. A parent resize/reallocation/destruction invalidates children; this lifetime rule must be explicit. `Canvas_Free` is safe for both descriptors and must not double-free.

Views need coordinate translation, clip intersection and **strided** access. Existing Text/Grid functions assume contiguous buffers; the Canvas adapter must translate view operations, not cast a view to a `TEXT_GRID_TYPE(char32)` and call non-view-aware functions.

## Suggested signatures

~~~c
/* Lifecycle & queries */
OPSTATUS TERMINAL_FUNC(Canvas_Create)(
    TERMINAL_TYPE(TCanvas) *outCanvas, TDUAL_TYPE(uint16) size);
void TERMINAL_FUNC(Canvas_Free)(TERMINAL_TYPE(TCanvas) *canvas);
OPSTATUS TERMINAL_FUNC(Canvas_Resize)(
    TERMINAL_TYPE(TCanvas) *canvas, TDUAL_TYPE(uint16) size);
OPSTATUS TERMINAL_FUNC(Canvas_GetSize)(
    const TERMINAL_TYPE(TCanvas) *canvas, TDUAL_TYPE(uint16) *outSize);

/* Stateful and one-shot text output */
OPSTATUS TERMINAL_FUNC(Canvas_Print)(
    TERMINAL_TYPE(TCanvas) *canvas, const char *format, ...);
OPSTATUS TERMINAL_FUNC(Canvas_PrintLn)(
    TERMINAL_TYPE(TCanvas) *canvas, const char *format, ...);
OPSTATUS TERMINAL_FUNC(Canvas_PrintAt)(
    TERMINAL_TYPE(TCanvas) *canvas,
    TDUAL_TYPE(uint16) position, const char *format, ...);
OPSTATUS TERMINAL_FUNC(Canvas_PrintStyled)(
    TERMINAL_TYPE(TCanvas) *canvas,
    TEXT_GRID_ATTRIBUTE_TYPE(Cell) style, const char *format, ...);
OPSTATUS TERMINAL_FUNC(Canvas_PrintStyledLn)(
    TERMINAL_TYPE(TCanvas) *canvas,
    TEXT_GRID_ATTRIBUTE_TYPE(Cell) style, const char *format, ...);
OPSTATUS TERMINAL_FUNC(Canvas_PrintStyledAt)(
    TERMINAL_TYPE(TCanvas) *canvas,
    TDUAL_TYPE(uint16) position,
    TEXT_GRID_ATTRIBUTE_TYPE(Cell) style, const char *format, ...);
OPSTATUS TERMINAL_FUNC(Canvas_Write)(
    TERMINAL_TYPE(TCanvas) *canvas,
    const char *utf8Text, size_t byteLength);
OPSTATUS TERMINAL_FUNC(Canvas_WriteLine)(
    TERMINAL_TYPE(TCanvas) *canvas, const char *text);
OPSTATUS TERMINAL_FUNC(Canvas_PutChar)(
    TERMINAL_TYPE(TCanvas) *canvas, uint32_t codepoint);

/* Independent drawing state */
OPSTATUS TERMINAL_FUNC(Canvas_Cursor_GetPosition)(
    const TERMINAL_TYPE(TCanvas) *canvas,
    TDUAL_TYPE(uint16) *outPosition);
OPSTATUS TERMINAL_FUNC(Canvas_Cursor_SetPosition)(
    TERMINAL_TYPE(TCanvas) *canvas, TDUAL_TYPE(uint16) position);
OPSTATUS TERMINAL_FUNC(Canvas_Cursor_Move)(
    TERMINAL_TYPE(TCanvas) *canvas,
    int32_t deltaX, int32_t deltaY);
void TERMINAL_FUNC(Canvas_Style_Set)(
    TERMINAL_TYPE(TCanvas) *canvas, TEXT_GRID_ATTRIBUTE_TYPE(Cell) style);
void TERMINAL_FUNC(Canvas_Style_Reset)(TERMINAL_TYPE(TCanvas) *canvas);
void TERMINAL_FUNC(Canvas_Attribute_Set)(
    TERMINAL_TYPE(TCanvas) *canvas, TEXT_GRID_ATTRIBUTE_TYPE(Flags) flags);

/* Cell operations reuse the established Text/Grid attribute type. */
OPSTATUS TERMINAL_FUNC(Canvas_Get)(
    const TERMINAL_TYPE(TCanvas) *canvas,
    TDUAL_TYPE(uint16) position,
    uint32_t *outCodepoint,
    TEXT_GRID_ATTRIBUTE_TYPE(Cell) *outAttribute);
OPSTATUS TERMINAL_FUNC(Canvas_Set)(
    TERMINAL_TYPE(TCanvas) *canvas,
    TDUAL_TYPE(uint16) position,
    uint32_t codepoint, TEXT_GRID_ATTRIBUTE_TYPE(Cell) attribute);
void TERMINAL_FUNC(Canvas_Clear)(TERMINAL_TYPE(TCanvas) *canvas);
OPSTATUS TERMINAL_FUNC(Canvas_Fill)(
    TERMINAL_TYPE(TCanvas) *canvas, uint32_t codepoint,
    TEXT_GRID_ATTRIBUTE_TYPE(Cell) attribute);
OPSTATUS TERMINAL_FUNC(Canvas_FillRegion)(
    TERMINAL_TYPE(TCanvas) *canvas, TQUAD_TYPE(uint16) region,
    uint32_t codepoint, TEXT_GRID_ATTRIBUTE_TYPE(Cell) attribute);

/* Composition: region bounds are inclusive, not x/y/width/height. */
OPSTATUS TERMINAL_FUNC(Canvas_Copy)(
    const TERMINAL_TYPE(TCanvas) *source,
    TERMINAL_TYPE(TCanvas) *outCanvas);
OPSTATUS TERMINAL_FUNC(Canvas_Crop)(
    const TERMINAL_TYPE(TCanvas) *source,
    TQUAD_TYPE(uint16) region,
    TERMINAL_TYPE(TCanvas) *outCanvas);
OPSTATUS TERMINAL_FUNC(Canvas_View)(
    TERMINAL_TYPE(TCanvas) *source,
    TQUAD_TYPE(uint16) region,
    TERMINAL_TYPE(TCanvas) *outView);
OPSTATUS TERMINAL_FUNC(Canvas_Blit)(
    TERMINAL_TYPE(TCanvas) *destination,
    TDUAL_TYPE(uint16) position,
    const TERMINAL_TYPE(TCanvas) *source);
OPSTATUS TERMINAL_FUNC(Canvas_BlitRegion)(
    TERMINAL_TYPE(TCanvas) *destination,
    TDUAL_TYPE(uint16) position,
    const TERMINAL_TYPE(TCanvas) *source,
    TQUAD_TYPE(uint16) region);

/* Presentation to the standard or an explicit terminal. */
OPSTATUS TERMINAL_FUNC(Canvas_Update)(
    const TERMINAL_TYPE(TCanvas) *canvas);
OPSTATUS TERMINAL_FUNC(Canvas_UpdateAt)(
    const TERMINAL_TYPE(TCanvas) *canvas,
    TDUAL_TYPE(uint16) position);
OPSTATUS TERMINAL_FUNC(Canvas_UpdateTo)(
    const TERMINAL_TYPE(TCanvas) *canvas,
    TERMINAL_TYPE(TTerminal) *terminal);
OPSTATUS TERMINAL_FUNC(Canvas_UpdateAtTo)(
    const TERMINAL_TYPE(TCanvas) *canvas,
    TERMINAL_TYPE(TTerminal) *terminal,
    TDUAL_TYPE(uint16) position);
~~~

## Contracts

1. All Canvas operations except `Update*` are **off-screen** and do not touch process stdout.
2. `Print` advances the canvas cursor; `PrintAt` uses a temporary cursor; `PrintStyled` uses temporary style; `PrintStyledAt` preserves both.
3. `Crop` is a deep owning copy; `View` aliases the original cells, with inherited clipping and independent writing cursor/style.
4. `TQUAD_TYPE(uint16)` boundaries are inclusive (`left <= right`; `top <= bottom`) and checked against parent dimensions. No ambiguous Rect convention.
5. `Canvas_Blit` respects clipping and cell-attribute composition policies from Text/Grid. Support safe overlapping sources/destinations (including self-blit and views).
6. Unicode: store Unicode scalar values in char32 storage and use Text/Unicode width; define continuation markers and fallback glyphs consistently for combining and wide characters.
7. No required hidden global canvas; no mandatory double buffering. Dirty-region tracking is optional internal optimization, and must invalidate on immediate terminal writes.
8. `Update` is logically a full presentation of the requested canvas region every time, even if the actual backend emits only changed cells.

### Example (conceptual)

~~~c
TERMINAL_TYPE(TCanvas) canvas = {0};
TDUAL_TYPE(uint16) size = {.col = 80, .row = 25};
TDUAL_TYPE(uint16) at = {.x = 10, .y = 5};

TERMINAL_FUNC(Canvas_Create)(&canvas, size);
TERMINAL_FUNC(Canvas_PrintAt)(&canvas, at, "HP: %u", hp);
TERMINAL_FUNC(Canvas_Update)(&canvas);
TERMINAL_FUNC(Canvas_Free)(&canvas);
~~~

## Tests

Include owning vs borrowed lifetimes, no double-free, resize invalidation, nested views, bounds/clipping, malformed UTF-8, wide glyphs, self-blit/overlaps, transparent backgrounds, partial screen updates, output redirection and immediate-write/cache coherency.
