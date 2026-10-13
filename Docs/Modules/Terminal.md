# Terminal

Header: `Codespace/Cosmeron/Modules/Graphics/Terminal/Terminal.h`

Immediate console/standard-stream I/O for the Cosmeron Graphics subsystem. The standard terminal is borrowed and available without initialization or teardown. The separate **Canvas** package now implements retained in-memory rendering; see [Canvas](Canvas.md).

## Basic use

```c
#include "Cosmeron/Modules/Graphics/Terminal/Terminal.h"

int main(void) {
    Terminal_PrintLn("Hello, %s!", "world");
    Terminal_WriteLine("Direct terminal output");
    return 0;
}
```

Functions use `TERMINAL_FUNC(...)` or their direct `Terminal_...` names when the default namespace is selected.

## Operations

| Family | Functions |
|---|---|
| Standard handles | `Standard`, `StandardHandle`, `WriteTo`, `ReadFrom`, `PrintTo`, `FlushHandle` |
| Immediate I/O | `Write`, `Read`, `Print`, `PrintLn`, `WriteLine`, `ReadLine`, `Input`, `Scan`, `PutChar`, `GetChar`, `Flush` |
| Console | `Cursor_GetPosition`, `Cursor_SetPosition`, `Cursor_Move`, `Cursor_Show`, `Cursor_Hide`, `Size_Get`, `Clear`, `ClearLine`, `Title_Set`, `Bell` |
| Scoped printing | `PrintAt`, `PrintStyled`, `PrintStyledLn`, `PrintStyledAt` |
| Appearance | `Style_Get`, `Style_Set`, `Style_Reset`, `Color_Set`, `Color_Reset`, `Attribute_Set`, `Attribute_Add`, `Attribute_Remove`, `Attribute_Reset` |
| Keyboard | `Key_Get`, `Key_GetEcho`, `Key_Hit`, `InputMode_Get`, `InputMode_Set` |

## Behavior and limitations

- `Write` handles bytes, while `PutChar` and `GetChar` encode/decode UTF-8 with the existing Text routines.
- Position, size, cursor and style reuse `Struct/TDual.h` and `Text/Grid/Attribute.h`.
- `ReadLine` removes the newline. On truncation it drains the rest of the input line and returns `INSUFFICIENT_SPACE`. End-of-file without bytes returns `NOT_FOUND`. The output length is set only on success.
- `Scan` supports numeric scanf conversions (`%d`, `%u`, `%x`, `%f`, etc.) and literals; it rejects `%s`, scansets, `%n`, `%c`, pointers and unsupported extensions. For text, prefer `ReadLine` and a bounded parser.
- On redirected stdout, interactive operations return `NOT_AVAILABLE` rather than emitting control sequences into logs or pipes.
- POSIX cursor-position querying is currently `NOT_SUPPORTED`; `Cursor_SetPosition` and relative movement emit ANSI/VT on interactive terminals. Windows uses native console APIs for cursor operations.
- Windows ANSI truecolor requires VT processing already enabled by the host console; unsupported consoles return `NOT_SUPPORTED`.
- Explicit input mode changes affect the process console. The original native mode is captured and registered for automatic restoration at normal process exit; explicit `INPUT_CANONICAL` restores that baseline. POSIX key polling and arrow-key reads use temporary cbreak without stdio prefetch.
- Only the library's saved style state is preserved across scoped writes. It cannot account for out-of-band terminal writes.
- Keyboard escape-sequence parsing is basic; comprehensive modifier/function-key support is future work.

See `Codespace/Cosmeron/Modules/Graphics/Terminal/README.md` for the design goals.


## Native event API and terminal capabilities

`TERMINAL_FUNC(Event_Poll)(TEvent *outEvent, bool *outAvailable)` polls
non-blockingly. Event kinds are `EVENT_KEY`, `EVENT_POINTER_MOVE`,
`EVENT_POINTER_BUTTON`, `EVENT_SCROLL`, and `EVENT_RESIZE`.
Key events use Unicode codepoints or `KEY_*` tokens. Coordinate units are
zero-based terminal cells.

`TERMINAL_FUNC(Mouse_Enable)(bool enabled)` enables/disables optional native
mouse reporting. It saves and restores original platform input settings.
The capture path requires interactive stdin/stdout and must not be mixed with
direct stdio/getch polling on the same descriptor.

`TERMINAL_FUNC(Capabilities_Get)(TCapabilities *outCapabilities)` detects
interactive output, ANSI, Unicode, truecolor and mouse support
conservatively. The detection uses WinConsole APIs or environment-based
POSIX heuristics: the reported capabilities are **not a formal terminal
negotiation handshake**.

## 2D rasterization

`Graphics/Rasterization/Rasterization.h` provides `RASTER_FUNC(Point)`, `Line`,
`Rectangle`, `Circle`, `Ellipse`, `Triangle`, `Polyline`, and `Polygon`, with
`FillRectangle`, `FillCircle`, `FillEllipse`, `FillTriangle`, and `FillPolygon`.
They draw into an existing `TERMINAL_TYPE(TCanvas)` using a one-cell character
and a Text/Grid attribute cell. Call `Canvas_Update` explicitly to display it.
Coordinates reuse Struct types; drawing is clipped to the canvas or view.
See [Rasterization](../../Codespace/Cosmeron/Modules/Graphics/Rasterization/README.md)
for signatures, examples, fill rules and Unicode handling.

## Raycasting and pseudo-3D road

- [Raycasting](../../Codespace/Cosmeron/Modules/Graphics/Raycasting/README.md):
  `Raycast_GridCast` traverses tile maps; `Raycast_CameraRay` creates column rays;
  `Raycast_RenderColumns` projects walls, floor and ceiling into Canvas.
- [Pseudo3DRoad](../../Codespace/Cosmeron/Modules/Graphics/Pseudo3DRoad/README.md):
  `Road_TrackValidate`, `Road_Project`, `Road_StyleDefault`, and `Road_Render`
  provide a road with perspective, bends, elevation, curbs and lane markings.

Both use existing Struct tuples and Rasterization. Computational headers can
be included without Terminal; each package's `Render.h` enables the Canvas
adapter. No initialization, implicit presentation, allocation or `-lm` required.
Compilable examples are in `Codespace/Examples/Graphics`.


## Sprites, composition and animation

[Sprite](../../Codespace/Cosmeron/Modules/Graphics/Sprite/README.md) draws existing
Canvas images with inclusive source crops, nearest-neighbor scaling, mirroring,
character-key transparency and transparent backgrounds. `Sprite_Compose`
provides ordered layers and a 2D camera over the destination Canvas/view.
`Sprite_FrameAt`/`Sprite_DrawFrame` use caller-provided elapsed time.

Raycast adds optional material textures, `RenderColumnsDepth` and `RenderSprites`.
Road adds `TrackSample`, `RenderDepth` and `RenderObjects` with terrain occlusion.
Buffers remain caller-owned; ordinary render calls need no buffers. Rasterization
also includes `Arc`, `BezierQuadratic` and `BezierCubic`.

---

# API reference

Terminal functions perform immediate console and standard-stream operations. Standard streams are borrowed and do not require a global initialization/shutdown pair. The signatures below follow the public headers. `TERMINAL_FUNC(Name)` resolves to `Terminal_Name` with the default namespace, including the `Canvas_*` family. Usage fragments assume external objects are initialized where necessary.

## Function summary

| Function | Return | Purpose |
| --- | --- | --- |
| `TERMINAL_FUNC(Standard)` | `TERMINAL_TYPE(TTerminal) *` | Returns the borrowed standard terminal abstraction. |
| `TERMINAL_FUNC(StandardHandle)` | `TERMINAL_TYPE(THandle) *` | Returns the borrowed handle for selected standard stream. |
| `TERMINAL_FUNC(WriteTo)` | `OPSTATUS` | Writes bytes directly to a selected handle. |
| `TERMINAL_FUNC(ReadFrom)` | `OPSTATUS` | Reads bytes from a selected handle. |
| `TERMINAL_FUNC(FlushHandle)` | `OPSTATUS` | Flushes a selected terminal stream handle. |
| `TERMINAL_FUNC(PrintTo)` | `OPSTATUS` | Formats output into a selected terminal handle. |
| `TERMINAL_FUNC(Write)` | `OPSTATUS` | Writes UTF-8 bytes without format interpretation. |
| `TERMINAL_FUNC(Read)` | `OPSTATUS` | Reads raw bytes from standard input. |
| `TERMINAL_FUNC(Print)` | `OPSTATUS` | Formats text into the Canvas at its cursor. |
| `TERMINAL_FUNC(PrintLn)` | `OPSTATUS` | Formats a line and moves to the next row. |
| `TERMINAL_FUNC(PrintAt)` | `OPSTATUS` | Prints formatted text at an explicit position. |
| `TERMINAL_FUNC(PrintStyled)` | `OPSTATUS` | Formats text with a temporary explicit style. |
| `TERMINAL_FUNC(PrintStyledLn)` | `OPSTATUS` | Formats a styled line. |
| `TERMINAL_FUNC(PrintStyledAt)` | `OPSTATUS` | Formats styled text at an explicit position. |
| `TERMINAL_FUNC(WriteLine)` | `OPSTATUS` | Writes a complete text line. |
| `TERMINAL_FUNC(ReadLine)` | `OPSTATUS` | Reads one line into caller storage. |
| `TERMINAL_FUNC(Input)` | `OPSTATUS` | Prints prompt and reads a line. |
| `TERMINAL_FUNC(Scan)` | `OPSTATUS` | Scans formatted input. |
| `TERMINAL_FUNC(PutChar)` | `OPSTATUS` | Writes one Unicode codepoint. |
| `TERMINAL_FUNC(GetChar)` | `OPSTATUS` | Reads a Unicode codepoint. |
| `TERMINAL_FUNC(Flush)` | `OPSTATUS` | Flushes standard output. |
| `TERMINAL_FUNC(Cursor_GetPosition)` | `OPSTATUS` | Queries cursor position. |
| `TERMINAL_FUNC(Cursor_SetPosition)` | `OPSTATUS` | Sets absolute cursor position. |
| `TERMINAL_FUNC(Cursor_Move)` | `OPSTATUS` | Moves cursor relatively. |
| `TERMINAL_FUNC(Cursor_Show)` | `OPSTATUS` | Shows cursor. |
| `TERMINAL_FUNC(Cursor_Hide)` | `OPSTATUS` | Hides cursor. |
| `TERMINAL_FUNC(Size_Get)` | `OPSTATUS` | Obtains terminal dimensions. |
| `TERMINAL_FUNC(Clear)` | `OPSTATUS` | Clears cells to the default state. |
| `TERMINAL_FUNC(ClearLine)` | `OPSTATUS` | Clears the current terminal line. |
| `TERMINAL_FUNC(Title_Set)` | `OPSTATUS` | Updates the title of the supported terminal. |
| `TERMINAL_FUNC(Bell)` | `OPSTATUS` | Emits terminal bell event. |
| `TERMINAL_FUNC(Style_Set)` | `OPSTATUS` | Sets active text style. |
| `TERMINAL_FUNC(Style_Get)` | `OPSTATUS` | Gets active text style. |
| `TERMINAL_FUNC(Style_Reset)` | `OPSTATUS` | Restores the default text style. |
| `TERMINAL_FUNC(Color_Set)` | `OPSTATUS` | Sets foreground and background colors. |
| `TERMINAL_FUNC(Color_Reset)` | `OPSTATUS` | Restores default colors. |
| `TERMINAL_FUNC(Attribute_Set)` | `OPSTATUS` | Replaces text attribute flags. |
| `TERMINAL_FUNC(Attribute_Add)` | `OPSTATUS` | Adds text attribute flags. |
| `TERMINAL_FUNC(Attribute_Remove)` | `OPSTATUS` | Removes selected attribute flags. |
| `TERMINAL_FUNC(Attribute_Reset)` | `OPSTATUS` | Restores default flags. |
| `TERMINAL_FUNC(Capabilities_Get)` | `OPSTATUS` | Reports terminal/backend capabilities. |
| `TERMINAL_FUNC(Event_Poll)` | `OPSTATUS` | Tries to obtain normalized input without blocking. |
| `TERMINAL_FUNC(Mouse_Enable)` | `OPSTATUS` | Enables or disables pointer events in the terminal. |
| `TERMINAL_FUNC(Key_Get)` | `OPSTATUS` | Reads a key event. |
| `TERMINAL_FUNC(Key_GetEcho)` | `OPSTATUS` | Reads a key event with echo. |
| `TERMINAL_FUNC(Key_Hit)` | `OPSTATUS` | Tests whether a key is available. |
| `TERMINAL_FUNC(InputMode_Get)` | `OPSTATUS` | Reads active input mode. |
| `TERMINAL_FUNC(InputMode_Set)` | `OPSTATUS` | Sets active input mode. |

# Standard

Returns the borrowed standard terminal abstraction.

### Syntax

#### Macro form

```c
TERMINAL_TYPE(TTerminal) * TERMINAL_FUNC(Standard)(void);
```

#### Direct form

```c
Terminal_TTerminal * Terminal_Standard(void);
```

### Parameters

- None.

### Return value

Returns `TERMINAL_TYPE(TTerminal) *` as a borrowed pointer; do not free the standard stream resource.

### Remarks

Terminal operations act immediately on process standard streams or native console state; they do not maintain a hidden Canvas.

### Example

```c
/* Exemplo isolado: handles e Canvas citados precisam estar inicializados. */
TERMINAL_TYPE(TTerminal) *terminal = TERMINAL_FUNC(Standard)();
```

# StandardHandle

Returns the borrowed handle for selected standard stream.

### Syntax

#### Macro form

```c
TERMINAL_TYPE(THandle) * TERMINAL_FUNC(StandardHandle)(TERMINAL_TYPE(StandardStream) stream);
```

#### Direct form

```c
Terminal_THandle * Terminal_StandardHandle(Terminal_StandardStream stream);
```

### Parameters

- `stream`: standard stream selector.

### Return value

Returns `TERMINAL_TYPE(THandle) *` as a borrowed pointer; do not free the standard stream resource.

### Remarks

Terminal operations act immediately on process standard streams or native console state; they do not maintain a hidden Canvas.

### Example

```c
/* Exemplo isolado: handles e Canvas citados precisam estar inicializados. */
TERMINAL_TYPE(THandle) *handle = TERMINAL_FUNC(StandardHandle)(stream);
```

# WriteTo

Writes bytes directly to a selected handle.

### Syntax

#### Macro form

```c
OPSTATUS TERMINAL_FUNC(WriteTo)(TERMINAL_TYPE(THandle) *handle, const void *data, size_t size, size_t *outWritten);
```

#### Direct form

```c
OPSTATUS Terminal_WriteTo(Terminal_THandle *handle, const void *data, size_t size, size_t *outWritten);
```

### Parameters

- `handle`: borrowed/native terminal handle.
- `data`: pointer to bytes sent without printf formatting.
- `size`: Canvas size or byte count as declared.
- `outWritten`: receives number of bytes written.

### Return value

`OPSTATUS`: SUCCESS if completed; errors include invalid pointers, out-of-range coordinates, unsupported console capabilities and I/O failures as applicable.

### Remarks

Terminal operations act immediately on process standard streams or native console state; they do not maintain a hidden Canvas.

### Example

```c
/* Exemplo isolado: handles e Canvas citados precisam estar inicializados. */
size_t bytes = 0;
OPSTATUS status = TERMINAL_FUNC(WriteTo)(handle, "hello", 5, &bytes);
```

# ReadFrom

Reads bytes from a selected handle.

### Syntax

#### Macro form

```c
OPSTATUS TERMINAL_FUNC(ReadFrom)(TERMINAL_TYPE(THandle) *handle, void *buffer, size_t capacity, size_t *outRead);
```

#### Direct form

```c
OPSTATUS Terminal_ReadFrom(Terminal_THandle *handle, void *buffer, size_t capacity, size_t *outRead);
```

### Parameters

- `handle`: borrowed/native terminal handle.
- `buffer`: mutable caller-owned byte buffer.
- `capacity`: buffer capacity in bytes.
- `outRead`: receives actual number of bytes read.

### Return value

`OPSTATUS`: SUCCESS if completed; errors include invalid pointers, out-of-range coordinates, unsupported console capabilities and I/O failures as applicable.

### Remarks

Terminal operations act immediately on process standard streams or native console state; they do not maintain a hidden Canvas.

### Example

```c
/* Exemplo isolado: handles e Canvas citados precisam estar inicializados. */
char buffer[256] = {0};
size_t bytes = 0;
OPSTATUS status = TERMINAL_FUNC(ReadFrom)(handle, buffer, sizeof buffer, &bytes);
```

# FlushHandle

Flushes a selected terminal stream handle.

### Syntax

#### Macro form

```c
OPSTATUS TERMINAL_FUNC(FlushHandle)(TERMINAL_TYPE(THandle) *handle);
```

#### Direct form

```c
OPSTATUS Terminal_FlushHandle(Terminal_THandle *handle);
```

### Parameters

- `handle`: borrowed/native terminal handle.

### Return value

`OPSTATUS`: SUCCESS if completed; errors include invalid pointers, out-of-range coordinates, unsupported console capabilities and I/O failures as applicable.

### Remarks

Terminal operations act immediately on process standard streams or native console state; they do not maintain a hidden Canvas.

### Example

```c
/* Exemplo isolado: handles e Canvas citados precisam estar inicializados. */
OPSTATUS status = TERMINAL_FUNC(FlushHandle)(handle);
```

# PrintTo

Formats output into a selected terminal handle.

### Syntax

#### Macro form

```c
OPSTATUS TERMINAL_FUNC(PrintTo)(TERMINAL_TYPE(THandle) *handle, const char *format, ...);
```

#### Direct form

```c
OPSTATUS Terminal_PrintTo(Terminal_THandle *handle, const char *format, ...);
```

### Parameters

- `handle`: borrowed/native terminal handle.
- `format`: printf-style format string; remaining arguments must match conversions.
- `...`: additional format arguments matching `format`.

### Return value

`OPSTATUS`: SUCCESS if completed; errors include invalid pointers, out-of-range coordinates, unsupported console capabilities and I/O failures as applicable.

### Remarks

Terminal operations act immediately on process standard streams or native console state; they do not maintain a hidden Canvas. This is a C varargs call: format specifiers and argument types must match.

### Example

```c
/* Exemplo isolado: handles e Canvas citados precisam estar inicializados. */
OPSTATUS status = TERMINAL_FUNC(PrintTo)(handle, "%s", "hello");
```

# Write

Writes UTF-8 bytes without format interpretation.

### Syntax

#### Macro form

```c
OPSTATUS TERMINAL_FUNC(Write)(const void *data, size_t size);
```

#### Direct form

```c
OPSTATUS Terminal_Write(const void *data, size_t size);
```

### Parameters

- `data`: pointer to bytes sent without printf formatting.
- `size`: Canvas size or byte count as declared.

### Return value

`OPSTATUS`: SUCCESS if completed; errors include invalid pointers, out-of-range coordinates, unsupported console capabilities and I/O failures as applicable.

### Remarks

Terminal operations act immediately on process standard streams or native console state; they do not maintain a hidden Canvas.

### Example

```c
/* Exemplo isolado: handles e Canvas citados precisam estar inicializados. */
OPSTATUS status = TERMINAL_FUNC(Write)("hello", 5);
```

# Read

Reads raw bytes from standard input.

### Syntax

#### Macro form

```c
OPSTATUS TERMINAL_FUNC(Read)(void *buffer, size_t capacity, size_t *outRead);
```

#### Direct form

```c
OPSTATUS Terminal_Read(void *buffer, size_t capacity, size_t *outRead);
```

### Parameters

- `buffer`: mutable caller-owned byte buffer.
- `capacity`: buffer capacity in bytes.
- `outRead`: receives actual number of bytes read.

### Return value

`OPSTATUS`: SUCCESS if completed; errors include invalid pointers, out-of-range coordinates, unsupported console capabilities and I/O failures as applicable.

### Remarks

Terminal operations act immediately on process standard streams or native console state; they do not maintain a hidden Canvas.

### Example

```c
/* Exemplo isolado: handles e Canvas citados precisam estar inicializados. */
char buffer[256] = {0};
size_t bytes = 0;
OPSTATUS status = TERMINAL_FUNC(Read)(buffer, sizeof buffer, &bytes);
```

# Print

Formats text into the Canvas at its cursor.

### Syntax

#### Macro form

```c
OPSTATUS TERMINAL_FUNC(Print)(const char *format, ...);
```

#### Direct form

```c
OPSTATUS Terminal_Print(const char *format, ...);
```

### Parameters

- `format`: printf-style format string; remaining arguments must match conversions.
- `...`: additional format arguments matching `format`.

### Return value

`OPSTATUS`: SUCCESS if completed; errors include invalid pointers, out-of-range coordinates, unsupported console capabilities and I/O failures as applicable.

### Remarks

Terminal operations act immediately on process standard streams or native console state; they do not maintain a hidden Canvas. This is a C varargs call: format specifiers and argument types must match.

### Example

```c
/* Exemplo isolado: handles e Canvas citados precisam estar inicializados. */
OPSTATUS status = TERMINAL_FUNC(Print)("%s", "hello");
```

# PrintLn

Formats a line and moves to the next row.

### Syntax

#### Macro form

```c
OPSTATUS TERMINAL_FUNC(PrintLn)(const char *format, ...);
```

#### Direct form

```c
OPSTATUS Terminal_PrintLn(const char *format, ...);
```

### Parameters

- `format`: printf-style format string; remaining arguments must match conversions.
- `...`: additional format arguments matching `format`.

### Return value

`OPSTATUS`: SUCCESS if completed; errors include invalid pointers, out-of-range coordinates, unsupported console capabilities and I/O failures as applicable.

### Remarks

Terminal operations act immediately on process standard streams or native console state; they do not maintain a hidden Canvas. This is a C varargs call: format specifiers and argument types must match.

### Example

```c
/* Exemplo isolado: handles e Canvas citados precisam estar inicializados. */
OPSTATUS status = TERMINAL_FUNC(PrintLn)("%s", "hello");
```

# PrintAt

Prints formatted text at an explicit position.

### Syntax

#### Macro form

```c
OPSTATUS TERMINAL_FUNC(PrintAt)(TDUAL_TYPE(uint16) position, const char *format, ...);
```

#### Direct form

```c
OPSTATUS Terminal_PrintAt(TDual_uint16 position, const char *format, ...);
```

### Parameters

- `position`: cell position in zero-based coordinates.
- `format`: printf-style format string; remaining arguments must match conversions.
- `...`: additional format arguments matching `format`.

### Return value

`OPSTATUS`: SUCCESS if completed; errors include invalid pointers, out-of-range coordinates, unsupported console capabilities and I/O failures as applicable.

### Remarks

Terminal operations act immediately on process standard streams or native console state; they do not maintain a hidden Canvas. This is a C varargs call: format specifiers and argument types must match.

### Example

```c
/* Exemplo isolado: handles e Canvas citados precisam estar inicializados. */
OPSTATUS status = TERMINAL_FUNC(PrintAt)((TDUAL_TYPE(uint16)){.x=2,.y=2}, "%s", "hello");
```

# PrintStyled

Formats text with a temporary explicit style.

### Syntax

#### Macro form

```c
OPSTATUS TERMINAL_FUNC(PrintStyled)(TEXT_GRID_ATTRIBUTE_TYPE(Cell) style, const char *format, ...);
```

#### Direct form

```c
OPSTATUS Terminal_PrintStyled(Text_Grid_Attribute_Cell style, const char *format, ...);
```

### Parameters

- `style`: Text/Grid cell attribute, not a terminal-global style.
- `format`: printf-style format string; remaining arguments must match conversions.
- `...`: additional format arguments matching `format`.

### Return value

`OPSTATUS`: SUCCESS if completed; errors include invalid pointers, out-of-range coordinates, unsupported console capabilities and I/O failures as applicable.

### Remarks

Terminal operations act immediately on process standard streams or native console state; they do not maintain a hidden Canvas. This is a C varargs call: format specifiers and argument types must match.

### Example

```c
/* Exemplo isolado: handles e Canvas citados precisam estar inicializados. */
OPSTATUS status = TERMINAL_FUNC(PrintStyled)(TEXT_GRID_ATTRIBUTE_FUNC(Default)(), "%s", "hello");
```

# PrintStyledLn

Formats a styled line.

### Syntax

#### Macro form

```c
OPSTATUS TERMINAL_FUNC(PrintStyledLn)(TEXT_GRID_ATTRIBUTE_TYPE(Cell) style, const char *format, ...);
```

#### Direct form

```c
OPSTATUS Terminal_PrintStyledLn(Text_Grid_Attribute_Cell style, const char *format, ...);
```

### Parameters

- `style`: Text/Grid cell attribute, not a terminal-global style.
- `format`: printf-style format string; remaining arguments must match conversions.
- `...`: additional format arguments matching `format`.

### Return value

`OPSTATUS`: SUCCESS if completed; errors include invalid pointers, out-of-range coordinates, unsupported console capabilities and I/O failures as applicable.

### Remarks

Terminal operations act immediately on process standard streams or native console state; they do not maintain a hidden Canvas. This is a C varargs call: format specifiers and argument types must match.

### Example

```c
/* Exemplo isolado: handles e Canvas citados precisam estar inicializados. */
OPSTATUS status = TERMINAL_FUNC(PrintStyledLn)(TEXT_GRID_ATTRIBUTE_FUNC(Default)(), "%s", "hello");
```

# PrintStyledAt

Formats styled text at an explicit position.

### Syntax

#### Macro form

```c
OPSTATUS TERMINAL_FUNC(PrintStyledAt)(TDUAL_TYPE(uint16) position, TEXT_GRID_ATTRIBUTE_TYPE(Cell) style, const char *format, ...);
```

#### Direct form

```c
OPSTATUS Terminal_PrintStyledAt(TDual_uint16 position, Text_Grid_Attribute_Cell style, const char *format, ...);
```

### Parameters

- `position`: cell position in zero-based coordinates.
- `style`: Text/Grid cell attribute, not a terminal-global style.
- `format`: printf-style format string; remaining arguments must match conversions.
- `...`: additional format arguments matching `format`.

### Return value

`OPSTATUS`: SUCCESS if completed; errors include invalid pointers, out-of-range coordinates, unsupported console capabilities and I/O failures as applicable.

### Remarks

Terminal operations act immediately on process standard streams or native console state; they do not maintain a hidden Canvas. This is a C varargs call: format specifiers and argument types must match.

### Example

```c
/* Exemplo isolado: handles e Canvas citados precisam estar inicializados. */
OPSTATUS status = TERMINAL_FUNC(PrintStyledAt)((TDUAL_TYPE(uint16)){.x=2,.y=2}, TEXT_GRID_ATTRIBUTE_FUNC(Default)(), "%s", "hello");
```

# WriteLine

Writes a complete text line.

### Syntax

#### Macro form

```c
OPSTATUS TERMINAL_FUNC(WriteLine)(const char *text);
```

#### Direct form

```c
OPSTATUS Terminal_WriteLine(const char *text);
```

### Parameters

- `text`: null-terminated input text.

### Return value

`OPSTATUS`: SUCCESS if completed; errors include invalid pointers, out-of-range coordinates, unsupported console capabilities and I/O failures as applicable.

### Remarks

Terminal operations act immediately on process standard streams or native console state; they do not maintain a hidden Canvas.

### Example

```c
/* Exemplo isolado: handles e Canvas citados precisam estar inicializados. */
OPSTATUS status = TERMINAL_FUNC(WriteLine)("hello");
```

# ReadLine

Reads one line into caller storage.

### Syntax

#### Macro form

```c
OPSTATUS TERMINAL_FUNC(ReadLine)(char *buffer, size_t capacity, size_t *outLength);
```

#### Direct form

```c
OPSTATUS Terminal_ReadLine(char *buffer, size_t capacity, size_t *outLength);
```

### Parameters

- `buffer`: mutable caller-owned byte buffer.
- `capacity`: buffer capacity in bytes.
- `outLength`: receives number of characters/bytes as specified by the operation.

### Return value

`OPSTATUS`: SUCCESS if completed; errors include invalid pointers, out-of-range coordinates, unsupported console capabilities and I/O failures as applicable.

### Remarks

Terminal operations act immediately on process standard streams or native console state; they do not maintain a hidden Canvas.

### Example

```c
/* Exemplo isolado: handles e Canvas citados precisam estar inicializados. */
char buffer[256] = {0};
size_t bytes = 0;
OPSTATUS status = TERMINAL_FUNC(ReadLine)(buffer, sizeof buffer, &bytes);
```

# Input

Prints prompt and reads a line.

### Syntax

#### Macro form

```c
OPSTATUS TERMINAL_FUNC(Input)(const char *prompt, char *buffer, size_t capacity, size_t *outLength);
```

#### Direct form

```c
OPSTATUS Terminal_Input(const char *prompt, char *buffer, size_t capacity, size_t *outLength);
```

### Parameters

- `prompt`: prompt printed before input.
- `buffer`: mutable caller-owned byte buffer.
- `capacity`: buffer capacity in bytes.
- `outLength`: receives number of characters/bytes as specified by the operation.

### Return value

`OPSTATUS`: SUCCESS if completed; errors include invalid pointers, out-of-range coordinates, unsupported console capabilities and I/O failures as applicable.

### Remarks

Terminal operations act immediately on process standard streams or native console state; they do not maintain a hidden Canvas.

### Example

```c
/* Exemplo isolado: handles e Canvas citados precisam estar inicializados. */
char buffer[256] = {0};
size_t bytes = 0;
OPSTATUS status = TERMINAL_FUNC(Input)("Input: ", buffer, sizeof buffer, &bytes);
```

# Scan

Scans formatted input.

### Syntax

#### Macro form

```c
OPSTATUS TERMINAL_FUNC(Scan)(const char *format, ...);
```

#### Direct form

```c
OPSTATUS Terminal_Scan(const char *format, ...);
```

### Parameters

- `format`: printf-style format string; remaining arguments must match conversions.
- `...`: additional format arguments matching `format`.

### Return value

`OPSTATUS`: SUCCESS if completed; errors include invalid pointers, out-of-range coordinates, unsupported console capabilities and I/O failures as applicable.

### Remarks

Terminal operations act immediately on process standard streams or native console state; they do not maintain a hidden Canvas. This is a C varargs call: format specifiers and argument types must match.

### Example

```c
/* Exemplo isolado: handles e Canvas citados precisam estar inicializados. */
OPSTATUS status = TERMINAL_FUNC(Scan)("%s", "hello");
```

# PutChar

Writes one Unicode codepoint.

### Syntax

#### Macro form

```c
OPSTATUS TERMINAL_FUNC(PutChar)(uint32_t codepoint);
```

#### Direct form

```c
OPSTATUS Terminal_PutChar(uint32_t codepoint);
```

### Parameters

- `codepoint`: one Unicode scalar value.

### Return value

`OPSTATUS`: SUCCESS if completed; errors include invalid pointers, out-of-range coordinates, unsupported console capabilities and I/O failures as applicable.

### Remarks

Terminal operations act immediately on process standard streams or native console state; they do not maintain a hidden Canvas.

### Example

```c
/* Exemplo isolado: handles e Canvas citados precisam estar inicializados. */
OPSTATUS status = TERMINAL_FUNC(PutChar)('#');
```

# GetChar

Reads a Unicode codepoint.

### Syntax

#### Macro form

```c
OPSTATUS TERMINAL_FUNC(GetChar)(uint32_t *outCodepoint);
```

#### Direct form

```c
OPSTATUS Terminal_GetChar(uint32_t *outCodepoint);
```

### Parameters

- `outCodepoint`: receives Unicode codepoint read.

### Return value

`OPSTATUS`: SUCCESS if completed; errors include invalid pointers, out-of-range coordinates, unsupported console capabilities and I/O failures as applicable.

### Remarks

Terminal operations act immediately on process standard streams or native console state; they do not maintain a hidden Canvas.

### Example

```c
/* Exemplo isolado: handles e Canvas citados precisam estar inicializados. */
uint32_t codepoint = 0;
OPSTATUS status = TERMINAL_FUNC(GetChar)(&codepoint);
```

# Flush

Flushes standard output.

### Syntax

#### Macro form

```c
OPSTATUS TERMINAL_FUNC(Flush)(void);
```

#### Direct form

```c
OPSTATUS Terminal_Flush(void);
```

### Parameters

- None.

### Return value

`OPSTATUS`: SUCCESS if completed; errors include invalid pointers, out-of-range coordinates, unsupported console capabilities and I/O failures as applicable.

### Remarks

Terminal operations act immediately on process standard streams or native console state; they do not maintain a hidden Canvas.

### Example

```c
/* Exemplo isolado: handles e Canvas citados precisam estar inicializados. */
OPSTATUS status = TERMINAL_FUNC(Flush)();
```

# Cursor_GetPosition

Queries cursor position.

### Syntax

#### Macro form

```c
OPSTATUS TERMINAL_FUNC(Cursor_GetPosition)(TDUAL_TYPE(uint16) *outPosition);
```

#### Direct form

```c
OPSTATUS Terminal_Cursor_GetPosition(TDual_uint16 *outPosition);
```

### Parameters

- `outPosition`: receives cursor position.

### Return value

`OPSTATUS`: SUCCESS if completed; errors include invalid pointers, out-of-range coordinates, unsupported console capabilities and I/O failures as applicable.

### Remarks

Terminal operations act immediately on process standard streams or native console state; they do not maintain a hidden Canvas.

### Example

```c
/* Exemplo isolado: handles e Canvas citados precisam estar inicializados. */
TDUAL_TYPE(uint16) coords = {0};
OPSTATUS status = TERMINAL_FUNC(Cursor_GetPosition)(&coords);
```

# Cursor_SetPosition

Sets absolute cursor position.

### Syntax

#### Macro form

```c
OPSTATUS TERMINAL_FUNC(Cursor_SetPosition)(TDUAL_TYPE(uint16) position);
```

#### Direct form

```c
OPSTATUS Terminal_Cursor_SetPosition(TDual_uint16 position);
```

### Parameters

- `position`: cell position in zero-based coordinates.

### Return value

`OPSTATUS`: SUCCESS if completed; errors include invalid pointers, out-of-range coordinates, unsupported console capabilities and I/O failures as applicable.

### Remarks

Terminal operations act immediately on process standard streams or native console state; they do not maintain a hidden Canvas.

### Example

```c
/* Exemplo isolado: handles e Canvas citados precisam estar inicializados. */
OPSTATUS status = TERMINAL_FUNC(Cursor_SetPosition)((TDUAL_TYPE(uint16)){.x=2,.y=2});
```

# Cursor_Move

Moves cursor relatively.

### Syntax

#### Macro form

```c
OPSTATUS TERMINAL_FUNC(Cursor_Move)(int32_t deltaX, int32_t deltaY);
```

#### Direct form

```c
OPSTATUS Terminal_Cursor_Move(int32_t deltaX, int32_t deltaY);
```

### Parameters

- `deltaX`: signed cursor displacement horizontally.
- `deltaY`: signed cursor displacement vertically.

### Return value

`OPSTATUS`: SUCCESS if completed; errors include invalid pointers, out-of-range coordinates, unsupported console capabilities and I/O failures as applicable.

### Remarks

Terminal operations act immediately on process standard streams or native console state; they do not maintain a hidden Canvas.

### Example

```c
/* Exemplo isolado: handles e Canvas citados precisam estar inicializados. */
OPSTATUS status = TERMINAL_FUNC(Cursor_Move)(1, 0);
```

# Cursor_Show

Shows cursor.

### Syntax

#### Macro form

```c
OPSTATUS TERMINAL_FUNC(Cursor_Show)(void);
```

#### Direct form

```c
OPSTATUS Terminal_Cursor_Show(void);
```

### Parameters

- None.

### Return value

`OPSTATUS`: SUCCESS if completed; errors include invalid pointers, out-of-range coordinates, unsupported console capabilities and I/O failures as applicable.

### Remarks

Terminal operations act immediately on process standard streams or native console state; they do not maintain a hidden Canvas.

### Example

```c
/* Exemplo isolado: handles e Canvas citados precisam estar inicializados. */
OPSTATUS status = TERMINAL_FUNC(Cursor_Show)();
```

# Cursor_Hide

Hides cursor.

### Syntax

#### Macro form

```c
OPSTATUS TERMINAL_FUNC(Cursor_Hide)(void);
```

#### Direct form

```c
OPSTATUS Terminal_Cursor_Hide(void);
```

### Parameters

- None.

### Return value

`OPSTATUS`: SUCCESS if completed; errors include invalid pointers, out-of-range coordinates, unsupported console capabilities and I/O failures as applicable.

### Remarks

Terminal operations act immediately on process standard streams or native console state; they do not maintain a hidden Canvas.

### Example

```c
/* Exemplo isolado: handles e Canvas citados precisam estar inicializados. */
OPSTATUS status = TERMINAL_FUNC(Cursor_Hide)();
```

# Size_Get

Obtains terminal dimensions.

### Syntax

#### Macro form

```c
OPSTATUS TERMINAL_FUNC(Size_Get)(TDUAL_TYPE(uint16) *outSize);
```

#### Direct form

```c
OPSTATUS Terminal_Size_Get(TDual_uint16 *outSize);
```

### Parameters

- `outSize`: output width and height.

### Return value

`OPSTATUS`: SUCCESS if completed; errors include invalid pointers, out-of-range coordinates, unsupported console capabilities and I/O failures as applicable.

### Remarks

Terminal operations act immediately on process standard streams or native console state; they do not maintain a hidden Canvas.

### Example

```c
/* Exemplo isolado: handles e Canvas citados precisam estar inicializados. */
TDUAL_TYPE(uint16) coords = {0};
OPSTATUS status = TERMINAL_FUNC(Size_Get)(&coords);
```

# Clear

Clears cells to the default state.

### Syntax

#### Macro form

```c
OPSTATUS TERMINAL_FUNC(Clear)(void);
```

#### Direct form

```c
OPSTATUS Terminal_Clear(void);
```

### Parameters

- None.

### Return value

`OPSTATUS`: SUCCESS if completed; errors include invalid pointers, out-of-range coordinates, unsupported console capabilities and I/O failures as applicable.

### Remarks

Terminal operations act immediately on process standard streams or native console state; they do not maintain a hidden Canvas.

### Example

```c
/* Exemplo isolado: handles e Canvas citados precisam estar inicializados. */
OPSTATUS status = TERMINAL_FUNC(Clear)();
```

# ClearLine

Clears the current terminal line.

### Syntax

#### Macro form

```c
OPSTATUS TERMINAL_FUNC(ClearLine)(void);
```

#### Direct form

```c
OPSTATUS Terminal_ClearLine(void);
```

### Parameters

- None.

### Return value

`OPSTATUS`: SUCCESS if completed; errors include invalid pointers, out-of-range coordinates, unsupported console capabilities and I/O failures as applicable.

### Remarks

Terminal operations act immediately on process standard streams or native console state; they do not maintain a hidden Canvas.

### Example

```c
/* Exemplo isolado: handles e Canvas citados precisam estar inicializados. */
OPSTATUS status = TERMINAL_FUNC(ClearLine)();
```

# Title_Set

Updates the title of the supported terminal.

### Syntax

#### Macro form

```c
OPSTATUS TERMINAL_FUNC(Title_Set)(const char *title);
```

#### Direct form

```c
OPSTATUS Terminal_Title_Set(const char *title);
```

### Parameters

- `title`: terminal title.

### Return value

`OPSTATUS`: SUCCESS if completed; errors include invalid pointers, out-of-range coordinates, unsupported console capabilities and I/O failures as applicable.

### Remarks

Terminal operations act immediately on process standard streams or native console state; they do not maintain a hidden Canvas.

### Example

```c
/* Exemplo isolado: handles e Canvas citados precisam estar inicializados. */
OPSTATUS status = TERMINAL_FUNC(Title_Set)("Cosmeron");
```

# Bell

Emits terminal bell event.

### Syntax

#### Macro form

```c
OPSTATUS TERMINAL_FUNC(Bell)(void);
```

#### Direct form

```c
OPSTATUS Terminal_Bell(void);
```

### Parameters

- None.

### Return value

`OPSTATUS`: SUCCESS if completed; errors include invalid pointers, out-of-range coordinates, unsupported console capabilities and I/O failures as applicable.

### Remarks

Terminal operations act immediately on process standard streams or native console state; they do not maintain a hidden Canvas.

### Example

```c
/* Exemplo isolado: handles e Canvas citados precisam estar inicializados. */
OPSTATUS status = TERMINAL_FUNC(Bell)();
```

# Style_Set

Sets active text style.

### Syntax

#### Macro form

```c
OPSTATUS TERMINAL_FUNC(Style_Set)(TEXT_GRID_ATTRIBUTE_TYPE(Cell) style);
```

#### Direct form

```c
OPSTATUS Terminal_Style_Set(Text_Grid_Attribute_Cell style);
```

### Parameters

- `style`: Text/Grid cell attribute, not a terminal-global style.

### Return value

`OPSTATUS`: SUCCESS if completed; errors include invalid pointers, out-of-range coordinates, unsupported console capabilities and I/O failures as applicable.

### Remarks

Terminal operations act immediately on process standard streams or native console state; they do not maintain a hidden Canvas.

### Example

```c
/* Exemplo isolado: handles e Canvas citados precisam estar inicializados. */
OPSTATUS status = TERMINAL_FUNC(Style_Set)(TEXT_GRID_ATTRIBUTE_FUNC(Default)());
```

# Style_Get

Gets active text style.

### Syntax

#### Macro form

```c
OPSTATUS TERMINAL_FUNC(Style_Get)(TEXT_GRID_ATTRIBUTE_TYPE(Cell) *outStyle);
```

#### Direct form

```c
OPSTATUS Terminal_Style_Get(Text_Grid_Attribute_Cell *outStyle);
```

### Parameters

- `outStyle`: receives style.

### Return value

`OPSTATUS`: SUCCESS if completed; errors include invalid pointers, out-of-range coordinates, unsupported console capabilities and I/O failures as applicable.

### Remarks

Terminal operations act immediately on process standard streams or native console state; they do not maintain a hidden Canvas.

### Example

```c
/* Exemplo isolado: handles e Canvas citados precisam estar inicializados. */
TEXT_GRID_ATTRIBUTE_TYPE(Cell) style = {0};
OPSTATUS status = TERMINAL_FUNC(Style_Get)(&style);
```

# Style_Reset

Restores the default text style.

### Syntax

#### Macro form

```c
OPSTATUS TERMINAL_FUNC(Style_Reset)(void);
```

#### Direct form

```c
OPSTATUS Terminal_Style_Reset(void);
```

### Parameters

- None.

### Return value

`OPSTATUS`: SUCCESS if completed; errors include invalid pointers, out-of-range coordinates, unsupported console capabilities and I/O failures as applicable.

### Remarks

Terminal operations act immediately on process standard streams or native console state; they do not maintain a hidden Canvas.

### Example

```c
/* Exemplo isolado: handles e Canvas citados precisam estar inicializados. */
OPSTATUS status = TERMINAL_FUNC(Style_Reset)();
```

# Color_Set

Sets foreground and background colors.

### Syntax

#### Macro form

```c
OPSTATUS TERMINAL_FUNC(Color_Set)(TEXT_GRID_COLOR_TYPE(RGB) foreground, TEXT_GRID_COLOR_TYPE(RGB) background);
```

#### Direct form

```c
OPSTATUS Terminal_Color_Set(Text_Grid_Color_RGB foreground, Text_Grid_Color_RGB background);
```

### Parameters

- `foreground`: foreground RGB.
- `background`: background RGB.

### Return value

`OPSTATUS`: SUCCESS if completed; errors include invalid pointers, out-of-range coordinates, unsupported console capabilities and I/O failures as applicable.

### Remarks

Terminal operations act immediately on process standard streams or native console state; they do not maintain a hidden Canvas.

### Example

```c
/* Exemplo isolado: handles e Canvas citados precisam estar inicializados. */
OPSTATUS status = TERMINAL_FUNC(Color_Set)(color, color);
```

# Color_Reset

Restores default colors.

### Syntax

#### Macro form

```c
OPSTATUS TERMINAL_FUNC(Color_Reset)(void);
```

#### Direct form

```c
OPSTATUS Terminal_Color_Reset(void);
```

### Parameters

- None.

### Return value

`OPSTATUS`: SUCCESS if completed; errors include invalid pointers, out-of-range coordinates, unsupported console capabilities and I/O failures as applicable.

### Remarks

Terminal operations act immediately on process standard streams or native console state; they do not maintain a hidden Canvas.

### Example

```c
/* Exemplo isolado: handles e Canvas citados precisam estar inicializados. */
OPSTATUS status = TERMINAL_FUNC(Color_Reset)();
```

# Attribute_Set

Replaces text attribute flags.

### Syntax

#### Macro form

```c
OPSTATUS TERMINAL_FUNC(Attribute_Set)(TEXT_GRID_ATTRIBUTE_TYPE(Flags) attributes);
```

#### Direct form

```c
OPSTATUS Terminal_Attribute_Set(Text_Grid_Attribute_Flags attributes);
```

### Parameters

- `attributes`: text attribute flags.

### Return value

`OPSTATUS`: SUCCESS if completed; errors include invalid pointers, out-of-range coordinates, unsupported console capabilities and I/O failures as applicable.

### Remarks

Terminal operations act immediately on process standard streams or native console state; they do not maintain a hidden Canvas.

### Example

```c
/* Exemplo isolado: handles e Canvas citados precisam estar inicializados. */
OPSTATUS status = TERMINAL_FUNC(Attribute_Set)(flags);
```

# Attribute_Add

Adds text attribute flags.

### Syntax

#### Macro form

```c
OPSTATUS TERMINAL_FUNC(Attribute_Add)(TEXT_GRID_ATTRIBUTE_TYPE(Flags) attributes);
```

#### Direct form

```c
OPSTATUS Terminal_Attribute_Add(Text_Grid_Attribute_Flags attributes);
```

### Parameters

- `attributes`: text attribute flags.

### Return value

`OPSTATUS`: SUCCESS if completed; errors include invalid pointers, out-of-range coordinates, unsupported console capabilities and I/O failures as applicable.

### Remarks

Terminal operations act immediately on process standard streams or native console state; they do not maintain a hidden Canvas.

### Example

```c
/* Exemplo isolado: handles e Canvas citados precisam estar inicializados. */
OPSTATUS status = TERMINAL_FUNC(Attribute_Add)(flags);
```

# Attribute_Remove

Removes selected attribute flags.

### Syntax

#### Macro form

```c
OPSTATUS TERMINAL_FUNC(Attribute_Remove)(TEXT_GRID_ATTRIBUTE_TYPE(Flags) attributes);
```

#### Direct form

```c
OPSTATUS Terminal_Attribute_Remove(Text_Grid_Attribute_Flags attributes);
```

### Parameters

- `attributes`: text attribute flags.

### Return value

`OPSTATUS`: SUCCESS if completed; errors include invalid pointers, out-of-range coordinates, unsupported console capabilities and I/O failures as applicable.

### Remarks

Terminal operations act immediately on process standard streams or native console state; they do not maintain a hidden Canvas.

### Example

```c
/* Exemplo isolado: handles e Canvas citados precisam estar inicializados. */
OPSTATUS status = TERMINAL_FUNC(Attribute_Remove)(flags);
```

# Attribute_Reset

Restores default flags.

### Syntax

#### Macro form

```c
OPSTATUS TERMINAL_FUNC(Attribute_Reset)(void);
```

#### Direct form

```c
OPSTATUS Terminal_Attribute_Reset(void);
```

### Parameters

- None.

### Return value

`OPSTATUS`: SUCCESS if completed; errors include invalid pointers, out-of-range coordinates, unsupported console capabilities and I/O failures as applicable.

### Remarks

Terminal operations act immediately on process standard streams or native console state; they do not maintain a hidden Canvas.

### Example

```c
/* Exemplo isolado: handles e Canvas citados precisam estar inicializados. */
OPSTATUS status = TERMINAL_FUNC(Attribute_Reset)();
```

# Capabilities_Get

Reports terminal/backend capabilities.

### Syntax

#### Macro form

```c
OPSTATUS TERMINAL_FUNC(Capabilities_Get)(TERMINAL_TYPE(TCapabilities) *outCapabilities);
```

#### Direct form

```c
OPSTATUS Terminal_Capabilities_Get(Terminal_TCapabilities *outCapabilities);
```

### Parameters

- `outCapabilities`: receives terminal capabilities.

### Return value

`OPSTATUS`: SUCCESS if completed; errors include invalid pointers, out-of-range coordinates, unsupported console capabilities and I/O failures as applicable.

### Remarks

Terminal operations act immediately on process standard streams or native console state; they do not maintain a hidden Canvas.

### Example

```c
/* Exemplo isolado: handles e Canvas citados precisam estar inicializados. */
TERMINAL_TYPE(TCapabilities) capabilities = {0};
OPSTATUS status = TERMINAL_FUNC(Capabilities_Get)(&capabilities);
```

# Event_Poll

Tries to obtain normalized input without blocking.

### Syntax

#### Macro form

```c
OPSTATUS TERMINAL_FUNC(Event_Poll)(TERMINAL_TYPE(TEvent) *outEvent, bool *outAvailable);
```

#### Direct form

```c
OPSTATUS Terminal_Event_Poll(Terminal_TEvent *outEvent, bool *outAvailable);
```

### Parameters

- `outEvent`: receives normalized input event.
- `outAvailable`: whether an input event/key was available.

### Return value

`OPSTATUS`: SUCCESS if completed; errors include invalid pointers, out-of-range coordinates, unsupported console capabilities and I/O failures as applicable.

### Remarks

Terminal operations act immediately on process standard streams or native console state; they do not maintain a hidden Canvas. Poll returns an availability flag without inventing an event on empty queues.

### Example

```c
/* Exemplo isolado: handles e Canvas citados precisam estar inicializados. */
TERMINAL_TYPE(TEvent) event = {0};
bool available = false;
OPSTATUS status = TERMINAL_FUNC(Event_Poll)(&event, &available);
```

# Mouse_Enable

Enables or disables pointer events in the terminal.

### Syntax

#### Macro form

```c
OPSTATUS TERMINAL_FUNC(Mouse_Enable)(bool enabled);
```

#### Direct form

```c
OPSTATUS Terminal_Mouse_Enable(bool enabled);
```

### Parameters

- `enabled`: turns mouse support on or off.

### Return value

`OPSTATUS`: SUCCESS if completed; errors include invalid pointers, out-of-range coordinates, unsupported console capabilities and I/O failures as applicable.

### Remarks

Terminal operations act immediately on process standard streams or native console state; they do not maintain a hidden Canvas.

### Example

```c
/* Exemplo isolado: handles e Canvas citados precisam estar inicializados. */
OPSTATUS status = TERMINAL_FUNC(Mouse_Enable)(true);
```

# Key_Get

Reads a key event.

### Syntax

#### Macro form

```c
OPSTATUS TERMINAL_FUNC(Key_Get)(TERMINAL_TYPE(TKey) *outKey);
```

#### Direct form

```c
OPSTATUS Terminal_Key_Get(Terminal_TKey *outKey);
```

### Parameters

- `outKey`: receives keyboard key event.

### Return value

`OPSTATUS`: SUCCESS if completed; errors include invalid pointers, out-of-range coordinates, unsupported console capabilities and I/O failures as applicable.

### Remarks

Terminal operations act immediately on process standard streams or native console state; they do not maintain a hidden Canvas.

### Example

```c
/* Exemplo isolado: handles e Canvas citados precisam estar inicializados. */
TERMINAL_TYPE(TKey) key = {0};
OPSTATUS status = TERMINAL_FUNC(Key_Get)(&key);
```

# Key_GetEcho

Reads a key event with echo.

### Syntax

#### Macro form

```c
OPSTATUS TERMINAL_FUNC(Key_GetEcho)(TERMINAL_TYPE(TKey) *outKey);
```

#### Direct form

```c
OPSTATUS Terminal_Key_GetEcho(Terminal_TKey *outKey);
```

### Parameters

- `outKey`: receives keyboard key event.

### Return value

`OPSTATUS`: SUCCESS if completed; errors include invalid pointers, out-of-range coordinates, unsupported console capabilities and I/O failures as applicable.

### Remarks

Terminal operations act immediately on process standard streams or native console state; they do not maintain a hidden Canvas.

### Example

```c
/* Exemplo isolado: handles e Canvas citados precisam estar inicializados. */
TERMINAL_TYPE(TKey) key = {0};
OPSTATUS status = TERMINAL_FUNC(Key_GetEcho)(&key);
```

# Key_Hit

Tests whether a key is available.

### Syntax

#### Macro form

```c
OPSTATUS TERMINAL_FUNC(Key_Hit)(bool *outAvailable);
```

#### Direct form

```c
OPSTATUS Terminal_Key_Hit(bool *outAvailable);
```

### Parameters

- `outAvailable`: whether an input event/key was available.

### Return value

`OPSTATUS`: SUCCESS if completed; errors include invalid pointers, out-of-range coordinates, unsupported console capabilities and I/O failures as applicable.

### Remarks

Terminal operations act immediately on process standard streams or native console state; they do not maintain a hidden Canvas.

### Example

```c
/* Exemplo isolado: handles e Canvas citados precisam estar inicializados. */
bool available = false;
OPSTATUS status = TERMINAL_FUNC(Key_Hit)(&available);
```

# InputMode_Get

Reads active input mode.

### Syntax

#### Macro form

```c
OPSTATUS TERMINAL_FUNC(InputMode_Get)(TERMINAL_TYPE(InputMode) *outMode);
```

#### Direct form

```c
OPSTATUS Terminal_InputMode_Get(Terminal_InputMode *outMode);
```

### Parameters

- `outMode`: receives input mode.

### Return value

`OPSTATUS`: SUCCESS if completed; errors include invalid pointers, out-of-range coordinates, unsupported console capabilities and I/O failures as applicable.

### Remarks

Terminal operations act immediately on process standard streams or native console state; they do not maintain a hidden Canvas.

### Example

```c
/* Exemplo isolado: handles e Canvas citados precisam estar inicializados. */
TERMINAL_TYPE(InputMode) mode;
OPSTATUS status = TERMINAL_FUNC(InputMode_Get)(&mode);
```

# InputMode_Set

Sets active input mode.

### Syntax

#### Macro form

```c
OPSTATUS TERMINAL_FUNC(InputMode_Set)(TERMINAL_TYPE(InputMode) mode);
```

#### Direct form

```c
OPSTATUS Terminal_InputMode_Set(Terminal_InputMode mode);
```

### Parameters

- `mode`: desired input mode.

### Return value

`OPSTATUS`: SUCCESS if completed; errors include invalid pointers, out-of-range coordinates, unsupported console capabilities and I/O failures as applicable.

### Remarks

Terminal operations act immediately on process standard streams or native console state; they do not maintain a hidden Canvas.

### Example

```c
/* Exemplo isolado: handles e Canvas citados precisam estar inicializados. */
OPSTATUS status = TERMINAL_FUNC(InputMode_Set)(mode);
```

# See also

- [UI](UI.md), [GUI](GUI.md), [Text](Text.md), [Struct](Struct.md) and [Bit](Bit.md).
