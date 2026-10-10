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
