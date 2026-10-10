# Graphics — implementation project

Status: **Terminal and Canvas foundations implemented**. UI, Rasterization, Raycasting and Pseudo3DRoad remain design specifications.

## Purpose

Build a portable C11, header-only terminal/graphics stack that combines the convenience of `stdio.h`, `conio.h`, WinCon, Python `input()` and Go `fmt`, with retained-mode textual rendering similar to ncurses.

**Two equal output paths:**

~~~text
Application
  |-- Terminal (immediate I/O) --------------------------|
  |-- Canvas (write in memory -> Update) ---------------|-> TTerminal -> backend
  |           ^                                          |
  |      UI / Rasterization / Raycasting / Pseudo3DRoad -|
  +-- keyboard / standard handles -----------------------|
~~~

- `Terminal_Print(...)` writes immediately, without `Init` or `Free` for the standard terminal.
- `Terminal_Canvas_Print(...)` modifies a canvas only; `Terminal_Canvas_Update(...)` presents it.
- There is **no mandatory public terminal buffer**. A canvas is an independent drawable surface, **not** an alias for a native console screen buffer.
- Canvas and direct output share terminal capabilities, formatting/Unicode policies and platform normalization.
- Backend-local queues, formatting scratch space or dirty-region caches are permitted, but are private implementation details.
- The library owns the *abstractions* for stdin/stdout/stderr and standard handles; the underlying process resources are **borrowed**, never closed by library cleanup.
- Canvas works without an attached terminal, and its views may be composed recursively.

## Packages

| Directory | Responsibility | Depends on |
|---|---|---|
| [Terminal](Terminal/README.md) | Standard I/O, keyboard, console state, one-shot drawing, normalized backends | Core, Struct, Text |
| [Canvas](Canvas/README.md) | In-memory textual surfaces, views, composition, explicit presentation | Struct, Text/Grid; Terminal only for presentation |
| [UI](UI/README.md) | TUI layout, widgets, focus and event processing | Canvas, Terminal input |
| [Rasterization](Rasterization/README.md) | Geometry-to-cells primitives, line/shape/fill operations | Canvas, Struct, Math as needed |
| [Raycasting](Raycasting/README.md) | Ray traversal, intersections and projections | Math/Struct, Rasterization/Canvas |
| [Pseudo3DRoad](Pseudo3DRoad/README.md) | Perspective road/terrain projection and frame drawing | Math/Struct, Rasterization/Canvas |

## Mandatory reuse: do not reinvent Struct or Text

The **existing** `Modules/Struct/TDual.h` and `TQuad.h` provide geometry:
- `TDUAL_TYPE(uint16)` for terminal/grid column+row, width+height, and unsigned positions.
- `TQUAD_TYPE(uint16)` for rectangles using `left, right, top, bottom`, **inclusive** boundaries, matching the current Text/Grid implementation.
- Use existing `TDUAL_TYPE(int32)`, `TDUAL_TYPE(float)`, `TTRIPLE_TYPE(float)`, etc. in mathematical packages when appropriate; never introduce parallel `TPosition`, `TSize`, `TRect` or `TPoint` structs just for Graphics.
- Existing `Text/Grid` owns character/attribute storage and cell composition. Prefer `TEXT_GRID_TYPE(char32)`, `TEXT_GRID_ATTRIBUTE_TYPE(Cell)`, `TEXT_GRID_COLOR_TYPE(RGB)`, Unicode encoding/width routines. Do not duplicate `TCell`, `TStyle`, RGB representations or encoding utilities without a demonstrated incompatibility.
- Implement views as borrowed **descriptors**, accounting for origin and stride over the underlying grid. Current Text/Grid functions assume owned contiguous backing; do not pretend that they already accept non-contiguous views.

## Namespace and public contract

Follow [PATTERN.md](../../../../Docs/Reference/PATTERN.md) (repository-relative source: `Docs/Reference/PATTERN.md`): `*_NS`, `*_TYPE`, `*_FUNC`, `*_CONST`; one readable `*_PROTOTYPE` macro per public function; `OPSTATUS` for fallible operations; outputs via `out` parameters; `camelCase` parameter names. Illustrative logical function names in package docs are `TERMINAL_FUNC(Print)`, `TERMINAL_FUNC(Canvas_Print)` etc. Actual include/API wiring is an implementation task.

This is a **proposed contract**, not a compilation-ready header.

## Portability and lifecycle

- C11, Windows Console + VT, POSIX/ANSI (Linux, macOS, BSD); SDL is an **optional presentation backend**, not an OS.
- No mandatory additional link flags (`-lm`, `-pthread`, `-lws2_32`) for the standard implementation.
- Detect redirection: plain pipes/files are still valid for byte I/O; cursor, color and terminal-only features return a capability/status error or documented fallback.
- Standard terminal and standard handles require neither `Create` nor `Free`. User-created canvases have explicit ownership.
- Immediate writes and canvas presentation must coordinate screen-state invalidation. No stale dirty-cache optimization may suppress later user-requested updates.
- One-shot print-at/styled operations preserve the persistent **library-managed** cursor/style. External applications writing to the same terminal are outside this guarantee.

## Delivery sequence

1. Terminal standard I/O + backend capability normalization and conio modes.
2. Canvas adapter backed by existing Text/Grid; basic drawing and full Update.
3. Borrowed Canvas views, clipping and composition; tests for nested views.
4. Partial-update optimization with safe invalidation; color/Unicode portability tests.
5. UI and rasterization; then raycasting and pseudo-3D road demos.

Every package must have focused tests, Linux GCC/Clang, macOS Clang, Windows MinGW/MSVC checks, and examples in **both macro and direct invocation** styles.
