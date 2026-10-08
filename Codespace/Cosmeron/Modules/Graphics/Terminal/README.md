# Terminal — immediate I/O and console foundation

**Design only.** Goal: a friendly, initialized-by-process, `stdio`-like API that can also perform direct positioned/styled console writes (WinCon/conio) while sharing output capabilities with Canvas. No public console buffer is required.

## Packages planned inside Terminal

| Component | Responsibilities |
|---|---|
| Standard | Borrowed standard streams/handles, terminal capabilities, redirected I/O |
| IO | Print/PrintLn/Read/Write/Scan/Input/ReadLine/Flush |
| Console | cursor, clear, dimensions, style, title, bell |
| Keyboard | raw/cbreak/canonical input modes, key detection, polling |
| Backend | private OS/VT/ANSI conversion and output normalization |

## Reused types

~~~c
/* Existing geometry; NO Terminal_Position/Size/Rect structs. */
TDUAL_TYPE(uint16) position;  /* .x / .y or .col / .row */
TDUAL_TYPE(uint16) size;      /* .col / .row */

/* Existing Text attributes combine RGB, flags, underline and defaults. */
TEXT_GRID_ATTRIBUTE_TYPE(Cell) style;
TEXT_GRID_COLOR_TYPE(RGB) rgb;
~~~

The following logical handle/key objects belong to Terminal because they are **not** geometric or cell-data duplicates:
~~~c
TERMINAL_TYPE(TTerminal);
TERMINAL_TYPE(THandle);
TERMINAL_TYPE(TKey);
TERMINAL_TYPE(InputMode);
TERMINAL_TYPE(StandardStream);
~~~

### Suggested signatures

All fallible calls return `OPSTATUS`. The examples describe *signatures to implement*, not functions present today.

~~~c
/* Standard: borrowed, implicitly available. */
TERMINAL_TYPE(TTerminal) *TERMINAL_FUNC(Standard)(void);
TERMINAL_TYPE(THandle) *TERMINAL_FUNC(StandardHandle)(
    TERMINAL_TYPE(StandardStream) stream);

/* Output and formatting */
OPSTATUS TERMINAL_FUNC(Print)(const char *format, ...);
OPSTATUS TERMINAL_FUNC(PrintLn)(const char *format, ...);
OPSTATUS TERMINAL_FUNC(PrintAt)(
    TDUAL_TYPE(uint16) position, const char *format, ...);
OPSTATUS TERMINAL_FUNC(PrintStyled)(
    TEXT_GRID_ATTRIBUTE_TYPE(Cell) style, const char *format, ...);
OPSTATUS TERMINAL_FUNC(PrintStyledLn)(
    TEXT_GRID_ATTRIBUTE_TYPE(Cell) style, const char *format, ...);
OPSTATUS TERMINAL_FUNC(PrintStyledAt)(
    TDUAL_TYPE(uint16) position,
    TEXT_GRID_ATTRIBUTE_TYPE(Cell) style,
    const char *format, ...);

/* Raw bytes and text input */
OPSTATUS TERMINAL_FUNC(Write)(const void *data, size_t size);
OPSTATUS TERMINAL_FUNC(Read)(
    void *buffer, size_t capacity, size_t *outRead);
OPSTATUS TERMINAL_FUNC(WriteLine)(const char *text);
OPSTATUS TERMINAL_FUNC(ReadLine)(
    char *buffer, size_t capacity, size_t *outLength);
OPSTATUS TERMINAL_FUNC(Scan)(const char *format, ...);
OPSTATUS TERMINAL_FUNC(Input)(
    const char *prompt, char *buffer, size_t capacity);
OPSTATUS TERMINAL_FUNC(PutChar)(uint32_t codepoint);
OPSTATUS TERMINAL_FUNC(GetChar)(uint32_t *outCodepoint);
OPSTATUS TERMINAL_FUNC(Flush)(void);

/* Explicit handle advanced API */
OPSTATUS TERMINAL_FUNC(PrintTo)(
    TERMINAL_TYPE(THandle) *handle, const char *format, ...);
OPSTATUS TERMINAL_FUNC(WriteTo)(
    TERMINAL_TYPE(THandle) *handle,
    const void *data, size_t size, size_t *outWritten);
OPSTATUS TERMINAL_FUNC(ReadFrom)(
    TERMINAL_TYPE(THandle) *handle,
    void *buffer, size_t capacity, size_t *outRead);
OPSTATUS TERMINAL_FUNC(FlushHandle)(
    TERMINAL_TYPE(THandle) *handle);

/* Console state */
OPSTATUS TERMINAL_FUNC(Cursor_GetPosition)(
    TDUAL_TYPE(uint16) *outPosition);
OPSTATUS TERMINAL_FUNC(Cursor_SetPosition)(TDUAL_TYPE(uint16) position);
OPSTATUS TERMINAL_FUNC(Cursor_Move)(int32_t deltaX, int32_t deltaY);
OPSTATUS TERMINAL_FUNC(Cursor_Show)(void);
OPSTATUS TERMINAL_FUNC(Cursor_Hide)(void);
OPSTATUS TERMINAL_FUNC(Size_Get)(TDUAL_TYPE(uint16) *outSize);
OPSTATUS TERMINAL_FUNC(Clear)(void);
OPSTATUS TERMINAL_FUNC(ClearLine)(void);
OPSTATUS TERMINAL_FUNC(Title_Set)(const char *title);
OPSTATUS TERMINAL_FUNC(Bell)(void);

/* Style is the existing Text/Grid cell attribute value. */
OPSTATUS TERMINAL_FUNC(Style_Set)(TEXT_GRID_ATTRIBUTE_TYPE(Cell) style);
OPSTATUS TERMINAL_FUNC(Style_Get)(
    TEXT_GRID_ATTRIBUTE_TYPE(Cell) *outStyle);
OPSTATUS TERMINAL_FUNC(Style_Reset)(void);
OPSTATUS TERMINAL_FUNC(Color_Set)(
    TEXT_GRID_COLOR_TYPE(RGB) foreground,
    TEXT_GRID_COLOR_TYPE(RGB) background);
OPSTATUS TERMINAL_FUNC(Color_Reset)(void);
OPSTATUS TERMINAL_FUNC(Attribute_Set)(
    TEXT_GRID_ATTRIBUTE_TYPE(Flags) attributes);
OPSTATUS TERMINAL_FUNC(Attribute_Add)(
    TEXT_GRID_ATTRIBUTE_TYPE(Flags) attributes);
OPSTATUS TERMINAL_FUNC(Attribute_Remove)(
    TEXT_GRID_ATTRIBUTE_TYPE(Flags) attributes);
OPSTATUS TERMINAL_FUNC(Attribute_Reset)(void);

/* Keyboard */
OPSTATUS TERMINAL_FUNC(Key_Get)(TERMINAL_TYPE(TKey) *outKey);
OPSTATUS TERMINAL_FUNC(Key_GetEcho)(TERMINAL_TYPE(TKey) *outKey);
OPSTATUS TERMINAL_FUNC(Key_Hit)(bool *outAvailable);
OPSTATUS TERMINAL_FUNC(InputMode_Get)(
    TERMINAL_TYPE(InputMode) *outMode);
OPSTATUS TERMINAL_FUNC(InputMode_Set)(
    TERMINAL_TYPE(InputMode) mode);
~~~

### Important semantics

| Function | Persistent cursor after call | Persistent style after call |
|---|---|---|
| `Print` / `PrintLn` | Advances | Unchanged |
| `PrintAt` | Unchanged | Unchanged |
| `PrintStyled` / `PrintStyledLn` | Advances | Unchanged |
| `PrintStyledAt` | Unchanged | Unchanged |
| `Cursor_SetPosition` | Replaced | Unchanged |
| `Style_Set` | Unchanged | Replaced |

- `PrintAt`/`PrintStyledAt` are **one-shot draw calls**; implement atomically at the backend abstraction layer when feasible, not necessarily by toggling shared state publicly.
- `Print` formats like `printf`; `Write` writes exact bytes including NUL. `WriteLine` writes a NUL-terminated string and newline.
- `Input`: print prompt, flush, read a line, consume trailing newline, NUL-terminate; handle EOF, empty input and truncation explicitly.
- `ReadLine` removes the newline on success; `Read` reports byte count in required `outRead`. Define EOF and partial-read behavior without a hidden last-error channel.
- `Scan` must protect users from unbounded conversion hazards; keep documented `scanf`-compatible subset and test format failures.
- `PutChar`/`GetChar` are Unicode text operations; raw bytes belong to `Write`/`Read`.
- `Key_Get` vs `GetChar`: raw key events versus text-stream character reading. Restore prior modes when temporary changes are necessary.
- Terminal capabilities (color depth, cursor addressing, interactivity) are probed; non-interactive stdout never silently emits raw escape sequences.
- No `Init`/`Free` for standard streams. Explicitly created nonstandard terminals can be considered later.
- Format implementation should reuse Core/Text routines and a common internal formatter sink to avoid duplicating formatting between Terminal and Canvas.

### Example (conceptual)

~~~c
TERMINAL_FUNC(PrintLn)("Ready");
TERMINAL_FUNC(PrintAt)((TDUAL_TYPE(uint16)){.x = 10, .y = 4},
                       "HP: %u", hp);
TERMINAL_FUNC(PrintStyled)(warningStyle, "Warning!");
TERMINAL_FUNC(Input)("Name: ", name, sizeof(name));
~~~

## Tests

Cover stream redirect/pipes, formatted input errors, truncation/EOF, Unicode width, cross-platform key modes, cursor/style preservation of one-shot calls, formatting/argument validation, unsupported capabilities and output synchronization with Canvas.
