# UI — terminal widgets

Header: `Codespace/Cosmeron/Modules/Graphics/UI/UI.h`
Namespace: `UI_FUNC(...)`, `UI_TYPE(...)`, `UI_CONST(...)`.
Uses C11, header-only, without mandatory extra linker flags.

The UI borrows a `Terminal_TCanvas` for every frame. It never writes to stdout
or calls `Canvas_Update`; applications choose when to present. The context
is a zero-initializable struct and requires no `Init`/`Free`. Widget values
are held by the application; persistent structs group options but do not own
native windows or grids.

The [pixel GUI](GUI.md) follows the same lifecycle and shares identity hashing,
focus traversal, pointer capture and UTF-8 editor primitives. Canvas and
Surface remain separate render targets.

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

---

# API reference

A retained-state/immediate-declaration terminal interface; a TContext consumes explicit events and renders widgets onto a borrowed Canvas.

The reference below is generated from the **public prototype declarations**, not from design proposals. `UI_FUNC(Name)` and `UI_Name` are equivalent with the default namespace. Examples are usage fragments: the context, geometry and struct configurations must be valid before calling widgets. They are not standalone runnable programs.

## Function summary

| Function | Return | Purpose |
| --- | --- | --- |
| `UI_FUNC(Visual_Default)` | `UI_TYPE(TVisual)` | Returns default widget visual settings. |
| `UI_FUNC(Visual_RGB)` | `TEXT_GRID_ATTRIBUTE_TYPE(Cell)` | Constructs a cell style from explicit RGB components. |
| `UI_FUNC(Visual_Push)` | `OPSTATUS` | Adds a temporary visual override. |
| `UI_FUNC(Visual_Pop)` | `OPSTATUS` | Restores the previous visual override. |
| `UI_FUNC(Theme_Default)` | `UI_TYPE(TTheme)` | Returns the package's default theme by value. |
| `UI_FUNC(Theme_Dark)` | `UI_TYPE(TTheme)` | Returns dark-theme settings. |
| `UI_FUNC(Theme_Light)` | `UI_TYPE(TTheme)` | Returns light-theme settings. |
| `UI_FUNC(Theme_Classic)` | `UI_TYPE(TTheme)` | Returns classic-theme settings. |
| `UI_FUNC(Theme_Monochrome)` | `UI_TYPE(TTheme)` | Returns monochrome-theme settings. |
| `UI_FUNC(Theme_Set)` | `OPSTATUS` | Applies a theme to the context. |
| `UI_FUNC(Theme_Get)` | `OPSTATUS` | Copies current theme into outTheme. |
| `UI_FUNC(Glyphs_ASCII)` | `UI_TYPE(TGlyphs)` | Creates ASCII-only glyph selections. |
| `UI_FUNC(Glyphs_Unicode)` | `UI_TYPE(TGlyphs)` | Creates Unicode glyph selections. |
| `UI_FUNC(PollEvents)` | `OPSTATUS` | Consumes normalized terminal events for the UI context. |
| `UI_FUNC(Resize_Take)` | `bool` | Retrieves and clears a pending terminal resize. |
| `UI_FUNC(Layout_Share)` | `OPSTATUS` | Splits current available layout space by the supplied rational fraction. |
| `UI_FUNC(Layout_MeasureText)` | `OPSTATUS` | Measures a text string at the package's logical scale. |
| `UI_FUNC(InputText_Select)` | `OPSTATUS` | Selects the range between anchor and caret positions. |
| `UI_FUNC(InputText_Copy)` | `OPSTATUS` | Copies selected text from a widget into a caller buffer. |
| `UI_FUNC(InputText_Cut)` | `OPSTATUS` | Copies and removes the selected text from the widget. |
| `UI_FUNC(InputText_Paste)` | `OPSTATUS` | Pastes validated UTF-8 into the input model. |
| `UI_FUNC(Begin)` | `OPSTATUS` | Begins a new explicit frame with a target Canvas or Surface. No implicit window presentation. |
| `UI_FUNC(End)` | `OPSTATUS` | Ends and finalizes the current frame. |
| `UI_FUNC(GetSize)` | `OPSTATUS` | Gets the active frame dimensions. |
| `UI_FUNC(Key)` | `OPSTATUS` | Feeds normalized keyboard input to UI state. |
| `UI_FUNC(PointerMove)` | `OPSTATUS` | Feeds cursor motion to UI interaction logic. |
| `UI_FUNC(PointerButton)` | `OPSTATUS` | Feeds pointer button transitions. |
| `UI_FUNC(Scroll)` | `OPSTATUS` | Feeds a mouse wheel or scroll delta. |
| `UI_FUNC(GetId)` | `UI_TYPE(WidgetId)` | Builds a stable widget ID from the label and current scope. |
| `UI_FUNC(PushId)` | `OPSTATUS` | Pushes a stable identifier scope for repeated widget labels. |
| `UI_FUNC(PopId)` | `OPSTATUS` | Restores previous widget identifier scope. |
| `UI_FUNC(Focus_Set)` | `OPSTATUS` | Moves keyboard focus to a widget ID. |
| `UI_FUNC(Focus_Next)` | `OPSTATUS` | Advances keyboard focus among eligible widgets. |
| `UI_FUNC(Focus_Previous)` | `OPSTATUS` | Moves keyboard focus backward. |
| `UI_FUNC(IsFocused)` | `bool` | Tests whether widget ID owns focus. |
| `UI_FUNC(Row_Begin)` | `OPSTATUS` | Begins a row layout region. |
| `UI_FUNC(Column_Begin)` | `OPSTATUS` | Begins a column layout region. |
| `UI_FUNC(Layout_Next)` | `OPSTATUS` | Requests the next rectangle in current layout. |
| `UI_FUNC(Layout_Remaining)` | `OPSTATUS` | Queries remaining layout space. |
| `UI_FUNC(Layout_End)` | `OPSTATUS` | Closes current layout scope. |
| `UI_FUNC(Panel_Begin)` | `OPSTATUS` | Begins a panel using direct geometry and title. |
| `UI_FUNC(Panel_BeginEx)` | `OPSTATUS` | Begins a panel using a struct configuration. |
| `UI_FUNC(Panel_End)` | `OPSTATUS` | Closes a panel scope. |
| `UI_FUNC(Label)` | `OPSTATUS` | Draws text as a non-interactive label. |
| `UI_FUNC(LabelEx)` | `OPSTATUS` | Draws a label using its struct configuration. |
| `UI_FUNC(Separator)` | `OPSTATUS` | Draws a visual separation rule. |
| `UI_FUNC(SeparatorEx)` | `OPSTATUS` | Draws a separator using its struct configuration. |
| `UI_FUNC(Button)` | `OPSTATUS` | Draws an interactive button and reports activation. |
| `UI_FUNC(ButtonEx)` | `OPSTATUS` | Draws a button from its struct configuration. |
| `UI_FUNC(Checkbox)` | `OPSTATUS` | Draws a checkbox and updates its selected state. |
| `UI_FUNC(CheckboxEx)` | `OPSTATUS` | Draws a checkbox from a struct. |
| `UI_FUNC(Radio)` | `OPSTATUS` | Draws a radio selection and updates selected value. |
| `UI_FUNC(RadioEx)` | `OPSTATUS` | Draws a radio widget configured by struct. |
| `UI_FUNC(Slider)` | `OPSTATUS` | Draws a ranged slider and modifies a caller-owned numeric value. |
| `UI_FUNC(SliderEx)` | `OPSTATUS` | Draws a slider from a struct. |
| `UI_FUNC(Progress)` | `OPSTATUS` | Draws normalized progress feedback. |
| `UI_FUNC(ProgressEx)` | `OPSTATUS` | Draws a progress widget from a struct. |
| `UI_FUNC(Menu)` | `OPSTATUS` | Draws and interacts with a list of menu items. |
| `UI_FUNC(MenuEx)` | `OPSTATUS` | Draws a menu with struct configuration. |
| `UI_FUNC(ProgressBar)` | `OPSTATUS` | Draws a progress bar with current and maximum values. |
| `UI_FUNC(ProgressBarEx)` | `OPSTATUS` | Draws a progress bar using struct settings. |
| `UI_FUNC(Dialog)` | `OPSTATUS` | Draws a dialog and reports dismissal. |
| `UI_FUNC(DialogEx)` | `OPSTATUS` | Draws a dialog from its full configuration. |
| `UI_FUNC(Confirm)` | `OPSTATUS` | Draws a confirm/reject dialog, reporting selected action. |
| `UI_FUNC(ConfirmEx)` | `OPSTATUS` | Draws a confirmation configured by struct. |
| `UI_FUNC(StatusBar)` | `OPSTATUS` | Draws status text in a bar. |
| `UI_FUNC(StatusBarEx)` | `OPSTATUS` | Draws a full status bar configuration. |
| `UI_FUNC(Toast)` | `OPSTATUS` | Draws a conditional non-modal notification. |
| `UI_FUNC(ToastEx)` | `OPSTATUS` | Draws a toast from its configuration. |
| `UI_FUNC(Spinner)` | `OPSTATUS` | Draws an animated spinner using caller-supplied phase. |
| `UI_FUNC(SpinnerEx)` | `OPSTATUS` | Draws a spinner from its configuration. |
| `UI_FUNC(InputText)` | `OPSTATUS` | Edits text in a caller-provided buffer. |
| `UI_FUNC(InputTextEx)` | `OPSTATUS` | Edits text with full input-widget state. |
| `UI_FUNC(TextArea)` | `OPSTATUS` | Edits multiline text in caller-provided memory. |
| `UI_FUNC(TextAreaEx)` | `OPSTATUS` | Edits multiline text through a state struct. |
| `UI_FUNC(List)` | `OPSTATUS` | Draws a selectable list. |
| `UI_FUNC(ListEx)` | `OPSTATUS` | Draws a struct-configured list. |

# Visual_Default

Returns default widget visual settings.

### Syntax

#### Macro form

```c
UI_TYPE(TVisual) UI_FUNC(Visual_Default)(void);
```

#### Direct form

```c
UI_TVisual UI_Visual_Default(void);
```

### Parameters

- None.

### Return value

Returns a value of type `UI_TYPE(TVisual)` by value.

### Remarks

Terminal UI widgets draw onto a Canvas in the active Begin/End frame, without taking permanent ownership of the caller's widget values.

### Example

```c
/* Usage fragment: context, geometry and widget structs are initialized by the caller. */

UI_TYPE(TVisual) visual = UI_FUNC(Visual_Default)();
```

# Visual_RGB

Constructs a cell style from explicit RGB components.

### Syntax

#### Macro form

```c
TEXT_GRID_ATTRIBUTE_TYPE(Cell) UI_FUNC(Visual_RGB)(TEXT_GRID_COLOR_TYPE(RGB) foreground, TEXT_GRID_COLOR_TYPE(RGB) background);
```

#### Direct form

```c
Text_Grid_Attribute_Cell UI_Visual_RGB(Text_Grid_Color_RGB foreground, Text_Grid_Color_RGB background);
```

### Parameters

- `foreground`: foreground RGB color.
- `background`: background RGB color.

### Return value

Returns a value of type `TEXT_GRID_ATTRIBUTE_TYPE(Cell)` by value.

### Remarks

Terminal UI widgets draw onto a Canvas in the active Begin/End frame, without taking permanent ownership of the caller's widget values.

### Example

```c
/* Usage fragment: context, geometry and widget structs are initialized by the caller. */

TEXT_GRID_ATTRIBUTE_TYPE(Cell) style = UI_FUNC(Visual_RGB)(foreground, background);
```

# Visual_Push

Adds a temporary visual override.

### Syntax

#### Macro form

```c
OPSTATUS UI_FUNC(Visual_Push)(UI_TYPE(TContext) *context, const UI_TYPE(TVisual) *visual);
```

#### Direct form

```c
OPSTATUS UI_Visual_Push(UI_TContext *context, const UI_TVisual *visual);
```

### Parameters

- `context`: UI or GUI context, initialized and inside its frame lifecycle as required.
- `visual`: visual override pushed on the context.

### Return value

`OPSTATUS`: SUCCESS if the operation completes; invalid frame/lifecycle, invalid range or incompatible input returns the corresponding status.

### Remarks

Terminal UI widgets draw onto a Canvas in the active Begin/End frame, without taking permanent ownership of the caller's widget values. Balance push/pop scope calls before completing the frame.

### Example

```c
/* Usage fragment: context, geometry and widget structs are initialized by the caller. */

OPSTATUS status = UI_FUNC(Visual_Push)(&context, &visual);
```

# Visual_Pop

Restores the previous visual override.

### Syntax

#### Macro form

```c
OPSTATUS UI_FUNC(Visual_Pop)(UI_TYPE(TContext) *context);
```

#### Direct form

```c
OPSTATUS UI_Visual_Pop(UI_TContext *context);
```

### Parameters

- `context`: UI or GUI context, initialized and inside its frame lifecycle as required.

### Return value

`OPSTATUS`: SUCCESS if the operation completes; invalid frame/lifecycle, invalid range or incompatible input returns the corresponding status.

### Remarks

Terminal UI widgets draw onto a Canvas in the active Begin/End frame, without taking permanent ownership of the caller's widget values. Balance push/pop scope calls before completing the frame.

### Example

```c
/* Usage fragment: context, geometry and widget structs are initialized by the caller. */

OPSTATUS status = UI_FUNC(Visual_Pop)(&context);
```

# Theme_Default

Returns the package's default theme by value.

### Syntax

#### Macro form

```c
UI_TYPE(TTheme) UI_FUNC(Theme_Default)(void);
```

#### Direct form

```c
UI_TTheme UI_Theme_Default(void);
```

### Parameters

- None.

### Return value

Returns a value of type `UI_TYPE(TTheme)` by value.

### Remarks

Terminal UI widgets draw onto a Canvas in the active Begin/End frame, without taking permanent ownership of the caller's widget values.

### Example

```c
/* Usage fragment: context, geometry and widget structs are initialized by the caller. */

UI_TYPE(TTheme) theme = UI_FUNC(Theme_Default)();
```

# Theme_Dark

Returns dark-theme settings.

### Syntax

#### Macro form

```c
UI_TYPE(TTheme) UI_FUNC(Theme_Dark)(void);
```

#### Direct form

```c
UI_TTheme UI_Theme_Dark(void);
```

### Parameters

- None.

### Return value

Returns a value of type `UI_TYPE(TTheme)` by value.

### Remarks

Terminal UI widgets draw onto a Canvas in the active Begin/End frame, without taking permanent ownership of the caller's widget values.

### Example

```c
/* Usage fragment: context, geometry and widget structs are initialized by the caller. */

UI_TYPE(TTheme) theme = UI_FUNC(Theme_Dark)();
```

# Theme_Light

Returns light-theme settings.

### Syntax

#### Macro form

```c
UI_TYPE(TTheme) UI_FUNC(Theme_Light)(void);
```

#### Direct form

```c
UI_TTheme UI_Theme_Light(void);
```

### Parameters

- None.

### Return value

Returns a value of type `UI_TYPE(TTheme)` by value.

### Remarks

Terminal UI widgets draw onto a Canvas in the active Begin/End frame, without taking permanent ownership of the caller's widget values.

### Example

```c
/* Usage fragment: context, geometry and widget structs are initialized by the caller. */

UI_TYPE(TTheme) theme = UI_FUNC(Theme_Light)();
```

# Theme_Classic

Returns classic-theme settings.

### Syntax

#### Macro form

```c
UI_TYPE(TTheme) UI_FUNC(Theme_Classic)(void);
```

#### Direct form

```c
UI_TTheme UI_Theme_Classic(void);
```

### Parameters

- None.

### Return value

Returns a value of type `UI_TYPE(TTheme)` by value.

### Remarks

Terminal UI widgets draw onto a Canvas in the active Begin/End frame, without taking permanent ownership of the caller's widget values.

### Example

```c
/* Usage fragment: context, geometry and widget structs are initialized by the caller. */

UI_TYPE(TTheme) theme = UI_FUNC(Theme_Classic)();
```

# Theme_Monochrome

Returns monochrome-theme settings.

### Syntax

#### Macro form

```c
UI_TYPE(TTheme) UI_FUNC(Theme_Monochrome)(void);
```

#### Direct form

```c
UI_TTheme UI_Theme_Monochrome(void);
```

### Parameters

- None.

### Return value

Returns a value of type `UI_TYPE(TTheme)` by value.

### Remarks

Terminal UI widgets draw onto a Canvas in the active Begin/End frame, without taking permanent ownership of the caller's widget values.

### Example

```c
/* Usage fragment: context, geometry and widget structs are initialized by the caller. */

UI_TYPE(TTheme) theme = UI_FUNC(Theme_Monochrome)();
```

# Theme_Set

Applies a theme to the context.

### Syntax

#### Macro form

```c
OPSTATUS UI_FUNC(Theme_Set)(UI_TYPE(TContext) *context, const UI_TYPE(TTheme) *theme);
```

#### Direct form

```c
OPSTATUS UI_Theme_Set(UI_TContext *context, const UI_TTheme *theme);
```

### Parameters

- `context`: UI or GUI context, initialized and inside its frame lifecycle as required.
- `theme`: theme supplied for a context.

### Return value

`OPSTATUS`: SUCCESS if the operation completes; invalid frame/lifecycle, invalid range or incompatible input returns the corresponding status.

### Remarks

Terminal UI widgets draw onto a Canvas in the active Begin/End frame, without taking permanent ownership of the caller's widget values.

### Example

```c
/* Usage fragment: context, geometry and widget structs are initialized by the caller. */

OPSTATUS status = UI_FUNC(Theme_Set)(&context, &theme);
```

# Theme_Get

Copies current theme into outTheme.

### Syntax

#### Macro form

```c
OPSTATUS UI_FUNC(Theme_Get)(const UI_TYPE(TContext) *context, UI_TYPE(TTheme) *outTheme);
```

#### Direct form

```c
OPSTATUS UI_Theme_Get(const UI_TContext *context, UI_TTheme *outTheme);
```

### Parameters

- `context`: UI or GUI context, initialized and inside its frame lifecycle as required.
- `outTheme`: output theme struct.

### Return value

`OPSTATUS`: SUCCESS if the operation completes; invalid frame/lifecycle, invalid range or incompatible input returns the corresponding status.

### Remarks

Terminal UI widgets draw onto a Canvas in the active Begin/End frame, without taking permanent ownership of the caller's widget values.

### Example

```c
/* Usage fragment: context, geometry and widget structs are initialized by the caller. */
UI_TYPE(TTheme) theme = {0};
OPSTATUS status = UI_FUNC(Theme_Get)(&context, &theme);
```

# Glyphs_ASCII

Creates ASCII-only glyph selections.

### Syntax

#### Macro form

```c
UI_TYPE(TGlyphs) UI_FUNC(Glyphs_ASCII)(void);
```

#### Direct form

```c
UI_TGlyphs UI_Glyphs_ASCII(void);
```

### Parameters

- None.

### Return value

Returns a value of type `UI_TYPE(TGlyphs)` by value.

### Remarks

Terminal UI widgets draw onto a Canvas in the active Begin/End frame, without taking permanent ownership of the caller's widget values.

### Example

```c
/* Usage fragment: context, geometry and widget structs are initialized by the caller. */

UI_TYPE(TGlyphs) glyphs = UI_FUNC(Glyphs_ASCII)();
```

# Glyphs_Unicode

Creates Unicode glyph selections.

### Syntax

#### Macro form

```c
UI_TYPE(TGlyphs) UI_FUNC(Glyphs_Unicode)(void);
```

#### Direct form

```c
UI_TGlyphs UI_Glyphs_Unicode(void);
```

### Parameters

- None.

### Return value

Returns a value of type `UI_TYPE(TGlyphs)` by value.

### Remarks

Terminal UI widgets draw onto a Canvas in the active Begin/End frame, without taking permanent ownership of the caller's widget values.

### Example

```c
/* Usage fragment: context, geometry and widget structs are initialized by the caller. */

UI_TYPE(TGlyphs) glyphs = UI_FUNC(Glyphs_Unicode)();
```

# PollEvents

Consumes normalized terminal events for the UI context.

### Syntax

#### Macro form

```c
OPSTATUS UI_FUNC(PollEvents)(UI_TYPE(TContext) *context, size_t *outCount);
```

#### Direct form

```c
OPSTATUS UI_PollEvents(UI_TContext *context, size_t *outCount);
```

### Parameters

- `context`: UI or GUI context, initialized and inside its frame lifecycle as required.
- `outCount`: output event count.

### Return value

`OPSTATUS`: SUCCESS if the operation completes; invalid frame/lifecycle, invalid range or incompatible input returns the corresponding status.

### Remarks

Terminal UI widgets draw onto a Canvas in the active Begin/End frame, without taking permanent ownership of the caller's widget values. The input path uses the Terminal backend; it is not a native pixel GUI event source.

### Example

```c
/* Usage fragment: context, geometry and widget structs are initialized by the caller. */
size_t count = 0;
OPSTATUS status = UI_FUNC(PollEvents)(&context, &count);
```

# Resize_Take

Retrieves and clears a pending terminal resize.

### Syntax

#### Macro form

```c
bool UI_FUNC(Resize_Take)(UI_TYPE(TContext) *context, TDUAL_TYPE(uint16) *outSize);
```

#### Direct form

```c
bool UI_Resize_Take(UI_TContext *context, TDual_uint16 *outSize);
```

### Parameters

- `context`: UI or GUI context, initialized and inside its frame lifecycle as required.
- `outSize`: receives computed dimensions.

### Return value

`bool`: indicates the state queried, not an OPSTATUS.

### Remarks

Terminal UI widgets draw onto a Canvas in the active Begin/End frame, without taking permanent ownership of the caller's widget values.

### Example

```c
/* Usage fragment: context, geometry and widget structs are initialized by the caller. */
TDUAL_TYPE(uint16) resultSize = {0};
bool result = UI_FUNC(Resize_Take)(&context, &resultSize);
```

# Layout_Share

Splits current available layout space by the supplied rational fraction.

### Syntax

#### Macro form

```c
OPSTATUS UI_FUNC(Layout_Share)(UI_TYPE(TContext) *context, uint16_t numerator, uint16_t denominator, TQUAD_TYPE(uint16) *outRegion);
```

#### Direct form

```c
OPSTATUS UI_Layout_Share(UI_TContext *context, uint16_t numerator, uint16_t denominator, TQuad_uint16 *outRegion);
```

### Parameters

- `context`: UI or GUI context, initialized and inside its frame lifecycle as required.
- `numerator`: fraction numerator for share allocation.
- `denominator`: nonzero fraction denominator.
- `outRegion`: output geometry region.

### Return value

`OPSTATUS`: SUCCESS if the operation completes; invalid frame/lifecycle, invalid range or incompatible input returns the corresponding status.

### Remarks

Terminal UI widgets draw onto a Canvas in the active Begin/End frame, without taking permanent ownership of the caller's widget values.

### Example

```c
/* Usage fragment: context, geometry and widget structs are initialized by the caller. */
TQUAD_TYPE(uint16) outRegion = {0};
OPSTATUS status = UI_FUNC(Layout_Share)(&context, 1, 2, &outRegion);
```

# Layout_MeasureText

Measures a text string at the package's logical scale.

### Syntax

#### Macro form

```c
OPSTATUS UI_FUNC(Layout_MeasureText)(const char *text, TDUAL_TYPE(uint16) padding, TDUAL_TYPE(uint16) *outSize);
```

#### Direct form

```c
OPSTATUS UI_Layout_MeasureText(const char *text, TDual_uint16 padding, TDual_uint16 *outSize);
```

### Parameters

- `text`: UTF-8 text used for display or measurement.
- `padding`: extra spacing around text.
- `outSize`: receives computed dimensions.

### Return value

`OPSTATUS`: SUCCESS if the operation completes; invalid frame/lifecycle, invalid range or incompatible input returns the corresponding status.

### Remarks

Terminal UI widgets draw onto a Canvas in the active Begin/End frame, without taking permanent ownership of the caller's widget values.

### Example

```c
/* Usage fragment: context, geometry and widget structs are initialized by the caller. */
TDUAL_TYPE(uint16) resultSize = {0};
OPSTATUS status = UI_FUNC(Layout_MeasureText)("Hello", padding, &resultSize);
```

# InputText_Select

Selects the range between anchor and caret positions.

### Syntax

#### Macro form

```c
OPSTATUS UI_FUNC(InputText_Select)(UI_TYPE(TInputText) *input, size_t anchor, size_t caret);
```

#### Direct form

```c
OPSTATUS UI_InputText_Select(UI_TInputText *input, size_t anchor, size_t caret);
```

### Parameters

- `input`: input-text state/configuration.
- `anchor`: selection anchor byte/character position per widget contract.
- `caret`: selection caret position.

### Return value

`OPSTATUS`: SUCCESS if the operation completes; invalid frame/lifecycle, invalid range or incompatible input returns the corresponding status.

### Remarks

Terminal UI widgets draw onto a Canvas in the active Begin/End frame, without taking permanent ownership of the caller's widget values. Editing preserves caller ownership of the buffer; capacity and UTF-8 validity must be respected.

### Example

```c
/* Usage fragment: context, geometry and widget structs are initialized by the caller. */

OPSTATUS status = UI_FUNC(InputText_Select)(&input, 0, 3);
```

# InputText_Copy

Copies selected text from a widget into a caller buffer.

### Syntax

#### Macro form

```c
OPSTATUS UI_FUNC(InputText_Copy)(const UI_TYPE(TInputText) *input, char *buffer, size_t capacity, size_t *outLength);
```

#### Direct form

```c
OPSTATUS UI_InputText_Copy(const UI_TInputText *input, char *buffer, size_t capacity, size_t *outLength);
```

### Parameters

- `input`: input-text state/configuration.
- `buffer`: editable caller-owned character buffer.
- `capacity`: capacity of input/copy destination in bytes.
- `outLength`: receives byte length copied or required.

### Return value

`OPSTATUS`: SUCCESS if the operation completes; invalid frame/lifecycle, invalid range or incompatible input returns the corresponding status.

### Remarks

Terminal UI widgets draw onto a Canvas in the active Begin/End frame, without taking permanent ownership of the caller's widget values. Editing preserves caller ownership of the buffer; capacity and UTF-8 validity must be respected.

### Example

```c
/* Usage fragment: context, geometry and widget structs are initialized by the caller. */
char buffer[128] = {0};
size_t outputLength = 0;
OPSTATUS status = UI_FUNC(InputText_Copy)(&input, buffer, sizeof buffer, &outputLength);
```

# InputText_Cut

Copies and removes the selected text from the widget.

### Syntax

#### Macro form

```c
OPSTATUS UI_FUNC(InputText_Cut)(UI_TYPE(TInputText) *input, char *buffer, size_t capacity, size_t *outLength);
```

#### Direct form

```c
OPSTATUS UI_InputText_Cut(UI_TInputText *input, char *buffer, size_t capacity, size_t *outLength);
```

### Parameters

- `input`: input-text state/configuration.
- `buffer`: editable caller-owned character buffer.
- `capacity`: capacity of input/copy destination in bytes.
- `outLength`: receives byte length copied or required.

### Return value

`OPSTATUS`: SUCCESS if the operation completes; invalid frame/lifecycle, invalid range or incompatible input returns the corresponding status.

### Remarks

Terminal UI widgets draw onto a Canvas in the active Begin/End frame, without taking permanent ownership of the caller's widget values. Editing preserves caller ownership of the buffer; capacity and UTF-8 validity must be respected.

### Example

```c
/* Usage fragment: context, geometry and widget structs are initialized by the caller. */
char buffer[128] = {0};
size_t outputLength = 0;
OPSTATUS status = UI_FUNC(InputText_Cut)(&input, buffer, sizeof buffer, &outputLength);
```

# InputText_Paste

Pastes validated UTF-8 into the input model.

### Syntax

#### Macro form

```c
OPSTATUS UI_FUNC(InputText_Paste)(UI_TYPE(TInputText) *input, const char *utf8Text, size_t byteLength);
```

#### Direct form

```c
OPSTATUS UI_InputText_Paste(UI_TInputText *input, const char *utf8Text, size_t byteLength);
```

### Parameters

- `input`: input-text state/configuration.
- `utf8Text`: UTF-8 text inserted from caller storage.
- `byteLength`: number of UTF-8 bytes to paste.

### Return value

`OPSTATUS`: SUCCESS if the operation completes; invalid frame/lifecycle, invalid range or incompatible input returns the corresponding status.

### Remarks

Terminal UI widgets draw onto a Canvas in the active Begin/End frame, without taking permanent ownership of the caller's widget values. Editing preserves caller ownership of the buffer; capacity and UTF-8 validity must be respected.

### Example

```c
/* Usage fragment: context, geometry and widget structs are initialized by the caller. */

OPSTATUS status = UI_FUNC(InputText_Paste)(&input, "Hello", 5);
```

# Begin

Begins a new explicit frame with a target Canvas or Surface. No implicit window presentation.

### Syntax

#### Macro form

```c
OPSTATUS UI_FUNC(Begin)(UI_TYPE(TContext) *context, TERMINAL_TYPE(TCanvas) *canvas);
```

#### Direct form

```c
OPSTATUS UI_Begin(UI_TContext *context, Terminal_TCanvas *canvas);
```

### Parameters

- `context`: UI or GUI context, initialized and inside its frame lifecycle as required.
- `canvas`: borrowed Canvas that receives text/cell drawing.

### Return value

`OPSTATUS`: SUCCESS if the operation completes; invalid frame/lifecycle, invalid range or incompatible input returns the corresponding status.

### Remarks

Terminal UI widgets draw onto a Canvas in the active Begin/End frame, without taking permanent ownership of the caller's widget values. Do not call twice without a matching End.

### Example

```c
/* Usage fragment: context, geometry and widget structs are initialized by the caller. */

OPSTATUS status = UI_FUNC(Begin)(&context, &canvas);
```

# End

Ends and finalizes the current frame.

### Syntax

#### Macro form

```c
OPSTATUS UI_FUNC(End)(UI_TYPE(TContext) *context);
```

#### Direct form

```c
OPSTATUS UI_End(UI_TContext *context);
```

### Parameters

- `context`: UI or GUI context, initialized and inside its frame lifecycle as required.

### Return value

`OPSTATUS`: SUCCESS if the operation completes; invalid frame/lifecycle, invalid range or incompatible input returns the corresponding status.

### Remarks

Terminal UI widgets draw onto a Canvas in the active Begin/End frame, without taking permanent ownership of the caller's widget values. This call does not necessarily update the external display on its own.

### Example

```c
/* Usage fragment: context, geometry and widget structs are initialized by the caller. */

OPSTATUS status = UI_FUNC(End)(&context);
```

# GetSize

Gets the active frame dimensions.

### Syntax

#### Macro form

```c
OPSTATUS UI_FUNC(GetSize)(const UI_TYPE(TContext) *context, TDUAL_TYPE(uint16) *outSize);
```

#### Direct form

```c
OPSTATUS UI_GetSize(const UI_TContext *context, TDual_uint16 *outSize);
```

### Parameters

- `context`: UI or GUI context, initialized and inside its frame lifecycle as required.
- `outSize`: receives computed dimensions.

### Return value

`OPSTATUS`: SUCCESS if the operation completes; invalid frame/lifecycle, invalid range or incompatible input returns the corresponding status.

### Remarks

Terminal UI widgets draw onto a Canvas in the active Begin/End frame, without taking permanent ownership of the caller's widget values.

### Example

```c
/* Usage fragment: context, geometry and widget structs are initialized by the caller. */
TDUAL_TYPE(uint16) resultSize = {0};
OPSTATUS status = UI_FUNC(GetSize)(&context, &resultSize);
```

# Key

Feeds normalized keyboard input to UI state.

### Syntax

#### Macro form

```c
OPSTATUS UI_FUNC(Key)(UI_TYPE(TContext) *context, TERMINAL_TYPE(TKey) key);
```

#### Direct form

```c
OPSTATUS UI_Key(UI_TContext *context, Terminal_TKey key);
```

### Parameters

- `context`: UI or GUI context, initialized and inside its frame lifecycle as required.
- `key`: normalized terminal key event.

### Return value

`OPSTATUS`: SUCCESS if the operation completes; invalid frame/lifecycle, invalid range or incompatible input returns the corresponding status.

### Remarks

Terminal UI widgets draw onto a Canvas in the active Begin/End frame, without taking permanent ownership of the caller's widget values.

### Example

```c
/* Usage fragment: context, geometry and widget structs are initialized by the caller. */

OPSTATUS status = UI_FUNC(Key)(&context, key);
```

# PointerMove

Feeds cursor motion to UI interaction logic.

### Syntax

#### Macro form

```c
OPSTATUS UI_FUNC(PointerMove)(UI_TYPE(TContext) *context, TDUAL_TYPE(uint16) position);
```

#### Direct form

```c
OPSTATUS UI_PointerMove(UI_TContext *context, TDual_uint16 position);
```

### Parameters

- `context`: UI or GUI context, initialized and inside its frame lifecycle as required.
- `position`: pointer location in logical coordinates.

### Return value

`OPSTATUS`: SUCCESS if the operation completes; invalid frame/lifecycle, invalid range or incompatible input returns the corresponding status.

### Remarks

Terminal UI widgets draw onto a Canvas in the active Begin/End frame, without taking permanent ownership of the caller's widget values.

### Example

```c
/* Usage fragment: context, geometry and widget structs are initialized by the caller. */

OPSTATUS status = UI_FUNC(PointerMove)(&context, position);
```

# PointerButton

Feeds pointer button transitions.

### Syntax

#### Macro form

```c
OPSTATUS UI_FUNC(PointerButton)(UI_TYPE(TContext) *context, bool pressed);
```

#### Direct form

```c
OPSTATUS UI_PointerButton(UI_TContext *context, bool pressed);
```

### Parameters

- `context`: UI or GUI context, initialized and inside its frame lifecycle as required.
- `pressed`: pointer button down/up state.

### Return value

`OPSTATUS`: SUCCESS if the operation completes; invalid frame/lifecycle, invalid range or incompatible input returns the corresponding status.

### Remarks

Terminal UI widgets draw onto a Canvas in the active Begin/End frame, without taking permanent ownership of the caller's widget values.

### Example

```c
/* Usage fragment: context, geometry and widget structs are initialized by the caller. */

OPSTATUS status = UI_FUNC(PointerButton)(&context, true);
```

# Scroll

Feeds a mouse wheel or scroll delta.

### Syntax

#### Macro form

```c
OPSTATUS UI_FUNC(Scroll)(UI_TYPE(TContext) *context, int32_t delta);
```

#### Direct form

```c
OPSTATUS UI_Scroll(UI_TContext *context, int32_t delta);
```

### Parameters

- `context`: UI or GUI context, initialized and inside its frame lifecycle as required.
- `delta`: scroll delta.

### Return value

`OPSTATUS`: SUCCESS if the operation completes; invalid frame/lifecycle, invalid range or incompatible input returns the corresponding status.

### Remarks

Terminal UI widgets draw onto a Canvas in the active Begin/End frame, without taking permanent ownership of the caller's widget values.

### Example

```c
/* Usage fragment: context, geometry and widget structs are initialized by the caller. */

OPSTATUS status = UI_FUNC(Scroll)(&context, 1);
```

# GetId

Builds a stable widget ID from the label and current scope.

### Syntax

#### Macro form

```c
UI_TYPE(WidgetId) UI_FUNC(GetId)(const UI_TYPE(TContext) *context, const char *label);
```

#### Direct form

```c
UI_WidgetId UI_GetId(const UI_TContext *context, const char *label);
```

### Parameters

- `context`: UI or GUI context, initialized and inside its frame lifecycle as required.
- `label`: widget label and stable identification text.

### Return value

Returns a value of type `UI_TYPE(WidgetId)` by value.

### Remarks

Terminal UI widgets draw onto a Canvas in the active Begin/End frame, without taking permanent ownership of the caller's widget values.

### Example

```c
/* Usage fragment: context, geometry and widget structs are initialized by the caller. */

UI_TYPE(WidgetId) id = UI_FUNC(GetId)(&context, "OK");
```

# PushId

Pushes a stable identifier scope for repeated widget labels.

### Syntax

#### Macro form

```c
OPSTATUS UI_FUNC(PushId)(UI_TYPE(TContext) *context, const char *scope);
```

#### Direct form

```c
OPSTATUS UI_PushId(UI_TContext *context, const char *scope);
```

### Parameters

- `context`: UI or GUI context, initialized and inside its frame lifecycle as required.
- `scope`: stable identifier scope segment.

### Return value

`OPSTATUS`: SUCCESS if the operation completes; invalid frame/lifecycle, invalid range or incompatible input returns the corresponding status.

### Remarks

Terminal UI widgets draw onto a Canvas in the active Begin/End frame, without taking permanent ownership of the caller's widget values. Balance push/pop scope calls before completing the frame.

### Example

```c
/* Usage fragment: context, geometry and widget structs are initialized by the caller. */

OPSTATUS status = UI_FUNC(PushId)(&context, "group");
```

# PopId

Restores previous widget identifier scope.

### Syntax

#### Macro form

```c
OPSTATUS UI_FUNC(PopId)(UI_TYPE(TContext) *context);
```

#### Direct form

```c
OPSTATUS UI_PopId(UI_TContext *context);
```

### Parameters

- `context`: UI or GUI context, initialized and inside its frame lifecycle as required.

### Return value

`OPSTATUS`: SUCCESS if the operation completes; invalid frame/lifecycle, invalid range or incompatible input returns the corresponding status.

### Remarks

Terminal UI widgets draw onto a Canvas in the active Begin/End frame, without taking permanent ownership of the caller's widget values. Balance push/pop scope calls before completing the frame.

### Example

```c
/* Usage fragment: context, geometry and widget structs are initialized by the caller. */

OPSTATUS status = UI_FUNC(PopId)(&context);
```

# Focus_Set

Moves keyboard focus to a widget ID.

### Syntax

#### Macro form

```c
OPSTATUS UI_FUNC(Focus_Set)(UI_TYPE(TContext) *context, UI_TYPE(WidgetId) widget);
```

#### Direct form

```c
OPSTATUS UI_Focus_Set(UI_TContext *context, UI_WidgetId widget);
```

### Parameters

- `context`: UI or GUI context, initialized and inside its frame lifecycle as required.
- `widget`: stable widget ID used by focus APIs.

### Return value

`OPSTATUS`: SUCCESS if the operation completes; invalid frame/lifecycle, invalid range or incompatible input returns the corresponding status.

### Remarks

Terminal UI widgets draw onto a Canvas in the active Begin/End frame, without taking permanent ownership of the caller's widget values.

### Example

```c
/* Usage fragment: context, geometry and widget structs are initialized by the caller. */

OPSTATUS status = UI_FUNC(Focus_Set)(&context, id);
```

# Focus_Next

Advances keyboard focus among eligible widgets.

### Syntax

#### Macro form

```c
OPSTATUS UI_FUNC(Focus_Next)(UI_TYPE(TContext) *context);
```

#### Direct form

```c
OPSTATUS UI_Focus_Next(UI_TContext *context);
```

### Parameters

- `context`: UI or GUI context, initialized and inside its frame lifecycle as required.

### Return value

`OPSTATUS`: SUCCESS if the operation completes; invalid frame/lifecycle, invalid range or incompatible input returns the corresponding status.

### Remarks

Terminal UI widgets draw onto a Canvas in the active Begin/End frame, without taking permanent ownership of the caller's widget values.

### Example

```c
/* Usage fragment: context, geometry and widget structs are initialized by the caller. */

OPSTATUS status = UI_FUNC(Focus_Next)(&context);
```

# Focus_Previous

Moves keyboard focus backward.

### Syntax

#### Macro form

```c
OPSTATUS UI_FUNC(Focus_Previous)(UI_TYPE(TContext) *context);
```

#### Direct form

```c
OPSTATUS UI_Focus_Previous(UI_TContext *context);
```

### Parameters

- `context`: UI or GUI context, initialized and inside its frame lifecycle as required.

### Return value

`OPSTATUS`: SUCCESS if the operation completes; invalid frame/lifecycle, invalid range or incompatible input returns the corresponding status.

### Remarks

Terminal UI widgets draw onto a Canvas in the active Begin/End frame, without taking permanent ownership of the caller's widget values.

### Example

```c
/* Usage fragment: context, geometry and widget structs are initialized by the caller. */

OPSTATUS status = UI_FUNC(Focus_Previous)(&context);
```

# IsFocused

Tests whether widget ID owns focus.

### Syntax

#### Macro form

```c
bool UI_FUNC(IsFocused)(const UI_TYPE(TContext) *context, UI_TYPE(WidgetId) widget);
```

#### Direct form

```c
bool UI_IsFocused(const UI_TContext *context, UI_WidgetId widget);
```

### Parameters

- `context`: UI or GUI context, initialized and inside its frame lifecycle as required.
- `widget`: stable widget ID used by focus APIs.

### Return value

`bool`: indicates the state queried, not an OPSTATUS.

### Remarks

Terminal UI widgets draw onto a Canvas in the active Begin/End frame, without taking permanent ownership of the caller's widget values.

### Example

```c
/* Usage fragment: context, geometry and widget structs are initialized by the caller. */

bool result = UI_FUNC(IsFocused)(&context, id);
```

# Row_Begin

Begins a row layout region.

### Syntax

#### Macro form

```c
OPSTATUS UI_FUNC(Row_Begin)(UI_TYPE(TContext) *context, TQUAD_TYPE(uint16) region, uint16_t gap);
```

#### Direct form

```c
OPSTATUS UI_Row_Begin(UI_TContext *context, TQuad_uint16 region, uint16_t gap);
```

### Parameters

- `context`: UI or GUI context, initialized and inside its frame lifecycle as required.
- `region`: rectangle relative to the current Canvas/Surface or layout.
- `gap`: distance between successive layout elements.

### Return value

`OPSTATUS`: SUCCESS if the operation completes; invalid frame/lifecycle, invalid range or incompatible input returns the corresponding status.

### Remarks

Terminal UI widgets draw onto a Canvas in the active Begin/End frame, without taking permanent ownership of the caller's widget values.

### Example

```c
/* Usage fragment: context, geometry and widget structs are initialized by the caller. */

OPSTATUS status = UI_FUNC(Row_Begin)(&context, region, 1);
```

# Column_Begin

Begins a column layout region.

### Syntax

#### Macro form

```c
OPSTATUS UI_FUNC(Column_Begin)(UI_TYPE(TContext) *context, TQUAD_TYPE(uint16) region, uint16_t gap);
```

#### Direct form

```c
OPSTATUS UI_Column_Begin(UI_TContext *context, TQuad_uint16 region, uint16_t gap);
```

### Parameters

- `context`: UI or GUI context, initialized and inside its frame lifecycle as required.
- `region`: rectangle relative to the current Canvas/Surface or layout.
- `gap`: distance between successive layout elements.

### Return value

`OPSTATUS`: SUCCESS if the operation completes; invalid frame/lifecycle, invalid range or incompatible input returns the corresponding status.

### Remarks

Terminal UI widgets draw onto a Canvas in the active Begin/End frame, without taking permanent ownership of the caller's widget values.

### Example

```c
/* Usage fragment: context, geometry and widget structs are initialized by the caller. */

OPSTATUS status = UI_FUNC(Column_Begin)(&context, region, 1);
```

# Layout_Next

Requests the next rectangle in current layout.

### Syntax

#### Macro form

```c
OPSTATUS UI_FUNC(Layout_Next)(UI_TYPE(TContext) *context, uint16_t extent, TQUAD_TYPE(uint16) *outRegion);
```

#### Direct form

```c
OPSTATUS UI_Layout_Next(UI_TContext *context, uint16_t extent, TQuad_uint16 *outRegion);
```

### Parameters

- `context`: UI or GUI context, initialized and inside its frame lifecycle as required.
- `extent`: requested layout element size.
- `outRegion`: output geometry region.

### Return value

`OPSTATUS`: SUCCESS if the operation completes; invalid frame/lifecycle, invalid range or incompatible input returns the corresponding status.

### Remarks

Terminal UI widgets draw onto a Canvas in the active Begin/End frame, without taking permanent ownership of the caller's widget values.

### Example

```c
/* Usage fragment: context, geometry and widget structs are initialized by the caller. */
TQUAD_TYPE(uint16) outRegion = {0};
OPSTATUS status = UI_FUNC(Layout_Next)(&context, 5, &outRegion);
```

# Layout_Remaining

Queries remaining layout space.

### Syntax

#### Macro form

```c
OPSTATUS UI_FUNC(Layout_Remaining)(UI_TYPE(TContext) *context, TQUAD_TYPE(uint16) *outRegion);
```

#### Direct form

```c
OPSTATUS UI_Layout_Remaining(UI_TContext *context, TQuad_uint16 *outRegion);
```

### Parameters

- `context`: UI or GUI context, initialized and inside its frame lifecycle as required.
- `outRegion`: output geometry region.

### Return value

`OPSTATUS`: SUCCESS if the operation completes; invalid frame/lifecycle, invalid range or incompatible input returns the corresponding status.

### Remarks

Terminal UI widgets draw onto a Canvas in the active Begin/End frame, without taking permanent ownership of the caller's widget values.

### Example

```c
/* Usage fragment: context, geometry and widget structs are initialized by the caller. */
TQUAD_TYPE(uint16) outRegion = {0};
OPSTATUS status = UI_FUNC(Layout_Remaining)(&context, &outRegion);
```

# Layout_End

Closes current layout scope.

### Syntax

#### Macro form

```c
OPSTATUS UI_FUNC(Layout_End)(UI_TYPE(TContext) *context);
```

#### Direct form

```c
OPSTATUS UI_Layout_End(UI_TContext *context);
```

### Parameters

- `context`: UI or GUI context, initialized and inside its frame lifecycle as required.

### Return value

`OPSTATUS`: SUCCESS if the operation completes; invalid frame/lifecycle, invalid range or incompatible input returns the corresponding status.

### Remarks

Terminal UI widgets draw onto a Canvas in the active Begin/End frame, without taking permanent ownership of the caller's widget values.

### Example

```c
/* Usage fragment: context, geometry and widget structs are initialized by the caller. */

OPSTATUS status = UI_FUNC(Layout_End)(&context);
```

# Panel_Begin

Begins a panel using direct geometry and title.

### Syntax

#### Macro form

```c
OPSTATUS UI_FUNC(Panel_Begin)(UI_TYPE(TContext) *context, TQUAD_TYPE(uint16) region, const char *title);
```

#### Direct form

```c
OPSTATUS UI_Panel_Begin(UI_TContext *context, TQuad_uint16 region, const char *title);
```

### Parameters

- `context`: UI or GUI context, initialized and inside its frame lifecycle as required.
- `region`: rectangle relative to the current Canvas/Surface or layout.
- `title`: caption for a panel or dialog.

### Return value

`OPSTATUS`: SUCCESS if the operation completes; invalid frame/lifecycle, invalid range or incompatible input returns the corresponding status.

### Remarks

Terminal UI widgets draw onto a Canvas in the active Begin/End frame, without taking permanent ownership of the caller's widget values.

### Example

```c
/* Usage fragment: context, geometry and widget structs are initialized by the caller. */

OPSTATUS status = UI_FUNC(Panel_Begin)(&context, region, "Title");
```

# Panel_BeginEx

Begins a panel using a struct configuration.

### Syntax

#### Macro form

```c
OPSTATUS UI_FUNC(Panel_BeginEx)(UI_TYPE(TContext) *context, const UI_TYPE(TPanel) *panel);
```

#### Direct form

```c
OPSTATUS UI_Panel_BeginEx(UI_TContext *context, const UI_TPanel *panel);
```

### Parameters

- `context`: UI or GUI context, initialized and inside its frame lifecycle as required.
- `panel`: panel widget configuration.

### Return value

`OPSTATUS`: SUCCESS if the operation completes; invalid frame/lifecycle, invalid range or incompatible input returns the corresponding status.

### Remarks

Terminal UI widgets draw onto a Canvas in the active Begin/End frame, without taking permanent ownership of the caller's widget values. The struct configuration form shares the same interaction implementation as the direct form.

### Example

```c
/* Usage fragment: context, geometry and widget structs are initialized by the caller. */

OPSTATUS status = UI_FUNC(Panel_BeginEx)(&context, &panel);
```

# Panel_End

Closes a panel scope.

### Syntax

#### Macro form

```c
OPSTATUS UI_FUNC(Panel_End)(UI_TYPE(TContext) *context);
```

#### Direct form

```c
OPSTATUS UI_Panel_End(UI_TContext *context);
```

### Parameters

- `context`: UI or GUI context, initialized and inside its frame lifecycle as required.

### Return value

`OPSTATUS`: SUCCESS if the operation completes; invalid frame/lifecycle, invalid range or incompatible input returns the corresponding status.

### Remarks

Terminal UI widgets draw onto a Canvas in the active Begin/End frame, without taking permanent ownership of the caller's widget values.

### Example

```c
/* Usage fragment: context, geometry and widget structs are initialized by the caller. */

OPSTATUS status = UI_FUNC(Panel_End)(&context);
```

# Label

Draws text as a non-interactive label.

### Syntax

#### Macro form

```c
OPSTATUS UI_FUNC(Label)(UI_TYPE(TContext) *context, TQUAD_TYPE(uint16) region, const char *text);
```

#### Direct form

```c
OPSTATUS UI_Label(UI_TContext *context, TQuad_uint16 region, const char *text);
```

### Parameters

- `context`: UI or GUI context, initialized and inside its frame lifecycle as required.
- `region`: rectangle relative to the current Canvas/Surface or layout.
- `text`: UTF-8 text used for display or measurement.

### Return value

`OPSTATUS`: SUCCESS if the operation completes; invalid frame/lifecycle, invalid range or incompatible input returns the corresponding status.

### Remarks

Terminal UI widgets draw onto a Canvas in the active Begin/End frame, without taking permanent ownership of the caller's widget values.

### Example

```c
/* Usage fragment: context, geometry and widget structs are initialized by the caller. */

OPSTATUS status = UI_FUNC(Label)(&context, region, "Hello");
```

# LabelEx

Draws a label using its struct configuration.

### Syntax

#### Macro form

```c
OPSTATUS UI_FUNC(LabelEx)(UI_TYPE(TContext) *context, const UI_TYPE(TLabel) *label);
```

#### Direct form

```c
OPSTATUS UI_LabelEx(UI_TContext *context, const UI_TLabel *label);
```

### Parameters

- `context`: UI or GUI context, initialized and inside its frame lifecycle as required.
- `label`: widget label and stable identification text.

### Return value

`OPSTATUS`: SUCCESS if the operation completes; invalid frame/lifecycle, invalid range or incompatible input returns the corresponding status.

### Remarks

Terminal UI widgets draw onto a Canvas in the active Begin/End frame, without taking permanent ownership of the caller's widget values. The struct configuration form shares the same interaction implementation as the direct form.

### Example

```c
/* Usage fragment: context, geometry and widget structs are initialized by the caller. */

OPSTATUS status = UI_FUNC(LabelEx)(&context, "OK");
```

# Separator

Draws a visual separation rule.

### Syntax

#### Macro form

```c
OPSTATUS UI_FUNC(Separator)(UI_TYPE(TContext) *context, TQUAD_TYPE(uint16) region);
```

#### Direct form

```c
OPSTATUS UI_Separator(UI_TContext *context, TQuad_uint16 region);
```

### Parameters

- `context`: UI or GUI context, initialized and inside its frame lifecycle as required.
- `region`: rectangle relative to the current Canvas/Surface or layout.

### Return value

`OPSTATUS`: SUCCESS if the operation completes; invalid frame/lifecycle, invalid range or incompatible input returns the corresponding status.

### Remarks

Terminal UI widgets draw onto a Canvas in the active Begin/End frame, without taking permanent ownership of the caller's widget values.

### Example

```c
/* Usage fragment: context, geometry and widget structs are initialized by the caller. */

OPSTATUS status = UI_FUNC(Separator)(&context, region);
```

# SeparatorEx

Draws a separator using its struct configuration.

### Syntax

#### Macro form

```c
OPSTATUS UI_FUNC(SeparatorEx)(UI_TYPE(TContext) *context, const UI_TYPE(TSeparator) *separator);
```

#### Direct form

```c
OPSTATUS UI_SeparatorEx(UI_TContext *context, const UI_TSeparator *separator);
```

### Parameters

- `context`: UI or GUI context, initialized and inside its frame lifecycle as required.
- `separator`: separator widget configuration.

### Return value

`OPSTATUS`: SUCCESS if the operation completes; invalid frame/lifecycle, invalid range or incompatible input returns the corresponding status.

### Remarks

Terminal UI widgets draw onto a Canvas in the active Begin/End frame, without taking permanent ownership of the caller's widget values. The struct configuration form shares the same interaction implementation as the direct form.

### Example

```c
/* Usage fragment: context, geometry and widget structs are initialized by the caller. */

OPSTATUS status = UI_FUNC(SeparatorEx)(&context, &separator);
```

# Button

Draws an interactive button and reports activation.

### Syntax

#### Macro form

```c
OPSTATUS UI_FUNC(Button)(UI_TYPE(TContext) *context, TQUAD_TYPE(uint16) region, const char *label, bool *outPressed);
```

#### Direct form

```c
OPSTATUS UI_Button(UI_TContext *context, TQuad_uint16 region, const char *label, bool *outPressed);
```

### Parameters

- `context`: UI or GUI context, initialized and inside its frame lifecycle as required.
- `region`: rectangle relative to the current Canvas/Surface or layout.
- `label`: widget label and stable identification text.
- `outPressed`: receives whether button activated.

### Return value

`OPSTATUS`: SUCCESS if the operation completes; invalid frame/lifecycle, invalid range or incompatible input returns the corresponding status.

### Remarks

Terminal UI widgets draw onto a Canvas in the active Begin/End frame, without taking permanent ownership of the caller's widget values.

### Example

```c
/* Usage fragment: context, geometry and widget structs are initialized by the caller. */
bool pressed = false;
OPSTATUS status = UI_FUNC(Button)(&context, region, "OK", &pressed);
```

# ButtonEx

Draws a button from its struct configuration.

### Syntax

#### Macro form

```c
OPSTATUS UI_FUNC(ButtonEx)(UI_TYPE(TContext) *context, UI_TYPE(TButton) *button);
```

#### Direct form

```c
OPSTATUS UI_ButtonEx(UI_TContext *context, UI_TButton *button);
```

### Parameters

- `context`: UI or GUI context, initialized and inside its frame lifecycle as required.
- `button`: button widget configuration.

### Return value

`OPSTATUS`: SUCCESS if the operation completes; invalid frame/lifecycle, invalid range or incompatible input returns the corresponding status.

### Remarks

Terminal UI widgets draw onto a Canvas in the active Begin/End frame, without taking permanent ownership of the caller's widget values. The struct configuration form shares the same interaction implementation as the direct form.

### Example

```c
/* Usage fragment: context, geometry and widget structs are initialized by the caller. */

OPSTATUS status = UI_FUNC(ButtonEx)(&context, &button);
```

# Checkbox

Draws a checkbox and updates its selected state.

### Syntax

#### Macro form

```c
OPSTATUS UI_FUNC(Checkbox)(UI_TYPE(TContext) *context, TQUAD_TYPE(uint16) region, const char *label, bool *checked);
```

#### Direct form

```c
OPSTATUS UI_Checkbox(UI_TContext *context, TQuad_uint16 region, const char *label, bool *checked);
```

### Parameters

- `context`: UI or GUI context, initialized and inside its frame lifecycle as required.
- `region`: rectangle relative to the current Canvas/Surface or layout.
- `label`: widget label and stable identification text.
- `checked`: borrowed boolean state updated upon checkbox interaction.

### Return value

`OPSTATUS`: SUCCESS if the operation completes; invalid frame/lifecycle, invalid range or incompatible input returns the corresponding status.

### Remarks

Terminal UI widgets draw onto a Canvas in the active Begin/End frame, without taking permanent ownership of the caller's widget values.

### Example

```c
/* Usage fragment: context, geometry and widget structs are initialized by the caller. */
bool checked = false;
OPSTATUS status = UI_FUNC(Checkbox)(&context, region, "OK", &checked);
```

# CheckboxEx

Draws a checkbox from a struct.

### Syntax

#### Macro form

```c
OPSTATUS UI_FUNC(CheckboxEx)(UI_TYPE(TContext) *context, UI_TYPE(TCheckbox) *checkbox);
```

#### Direct form

```c
OPSTATUS UI_CheckboxEx(UI_TContext *context, UI_TCheckbox *checkbox);
```

### Parameters

- `context`: UI or GUI context, initialized and inside its frame lifecycle as required.
- `checkbox`: checkbox widget configuration.

### Return value

`OPSTATUS`: SUCCESS if the operation completes; invalid frame/lifecycle, invalid range or incompatible input returns the corresponding status.

### Remarks

Terminal UI widgets draw onto a Canvas in the active Begin/End frame, without taking permanent ownership of the caller's widget values. The struct configuration form shares the same interaction implementation as the direct form.

### Example

```c
/* Usage fragment: context, geometry and widget structs are initialized by the caller. */

OPSTATUS status = UI_FUNC(CheckboxEx)(&context, &checkbox);
```

# Radio

Draws a radio selection and updates selected value.

### Syntax

#### Macro form

```c
OPSTATUS UI_FUNC(Radio)(UI_TYPE(TContext) *context, TQUAD_TYPE(uint16) region, const char *label, uint32_t option, uint32_t *selected);
```

#### Direct form

```c
OPSTATUS UI_Radio(UI_TContext *context, TQuad_uint16 region, const char *label, uint32_t option, uint32_t *selected);
```

### Parameters

- `context`: UI or GUI context, initialized and inside its frame lifecycle as required.
- `region`: rectangle relative to the current Canvas/Surface or layout.
- `label`: widget label and stable identification text.
- `option`: value corresponding to radio choice.
- `selected`: caller-owned selected index or option.

### Return value

`OPSTATUS`: SUCCESS if the operation completes; invalid frame/lifecycle, invalid range or incompatible input returns the corresponding status.

### Remarks

Terminal UI widgets draw onto a Canvas in the active Begin/End frame, without taking permanent ownership of the caller's widget values.

### Example

```c
/* Usage fragment: context, geometry and widget structs are initialized by the caller. */
uint32_t selected = 0;
OPSTATUS status = UI_FUNC(Radio)(&context, region, "OK", 1, &selected);
```

# RadioEx

Draws a radio widget configured by struct.

### Syntax

#### Macro form

```c
OPSTATUS UI_FUNC(RadioEx)(UI_TYPE(TContext) *context, UI_TYPE(TRadio) *radio);
```

#### Direct form

```c
OPSTATUS UI_RadioEx(UI_TContext *context, UI_TRadio *radio);
```

### Parameters

- `context`: UI or GUI context, initialized and inside its frame lifecycle as required.
- `radio`: radio widget configuration.

### Return value

`OPSTATUS`: SUCCESS if the operation completes; invalid frame/lifecycle, invalid range or incompatible input returns the corresponding status.

### Remarks

Terminal UI widgets draw onto a Canvas in the active Begin/End frame, without taking permanent ownership of the caller's widget values. The struct configuration form shares the same interaction implementation as the direct form.

### Example

```c
/* Usage fragment: context, geometry and widget structs are initialized by the caller. */

OPSTATUS status = UI_FUNC(RadioEx)(&context, &radio);
```

# Slider

Draws a ranged slider and modifies a caller-owned numeric value.

### Syntax

#### Macro form

```c
OPSTATUS UI_FUNC(Slider)(UI_TYPE(TContext) *context, TQUAD_TYPE(uint16) region, int32_t minimum, int32_t maximum, int32_t *value);
```

#### Direct form

```c
OPSTATUS UI_Slider(UI_TContext *context, TQuad_uint16 region, int32_t minimum, int32_t maximum, int32_t *value);
```

### Parameters

- `context`: UI or GUI context, initialized and inside its frame lifecycle as required.
- `region`: rectangle relative to the current Canvas/Surface or layout.
- `minimum`: lower value bound.
- `maximum`: upper value bound.
- `value`: widget value to show or change.

### Return value

`OPSTATUS`: SUCCESS if the operation completes; invalid frame/lifecycle, invalid range or incompatible input returns the corresponding status.

### Remarks

Terminal UI widgets draw onto a Canvas in the active Begin/End frame, without taking permanent ownership of the caller's widget values.

### Example

```c
/* Usage fragment: context, geometry and widget structs are initialized by the caller. */
int32_t value = 10;
OPSTATUS status = UI_FUNC(Slider)(&context, region, 0, 100, &value);
```

# SliderEx

Draws a slider from a struct.

### Syntax

#### Macro form

```c
OPSTATUS UI_FUNC(SliderEx)(UI_TYPE(TContext) *context, UI_TYPE(TSlider) *slider);
```

#### Direct form

```c
OPSTATUS UI_SliderEx(UI_TContext *context, UI_TSlider *slider);
```

### Parameters

- `context`: UI or GUI context, initialized and inside its frame lifecycle as required.
- `slider`: slider widget configuration.

### Return value

`OPSTATUS`: SUCCESS if the operation completes; invalid frame/lifecycle, invalid range or incompatible input returns the corresponding status.

### Remarks

Terminal UI widgets draw onto a Canvas in the active Begin/End frame, without taking permanent ownership of the caller's widget values. The struct configuration form shares the same interaction implementation as the direct form.

### Example

```c
/* Usage fragment: context, geometry and widget structs are initialized by the caller. */

OPSTATUS status = UI_FUNC(SliderEx)(&context, &slider);
```

# Progress

Draws normalized progress feedback.

### Syntax

#### Macro form

```c
OPSTATUS UI_FUNC(Progress)(UI_TYPE(TContext) *context, TQUAD_TYPE(uint16) region, float fraction);
```

#### Direct form

```c
OPSTATUS UI_Progress(UI_TContext *context, TQuad_uint16 region, float fraction);
```

### Parameters

- `context`: UI or GUI context, initialized and inside its frame lifecycle as required.
- `region`: rectangle relative to the current Canvas/Surface or layout.
- `fraction`: normalized progress fraction.

### Return value

`OPSTATUS`: SUCCESS if the operation completes; invalid frame/lifecycle, invalid range or incompatible input returns the corresponding status.

### Remarks

Terminal UI widgets draw onto a Canvas in the active Begin/End frame, without taking permanent ownership of the caller's widget values.

### Example

```c
/* Usage fragment: context, geometry and widget structs are initialized by the caller. */

OPSTATUS status = UI_FUNC(Progress)(&context, region, 0.5f);
```

# ProgressEx

Draws a progress widget from a struct.

### Syntax

#### Macro form

```c
OPSTATUS UI_FUNC(ProgressEx)(UI_TYPE(TContext) *context, const UI_TYPE(TProgress) *progress);
```

#### Direct form

```c
OPSTATUS UI_ProgressEx(UI_TContext *context, const UI_TProgress *progress);
```

### Parameters

- `context`: UI or GUI context, initialized and inside its frame lifecycle as required.
- `progress`: progress descriptor/state.

### Return value

`OPSTATUS`: SUCCESS if the operation completes; invalid frame/lifecycle, invalid range or incompatible input returns the corresponding status.

### Remarks

Terminal UI widgets draw onto a Canvas in the active Begin/End frame, without taking permanent ownership of the caller's widget values. The struct configuration form shares the same interaction implementation as the direct form.

### Example

```c
/* Usage fragment: context, geometry and widget structs are initialized by the caller. */

OPSTATUS status = UI_FUNC(ProgressEx)(&context, &progress);
```

# Menu

Draws and interacts with a list of menu items.

### Syntax

#### Macro form

```c
OPSTATUS UI_FUNC(Menu)(UI_TYPE(TContext) *context, TQUAD_TYPE(uint16) region, const char *const *items, size_t count, size_t *selected, bool *outActivated);
```

#### Direct form

```c
OPSTATUS UI_Menu(UI_TContext *context, TQuad_uint16 region, const char *const *items, size_t count, size_t *selected, bool *outActivated);
```

### Parameters

- `context`: UI or GUI context, initialized and inside its frame lifecycle as required.
- `region`: rectangle relative to the current Canvas/Surface or layout.
- `items`: borrowed array of display-item strings.
- `count`: number of items or render commands.
- `selected`: caller-owned selected index or option.
- `outActivated`: receives whether the selected item was activated.

### Return value

`OPSTATUS`: SUCCESS if the operation completes; invalid frame/lifecycle, invalid range or incompatible input returns the corresponding status.

### Remarks

Terminal UI widgets draw onto a Canvas in the active Begin/End frame, without taking permanent ownership of the caller's widget values.

### Example

```c
/* Usage fragment: context, geometry and widget structs are initialized by the caller. */
const char *items[] = {"One", "Two"};
size_t selected = 0;
bool activated = false;
OPSTATUS status = UI_FUNC(Menu)(&context, region, items, 2, &selected, &activated);
```

# MenuEx

Draws a menu with struct configuration.

### Syntax

#### Macro form

```c
OPSTATUS UI_FUNC(MenuEx)(UI_TYPE(TContext) *context, UI_TYPE(TMenu) *menu);
```

#### Direct form

```c
OPSTATUS UI_MenuEx(UI_TContext *context, UI_TMenu *menu);
```

### Parameters

- `context`: UI or GUI context, initialized and inside its frame lifecycle as required.
- `menu`: menu widget configuration.

### Return value

`OPSTATUS`: SUCCESS if the operation completes; invalid frame/lifecycle, invalid range or incompatible input returns the corresponding status.

### Remarks

Terminal UI widgets draw onto a Canvas in the active Begin/End frame, without taking permanent ownership of the caller's widget values. The struct configuration form shares the same interaction implementation as the direct form.

### Example

```c
/* Usage fragment: context, geometry and widget structs are initialized by the caller. */

OPSTATUS status = UI_FUNC(MenuEx)(&context, &menu);
```

# ProgressBar

Draws a progress bar with current and maximum values.

### Syntax

#### Macro form

```c
OPSTATUS UI_FUNC(ProgressBar)(UI_TYPE(TContext) *context, TQUAD_TYPE(uint16) region, uint64_t value, uint64_t maximum);
```

#### Direct form

```c
OPSTATUS UI_ProgressBar(UI_TContext *context, TQuad_uint16 region, uint64_t value, uint64_t maximum);
```

### Parameters

- `context`: UI or GUI context, initialized and inside its frame lifecycle as required.
- `region`: rectangle relative to the current Canvas/Surface or layout.
- `value`: widget value to show or change.
- `maximum`: upper value bound.

### Return value

`OPSTATUS`: SUCCESS if the operation completes; invalid frame/lifecycle, invalid range or incompatible input returns the corresponding status.

### Remarks

Terminal UI widgets draw onto a Canvas in the active Begin/End frame, without taking permanent ownership of the caller's widget values.

### Example

```c
/* Usage fragment: context, geometry and widget structs are initialized by the caller. */

OPSTATUS status = UI_FUNC(ProgressBar)(&context, region, 50, 100);
```

# ProgressBarEx

Draws a progress bar using struct settings.

### Syntax

#### Macro form

```c
OPSTATUS UI_FUNC(ProgressBarEx)(UI_TYPE(TContext) *context, const UI_TYPE(TProgressBar) *progress);
```

#### Direct form

```c
OPSTATUS UI_ProgressBarEx(UI_TContext *context, const UI_TProgressBar *progress);
```

### Parameters

- `context`: UI or GUI context, initialized and inside its frame lifecycle as required.
- `progress`: progress descriptor/state.

### Return value

`OPSTATUS`: SUCCESS if the operation completes; invalid frame/lifecycle, invalid range or incompatible input returns the corresponding status.

### Remarks

Terminal UI widgets draw onto a Canvas in the active Begin/End frame, without taking permanent ownership of the caller's widget values. The struct configuration form shares the same interaction implementation as the direct form.

### Example

```c
/* Usage fragment: context, geometry and widget structs are initialized by the caller. */

OPSTATUS status = UI_FUNC(ProgressBarEx)(&context, &progress);
```

# Dialog

Draws a dialog and reports dismissal.

### Syntax

#### Macro form

```c
OPSTATUS UI_FUNC(Dialog)(UI_TYPE(TContext) *context, TQUAD_TYPE(uint16) region, const char *title, const char *message, bool *outDismissed);
```

#### Direct form

```c
OPSTATUS UI_Dialog(UI_TContext *context, TQuad_uint16 region, const char *title, const char *message, bool *outDismissed);
```

### Parameters

- `context`: UI or GUI context, initialized and inside its frame lifecycle as required.
- `region`: rectangle relative to the current Canvas/Surface or layout.
- `title`: caption for a panel or dialog.
- `message`: message displayed to users.
- `outDismissed`: receives whether the dialog was dismissed.

### Return value

`OPSTATUS`: SUCCESS if the operation completes; invalid frame/lifecycle, invalid range or incompatible input returns the corresponding status.

### Remarks

Terminal UI widgets draw onto a Canvas in the active Begin/End frame, without taking permanent ownership of the caller's widget values.

### Example

```c
/* Usage fragment: context, geometry and widget structs are initialized by the caller. */
bool dismissed = false;
OPSTATUS status = UI_FUNC(Dialog)(&context, region, "Title", "Message", &dismissed);
```

# DialogEx

Draws a dialog from its full configuration.

### Syntax

#### Macro form

```c
OPSTATUS UI_FUNC(DialogEx)(UI_TYPE(TContext) *context, UI_TYPE(TDialog) *dialog);
```

#### Direct form

```c
OPSTATUS UI_DialogEx(UI_TContext *context, UI_TDialog *dialog);
```

### Parameters

- `context`: UI or GUI context, initialized and inside its frame lifecycle as required.
- `dialog`: dialog or confirmation descriptor.

### Return value

`OPSTATUS`: SUCCESS if the operation completes; invalid frame/lifecycle, invalid range or incompatible input returns the corresponding status.

### Remarks

Terminal UI widgets draw onto a Canvas in the active Begin/End frame, without taking permanent ownership of the caller's widget values. The struct configuration form shares the same interaction implementation as the direct form.

### Example

```c
/* Usage fragment: context, geometry and widget structs are initialized by the caller. */

OPSTATUS status = UI_FUNC(DialogEx)(&context, &dialog);
```

# Confirm

Draws a confirm/reject dialog, reporting selected action.

### Syntax

#### Macro form

```c
OPSTATUS UI_FUNC(Confirm)(UI_TYPE(TContext) *context, TQUAD_TYPE(uint16) region, const char *message, bool *outAccepted, bool *outRejected);
```

#### Direct form

```c
OPSTATUS UI_Confirm(UI_TContext *context, TQuad_uint16 region, const char *message, bool *outAccepted, bool *outRejected);
```

### Parameters

- `context`: UI or GUI context, initialized and inside its frame lifecycle as required.
- `region`: rectangle relative to the current Canvas/Surface or layout.
- `message`: message displayed to users.
- `outAccepted`: receives positive confirmation.
- `outRejected`: receives negative confirmation.

### Return value

`OPSTATUS`: SUCCESS if the operation completes; invalid frame/lifecycle, invalid range or incompatible input returns the corresponding status.

### Remarks

Terminal UI widgets draw onto a Canvas in the active Begin/End frame, without taking permanent ownership of the caller's widget values.

### Example

```c
/* Usage fragment: context, geometry and widget structs are initialized by the caller. */
bool accepted = false;
bool rejected = false;
OPSTATUS status = UI_FUNC(Confirm)(&context, region, "Message", &accepted, &rejected);
```

# ConfirmEx

Draws a confirmation configured by struct.

### Syntax

#### Macro form

```c
OPSTATUS UI_FUNC(ConfirmEx)(UI_TYPE(TContext) *context, UI_TYPE(TDialog) *dialog);
```

#### Direct form

```c
OPSTATUS UI_ConfirmEx(UI_TContext *context, UI_TDialog *dialog);
```

### Parameters

- `context`: UI or GUI context, initialized and inside its frame lifecycle as required.
- `dialog`: dialog or confirmation descriptor.

### Return value

`OPSTATUS`: SUCCESS if the operation completes; invalid frame/lifecycle, invalid range or incompatible input returns the corresponding status.

### Remarks

Terminal UI widgets draw onto a Canvas in the active Begin/End frame, without taking permanent ownership of the caller's widget values. The struct configuration form shares the same interaction implementation as the direct form.

### Example

```c
/* Usage fragment: context, geometry and widget structs are initialized by the caller. */

OPSTATUS status = UI_FUNC(ConfirmEx)(&context, &dialog);
```

# StatusBar

Draws status text in a bar.

### Syntax

#### Macro form

```c
OPSTATUS UI_FUNC(StatusBar)(UI_TYPE(TContext) *context, TQUAD_TYPE(uint16) region, const char *left, const char *right);
```

#### Direct form

```c
OPSTATUS UI_StatusBar(UI_TContext *context, TQuad_uint16 region, const char *left, const char *right);
```

### Parameters

- `context`: UI or GUI context, initialized and inside its frame lifecycle as required.
- `region`: rectangle relative to the current Canvas/Surface or layout.
- `left`: left status bar text.
- `right`: right status bar text.

### Return value

`OPSTATUS`: SUCCESS if the operation completes; invalid frame/lifecycle, invalid range or incompatible input returns the corresponding status.

### Remarks

Terminal UI widgets draw onto a Canvas in the active Begin/End frame, without taking permanent ownership of the caller's widget values.

### Example

```c
/* Usage fragment: context, geometry and widget structs are initialized by the caller. */

OPSTATUS status = UI_FUNC(StatusBar)(&context, region, &left, &right);
```

# StatusBarEx

Draws a full status bar configuration.

### Syntax

#### Macro form

```c
OPSTATUS UI_FUNC(StatusBarEx)(UI_TYPE(TContext) *context, const UI_TYPE(TStatusBar) *bar);
```

#### Direct form

```c
OPSTATUS UI_StatusBarEx(UI_TContext *context, const UI_TStatusBar *bar);
```

### Parameters

- `context`: UI or GUI context, initialized and inside its frame lifecycle as required.
- `bar`: status bar descriptor.

### Return value

`OPSTATUS`: SUCCESS if the operation completes; invalid frame/lifecycle, invalid range or incompatible input returns the corresponding status.

### Remarks

Terminal UI widgets draw onto a Canvas in the active Begin/End frame, without taking permanent ownership of the caller's widget values. The struct configuration form shares the same interaction implementation as the direct form.

### Example

```c
/* Usage fragment: context, geometry and widget structs are initialized by the caller. */

OPSTATUS status = UI_FUNC(StatusBarEx)(&context, &bar);
```

# Toast

Draws a conditional non-modal notification.

### Syntax

#### Macro form

```c
OPSTATUS UI_FUNC(Toast)(UI_TYPE(TContext) *context, TQUAD_TYPE(uint16) region, const char *message, bool visible);
```

#### Direct form

```c
OPSTATUS UI_Toast(UI_TContext *context, TQuad_uint16 region, const char *message, bool visible);
```

### Parameters

- `context`: UI or GUI context, initialized and inside its frame lifecycle as required.
- `region`: rectangle relative to the current Canvas/Surface or layout.
- `message`: message displayed to users.
- `visible`: controls visibility of toast or scrollbar content.

### Return value

`OPSTATUS`: SUCCESS if the operation completes; invalid frame/lifecycle, invalid range or incompatible input returns the corresponding status.

### Remarks

Terminal UI widgets draw onto a Canvas in the active Begin/End frame, without taking permanent ownership of the caller's widget values.

### Example

```c
/* Usage fragment: context, geometry and widget structs are initialized by the caller. */

OPSTATUS status = UI_FUNC(Toast)(&context, region, "Message", true);
```

# ToastEx

Draws a toast from its configuration.

### Syntax

#### Macro form

```c
OPSTATUS UI_FUNC(ToastEx)(UI_TYPE(TContext) *context, const UI_TYPE(TToast) *toast);
```

#### Direct form

```c
OPSTATUS UI_ToastEx(UI_TContext *context, const UI_TToast *toast);
```

### Parameters

- `context`: UI or GUI context, initialized and inside its frame lifecycle as required.
- `toast`: caller-owned value or borrowed struct matching its declared type.

### Return value

`OPSTATUS`: SUCCESS if the operation completes; invalid frame/lifecycle, invalid range or incompatible input returns the corresponding status.

### Remarks

Terminal UI widgets draw onto a Canvas in the active Begin/End frame, without taking permanent ownership of the caller's widget values. The struct configuration form shares the same interaction implementation as the direct form.

### Example

```c
/* Usage fragment: context, geometry and widget structs are initialized by the caller. */

OPSTATUS status = UI_FUNC(ToastEx)(&context, &toast);
```

# Spinner

Draws an animated spinner using caller-supplied phase.

### Syntax

#### Macro form

```c
OPSTATUS UI_FUNC(Spinner)(UI_TYPE(TContext) *context, TQUAD_TYPE(uint16) region, const char *label, uint64_t phase);
```

#### Direct form

```c
OPSTATUS UI_Spinner(UI_TContext *context, TQuad_uint16 region, const char *label, uint64_t phase);
```

### Parameters

- `context`: UI or GUI context, initialized and inside its frame lifecycle as required.
- `region`: rectangle relative to the current Canvas/Surface or layout.
- `label`: widget label and stable identification text.
- `phase`: caller-supplied spinner animation phase.

### Return value

`OPSTATUS`: SUCCESS if the operation completes; invalid frame/lifecycle, invalid range or incompatible input returns the corresponding status.

### Remarks

Terminal UI widgets draw onto a Canvas in the active Begin/End frame, without taking permanent ownership of the caller's widget values.

### Example

```c
/* Usage fragment: context, geometry and widget structs are initialized by the caller. */

OPSTATUS status = UI_FUNC(Spinner)(&context, region, "OK", 0);
```

# SpinnerEx

Draws a spinner from its configuration.

### Syntax

#### Macro form

```c
OPSTATUS UI_FUNC(SpinnerEx)(UI_TYPE(TContext) *context, const UI_TYPE(TSpinner) *spinner);
```

#### Direct form

```c
OPSTATUS UI_SpinnerEx(UI_TContext *context, const UI_TSpinner *spinner);
```

### Parameters

- `context`: UI or GUI context, initialized and inside its frame lifecycle as required.
- `spinner`: caller-owned value or borrowed struct matching its declared type.

### Return value

`OPSTATUS`: SUCCESS if the operation completes; invalid frame/lifecycle, invalid range or incompatible input returns the corresponding status.

### Remarks

Terminal UI widgets draw onto a Canvas in the active Begin/End frame, without taking permanent ownership of the caller's widget values. The struct configuration form shares the same interaction implementation as the direct form.

### Example

```c
/* Usage fragment: context, geometry and widget structs are initialized by the caller. */

OPSTATUS status = UI_FUNC(SpinnerEx)(&context, &spinner);
```

# InputText

Edits text in a caller-provided buffer.

### Syntax

#### Macro form

```c
OPSTATUS UI_FUNC(InputText)(UI_TYPE(TContext) *context, TQUAD_TYPE(uint16) region, char *buffer, size_t capacity, size_t *inoutLength);
```

#### Direct form

```c
OPSTATUS UI_InputText(UI_TContext *context, TQuad_uint16 region, char *buffer, size_t capacity, size_t *inoutLength);
```

### Parameters

- `context`: UI or GUI context, initialized and inside its frame lifecycle as required.
- `region`: rectangle relative to the current Canvas/Surface or layout.
- `buffer`: editable caller-owned character buffer.
- `capacity`: capacity of input/copy destination in bytes.
- `inoutLength`: current text length on input; receives new length after edit.

### Return value

`OPSTATUS`: SUCCESS if the operation completes; invalid frame/lifecycle, invalid range or incompatible input returns the corresponding status.

### Remarks

Terminal UI widgets draw onto a Canvas in the active Begin/End frame, without taking permanent ownership of the caller's widget values. Editing preserves caller ownership of the buffer; capacity and UTF-8 validity must be respected.

### Example

```c
/* Usage fragment: context, geometry and widget structs are initialized by the caller. */
char buffer[128] = {0};
size_t length = 0;
OPSTATUS status = UI_FUNC(InputText)(&context, region, buffer, sizeof buffer, &length);
```

# InputTextEx

Edits text with full input-widget state.

### Syntax

#### Macro form

```c
OPSTATUS UI_FUNC(InputTextEx)(UI_TYPE(TContext) *context, UI_TYPE(TInputText) *input);
```

#### Direct form

```c
OPSTATUS UI_InputTextEx(UI_TContext *context, UI_TInputText *input);
```

### Parameters

- `context`: UI or GUI context, initialized and inside its frame lifecycle as required.
- `input`: input-text state/configuration.

### Return value

`OPSTATUS`: SUCCESS if the operation completes; invalid frame/lifecycle, invalid range or incompatible input returns the corresponding status.

### Remarks

Terminal UI widgets draw onto a Canvas in the active Begin/End frame, without taking permanent ownership of the caller's widget values. The struct configuration form shares the same interaction implementation as the direct form. Editing preserves caller ownership of the buffer; capacity and UTF-8 validity must be respected.

### Example

```c
/* Usage fragment: context, geometry and widget structs are initialized by the caller. */

OPSTATUS status = UI_FUNC(InputTextEx)(&context, &input);
```

# TextArea

Edits multiline text in caller-provided memory.

### Syntax

#### Macro form

```c
OPSTATUS UI_FUNC(TextArea)(UI_TYPE(TContext) *context, TQUAD_TYPE(uint16) region, char *buffer, size_t capacity, size_t *inoutLength);
```

#### Direct form

```c
OPSTATUS UI_TextArea(UI_TContext *context, TQuad_uint16 region, char *buffer, size_t capacity, size_t *inoutLength);
```

### Parameters

- `context`: UI or GUI context, initialized and inside its frame lifecycle as required.
- `region`: rectangle relative to the current Canvas/Surface or layout.
- `buffer`: editable caller-owned character buffer.
- `capacity`: capacity of input/copy destination in bytes.
- `inoutLength`: current text length on input; receives new length after edit.

### Return value

`OPSTATUS`: SUCCESS if the operation completes; invalid frame/lifecycle, invalid range or incompatible input returns the corresponding status.

### Remarks

Terminal UI widgets draw onto a Canvas in the active Begin/End frame, without taking permanent ownership of the caller's widget values. Editing preserves caller ownership of the buffer; capacity and UTF-8 validity must be respected.

### Example

```c
/* Usage fragment: context, geometry and widget structs are initialized by the caller. */
char buffer[128] = {0};
size_t length = 0;
OPSTATUS status = UI_FUNC(TextArea)(&context, region, buffer, sizeof buffer, &length);
```

# TextAreaEx

Edits multiline text through a state struct.

### Syntax

#### Macro form

```c
OPSTATUS UI_FUNC(TextAreaEx)(UI_TYPE(TContext) *context, UI_TYPE(TTextArea) *input);
```

#### Direct form

```c
OPSTATUS UI_TextAreaEx(UI_TContext *context, UI_TTextArea *input);
```

### Parameters

- `context`: UI or GUI context, initialized and inside its frame lifecycle as required.
- `input`: input-text state/configuration.

### Return value

`OPSTATUS`: SUCCESS if the operation completes; invalid frame/lifecycle, invalid range or incompatible input returns the corresponding status.

### Remarks

Terminal UI widgets draw onto a Canvas in the active Begin/End frame, without taking permanent ownership of the caller's widget values. The struct configuration form shares the same interaction implementation as the direct form. Editing preserves caller ownership of the buffer; capacity and UTF-8 validity must be respected.

### Example

```c
/* Usage fragment: context, geometry and widget structs are initialized by the caller. */

OPSTATUS status = UI_FUNC(TextAreaEx)(&context, &input);
```

# List

Draws a selectable list.

### Syntax

#### Macro form

```c
OPSTATUS UI_FUNC(List)(UI_TYPE(TContext) *context, TQUAD_TYPE(uint16) region, const char *const *items, size_t count, size_t *selected);
```

#### Direct form

```c
OPSTATUS UI_List(UI_TContext *context, TQuad_uint16 region, const char *const *items, size_t count, size_t *selected);
```

### Parameters

- `context`: UI or GUI context, initialized and inside its frame lifecycle as required.
- `region`: rectangle relative to the current Canvas/Surface or layout.
- `items`: borrowed array of display-item strings.
- `count`: number of items or render commands.
- `selected`: caller-owned selected index or option.

### Return value

`OPSTATUS`: SUCCESS if the operation completes; invalid frame/lifecycle, invalid range or incompatible input returns the corresponding status.

### Remarks

Terminal UI widgets draw onto a Canvas in the active Begin/End frame, without taking permanent ownership of the caller's widget values.

### Example

```c
/* Usage fragment: context, geometry and widget structs are initialized by the caller. */
const char *items[] = {"One", "Two"};
size_t selected = 0;
OPSTATUS status = UI_FUNC(List)(&context, region, items, 2, &selected);
```

# ListEx

Draws a struct-configured list.

### Syntax

#### Macro form

```c
OPSTATUS UI_FUNC(ListEx)(UI_TYPE(TContext) *context, UI_TYPE(TList) *list);
```

#### Direct form

```c
OPSTATUS UI_ListEx(UI_TContext *context, UI_TList *list);
```

### Parameters

- `context`: UI or GUI context, initialized and inside its frame lifecycle as required.
- `list`: list widget configuration.

### Return value

`OPSTATUS`: SUCCESS if the operation completes; invalid frame/lifecycle, invalid range or incompatible input returns the corresponding status.

### Remarks

Terminal UI widgets draw onto a Canvas in the active Begin/End frame, without taking permanent ownership of the caller's widget values. The struct configuration form shares the same interaction implementation as the direct form.

### Example

```c
/* Usage fragment: context, geometry and widget structs are initialized by the caller. */

OPSTATUS status = UI_FUNC(ListEx)(&context, &list);
```

# Integration and requirements

- C11, header-only; the library's namespace and status patterns are retained.
- `Begin` and `End` establish the UI frame; widget values and arrays remain owned by the caller.
- The direct functions and `*Ex` struct wrappers share interaction semantics.
- See [Canvas](Canvas.md), [Terminal](Terminal.md), [Surface](Surface.md), [Window](Window.md), [Input](Input.md) and [Bit](Bit.md).
