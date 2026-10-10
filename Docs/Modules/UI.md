# UI — terminal widgets

Header: `Codespace/Cosmeron/Modules/Graphics/UI/UI.h`
Namespace: `UI_FUNC(...)`, `UI_TYPE(...)`, `UI_CONST(...)`.
Uses C11, header-only, without mandatory extra linker flags.

The UI borrows a `Terminal_TCanvas` for every frame. It never writes to stdout
or calls `Canvas_Update`; applications choose when to present. The context
is a zero-initializable struct and requires no `Init`/`Free`. Widget values
are held by the application; persistent structs group options but do not own
native windows or grids.

## API, direct and struct

```c
#include "Cosmeron/Modules/Graphics/UI/UI.h"

UI_TYPE(TContext) ui = {0};
TERMINAL_TYPE(TCanvas) canvas = {0};
UI_TYPE(TButton) button = {
    .region = { .left=2, .right=18, .top=2, .bottom=4 },
    .label = "Launch"
};
bool pressed = false;
TERMINAL_FUNC(Canvas_Create)(
    &canvas, (TDUAL_TYPE(uint16)){.col=40,.row=14});

/* In each frame; collect keys/mouse events before Begin. */
TERMINAL_FUNC(Canvas_Clear)(&canvas);
UI_FUNC(Begin)(&ui,&canvas);
UI_FUNC(ButtonEx)(&ui,&button);
UI_FUNC(Button)(&ui,
    (TQUAD_TYPE(uint16)){.left=2,.right=18,.top=6,.bottom=8},
    "Cancel",&pressed);
UI_FUNC(End)(&ui);
TERMINAL_FUNC(Canvas_Update)(&canvas);
/* On shutdown */
TERMINAL_FUNC(Canvas_Free)(&canvas);
```

Without a custom library namespace, these expand to
`UI_Begin`, `UI_ButtonEx`, `UI_Button`, `UI_End`, and
`Terminal_Canvas_Update`.

## Widget families

| Component | Direct | Struct |
|---|---|---|
| Label | Label | LabelEx / TLabel |
| Button | Button | ButtonEx / TButton |
| Checkbox | Checkbox | CheckboxEx / TCheckbox |
| Radio | Radio | RadioEx / TRadio |
| Slider | Slider | SliderEx / TSlider |
| Progress (fração 0..1) | Progress | ProgressEx / TProgress |
| ProgressBar (valor/máximo) | ProgressBar | ProgressBarEx / TProgressBar |
| Menu clássico de jogos | Menu | MenuEx / TMenu |
| InputText | InputText | InputTextEx / TInputText |
| TextArea | TextArea | TextAreaEx / TTextArea |
| List | List | ListEx / TList |
| Separator | Separator | SeparatorEx / TSeparator |
| Panel | Panel_Begin | Panel_BeginEx / TPanel |

A direct call constructs a temporary descriptor and delegates to the same Ex
implementation. The `pressed` flag is transient, while `checked`, `value`,
input buffers and selection survive across frames through caller-owned values.

## Themes and override order

`Theme_Default`, `Theme_Dark`, `Theme_Light`, `Theme_Classic`,
`Theme_Monochrome`, `Theme_Set`, `Theme_Get`, `Visual_Default`,
`Visual_RGB`, `Glyphs_ASCII`, `Glyphs_Unicode`,
`Visual_Push`, `Visual_Pop`.

All appearance cells are `TEXT_GRID_ATTRIBUTE_TYPE(Cell)`. Positions/sizes
reuse `TDUAL_TYPE(uint16)`, inclusive regions use `TQUAD_TYPE(uint16)`.
Precedence: component `.visual` > visual stack > theme visual. `Theme_Set`
copies the theme and must be called outside an active frame. A struct's
optional `.visual` pointer is borrowed for that widget call.

## Focus/events/layout

`Key`, `PointerMove`, `PointerButton`, `Scroll`,
`Focus_Set`, `Focus_Next`, `Focus_Previous`, `IsFocused`,
`GetId`, `PushId`, `PopId`, `Row_Begin`, `Column_Begin`,
`Layout_Next`, `Layout_Remaining`, `Layout_End`,
`Panel_Begin`, `Panel_BeginEx`, `Panel_End`.

Pass normalized keys through `UI_Key` before each frame. Tab advances focus
using the previous frame's visible order. Enter/Space activates focused
buttons. Sliders use left/right arrows, lists up/down. Text editors append
valid Unicode scalars, delete complete UTF-8 codepoints and use caller-owned
buffers. Multiline text area accepts Enter. Pointer coordinates are in root
canvas columns and rows; a mouse click requires down and release events.

IDs are stable hashes of labels, optionally scoped with `PushId`/`PopId`.
For unlabeled widgets such as Slider, InputText and List, specify an explicit
nonzero `.id` when several instances appear in the same scope, or use
`PushId` around each direct call. Duplicate IDs in a frame return
`ALREADY_EXISTS`. The bounded arrays in `TContext` can be configured
using `UI_MAX_WIDGETS`, `UI_MAX_KEYS`, `UI_MAX_SCOPES`.

Panels create borrowed Canvas views and must be closed with `Panel_End`.
Do not resize/free the owner while a panel or UI frame is active. `End`
requires all style, layout, panel and ID scopes to be balanced.

## Current limitations

- Native terminal mouse reporting/event polling must still be integrated.
- No terminal output is automatic; call `Terminal_Canvas_Update` yourself.
- Glyph width depends on terminal fonts; text shaping/combining marks are not
  implemented by the Canvas.
- Editing is currently append/backspace rather than an advanced cursor editor.
- The current list shows from its first item; scrollbar, scroll viewport,
  select-on-double-click and virtualized lists are later work.
- The UI is single-threaded and not a standalone GUI/windowing system.


## Native mouse, keyboard and terminal resize (October 2026)

`Terminal_Mouse_Enable(true)` enables SGR 1006 mouse reporting on
POSIX/ANSI consoles (motion/click/drag/wheel); Windows uses
`ReadConsoleInputW` and native console mouse events. Always disable it on
shutdown with `Terminal_Mouse_Enable(false)`. Normal exit also restores the
saved console mode. Mode setup requires interactive stdin AND stdout.

`UI_PollEvents(&ui, &eventCount)` consumes currently available normalized
`Terminal_TEvent` events; call **before** `UI_Begin`, not during a frame.
It dispatches Unicode text, special keys, mouse, wheel, and resize.
Use `UI_Resize_Take(&ui,&newSize)` to check pending size changes and decide
when to resize the application-owned canvas. Never combine event polling with
direct `Terminal_GetChar`/`Key_Get` on the same input stream.

```c
/* After the owning Canvas has been created: */
(void)TERMINAL_FUNC(Mouse_Enable)(true);

/* Each iteration: */
size_t count=0;
(void)UI_FUNC(PollEvents)(&ui,&count);
TDUAL_TYPE(uint16) newSize;
if(UI_FUNC(Resize_Take)(&ui,&newSize))
    (void)TERMINAL_FUNC(Canvas_Resize)(&canvas,newSize);
TERMINAL_FUNC(Canvas_Clear)(&canvas);
(void)UI_FUNC(Begin)(&ui,&canvas);
/* draw ButtonEx, InputTextEx, ... */
(void)UI_FUNC(End)(&ui);
(void)TERMINAL_FUNC(Canvas_Update)(&canvas);

/* Shutdown: */
(void)TERMINAL_FUNC(Mouse_Enable)(false);
```

There is deliberately **no mandatory UI Init or Free**. Use Chronometry or
the host scheduler to avoid a busy-loop and handle any noninteractive
`NOT_AVAILABLE` status; applications can still inject events manually.

## Editing and advanced layout

`TInputText` includes byte-indexed `caret`, `selectionAnchor` and
`firstVisible`; UTF-8 codepoint boundaries are respected by
`InputText_Select`, `InputText_Copy`, `InputText_Cut`,
`InputText_Paste`. These functions operate on caller-owned UTF-8 memory
rather than an OS-wide clipboard (external clipboard integration is not
provided); paste source must not alias the input buffer.

Arrow keys, Home, End, Delete, Backspace and Enter are handled by the editor;
multiline supports up/down and line-by-line scrolling. The caret and selected
range are visually distinguished by inverse attributes. This is a
codepoint-based editor, **not** a full grapheme-cluster/shaping implementation;
combining marks, multi-codepoint emoji and clipboard shortcuts need further work.

`Layout_Share` consumes a fraction of remaining horizontal/vertical space;
`Layout_MeasureText` returns terminal-cell text dimensions with padding.

A `TList` now retains `firstVisible`: keyboard selection automatically
scrolls the viewport, and mouse wheel scrolls when the pointer hovers over it.

## Incremental presentation

`Terminal_Canvas_UpdateDiff(&current,&previous)` writes changed cells;
`Terminal_Canvas_UpdateDiffAt` draws the same diff at an explicit position.
`previous` must be a valid snapshot of the last *successfully displayed*
frame, updated by the application with `Canvas_Copy`. If sizes differ, the
new implementation performs a full redraw. Use a full `Canvas_Update` after
external writes, terminal clear/resize, or state uncertainty.


## Classic game menus

The menu is **one immediate-mode focusable component** with a simple borrowed
array of UTF-8 strings: no tree of Button objects or allocation per item.
Navigation works with **Up/Down**, **Home/End**, **Enter/Space** to activate,
**Escape** to cancel, and optionally mouse clicks and wheel navigation.
A menu also has an \`itemDisabled\` mask, \`wrap\` and scrolling viewport.
Selection persists in the caller-owned \`size_t selected\`.

\`\`\`c
const char *options[] = {"New game", "Continue", "Options", "Exit"};
size_t selected = 0;
bool activated = false;

/* One-line direct API, called each frame between Begin/End: */
UI_FUNC(Menu)(&ui,
    (TQUAD_TYPE(uint16)){.left=3,.top=2,.right=26,.bottom=10},
    options,4,&selected,&activated);
if (activated) { /* dispatch based on selected */ }
\`\`\`

The configurable struct API allows a title, disabled entries, wrapping,
different visual style and viewport state:

\`\`\`c
const bool disabled[] = {false, true, false, false};
UI_TYPE(TMenu) menu = {
    .region = {.left=3,.top=2,.right=26,.bottom=10},
    .title = "MAIN MENU",
    .items = options, .itemDisabled = disabled,
    .count = 4, .selected = &selected, .wrap = true
};

/* Called each frame: */
UI_FUNC(MenuEx)(&ui,&menu);
if(menu.activated) { /* chosen menu.selected index */ }
if(menu.cancelled) { /* Escape: return to previous screen */ }
\`\`\`

The current theme's \`selection\` visual, border and
\`theme.glyphs.selection\` (Unicode \`▶\`, ASCII \`>\`) control the display.
The selection marker and selected row are rendered in a Canvas view.
No sound effects are implicit: the application can play them via Audio.

Use \`PushId\` around multiple direct menus, or set \`menu.id\` explicitly,
so there is no duplicate \`##menu\` widget ID in one frame.
Empty lists render without activation; disabled options are skipped by
arrow navigation and cannot be activated. The menu's own \`activated\` and
\`cancelled\` flags reset every frame.

## Advanced progress bars

\`UI_Progress\`/\`UI_ProgressEx\` remain available for normalized fractions
(\`0.0f\` through \`1.0f\`). For real task values, use
\`UI_ProgressBar\` / \`UI_ProgressBarEx\`:

\`\`\`c
UI_FUNC(ProgressBar)(&ui,
    (TQUAD_TYPE(uint16)){.left=2,.top=12,.right=30,.bottom=14},
    completed,total);
\`\`\`

The direct form displays the percentage by default. The struct form
supports a title, percentage toggle, vertical orientation, and an
indeterminate pulse controlled by caller-supplied \`phase\`.

\`\`\`c
UI_TYPE(TProgressBar) loading = {
    .region = {.left=2,.top=12,.right=30,.bottom=16},
    .label = "Loading assets",
    .value = completed, .maximum = total,
    .showPercent = true
};
UI_FUNC(ProgressBarEx)(&ui,&loading);

/* For a task with no measurable end: */
loading.indeterminate = true;
loading.phase = frameCounter++; /* schedule your frame rate externally */
UI_FUNC(ProgressBarEx)(&ui,&loading);
\`\`\`

A determinate progress bar requires \`maximum > 0\` and
\`value <= maximum\`. The code uses wide arithmetic for fraction/percentage
calculation and requires no \`-lm\`. A zero maximum is allowed only in
indeterminate mode. No animation timer runs implicitly inside the library;
the application supplies \`phase\`.

The progress bar reuses \`theme.progress\`, existing Text/Grid attributes,
\`glyphs.progressFull\` and \`glyphs.progressEmpty\`.
A narrowly-sized region without enough interior cells returns an error.
