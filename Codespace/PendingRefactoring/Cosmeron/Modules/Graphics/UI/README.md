# UI — terminal user interface package

**Implementation available in [UI.h](UI.h).** Includes immediate-mode widgets, keyboard/mouse events, resizes, scrollable lists, themes, layouts and UTF-8 editing. See [Docs/Modules/UI.md](../../../../../Docs/Modules/UI.md) for configuration.

## Objective

Implement retained/immediate-friendly TUI widgets using **Canvas as the rendering target** and **Terminal as the input/event source**, never a separate native terminal screen buffer. The UI layer must not own the process standard handles.

## Dependencies and reuse

- `Graphics/Canvas`: drawing, clipping, cropped or borrowed nested views, explicit update.
- `Graphics/Terminal`: native keyboard/mouse/resize events and console mode, capabilities.
- `Struct/TDual.h`, `Struct/TQuad.h`: use `TDUAL_TYPE(uint16)` positions/dimensions and inclusive `TQUAD_TYPE(uint16)` rects; **do not invent `TPosition`, `TSize`, `TRect`**.
- `Text/Grid` / `Text/Unicode`: glyphs, cell attributes, Unicode widths.
- Optional `Container` for collections and `Chronometry` for animation/timers.

## Proposed subpackages

| Package | Work |
|---|---|
| Layout | rows, columns, panels, measuring, clipping, nested views |
| Component | button, label, checkbox, text input, list, scrollbar, textbox |
| Focus | focus traversal, keyboard shortcuts and modality |
| Event | normalized key, mouse and resize messages |
| Theme | use Text/Grid cell attributes; no duplicate Style representation |
| Render | render components to a Canvas, then explicitly update the target |

## Implemented public signatures (see UI.h)

```c
OPSTATUS UI_FUNC(Layout)(
    UI_TYPE(TContext) *context, TDUAL_TYPE(uint16) viewportSize);
OPSTATUS UI_FUNC(Render)(
    const UI_TYPE(TContext) *context, TERMINAL_TYPE(TCanvas) *canvas);
OPSTATUS UI_FUNC(Dispatch)(
    UI_TYPE(TContext) *context, const UI_TYPE(TEvent) *event);
OPSTATUS UI_FUNC(Focus_Set)(
    UI_TYPE(TContext) *context, UI_TYPE(TWidgetId) widget);
```

Widget types and context are genuine UI concepts, not duplicated geometric primitives. All package function naming must follow PATTERN.md during implementation.

## Contracts

1. Rendering never directly modifies stdout; only Canvas_Update/Terminal immediate functions do.
2. Layout and child clipping operate with borrowed canvas views; nested view lifetime rules apply.
3. UI must support terminal resizing and keyboard-only control.
4. Avoid curses-style mandatory terminal initialization; any optional UI state is app-owned.
5. The `Terminal_Input` prompt API and interactive UI textbox are separate, composable facilities.

## Acceptance tests

Nested layout clipping; focus order; UTF-8 and double-width glyphs; resize; no output until Canvas_Update; keyboard event handling across POSIX and Windows.
