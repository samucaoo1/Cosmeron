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
| Progress | Progress | ProgressEx / TProgress |
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
