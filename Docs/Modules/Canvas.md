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
