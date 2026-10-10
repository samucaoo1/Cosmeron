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
| Dialog (OK) | Dialog | DialogEx / TDialog |
| Confirm (Yes/No) | Confirm | ConfirmEx / TDialog |
| StatusBar | StatusBar | StatusBarEx / TStatusBar |
| Toast | Toast | ToastEx / TToast |
| Spinner | Spinner | SpinnerEx / TSpinner |
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

- Native mouse and keyboard event polling exist; terminal-specific behavior still needs interactive TTY regression tests.
- No terminal output is automatic; call `Terminal_Canvas_Update` yourself.
- Glyph width depends on terminal fonts; text shaping/combining marks are not
  implemented by the Canvas.
- Editing supports Unicode codepoint cursor navigation, selection helpers and buffer paste, but not full grapheme/IME editing.
- Lists have a scrolling viewport; scrollbar widgets, multi-select, double-click and virtualized lists are later work.
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


## Retro dialogs, confirmation, status bar, toast and spinner

These are immediate-mode widgets over the **existing Canvas**. They require
no widget allocation, no hidden windows and no Init/Free. Direct calls create
temporary descriptors and delegate to \`*Ex\` functions.

### Dialog and Confirm

\`UI_Dialog\` is an informational OK/ESC dialog; its output becomes true
when the user activates OK or dismisses via Escape. For multiple actions,
\`UI_DialogEx\` accepts a \`TDialog\` with an array of borrowed action labels,
\`actionCount\` and caller-persistent \`selected\` index. On each frame,
\`activated\` and \`dismissed\` are reset. Arrow keys / Home / End navigate,
Enter or Space activates, Escape dismisses, and mouse click/release chooses
an action. Text can wrap within the rectangle. A zero action count displays
the default OK action.

\`\`\`c
UI_TYPE(TDialog) dialog = {
    .region = {.left=10,.top=4,.right=48,.bottom=13},
    .title = "MISSION",
    .message = "Continue this quest?"
};
UI_FUNC(ConfirmEx)(&ui,&dialog);  /* 0 = Yes; 1 = No */
if(dialog.activated && dialog.selected==0) { /* accepted */ }
if(dialog.dismissed) { /* Escape */ }
\`\`\`

\`UI_Confirm\` is the two-output convenience form:

\`\`\`c
bool accepted=false, rejected=false;
UI_FUNC(Confirm)(&ui,dialog.region,
                 "Delete save?",&accepted,&rejected);
\`\`\`

\`ConfirmEx\` allows exactly two custom action strings, or defaults to
"Yes"/"No". Direct Confirm stores its two-choice cursor in \`TContext\`
between frames; explicitly scoped IDs or \`TDialog\` are preferable if
multiple confirmations are required.

**Overlay ordering:** draw background first and the dialog last; while
the dialog is open, the application should skip interactive background
widgets. The dialog captures keyboard focus when drawn, but this version
does not retroactively cancel events already consumed by widgets drawn
earlier in the same frame. Rendering the background without interactive
widgets is the supported modal pattern.

### StatusBar

\`\`\`c
UI_FUNC(StatusBar)(&ui,
    (TQUAD_TYPE(uint16)){.left=0,.top=23,.right=79,.bottom=23},
    "HP 100  |  MP 50","F1 Help   ESC Menu");
\`\`\`

\`TStatusBar\` adds optional \`visual\` styling. Left text is clipped to
leave room for right-aligned hints. Both positions are calculated in
terminal-cell units. A one-line StatusBar uses \`theme.text\` by default,
so it can be drawn without a border.

### Toast

\`\`\`c
UI_TYPE(TToast) notice = {
    .region = {.left=42,.top=1,.right=77,.bottom=5},
    .title = "Saved",
    .message = "Progress stored successfully.",
    .visible = true,
    .nowTick = ticks,
    .expiresAtTick = 9000
};
UI_FUNC(ToastEx)(&ui,&notice);
\`\`\`

When not visible or expired, the toast returns success and renders nothing.
The application supplies clock/tick values in any consistent units.
\`expiresAtTick==0\` means **no automatic expiry**. \`UI_Toast\` is the
direct form with a boolean \`visible\`. A toast does not receive focus or
steal input. The caller clears/redraws its Canvas each frame to remove
expired notifications.

### Spinner

\`\`\`c
UI_FUNC(Spinner)(&ui,smallRegion,"Loading...",frameCounter);

static const char *const customFrames[]={"|","/","-","\\\\"};
UI_TYPE(TSpinner) spinner = {
    .region = smallRegion,
    .frames = customFrames, .frameCount = 4,
    .phase = frameCounter
};
UI_FUNC(SpinnerEx)(&ui,&spinner);
\`\`\`

\`phase % frameCount\` chooses a UTF-8 frame. Without custom frames,
the default animation is ASCII (\`| / - \\\\\`) in an ASCII-themed terminal,
or a Unicode quarter-circle spinner otherwise. The application owns frame
timing (Chronometry can provide scheduling): no blocking, threads or hidden
timers are introduced.

### Validation and boundaries

Dialogs require space for body and actions. All widgets use the UI's
inclusive \`TQuad_uint16\` regions, existing Text/Grid cells, UTF-8
validation and \`OPSTATUS\`. These widgets are terminal-first; full dialog
modal event trapping, IME, clipboard integration, text shaping and real-TTY
visual QA remain tracked in
[issue #119](https://github.com/samucaoo1/Cosmeron/issues/119).
