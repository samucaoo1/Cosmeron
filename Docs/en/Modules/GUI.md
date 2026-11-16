# GUI — pixel widgets

Header: `Codespace/Cosmeron/Modules/Graphics/GUI/GUI.h`.
Namespaces: `GUI_TYPE`, `GUI_FUNC`, `GUI_CONST`.
C11, header-only, with no mandatory extra linker flags for the GUI core.

![Cosmeron GUI Lab](../Assets/GUI-Lab.png)

GUI follows the terminal UI contract: an application declares its interface on
every frame, supplies input explicitly, and chooses when to present. A widget
can be called directly or through a persistent configuration struct. Both paths
use the same interaction and renderer. Values remain owned by the application.

## Frame lifecycle

```c
GUI_TYPE(TContext) gui = {0};
GRAPHICS_TYPE(TSurface) surface = {0};

OPSTATUS status = GRAPHICS_FUNC(Surface_Create)(
    &surface, (TDUAL_TYPE(uint32)){.x = 640, .y = 480});
/* Handle status before continuing. */

status = GUI_FUNC(Begin)(&gui, &surface);
/* Supply GUI_Event calls before interactive widgets. */
/* Declare the widgets. */
status = GUI_FUNC(End)(&gui);
/* Present only after successful End: Window_Update or a host adapter. */

GRAPHICS_FUNC(Surface_Free)(&surface);
```

The context requires neither Init nor Free. Begin resets transient commands,
input and stack depths; focus, pointer capture and editor state survive frames.
End renders into the borrowed surface and never creates a window or writes to
stdout. An unbalanced scope makes End fail without rendering, and releases the
frame so that the next Begin can recover. A custom renderer can replace the
software renderer through `context.renderer` and `rendererUser`.

`OPSTATUS` reports operational success. Button activation is a separate output.
Required outputs are unchanged when an operation fails. Editor buffer and widget
values are not committed after a failed edit; context registration/focus may
already have changed. A whole frame is not an application-state transaction.

## Direct and struct forms

```c
bool pressed = false;
OPSTATUS status = GUI_FUNC(Button)(
    &gui,
    (TQUAD_TYPE(float)){.value1=20, .value2=20, .value3=160, .value4=32},
    "Save", &pressed);
```

```c
GUI_TYPE(TButton) button = {
    .id = 42,
    .region = {.value1=20, .value2=20, .value3=160, .value4=32},
    .label = "Save"
};
OPSTATUS status = GUI_FUNC(ButtonEx)(&gui, &button);
/* On SUCCESS, button.pressed is the activation output for this frame. */
```

Default expanded names are `GUI_Button`, `GUI_ButtonEx`, `Graphics_Surface_Create`,
and `Window_Update`. Namespaced code should use the macros above. The optional
`GUI_FUNC(functions)` table exposes all public operations with camelCase fields;
`GUI_DISABLE_FUNCTION_TABLE` suppresses its type, instance and generation macros.

## Geometry, identity and focus

Regions use Struct/TQuad `value1..value4` as **x, y, width, height**. This differs
from the terminal UI's inclusive left/right/top/bottom bounds. Coordinates are
absolute in the surface's logical coordinate system, including inside a panel.
Use Struct/TDual for positions and sizes; no additional point/rectangle types are
introduced. Regions are half-open. Negative positions permit off-screen drawing.

`GUI_Scale_Set` sets the logical-to-pixel scale between frames. Pointer events use
physical surface pixels and are divided by that same scale. Hosts must translate
screen coordinates to the window's client coordinates. Scale is explicit; native
window adapters do not automatically track monitor DPI changes.

Direct labeled widgets derive IDs from labels and the PushId/PopId scope. Other
direct controls use their kind and region; with an explicit scope they use kind
and scope. Use a stable scope or an Ex widget's `id` when controls move or labels
change. Repeated enabled IDs return ALREADY_EXISTS. Keep distinct IDs within a
frame. Disabled controls are skipped in keyboard navigation.

Tab and Shift+Tab move through the preceding frame's focus list. Enter/Space
activate buttons. Press/release inside a button activates once; releasing outside
does not. Sliders retain capture during an outside drag. Focus loss cancels held
input. Submit events before widgets; the GUI never drains Input's global queue.
A frame aggregates pointer transitions into one click/drag interaction; hosts
should avoid batching multiple distinct click gestures into one frame.

## API families

Every public signature is defined by a `GUI_*_PROTOTYPE` macro in GUI.h.

| Family | Operations |
|---|---|
| Frame | Begin, Event, End, GetSize, Scale_Set |
| Identity/focus | GetId, PushId, PopId, Focus_Set, Focus_Next, Focus_Previous, IsFocused |
| Style | Theme_Default/Dark/Light/Classic/Monochrome, Theme_Set/Get, Visual_Push/Pop |
| Layout | Row_Begin, Column_Begin, Layout_Next, Layout_End, Layout_MeasureText |
| Clipping | Clip_Push, Clip_Pop |
| Drawing | DrawRectangle, DrawText, Image/ImageEx, RenderSoftware |
| Basic controls | Label/LabelEx, Separator, Button/ButtonEx, Checkbox/CheckboxEx, Radio/RadioEx |
| Values | Slider/SliderEx, Progress, ProgressBar/ProgressBarEx, Number/NumberEx |
| Editors | InputText/InputTextEx, TextArea/TextAreaEx, InputText_Select/Copy/Cut/Paste |
| Collections | List/ListEx, Menu/MenuEx, ComboBox/ComboBoxEx, Tabs/TabsEx, Table/TableEx |
| Tree | TreeNode/TreeNodeEx; the application submits children when expanded |
| Panels/modals | Panel_Begin/Panel_BeginEx/Panel_End, Modal_Begin/Modal_End, Confirm/ConfirmEx, Dialog/DialogEx |
| Extras | StatusBar, Toast, Spinner, ImageButton/ImageButtonEx, ColorPicker/ColorPickerEx, Scrollbar/ScrollbarEx |

Layouts allocate successive fixed extents within a row or column. Layout_Next
returns OUT_OF_RANGE if the next extent does not fit. Panels add an intersected
clip, and Panel_End checks that nested clips were balanced first. They do not
change the coordinate origin or automatically resize the application's controls.

List/Menu, ComboBox and Table Ex forms preserve `firstVisible` in the supplied struct. Their visible
rows are clipped, and wheel/keyboard navigation update the viewport. The direct
collection forms cache the viewport in the context. Table cells are a borrowed, row-major string array with equal
column widths. Menu is the classic selectable game menu. TreeNode provides an
immediate disclosure/selection row; the application's data owns hierarchy.

ComboBox renders an overlay of up to six visible choices. Tabs returns a selected
index; the application chooses the page content. ColorPicker exposes four channel
sliders in A, R, G, B order. Number supports arrows and wheel increments. Scrollbar
supports horizontal and vertical offsets over `maximum - visible`.

Declare Modal_Begin or Confirm/Dialog **before other interactive widgets** in the
frame. Modal commands are moved after background commands at End. Background
input and focus are blocked while a modal is active. The last modal also blocks
background input during its closing frame. Dialog currently uses the same OK and
Cancel actions as Confirm, and its direct form reports either action as dismissal.
Only one modal is supported per frame.

## Fonts and editing

The default font reuses the public-domain `VGA8x8.asf` asset. `GUI_Font_ASF` adapts
other compile-time ASF glyph art as a monochrome mask: non-space artwork becomes
ink. ASCII assets display unsupported Unicode characters as `?` while the editor
retains their original UTF-8 bytes. ASF has no runtime loading requirement.

`GUI_TFont` can alternatively supply height, advance and coverage callbacks for
Unicode and proportional glyphs. `advance` is measured in font units;
`coverage` returns 0..255 per glyph sample. The renderer copies font descriptors
and borrows callback user data. No TTF/OTF decoder, shaping or bidirectional-text
engine is bundled in this implementation.

Editing uses UTF-8 byte offsets at valid codepoint boundaries. It supports mouse
selection, Shift+arrows, Home/End, Ctrl+A, Backspace/Delete, and vertical navigation
in TextArea. Movement and deletion operate on Unicode scalars, not grapheme
clusters. TextArea draws explicit newlines and scrolls to the caret; it has no
automatic word wrapping. Single-line InputText ignores newline text events.

Input now includes EVENT_TEXT_INPUT and EVENT_TEXT_COMPOSITION. Events own up to
63 UTF-8 bytes plus a terminator; longer commits can be split at codepoint
boundaries. Composition is provisional and does not enter the edited buffer until
committed. Composition offsets are UTF-8 byte offsets. Application capture queues
copy these events and preserve keyboard modifiers. Native Window adapters supply
committed text; full native IME preedit/candidate positioning requires a host
adapter. GUI_Event accepts composition supplied by such a host.

Copy/Cut/Paste are explicit buffer operations. System clipboard access belongs to
the host. Paste sources and Cut outputs must not alias the edited buffer.

## Memory and rendering

The core allocates no memory per frame. Command, copied-text, event, ID and stack
storage live in the context; applications can override these capacities before
including GUI.h:

| Macro | Default |
|---|---:|
| GUI_MAX_WIDGETS | 128 |
| GUI_MAX_SCOPES | 16 |
| GUI_MAX_COMMANDS | 2048 |
| GUI_MAX_EVENTS | 128 |
| GUI_TEXT_CAPACITY | 16384 |

Capacity exhaustion returns INSUFFICIENT_SPACE. Scalar-based selection drawing
can need one command per selected codepoint. Direct editor/collection state has a
bounded context cache; use Ex structs for applications with many changing IDs.

The software renderer supports straight-alpha ARGB blending, rounded fills,
borders, bitmap/callback text and nearest-neighbor images. Commands copy text at
submission. Images and callback data are borrowed through End (and through any
later replay); source/destination image storage must not overlap. Commands and
copied text remain valid until the next Begin. A custom renderer receives the
same commands, already ordered for overlays, and the display scale.

Surface allocation, borrowing, stride, views, resizing, alpha blending and a line
primitive are documented in [Surface](Surface.md). The line walker is shared with
terminal Rasterization. UI/GUI also share identity, focus, pointer activation and
UTF-8 editor primitives. GUI.h does not depend on Terminal/Canvas.

## Window presentation

[Window](Window.md) is optional. Native backends are Win32/GDI and X11; host
callbacks integrate existing windows or other displays. GUI does not call them
implicitly. Use `Codespace/Examples/GUI/Lab.c` for a complete event/render/present
loop, including resizing, cleanup, Chronometry's frame limiter and SleepFor.

```sh
cc -std=c11 -D_POSIX_C_SOURCE=200809L Codespace/Examples/GUI/Lab.c -o lab
./lab
./lab --frames 3
./lab --ppm preview.ppm
make -C Codespace/Tests/GUI run
```

The headless mode runs the same widgets and software renderer. Tests cover both
APIs, focus/capture, UTF-8 editing and composition, clipping, overlays, capacities,
custom fonts/renderers, namespace customization, function-table disabling and
multiple translation units. The opt-in X11 test verifies actual presented pixels,
resize and committed text in a native window.

---

# API reference

Immediate pixel widgets with an explicit context, source Surface, event forwarding and optional native Window presentation.

The reference below is generated from the **public prototype declarations**, not from design proposals. `GUI_FUNC(Name)` and `GUI_Name` are equivalent with the default namespace. Examples are usage fragments: the context, geometry and struct configurations must be valid before calling widgets. They are not standalone runnable programs.

## Function summary

| Function | Return | Purpose |
| --- | --- | --- |
| `GUI_FUNC(Font_ASF)` | `GUI_TYPE(TFont)` | Wraps a native C11 .asf font for GUI text drawing. |
| `GUI_FUNC(Theme_Dark)` | `GUI_TYPE(TTheme)` | Returns dark-theme settings. |
| `GUI_FUNC(Theme_Light)` | `GUI_TYPE(TTheme)` | Returns light-theme settings. |
| `GUI_FUNC(Theme_Classic)` | `GUI_TYPE(TTheme)` | Returns classic-theme settings. |
| `GUI_FUNC(Theme_Monochrome)` | `GUI_TYPE(TTheme)` | Returns monochrome-theme settings. |
| `GUI_FUNC(Theme_Default)` | `GUI_TYPE(TTheme)` | Returns the package's default theme by value. |
| `GUI_FUNC(Theme_Set)` | `OPSTATUS` | Applies a theme to the context. |
| `GUI_FUNC(Theme_Get)` | `OPSTATUS` | Copies current theme into outTheme. |
| `GUI_FUNC(Scale_Set)` | `OPSTATUS` | Sets pixel GUI scale. |
| `GUI_FUNC(Visual_Push)` | `OPSTATUS` | Adds a temporary visual override. |
| `GUI_FUNC(Visual_Pop)` | `OPSTATUS` | Restores the previous visual override. |
| `GUI_FUNC(Begin)` | `OPSTATUS` | Begins a new explicit frame with a target Canvas or Surface. No implicit window presentation. |
| `GUI_FUNC(End)` | `OPSTATUS` | Ends and finalizes the current frame. |
| `GUI_FUNC(Event)` | `OPSTATUS` | Forwards an input event to GUI context state. |
| `GUI_FUNC(GetSize)` | `OPSTATUS` | Gets the active frame dimensions. |
| `GUI_FUNC(GetId)` | `GUI_TYPE(WidgetId)` | Builds a stable widget ID from the label and current scope. |
| `GUI_FUNC(PushId)` | `OPSTATUS` | Pushes a stable identifier scope for repeated widget labels. |
| `GUI_FUNC(PopId)` | `OPSTATUS` | Restores previous widget identifier scope. |
| `GUI_FUNC(Focus_Set)` | `OPSTATUS` | Moves keyboard focus to a widget ID. |
| `GUI_FUNC(Focus_Next)` | `OPSTATUS` | Advances keyboard focus among eligible widgets. |
| `GUI_FUNC(Focus_Previous)` | `OPSTATUS` | Moves keyboard focus backward. |
| `GUI_FUNC(IsFocused)` | `bool` | Tests whether widget ID owns focus. |
| `GUI_FUNC(Clip_Push)` | `OPSTATUS` | Restricts drawing to an additional rectangle. |
| `GUI_FUNC(Clip_Pop)` | `OPSTATUS` | Restores prior clip rectangle. |
| `GUI_FUNC(Row_Begin)` | `OPSTATUS` | Begins a row layout region. |
| `GUI_FUNC(Column_Begin)` | `OPSTATUS` | Begins a column layout region. |
| `GUI_FUNC(Layout_Next)` | `OPSTATUS` | Requests the next rectangle in current layout. |
| `GUI_FUNC(Layout_End)` | `OPSTATUS` | Closes current layout scope. |
| `GUI_FUNC(Layout_MeasureText)` | `OPSTATUS` | Measures a text string at the package's logical scale. |
| `GUI_FUNC(DrawRectangle)` | `OPSTATUS` | Draws filled or rounded geometry into the pixel GUI. |
| `GUI_FUNC(DrawText)` | `OPSTATUS` | Draws text into the GUI's current drawing context. |
| `GUI_FUNC(Image)` | `OPSTATUS` | Draws a borrowed pixel Surface. |
| `GUI_FUNC(ImageEx)` | `OPSTATUS` | Draws a configured image widget. |
| `GUI_FUNC(Label)` | `OPSTATUS` | Draws text as a non-interactive label. |
| `GUI_FUNC(LabelEx)` | `OPSTATUS` | Draws a label using its struct configuration. |
| `GUI_FUNC(Separator)` | `OPSTATUS` | Draws a visual separation rule. |
| `GUI_FUNC(Button)` | `OPSTATUS` | Draws an interactive button and reports activation. |
| `GUI_FUNC(ButtonEx)` | `OPSTATUS` | Draws a button from its struct configuration. |
| `GUI_FUNC(Checkbox)` | `OPSTATUS` | Draws a checkbox and updates its selected state. |
| `GUI_FUNC(CheckboxEx)` | `OPSTATUS` | Draws a checkbox from a struct. |
| `GUI_FUNC(Radio)` | `OPSTATUS` | Draws a radio selection and updates selected value. |
| `GUI_FUNC(RadioEx)` | `OPSTATUS` | Draws a radio widget configured by struct. |
| `GUI_FUNC(Slider)` | `OPSTATUS` | Draws a ranged slider and modifies a caller-owned numeric value. |
| `GUI_FUNC(SliderEx)` | `OPSTATUS` | Draws a slider from a struct. |
| `GUI_FUNC(Progress)` | `OPSTATUS` | Draws normalized progress feedback. |
| `GUI_FUNC(ProgressBar)` | `OPSTATUS` | Draws a progress bar with current and maximum values. |
| `GUI_FUNC(ProgressBarEx)` | `OPSTATUS` | Draws a progress bar using struct settings. |
| `GUI_FUNC(InputText)` | `OPSTATUS` | Edits text in a caller-provided buffer. |
| `GUI_FUNC(InputTextEx)` | `OPSTATUS` | Edits text with full input-widget state. |
| `GUI_FUNC(TextArea)` | `OPSTATUS` | Edits multiline text in caller-provided memory. |
| `GUI_FUNC(TextAreaEx)` | `OPSTATUS` | Edits multiline text through a state struct. |
| `GUI_FUNC(InputText_Select)` | `OPSTATUS` | Selects the range between anchor and caret positions. |
| `GUI_FUNC(InputText_Copy)` | `OPSTATUS` | Copies selected text from a widget into a caller buffer. |
| `GUI_FUNC(InputText_Cut)` | `OPSTATUS` | Copies and removes the selected text from the widget. |
| `GUI_FUNC(InputText_Paste)` | `OPSTATUS` | Pastes validated UTF-8 into the input model. |
| `GUI_FUNC(List)` | `OPSTATUS` | Draws a selectable list. |
| `GUI_FUNC(ListEx)` | `OPSTATUS` | Draws a struct-configured list. |
| `GUI_FUNC(Menu)` | `OPSTATUS` | Draws and interacts with a list of menu items. |
| `GUI_FUNC(MenuEx)` | `OPSTATUS` | Draws a menu with struct configuration. |
| `GUI_FUNC(Panel_Begin)` | `OPSTATUS` | Begins a panel using direct geometry and title. |
| `GUI_FUNC(Panel_BeginEx)` | `OPSTATUS` | Begins a panel using a struct configuration. |
| `GUI_FUNC(Panel_End)` | `OPSTATUS` | Closes a panel scope. |
| `GUI_FUNC(Modal_Begin)` | `OPSTATUS` | Begins a focus-modal region. |
| `GUI_FUNC(Modal_End)` | `OPSTATUS` | Ends a focus-modal region. |
| `GUI_FUNC(Confirm)` | `OPSTATUS` | Draws a confirm/reject dialog, reporting selected action. |
| `GUI_FUNC(ConfirmEx)` | `OPSTATUS` | Draws a confirmation configured by struct. |
| `GUI_FUNC(Dialog)` | `OPSTATUS` | Draws a dialog and reports dismissal. |
| `GUI_FUNC(DialogEx)` | `OPSTATUS` | Draws a dialog from its full configuration. |
| `GUI_FUNC(StatusBar)` | `OPSTATUS` | Draws status text in a bar. |
| `GUI_FUNC(Toast)` | `OPSTATUS` | Draws a conditional non-modal notification. |
| `GUI_FUNC(Spinner)` | `OPSTATUS` | Draws an animated spinner using caller-supplied phase. |
| `GUI_FUNC(RenderSoftware)` | `OPSTATUS` | Rasterizes a sequence of GUI drawing commands to a pixel Surface. |
| `GUI_FUNC(ImageButton)` | `OPSTATUS` | Draws a clickable image. |
| `GUI_FUNC(ImageButtonEx)` | `OPSTATUS` | Draws an image button configured by struct. |
| `GUI_FUNC(ComboBox)` | `OPSTATUS` | Draws a list selection widget with open/closed state. |
| `GUI_FUNC(ComboBoxEx)` | `OPSTATUS` | Draws a combo box configured by struct. |
| `GUI_FUNC(Tabs)` | `OPSTATUS` | Draws tabs and updates the selected tab. |
| `GUI_FUNC(TabsEx)` | `OPSTATUS` | Draws a tab set configured by struct. |
| `GUI_FUNC(TreeNode)` | `OPSTATUS` | Draws a foldable, optionally selectable tree node. |
| `GUI_FUNC(TreeNodeEx)` | `OPSTATUS` | Draws a tree node using a struct. |
| `GUI_FUNC(Table)` | `OPSTATUS` | Draws tabular entries and updates selected row. |
| `GUI_FUNC(TableEx)` | `OPSTATUS` | Draws a table from configuration. |
| `GUI_FUNC(ColorPicker)` | `OPSTATUS` | Draws a color picker that changes a caller-owned ARGB color. |
| `GUI_FUNC(ColorPickerEx)` | `OPSTATUS` | Draws the configured color picker. |
| `GUI_FUNC(Number)` | `OPSTATUS` | Draws a floating-point numeric adjustment widget. |
| `GUI_FUNC(NumberEx)` | `OPSTATUS` | Draws a numeric editor configured by struct. |
| `GUI_FUNC(Scrollbar)` | `OPSTATUS` | Draws a scrollbar and modifies its offset. |
| `GUI_FUNC(ScrollbarEx)` | `OPSTATUS` | Draws a scrollbar from its configuration. |

# Font_ASF

Wraps a native C11 .asf font for GUI text drawing.

### Syntax

#### Macro form

```c
GUI_TYPE(TFont) GUI_FUNC(Font_ASF)(const FONT_TYPE(TFont) * font);
```

#### Direct form

```c
GUI_TFont GUI_Font_ASF(const Font_TFont * font);
```

### Parameters

- `font`: font used to measure and render text.

### Return value

Returns a value of type `GUI_TYPE(TFont)` by value.

### Remarks

Pixel GUI widgets draw onto a Surface in an explicit Begin/End frame; Window presentation is a separate step.

### Example

```c
/* Usage fragment: context, geometry and widget structs are initialized by the caller. */

GUI_TYPE(TFont) guiFont = GUI_FUNC(Font_ASF)(&font);
```

# Theme_Dark

Returns dark-theme settings.

### Syntax

#### Macro form

```c
GUI_TYPE(TTheme) GUI_FUNC(Theme_Dark)(void);
```

#### Direct form

```c
GUI_TTheme GUI_Theme_Dark(void);
```

### Parameters

- None.

### Return value

Returns a value of type `GUI_TYPE(TTheme)` by value.

### Remarks

Pixel GUI widgets draw onto a Surface in an explicit Begin/End frame; Window presentation is a separate step.

### Example

```c
/* Usage fragment: context, geometry and widget structs are initialized by the caller. */

GUI_TYPE(TTheme) theme = GUI_FUNC(Theme_Dark)();
```

# Theme_Light

Returns light-theme settings.

### Syntax

#### Macro form

```c
GUI_TYPE(TTheme) GUI_FUNC(Theme_Light)(void);
```

#### Direct form

```c
GUI_TTheme GUI_Theme_Light(void);
```

### Parameters

- None.

### Return value

Returns a value of type `GUI_TYPE(TTheme)` by value.

### Remarks

Pixel GUI widgets draw onto a Surface in an explicit Begin/End frame; Window presentation is a separate step.

### Example

```c
/* Usage fragment: context, geometry and widget structs are initialized by the caller. */

GUI_TYPE(TTheme) theme = GUI_FUNC(Theme_Light)();
```

# Theme_Classic

Returns classic-theme settings.

### Syntax

#### Macro form

```c
GUI_TYPE(TTheme) GUI_FUNC(Theme_Classic)(void);
```

#### Direct form

```c
GUI_TTheme GUI_Theme_Classic(void);
```

### Parameters

- None.

### Return value

Returns a value of type `GUI_TYPE(TTheme)` by value.

### Remarks

Pixel GUI widgets draw onto a Surface in an explicit Begin/End frame; Window presentation is a separate step.

### Example

```c
/* Usage fragment: context, geometry and widget structs are initialized by the caller. */

GUI_TYPE(TTheme) theme = GUI_FUNC(Theme_Classic)();
```

# Theme_Monochrome

Returns monochrome-theme settings.

### Syntax

#### Macro form

```c
GUI_TYPE(TTheme) GUI_FUNC(Theme_Monochrome)(void);
```

#### Direct form

```c
GUI_TTheme GUI_Theme_Monochrome(void);
```

### Parameters

- None.

### Return value

Returns a value of type `GUI_TYPE(TTheme)` by value.

### Remarks

Pixel GUI widgets draw onto a Surface in an explicit Begin/End frame; Window presentation is a separate step.

### Example

```c
/* Usage fragment: context, geometry and widget structs are initialized by the caller. */

GUI_TYPE(TTheme) theme = GUI_FUNC(Theme_Monochrome)();
```

# Theme_Default

Returns the package's default theme by value.

### Syntax

#### Macro form

```c
GUI_TYPE(TTheme) GUI_FUNC(Theme_Default)(void);
```

#### Direct form

```c
GUI_TTheme GUI_Theme_Default(void);
```

### Parameters

- None.

### Return value

Returns a value of type `GUI_TYPE(TTheme)` by value.

### Remarks

Pixel GUI widgets draw onto a Surface in an explicit Begin/End frame; Window presentation is a separate step.

### Example

```c
/* Usage fragment: context, geometry and widget structs are initialized by the caller. */

GUI_TYPE(TTheme) theme = GUI_FUNC(Theme_Default)();
```

# Theme_Set

Applies a theme to the context.

### Syntax

#### Macro form

```c
OPSTATUS GUI_FUNC(Theme_Set)(GUI_TYPE(TContext) * context, const GUI_TYPE(TTheme) * theme);
```

#### Direct form

```c
OPSTATUS GUI_Theme_Set(GUI_TContext * context, const GUI_TTheme * theme);
```

### Parameters

- `context`: UI or GUI context, initialized and inside its frame lifecycle as required.
- `theme`: theme supplied for a context.

### Return value

`OPSTATUS`: SUCCESS if the operation completes; invalid frame/lifecycle, invalid range or incompatible input returns the corresponding status.

### Remarks

Pixel GUI widgets draw onto a Surface in an explicit Begin/End frame; Window presentation is a separate step.

### Example

```c
/* Usage fragment: context, geometry and widget structs are initialized by the caller. */

OPSTATUS status = GUI_FUNC(Theme_Set)(&context, &theme);
```

# Theme_Get

Copies current theme into outTheme.

### Syntax

#### Macro form

```c
OPSTATUS GUI_FUNC(Theme_Get)(const GUI_TYPE(TContext) * context, GUI_TYPE(TTheme) * outTheme);
```

#### Direct form

```c
OPSTATUS GUI_Theme_Get(const GUI_TContext * context, GUI_TTheme * outTheme);
```

### Parameters

- `context`: UI or GUI context, initialized and inside its frame lifecycle as required.
- `outTheme`: output theme struct.

### Return value

`OPSTATUS`: SUCCESS if the operation completes; invalid frame/lifecycle, invalid range or incompatible input returns the corresponding status.

### Remarks

Pixel GUI widgets draw onto a Surface in an explicit Begin/End frame; Window presentation is a separate step.

### Example

```c
/* Usage fragment: context, geometry and widget structs are initialized by the caller. */
GUI_TYPE(TTheme) theme = {0};
OPSTATUS status = GUI_FUNC(Theme_Get)(&context, &theme);
```

# Scale_Set

Sets pixel GUI scale.

### Syntax

#### Macro form

```c
OPSTATUS GUI_FUNC(Scale_Set)(GUI_TYPE(TContext) * context, float scale);
```

#### Direct form

```c
OPSTATUS GUI_Scale_Set(GUI_TContext * context, float scale);
```

### Parameters

- `context`: UI or GUI context, initialized and inside its frame lifecycle as required.
- `scale`: positive drawing or font scale.

### Return value

`OPSTATUS`: SUCCESS if the operation completes; invalid frame/lifecycle, invalid range or incompatible input returns the corresponding status.

### Remarks

Pixel GUI widgets draw onto a Surface in an explicit Begin/End frame; Window presentation is a separate step.

### Example

```c
/* Usage fragment: context, geometry and widget structs are initialized by the caller. */

OPSTATUS status = GUI_FUNC(Scale_Set)(&context, 1.0f);
```

# Visual_Push

Adds a temporary visual override.

### Syntax

#### Macro form

```c
OPSTATUS GUI_FUNC(Visual_Push)(GUI_TYPE(TContext) * context, const GUI_TYPE(TVisual) * visual);
```

#### Direct form

```c
OPSTATUS GUI_Visual_Push(GUI_TContext * context, const GUI_TVisual * visual);
```

### Parameters

- `context`: UI or GUI context, initialized and inside its frame lifecycle as required.
- `visual`: visual override pushed on the context.

### Return value

`OPSTATUS`: SUCCESS if the operation completes; invalid frame/lifecycle, invalid range or incompatible input returns the corresponding status.

### Remarks

Pixel GUI widgets draw onto a Surface in an explicit Begin/End frame; Window presentation is a separate step. Balance push/pop scope calls before completing the frame.

### Example

```c
/* Usage fragment: context, geometry and widget structs are initialized by the caller. */

OPSTATUS status = GUI_FUNC(Visual_Push)(&context, &visual);
```

# Visual_Pop

Restores the previous visual override.

### Syntax

#### Macro form

```c
OPSTATUS GUI_FUNC(Visual_Pop)(GUI_TYPE(TContext) * context);
```

#### Direct form

```c
OPSTATUS GUI_Visual_Pop(GUI_TContext * context);
```

### Parameters

- `context`: UI or GUI context, initialized and inside its frame lifecycle as required.

### Return value

`OPSTATUS`: SUCCESS if the operation completes; invalid frame/lifecycle, invalid range or incompatible input returns the corresponding status.

### Remarks

Pixel GUI widgets draw onto a Surface in an explicit Begin/End frame; Window presentation is a separate step. Balance push/pop scope calls before completing the frame.

### Example

```c
/* Usage fragment: context, geometry and widget structs are initialized by the caller. */

OPSTATUS status = GUI_FUNC(Visual_Pop)(&context);
```

# Begin

Begins a new explicit frame with a target Canvas or Surface. No implicit window presentation.

### Syntax

#### Macro form

```c
OPSTATUS GUI_FUNC(Begin)(GUI_TYPE(TContext) * context, GRAPHICS_TYPE(TSurface) * surface);
```

#### Direct form

```c
OPSTATUS GUI_Begin(GUI_TContext * context, Graphics_TSurface * surface);
```

### Parameters

- `context`: UI or GUI context, initialized and inside its frame lifecycle as required.
- `surface`: pixel Surface for GUI drawing, image display or rendering.

### Return value

`OPSTATUS`: SUCCESS if the operation completes; invalid frame/lifecycle, invalid range or incompatible input returns the corresponding status.

### Remarks

Pixel GUI widgets draw onto a Surface in an explicit Begin/End frame; Window presentation is a separate step. Do not call twice without a matching End.

### Example

```c
/* Usage fragment: context, geometry and widget structs are initialized by the caller. */

OPSTATUS status = GUI_FUNC(Begin)(&context, &surface);
```

# End

Ends and finalizes the current frame.

### Syntax

#### Macro form

```c
OPSTATUS GUI_FUNC(End)(GUI_TYPE(TContext) * context);
```

#### Direct form

```c
OPSTATUS GUI_End(GUI_TContext * context);
```

### Parameters

- `context`: UI or GUI context, initialized and inside its frame lifecycle as required.

### Return value

`OPSTATUS`: SUCCESS if the operation completes; invalid frame/lifecycle, invalid range or incompatible input returns the corresponding status.

### Remarks

Pixel GUI widgets draw onto a Surface in an explicit Begin/End frame; Window presentation is a separate step. This call does not necessarily update the external display on its own.

### Example

```c
/* Usage fragment: context, geometry and widget structs are initialized by the caller. */

OPSTATUS status = GUI_FUNC(End)(&context);
```

# Event

Forwards an input event to GUI context state.

### Syntax

#### Macro form

```c
OPSTATUS GUI_FUNC(Event)(GUI_TYPE(TContext) * context, const INPUT_TYPE(TEvent) * event);
```

#### Direct form

```c
OPSTATUS GUI_Event(GUI_TContext * context, const Input_TEvent * event);
```

### Parameters

- `context`: UI or GUI context, initialized and inside its frame lifecycle as required.
- `event`: input event provided by the host/application.

### Return value

`OPSTATUS`: SUCCESS if the operation completes; invalid frame/lifecycle, invalid range or incompatible input returns the corresponding status.

### Remarks

Pixel GUI widgets draw onto a Surface in an explicit Begin/End frame; Window presentation is a separate step. The event comes from Input or a native/host window adapter.

### Example

```c
/* Usage fragment: context, geometry and widget structs are initialized by the caller. */

OPSTATUS status = GUI_FUNC(Event)(&context, &event);
```

# GetSize

Gets the active frame dimensions.

### Syntax

#### Macro form

```c
OPSTATUS GUI_FUNC(GetSize)(const GUI_TYPE(TContext) * context, TDUAL_TYPE(float) * outSize);
```

#### Direct form

```c
OPSTATUS GUI_GetSize(const GUI_TContext * context, TDual_float * outSize);
```

### Parameters

- `context`: UI or GUI context, initialized and inside its frame lifecycle as required.
- `outSize`: receives computed dimensions.

### Return value

`OPSTATUS`: SUCCESS if the operation completes; invalid frame/lifecycle, invalid range or incompatible input returns the corresponding status.

### Remarks

Pixel GUI widgets draw onto a Surface in an explicit Begin/End frame; Window presentation is a separate step.

### Example

```c
/* Usage fragment: context, geometry and widget structs are initialized by the caller. */
TDUAL_TYPE(float) resultSize = {0};
OPSTATUS status = GUI_FUNC(GetSize)(&context, &resultSize);
```

# GetId

Builds a stable widget ID from the label and current scope.

### Syntax

#### Macro form

```c
GUI_TYPE(WidgetId) GUI_FUNC(GetId)(const GUI_TYPE(TContext) * context, const char *label);
```

#### Direct form

```c
GUI_WidgetId GUI_GetId(const GUI_TContext * context, const char *label);
```

### Parameters

- `context`: UI or GUI context, initialized and inside its frame lifecycle as required.
- `label`: widget label and stable identification text.

### Return value

Returns a value of type `GUI_TYPE(WidgetId)` by value.

### Remarks

Pixel GUI widgets draw onto a Surface in an explicit Begin/End frame; Window presentation is a separate step.

### Example

```c
/* Usage fragment: context, geometry and widget structs are initialized by the caller. */

GUI_TYPE(WidgetId) id = GUI_FUNC(GetId)(&context, "OK");
```

# PushId

Pushes a stable identifier scope for repeated widget labels.

### Syntax

#### Macro form

```c
OPSTATUS GUI_FUNC(PushId)(GUI_TYPE(TContext) * context, const char *scope);
```

#### Direct form

```c
OPSTATUS GUI_PushId(GUI_TContext * context, const char *scope);
```

### Parameters

- `context`: UI or GUI context, initialized and inside its frame lifecycle as required.
- `scope`: stable identifier scope segment.

### Return value

`OPSTATUS`: SUCCESS if the operation completes; invalid frame/lifecycle, invalid range or incompatible input returns the corresponding status.

### Remarks

Pixel GUI widgets draw onto a Surface in an explicit Begin/End frame; Window presentation is a separate step. Balance push/pop scope calls before completing the frame.

### Example

```c
/* Usage fragment: context, geometry and widget structs are initialized by the caller. */

OPSTATUS status = GUI_FUNC(PushId)(&context, "group");
```

# PopId

Restores previous widget identifier scope.

### Syntax

#### Macro form

```c
OPSTATUS GUI_FUNC(PopId)(GUI_TYPE(TContext) * context);
```

#### Direct form

```c
OPSTATUS GUI_PopId(GUI_TContext * context);
```

### Parameters

- `context`: UI or GUI context, initialized and inside its frame lifecycle as required.

### Return value

`OPSTATUS`: SUCCESS if the operation completes; invalid frame/lifecycle, invalid range or incompatible input returns the corresponding status.

### Remarks

Pixel GUI widgets draw onto a Surface in an explicit Begin/End frame; Window presentation is a separate step. Balance push/pop scope calls before completing the frame.

### Example

```c
/* Usage fragment: context, geometry and widget structs are initialized by the caller. */

OPSTATUS status = GUI_FUNC(PopId)(&context);
```

# Focus_Set

Moves keyboard focus to a widget ID.

### Syntax

#### Macro form

```c
OPSTATUS GUI_FUNC(Focus_Set)(GUI_TYPE(TContext) * context, GUI_TYPE(WidgetId) widget);
```

#### Direct form

```c
OPSTATUS GUI_Focus_Set(GUI_TContext * context, GUI_WidgetId widget);
```

### Parameters

- `context`: UI or GUI context, initialized and inside its frame lifecycle as required.
- `widget`: stable widget ID used by focus APIs.

### Return value

`OPSTATUS`: SUCCESS if the operation completes; invalid frame/lifecycle, invalid range or incompatible input returns the corresponding status.

### Remarks

Pixel GUI widgets draw onto a Surface in an explicit Begin/End frame; Window presentation is a separate step.

### Example

```c
/* Usage fragment: context, geometry and widget structs are initialized by the caller. */

OPSTATUS status = GUI_FUNC(Focus_Set)(&context, id);
```

# Focus_Next

Advances keyboard focus among eligible widgets.

### Syntax

#### Macro form

```c
OPSTATUS GUI_FUNC(Focus_Next)(GUI_TYPE(TContext) * context);
```

#### Direct form

```c
OPSTATUS GUI_Focus_Next(GUI_TContext * context);
```

### Parameters

- `context`: UI or GUI context, initialized and inside its frame lifecycle as required.

### Return value

`OPSTATUS`: SUCCESS if the operation completes; invalid frame/lifecycle, invalid range or incompatible input returns the corresponding status.

### Remarks

Pixel GUI widgets draw onto a Surface in an explicit Begin/End frame; Window presentation is a separate step.

### Example

```c
/* Usage fragment: context, geometry and widget structs are initialized by the caller. */

OPSTATUS status = GUI_FUNC(Focus_Next)(&context);
```

# Focus_Previous

Moves keyboard focus backward.

### Syntax

#### Macro form

```c
OPSTATUS GUI_FUNC(Focus_Previous)(GUI_TYPE(TContext) * context);
```

#### Direct form

```c
OPSTATUS GUI_Focus_Previous(GUI_TContext * context);
```

### Parameters

- `context`: UI or GUI context, initialized and inside its frame lifecycle as required.

### Return value

`OPSTATUS`: SUCCESS if the operation completes; invalid frame/lifecycle, invalid range or incompatible input returns the corresponding status.

### Remarks

Pixel GUI widgets draw onto a Surface in an explicit Begin/End frame; Window presentation is a separate step.

### Example

```c
/* Usage fragment: context, geometry and widget structs are initialized by the caller. */

OPSTATUS status = GUI_FUNC(Focus_Previous)(&context);
```

# IsFocused

Tests whether widget ID owns focus.

### Syntax

#### Macro form

```c
bool GUI_FUNC(IsFocused)(const GUI_TYPE(TContext) * context, GUI_TYPE(WidgetId) widget);
```

#### Direct form

```c
bool GUI_IsFocused(const GUI_TContext * context, GUI_WidgetId widget);
```

### Parameters

- `context`: UI or GUI context, initialized and inside its frame lifecycle as required.
- `widget`: stable widget ID used by focus APIs.

### Return value

`bool`: indicates the state queried, not an OPSTATUS.

### Remarks

Pixel GUI widgets draw onto a Surface in an explicit Begin/End frame; Window presentation is a separate step.

### Example

```c
/* Usage fragment: context, geometry and widget structs are initialized by the caller. */

bool result = GUI_FUNC(IsFocused)(&context, id);
```

# Clip_Push

Restricts drawing to an additional rectangle.

### Syntax

#### Macro form

```c
OPSTATUS GUI_FUNC(Clip_Push)(GUI_TYPE(TContext) * context, TQUAD_TYPE(float) region);
```

#### Direct form

```c
OPSTATUS GUI_Clip_Push(GUI_TContext * context, TQuad_float region);
```

### Parameters

- `context`: UI or GUI context, initialized and inside its frame lifecycle as required.
- `region`: rectangle relative to the current Canvas/Surface or layout.

### Return value

`OPSTATUS`: SUCCESS if the operation completes; invalid frame/lifecycle, invalid range or incompatible input returns the corresponding status.

### Remarks

Pixel GUI widgets draw onto a Surface in an explicit Begin/End frame; Window presentation is a separate step. Balance push/pop scope calls before completing the frame.

### Example

```c
/* Usage fragment: context, geometry and widget structs are initialized by the caller. */

OPSTATUS status = GUI_FUNC(Clip_Push)(&context, region);
```

# Clip_Pop

Restores prior clip rectangle.

### Syntax

#### Macro form

```c
OPSTATUS GUI_FUNC(Clip_Pop)(GUI_TYPE(TContext) * context);
```

#### Direct form

```c
OPSTATUS GUI_Clip_Pop(GUI_TContext * context);
```

### Parameters

- `context`: UI or GUI context, initialized and inside its frame lifecycle as required.

### Return value

`OPSTATUS`: SUCCESS if the operation completes; invalid frame/lifecycle, invalid range or incompatible input returns the corresponding status.

### Remarks

Pixel GUI widgets draw onto a Surface in an explicit Begin/End frame; Window presentation is a separate step. Balance push/pop scope calls before completing the frame.

### Example

```c
/* Usage fragment: context, geometry and widget structs are initialized by the caller. */

OPSTATUS status = GUI_FUNC(Clip_Pop)(&context);
```

# Row_Begin

Begins a row layout region.

### Syntax

#### Macro form

```c
OPSTATUS GUI_FUNC(Row_Begin)(GUI_TYPE(TContext) * context, TQUAD_TYPE(float) region, float gap);
```

#### Direct form

```c
OPSTATUS GUI_Row_Begin(GUI_TContext * context, TQuad_float region, float gap);
```

### Parameters

- `context`: UI or GUI context, initialized and inside its frame lifecycle as required.
- `region`: rectangle relative to the current Canvas/Surface or layout.
- `gap`: distance between successive layout elements.

### Return value

`OPSTATUS`: SUCCESS if the operation completes; invalid frame/lifecycle, invalid range or incompatible input returns the corresponding status.

### Remarks

Pixel GUI widgets draw onto a Surface in an explicit Begin/End frame; Window presentation is a separate step.

### Example

```c
/* Usage fragment: context, geometry and widget structs are initialized by the caller. */

OPSTATUS status = GUI_FUNC(Row_Begin)(&context, region, 1);
```

# Column_Begin

Begins a column layout region.

### Syntax

#### Macro form

```c
OPSTATUS GUI_FUNC(Column_Begin)(GUI_TYPE(TContext) * context, TQUAD_TYPE(float) region, float gap);
```

#### Direct form

```c
OPSTATUS GUI_Column_Begin(GUI_TContext * context, TQuad_float region, float gap);
```

### Parameters

- `context`: UI or GUI context, initialized and inside its frame lifecycle as required.
- `region`: rectangle relative to the current Canvas/Surface or layout.
- `gap`: distance between successive layout elements.

### Return value

`OPSTATUS`: SUCCESS if the operation completes; invalid frame/lifecycle, invalid range or incompatible input returns the corresponding status.

### Remarks

Pixel GUI widgets draw onto a Surface in an explicit Begin/End frame; Window presentation is a separate step.

### Example

```c
/* Usage fragment: context, geometry and widget structs are initialized by the caller. */

OPSTATUS status = GUI_FUNC(Column_Begin)(&context, region, 1);
```

# Layout_Next

Requests the next rectangle in current layout.

### Syntax

#### Macro form

```c
OPSTATUS GUI_FUNC(Layout_Next)(GUI_TYPE(TContext) * context, float extent, TQUAD_TYPE(float) * outRegion);
```

#### Direct form

```c
OPSTATUS GUI_Layout_Next(GUI_TContext * context, float extent, TQuad_float * outRegion);
```

### Parameters

- `context`: UI or GUI context, initialized and inside its frame lifecycle as required.
- `extent`: requested layout element size.
- `outRegion`: output geometry region.

### Return value

`OPSTATUS`: SUCCESS if the operation completes; invalid frame/lifecycle, invalid range or incompatible input returns the corresponding status.

### Remarks

Pixel GUI widgets draw onto a Surface in an explicit Begin/End frame; Window presentation is a separate step.

### Example

```c
/* Usage fragment: context, geometry and widget structs are initialized by the caller. */
TQUAD_TYPE(float) outRegion = {0};
OPSTATUS status = GUI_FUNC(Layout_Next)(&context, 5, &outRegion);
```

# Layout_End

Closes current layout scope.

### Syntax

#### Macro form

```c
OPSTATUS GUI_FUNC(Layout_End)(GUI_TYPE(TContext) * context);
```

#### Direct form

```c
OPSTATUS GUI_Layout_End(GUI_TContext * context);
```

### Parameters

- `context`: UI or GUI context, initialized and inside its frame lifecycle as required.

### Return value

`OPSTATUS`: SUCCESS if the operation completes; invalid frame/lifecycle, invalid range or incompatible input returns the corresponding status.

### Remarks

Pixel GUI widgets draw onto a Surface in an explicit Begin/End frame; Window presentation is a separate step.

### Example

```c
/* Usage fragment: context, geometry and widget structs are initialized by the caller. */

OPSTATUS status = GUI_FUNC(Layout_End)(&context);
```

# Layout_MeasureText

Measures a text string at the package's logical scale.

### Syntax

#### Macro form

```c
OPSTATUS GUI_FUNC(Layout_MeasureText)(const GUI_TYPE(TFont) * font, const char *text, float scale, TDUAL_TYPE(float) * outSize);
```

#### Direct form

```c
OPSTATUS GUI_Layout_MeasureText(const GUI_TFont * font, const char *text, float scale, TDual_float * outSize);
```

### Parameters

- `font`: font used to measure and render text.
- `text`: UTF-8 text used for display or measurement.
- `scale`: positive drawing or font scale.
- `outSize`: receives computed dimensions.

### Return value

`OPSTATUS`: SUCCESS if the operation completes; invalid frame/lifecycle, invalid range or incompatible input returns the corresponding status.

### Remarks

Pixel GUI widgets draw onto a Surface in an explicit Begin/End frame; Window presentation is a separate step.

### Example

```c
/* Usage fragment: context, geometry and widget structs are initialized by the caller. */
TDUAL_TYPE(float) resultSize = {0};
OPSTATUS status = GUI_FUNC(Layout_MeasureText)(&font, "Hello", 1.0f, &resultSize);
```

# DrawRectangle

Draws filled or rounded geometry into the pixel GUI.

### Syntax

#### Macro form

```c
OPSTATUS GUI_FUNC(DrawRectangle)(GUI_TYPE(TContext) * context, TQUAD_TYPE(float) region, uint32_t color, float radius);
```

#### Direct form

```c
OPSTATUS GUI_DrawRectangle(GUI_TContext * context, TQuad_float region, uint32_t color, float radius);
```

### Parameters

- `context`: UI or GUI context, initialized and inside its frame lifecycle as required.
- `region`: rectangle relative to the current Canvas/Surface or layout.
- `color`: ARGB drawing color or pointer to editable color.
- `radius`: corner radius for drawing rectangles.

### Return value

`OPSTATUS`: SUCCESS if the operation completes; invalid frame/lifecycle, invalid range or incompatible input returns the corresponding status.

### Remarks

Pixel GUI widgets draw onto a Surface in an explicit Begin/End frame; Window presentation is a separate step.

### Example

```c
/* Usage fragment: context, geometry and widget structs are initialized by the caller. */

OPSTATUS status = GUI_FUNC(DrawRectangle)(&context, region, UINT32_C(0xFF3399CC), 4.0f);
```

# DrawText

Draws text into the GUI's current drawing context.

### Syntax

#### Macro form

```c
OPSTATUS GUI_FUNC(DrawText)(GUI_TYPE(TContext) * context, TQUAD_TYPE(float) region, const char *text, uint32_t color, const GUI_TYPE(TFont) * font, float scale);
```

#### Direct form

```c
OPSTATUS GUI_DrawText(GUI_TContext * context, TQuad_float region, const char *text, uint32_t color, const GUI_TFont * font, float scale);
```

### Parameters

- `context`: UI or GUI context, initialized and inside its frame lifecycle as required.
- `region`: rectangle relative to the current Canvas/Surface or layout.
- `text`: UTF-8 text used for display or measurement.
- `color`: ARGB drawing color or pointer to editable color.
- `font`: font used to measure and render text.
- `scale`: positive drawing or font scale.

### Return value

`OPSTATUS`: SUCCESS if the operation completes; invalid frame/lifecycle, invalid range or incompatible input returns the corresponding status.

### Remarks

Pixel GUI widgets draw onto a Surface in an explicit Begin/End frame; Window presentation is a separate step.

### Example

```c
/* Usage fragment: context, geometry and widget structs are initialized by the caller. */

OPSTATUS status = GUI_FUNC(DrawText)(&context, region, "Hello", UINT32_C(0xFF3399CC), &font, 1.0f);
```

# Image

Draws a borrowed pixel Surface.

### Syntax

#### Macro form

```c
OPSTATUS GUI_FUNC(Image)(GUI_TYPE(TContext) * context, TQUAD_TYPE(float) region, const GRAPHICS_TYPE(TSurface) * surface);
```

#### Direct form

```c
OPSTATUS GUI_Image(GUI_TContext * context, TQuad_float region, const Graphics_TSurface * surface);
```

### Parameters

- `context`: UI or GUI context, initialized and inside its frame lifecycle as required.
- `region`: rectangle relative to the current Canvas/Surface or layout.
- `surface`: pixel Surface for GUI drawing, image display or rendering.

### Return value

`OPSTATUS`: SUCCESS if the operation completes; invalid frame/lifecycle, invalid range or incompatible input returns the corresponding status.

### Remarks

Pixel GUI widgets draw onto a Surface in an explicit Begin/End frame; Window presentation is a separate step.

### Example

```c
/* Usage fragment: context, geometry and widget structs are initialized by the caller. */

OPSTATUS status = GUI_FUNC(Image)(&context, region, &surface);
```

# ImageEx

Draws a configured image widget.

### Syntax

#### Macro form

```c
OPSTATUS GUI_FUNC(ImageEx)(GUI_TYPE(TContext) * context, const GUI_TYPE(TImage) * image);
```

#### Direct form

```c
OPSTATUS GUI_ImageEx(GUI_TContext * context, const GUI_TImage * image);
```

### Parameters

- `context`: UI or GUI context, initialized and inside its frame lifecycle as required.
- `image`: widget image descriptor.

### Return value

`OPSTATUS`: SUCCESS if the operation completes; invalid frame/lifecycle, invalid range or incompatible input returns the corresponding status.

### Remarks

Pixel GUI widgets draw onto a Surface in an explicit Begin/End frame; Window presentation is a separate step. The struct configuration form shares the same interaction implementation as the direct form.

### Example

```c
/* Usage fragment: context, geometry and widget structs are initialized by the caller. */

OPSTATUS status = GUI_FUNC(ImageEx)(&context, &image);
```

# Label

Draws text as a non-interactive label.

### Syntax

#### Macro form

```c
OPSTATUS GUI_FUNC(Label)(GUI_TYPE(TContext) * context, TQUAD_TYPE(float) region, const char *text);
```

#### Direct form

```c
OPSTATUS GUI_Label(GUI_TContext * context, TQuad_float region, const char *text);
```

### Parameters

- `context`: UI or GUI context, initialized and inside its frame lifecycle as required.
- `region`: rectangle relative to the current Canvas/Surface or layout.
- `text`: UTF-8 text used for display or measurement.

### Return value

`OPSTATUS`: SUCCESS if the operation completes; invalid frame/lifecycle, invalid range or incompatible input returns the corresponding status.

### Remarks

Pixel GUI widgets draw onto a Surface in an explicit Begin/End frame; Window presentation is a separate step.

### Example

```c
/* Usage fragment: context, geometry and widget structs are initialized by the caller. */

OPSTATUS status = GUI_FUNC(Label)(&context, region, "Hello");
```

# LabelEx

Draws a label using its struct configuration.

### Syntax

#### Macro form

```c
OPSTATUS GUI_FUNC(LabelEx)(GUI_TYPE(TContext) * context, const GUI_TYPE(TLabel) * label);
```

#### Direct form

```c
OPSTATUS GUI_LabelEx(GUI_TContext * context, const GUI_TLabel * label);
```

### Parameters

- `context`: UI or GUI context, initialized and inside its frame lifecycle as required.
- `label`: widget label and stable identification text.

### Return value

`OPSTATUS`: SUCCESS if the operation completes; invalid frame/lifecycle, invalid range or incompatible input returns the corresponding status.

### Remarks

Pixel GUI widgets draw onto a Surface in an explicit Begin/End frame; Window presentation is a separate step. The struct configuration form shares the same interaction implementation as the direct form.

### Example

```c
/* Usage fragment: context, geometry and widget structs are initialized by the caller. */

OPSTATUS status = GUI_FUNC(LabelEx)(&context, "OK");
```

# Separator

Draws a visual separation rule.

### Syntax

#### Macro form

```c
OPSTATUS GUI_FUNC(Separator)(GUI_TYPE(TContext) * context, TQUAD_TYPE(float) region);
```

#### Direct form

```c
OPSTATUS GUI_Separator(GUI_TContext * context, TQuad_float region);
```

### Parameters

- `context`: UI or GUI context, initialized and inside its frame lifecycle as required.
- `region`: rectangle relative to the current Canvas/Surface or layout.

### Return value

`OPSTATUS`: SUCCESS if the operation completes; invalid frame/lifecycle, invalid range or incompatible input returns the corresponding status.

### Remarks

Pixel GUI widgets draw onto a Surface in an explicit Begin/End frame; Window presentation is a separate step.

### Example

```c
/* Usage fragment: context, geometry and widget structs are initialized by the caller. */

OPSTATUS status = GUI_FUNC(Separator)(&context, region);
```

# Button

Draws an interactive button and reports activation.

### Syntax

#### Macro form

```c
OPSTATUS GUI_FUNC(Button)(GUI_TYPE(TContext) * context, TQUAD_TYPE(float) region, const char *label, bool *outPressed);
```

#### Direct form

```c
OPSTATUS GUI_Button(GUI_TContext * context, TQuad_float region, const char *label, bool *outPressed);
```

### Parameters

- `context`: UI or GUI context, initialized and inside its frame lifecycle as required.
- `region`: rectangle relative to the current Canvas/Surface or layout.
- `label`: widget label and stable identification text.
- `outPressed`: receives whether button activated.

### Return value

`OPSTATUS`: SUCCESS if the operation completes; invalid frame/lifecycle, invalid range or incompatible input returns the corresponding status.

### Remarks

Pixel GUI widgets draw onto a Surface in an explicit Begin/End frame; Window presentation is a separate step.

### Example

```c
/* Usage fragment: context, geometry and widget structs are initialized by the caller. */
bool pressed = false;
OPSTATUS status = GUI_FUNC(Button)(&context, region, "OK", &pressed);
```

# ButtonEx

Draws a button from its struct configuration.

### Syntax

#### Macro form

```c
OPSTATUS GUI_FUNC(ButtonEx)(GUI_TYPE(TContext) * context, GUI_TYPE(TButton) * button);
```

#### Direct form

```c
OPSTATUS GUI_ButtonEx(GUI_TContext * context, GUI_TButton * button);
```

### Parameters

- `context`: UI or GUI context, initialized and inside its frame lifecycle as required.
- `button`: button widget configuration.

### Return value

`OPSTATUS`: SUCCESS if the operation completes; invalid frame/lifecycle, invalid range or incompatible input returns the corresponding status.

### Remarks

Pixel GUI widgets draw onto a Surface in an explicit Begin/End frame; Window presentation is a separate step. The struct configuration form shares the same interaction implementation as the direct form.

### Example

```c
/* Usage fragment: context, geometry and widget structs are initialized by the caller. */

OPSTATUS status = GUI_FUNC(ButtonEx)(&context, &button);
```

# Checkbox

Draws a checkbox and updates its selected state.

### Syntax

#### Macro form

```c
OPSTATUS GUI_FUNC(Checkbox)(GUI_TYPE(TContext) * context, TQUAD_TYPE(float) region, const char *label, bool *checked);
```

#### Direct form

```c
OPSTATUS GUI_Checkbox(GUI_TContext * context, TQuad_float region, const char *label, bool *checked);
```

### Parameters

- `context`: UI or GUI context, initialized and inside its frame lifecycle as required.
- `region`: rectangle relative to the current Canvas/Surface or layout.
- `label`: widget label and stable identification text.
- `checked`: borrowed boolean state updated upon checkbox interaction.

### Return value

`OPSTATUS`: SUCCESS if the operation completes; invalid frame/lifecycle, invalid range or incompatible input returns the corresponding status.

### Remarks

Pixel GUI widgets draw onto a Surface in an explicit Begin/End frame; Window presentation is a separate step.

### Example

```c
/* Usage fragment: context, geometry and widget structs are initialized by the caller. */
bool checked = false;
OPSTATUS status = GUI_FUNC(Checkbox)(&context, region, "OK", &checked);
```

# CheckboxEx

Draws a checkbox from a struct.

### Syntax

#### Macro form

```c
OPSTATUS GUI_FUNC(CheckboxEx)(GUI_TYPE(TContext) * context, GUI_TYPE(TCheckbox) * checkbox);
```

#### Direct form

```c
OPSTATUS GUI_CheckboxEx(GUI_TContext * context, GUI_TCheckbox * checkbox);
```

### Parameters

- `context`: UI or GUI context, initialized and inside its frame lifecycle as required.
- `checkbox`: checkbox widget configuration.

### Return value

`OPSTATUS`: SUCCESS if the operation completes; invalid frame/lifecycle, invalid range or incompatible input returns the corresponding status.

### Remarks

Pixel GUI widgets draw onto a Surface in an explicit Begin/End frame; Window presentation is a separate step. The struct configuration form shares the same interaction implementation as the direct form.

### Example

```c
/* Usage fragment: context, geometry and widget structs are initialized by the caller. */

OPSTATUS status = GUI_FUNC(CheckboxEx)(&context, &checkbox);
```

# Radio

Draws a radio selection and updates selected value.

### Syntax

#### Macro form

```c
OPSTATUS GUI_FUNC(Radio)(GUI_TYPE(TContext) * context, TQUAD_TYPE(float) region, const char *label, uint32_t option, uint32_t *selected);
```

#### Direct form

```c
OPSTATUS GUI_Radio(GUI_TContext * context, TQuad_float region, const char *label, uint32_t option, uint32_t *selected);
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

Pixel GUI widgets draw onto a Surface in an explicit Begin/End frame; Window presentation is a separate step.

### Example

```c
/* Usage fragment: context, geometry and widget structs are initialized by the caller. */
uint32_t selected = 0;
OPSTATUS status = GUI_FUNC(Radio)(&context, region, "OK", 1, &selected);
```

# RadioEx

Draws a radio widget configured by struct.

### Syntax

#### Macro form

```c
OPSTATUS GUI_FUNC(RadioEx)(GUI_TYPE(TContext) * context, GUI_TYPE(TRadio) * radio);
```

#### Direct form

```c
OPSTATUS GUI_RadioEx(GUI_TContext * context, GUI_TRadio * radio);
```

### Parameters

- `context`: UI or GUI context, initialized and inside its frame lifecycle as required.
- `radio`: radio widget configuration.

### Return value

`OPSTATUS`: SUCCESS if the operation completes; invalid frame/lifecycle, invalid range or incompatible input returns the corresponding status.

### Remarks

Pixel GUI widgets draw onto a Surface in an explicit Begin/End frame; Window presentation is a separate step. The struct configuration form shares the same interaction implementation as the direct form.

### Example

```c
/* Usage fragment: context, geometry and widget structs are initialized by the caller. */

OPSTATUS status = GUI_FUNC(RadioEx)(&context, &radio);
```

# Slider

Draws a ranged slider and modifies a caller-owned numeric value.

### Syntax

#### Macro form

```c
OPSTATUS GUI_FUNC(Slider)(GUI_TYPE(TContext) * context, TQUAD_TYPE(float) region, float minimum, float maximum, float *value);
```

#### Direct form

```c
OPSTATUS GUI_Slider(GUI_TContext * context, TQuad_float region, float minimum, float maximum, float *value);
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

Pixel GUI widgets draw onto a Surface in an explicit Begin/End frame; Window presentation is a separate step.

### Example

```c
/* Usage fragment: context, geometry and widget structs are initialized by the caller. */
float value = 0.5f;
OPSTATUS status = GUI_FUNC(Slider)(&context, region, 0, 100, &value);
```

# SliderEx

Draws a slider from a struct.

### Syntax

#### Macro form

```c
OPSTATUS GUI_FUNC(SliderEx)(GUI_TYPE(TContext) * context, GUI_TYPE(TSlider) * slider);
```

#### Direct form

```c
OPSTATUS GUI_SliderEx(GUI_TContext * context, GUI_TSlider * slider);
```

### Parameters

- `context`: UI or GUI context, initialized and inside its frame lifecycle as required.
- `slider`: slider widget configuration.

### Return value

`OPSTATUS`: SUCCESS if the operation completes; invalid frame/lifecycle, invalid range or incompatible input returns the corresponding status.

### Remarks

Pixel GUI widgets draw onto a Surface in an explicit Begin/End frame; Window presentation is a separate step. The struct configuration form shares the same interaction implementation as the direct form.

### Example

```c
/* Usage fragment: context, geometry and widget structs are initialized by the caller. */

OPSTATUS status = GUI_FUNC(SliderEx)(&context, &slider);
```

# Progress

Draws normalized progress feedback.

### Syntax

#### Macro form

```c
OPSTATUS GUI_FUNC(Progress)(GUI_TYPE(TContext) * context, TQUAD_TYPE(float) region, float fraction);
```

#### Direct form

```c
OPSTATUS GUI_Progress(GUI_TContext * context, TQuad_float region, float fraction);
```

### Parameters

- `context`: UI or GUI context, initialized and inside its frame lifecycle as required.
- `region`: rectangle relative to the current Canvas/Surface or layout.
- `fraction`: normalized progress fraction.

### Return value

`OPSTATUS`: SUCCESS if the operation completes; invalid frame/lifecycle, invalid range or incompatible input returns the corresponding status.

### Remarks

Pixel GUI widgets draw onto a Surface in an explicit Begin/End frame; Window presentation is a separate step.

### Example

```c
/* Usage fragment: context, geometry and widget structs are initialized by the caller. */

OPSTATUS status = GUI_FUNC(Progress)(&context, region, 0.5f);
```

# ProgressBar

Draws a progress bar with current and maximum values.

### Syntax

#### Macro form

```c
OPSTATUS GUI_FUNC(ProgressBar)(GUI_TYPE(TContext) * context, TQUAD_TYPE(float) region, uint64_t value, uint64_t maximum);
```

#### Direct form

```c
OPSTATUS GUI_ProgressBar(GUI_TContext * context, TQuad_float region, uint64_t value, uint64_t maximum);
```

### Parameters

- `context`: UI or GUI context, initialized and inside its frame lifecycle as required.
- `region`: rectangle relative to the current Canvas/Surface or layout.
- `value`: widget value to show or change.
- `maximum`: upper value bound.

### Return value

`OPSTATUS`: SUCCESS if the operation completes; invalid frame/lifecycle, invalid range or incompatible input returns the corresponding status.

### Remarks

Pixel GUI widgets draw onto a Surface in an explicit Begin/End frame; Window presentation is a separate step.

### Example

```c
/* Usage fragment: context, geometry and widget structs are initialized by the caller. */

OPSTATUS status = GUI_FUNC(ProgressBar)(&context, region, 50, 100);
```

# ProgressBarEx

Draws a progress bar using struct settings.

### Syntax

#### Macro form

```c
OPSTATUS GUI_FUNC(ProgressBarEx)(GUI_TYPE(TContext) * context, const GUI_TYPE(TProgressBar) * progress);
```

#### Direct form

```c
OPSTATUS GUI_ProgressBarEx(GUI_TContext * context, const GUI_TProgressBar * progress);
```

### Parameters

- `context`: UI or GUI context, initialized and inside its frame lifecycle as required.
- `progress`: progress descriptor/state.

### Return value

`OPSTATUS`: SUCCESS if the operation completes; invalid frame/lifecycle, invalid range or incompatible input returns the corresponding status.

### Remarks

Pixel GUI widgets draw onto a Surface in an explicit Begin/End frame; Window presentation is a separate step. The struct configuration form shares the same interaction implementation as the direct form.

### Example

```c
/* Usage fragment: context, geometry and widget structs are initialized by the caller. */

OPSTATUS status = GUI_FUNC(ProgressBarEx)(&context, &progress);
```

# InputText

Edits text in a caller-provided buffer.

### Syntax

#### Macro form

```c
OPSTATUS GUI_FUNC(InputText)(GUI_TYPE(TContext) * context, TQUAD_TYPE(float) region, char *buffer, size_t capacity, size_t *inoutLength);
```

#### Direct form

```c
OPSTATUS GUI_InputText(GUI_TContext * context, TQuad_float region, char *buffer, size_t capacity, size_t *inoutLength);
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

Pixel GUI widgets draw onto a Surface in an explicit Begin/End frame; Window presentation is a separate step. Editing preserves caller ownership of the buffer; capacity and UTF-8 validity must be respected.

### Example

```c
/* Usage fragment: context, geometry and widget structs are initialized by the caller. */
char buffer[128] = {0};
size_t length = 0;
OPSTATUS status = GUI_FUNC(InputText)(&context, region, buffer, sizeof buffer, &length);
```

# InputTextEx

Edits text with full input-widget state.

### Syntax

#### Macro form

```c
OPSTATUS GUI_FUNC(InputTextEx)(GUI_TYPE(TContext) * context, GUI_TYPE(TInputText) * input);
```

#### Direct form

```c
OPSTATUS GUI_InputTextEx(GUI_TContext * context, GUI_TInputText * input);
```

### Parameters

- `context`: UI or GUI context, initialized and inside its frame lifecycle as required.
- `input`: input-text state/configuration.

### Return value

`OPSTATUS`: SUCCESS if the operation completes; invalid frame/lifecycle, invalid range or incompatible input returns the corresponding status.

### Remarks

Pixel GUI widgets draw onto a Surface in an explicit Begin/End frame; Window presentation is a separate step. The struct configuration form shares the same interaction implementation as the direct form. Editing preserves caller ownership of the buffer; capacity and UTF-8 validity must be respected.

### Example

```c
/* Usage fragment: context, geometry and widget structs are initialized by the caller. */

OPSTATUS status = GUI_FUNC(InputTextEx)(&context, &input);
```

# TextArea

Edits multiline text in caller-provided memory.

### Syntax

#### Macro form

```c
OPSTATUS GUI_FUNC(TextArea)(GUI_TYPE(TContext) * context, TQUAD_TYPE(float) region, char *buffer, size_t capacity, size_t *inoutLength);
```

#### Direct form

```c
OPSTATUS GUI_TextArea(GUI_TContext * context, TQuad_float region, char *buffer, size_t capacity, size_t *inoutLength);
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

Pixel GUI widgets draw onto a Surface in an explicit Begin/End frame; Window presentation is a separate step. Editing preserves caller ownership of the buffer; capacity and UTF-8 validity must be respected.

### Example

```c
/* Usage fragment: context, geometry and widget structs are initialized by the caller. */
char buffer[128] = {0};
size_t length = 0;
OPSTATUS status = GUI_FUNC(TextArea)(&context, region, buffer, sizeof buffer, &length);
```

# TextAreaEx

Edits multiline text through a state struct.

### Syntax

#### Macro form

```c
OPSTATUS GUI_FUNC(TextAreaEx)(GUI_TYPE(TContext) * context, GUI_TYPE(TTextArea) * input);
```

#### Direct form

```c
OPSTATUS GUI_TextAreaEx(GUI_TContext * context, GUI_TTextArea * input);
```

### Parameters

- `context`: UI or GUI context, initialized and inside its frame lifecycle as required.
- `input`: input-text state/configuration.

### Return value

`OPSTATUS`: SUCCESS if the operation completes; invalid frame/lifecycle, invalid range or incompatible input returns the corresponding status.

### Remarks

Pixel GUI widgets draw onto a Surface in an explicit Begin/End frame; Window presentation is a separate step. The struct configuration form shares the same interaction implementation as the direct form. Editing preserves caller ownership of the buffer; capacity and UTF-8 validity must be respected.

### Example

```c
/* Usage fragment: context, geometry and widget structs are initialized by the caller. */

OPSTATUS status = GUI_FUNC(TextAreaEx)(&context, &input);
```

# InputText_Select

Selects the range between anchor and caret positions.

### Syntax

#### Macro form

```c
OPSTATUS GUI_FUNC(InputText_Select)(GUI_TYPE(TInputText) * input, size_t anchor, size_t caret);
```

#### Direct form

```c
OPSTATUS GUI_InputText_Select(GUI_TInputText * input, size_t anchor, size_t caret);
```

### Parameters

- `input`: input-text state/configuration.
- `anchor`: selection anchor byte/character position per widget contract.
- `caret`: selection caret position.

### Return value

`OPSTATUS`: SUCCESS if the operation completes; invalid frame/lifecycle, invalid range or incompatible input returns the corresponding status.

### Remarks

Pixel GUI widgets draw onto a Surface in an explicit Begin/End frame; Window presentation is a separate step. Editing preserves caller ownership of the buffer; capacity and UTF-8 validity must be respected.

### Example

```c
/* Usage fragment: context, geometry and widget structs are initialized by the caller. */

OPSTATUS status = GUI_FUNC(InputText_Select)(&input, 0, 3);
```

# InputText_Copy

Copies selected text from a widget into a caller buffer.

### Syntax

#### Macro form

```c
OPSTATUS GUI_FUNC(InputText_Copy)(const GUI_TYPE(TInputText) * input, char *buffer, size_t capacity, size_t *outLength);
```

#### Direct form

```c
OPSTATUS GUI_InputText_Copy(const GUI_TInputText * input, char *buffer, size_t capacity, size_t *outLength);
```

### Parameters

- `input`: input-text state/configuration.
- `buffer`: editable caller-owned character buffer.
- `capacity`: capacity of input/copy destination in bytes.
- `outLength`: receives byte length copied or required.

### Return value

`OPSTATUS`: SUCCESS if the operation completes; invalid frame/lifecycle, invalid range or incompatible input returns the corresponding status.

### Remarks

Pixel GUI widgets draw onto a Surface in an explicit Begin/End frame; Window presentation is a separate step. Editing preserves caller ownership of the buffer; capacity and UTF-8 validity must be respected.

### Example

```c
/* Usage fragment: context, geometry and widget structs are initialized by the caller. */
char buffer[128] = {0};
size_t outputLength = 0;
OPSTATUS status = GUI_FUNC(InputText_Copy)(&input, buffer, sizeof buffer, &outputLength);
```

# InputText_Cut

Copies and removes the selected text from the widget.

### Syntax

#### Macro form

```c
OPSTATUS GUI_FUNC(InputText_Cut)(GUI_TYPE(TInputText) * input, char *buffer, size_t capacity, size_t *outLength);
```

#### Direct form

```c
OPSTATUS GUI_InputText_Cut(GUI_TInputText * input, char *buffer, size_t capacity, size_t *outLength);
```

### Parameters

- `input`: input-text state/configuration.
- `buffer`: editable caller-owned character buffer.
- `capacity`: capacity of input/copy destination in bytes.
- `outLength`: receives byte length copied or required.

### Return value

`OPSTATUS`: SUCCESS if the operation completes; invalid frame/lifecycle, invalid range or incompatible input returns the corresponding status.

### Remarks

Pixel GUI widgets draw onto a Surface in an explicit Begin/End frame; Window presentation is a separate step. Editing preserves caller ownership of the buffer; capacity and UTF-8 validity must be respected.

### Example

```c
/* Usage fragment: context, geometry and widget structs are initialized by the caller. */
char buffer[128] = {0};
size_t outputLength = 0;
OPSTATUS status = GUI_FUNC(InputText_Cut)(&input, buffer, sizeof buffer, &outputLength);
```

# InputText_Paste

Pastes validated UTF-8 into the input model.

### Syntax

#### Macro form

```c
OPSTATUS GUI_FUNC(InputText_Paste)(GUI_TYPE(TInputText) * input, const char *utf8Text, size_t byteLength);
```

#### Direct form

```c
OPSTATUS GUI_InputText_Paste(GUI_TInputText * input, const char *utf8Text, size_t byteLength);
```

### Parameters

- `input`: input-text state/configuration.
- `utf8Text`: UTF-8 text inserted from caller storage.
- `byteLength`: number of UTF-8 bytes to paste.

### Return value

`OPSTATUS`: SUCCESS if the operation completes; invalid frame/lifecycle, invalid range or incompatible input returns the corresponding status.

### Remarks

Pixel GUI widgets draw onto a Surface in an explicit Begin/End frame; Window presentation is a separate step. Editing preserves caller ownership of the buffer; capacity and UTF-8 validity must be respected.

### Example

```c
/* Usage fragment: context, geometry and widget structs are initialized by the caller. */

OPSTATUS status = GUI_FUNC(InputText_Paste)(&input, "Hello", 5);
```

# List

Draws a selectable list.

### Syntax

#### Macro form

```c
OPSTATUS GUI_FUNC(List)(GUI_TYPE(TContext) * context, TQUAD_TYPE(float) region, const char *const *items, size_t count, size_t *selected);
```

#### Direct form

```c
OPSTATUS GUI_List(GUI_TContext * context, TQuad_float region, const char *const *items, size_t count, size_t *selected);
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

Pixel GUI widgets draw onto a Surface in an explicit Begin/End frame; Window presentation is a separate step.

### Example

```c
/* Usage fragment: context, geometry and widget structs are initialized by the caller. */
const char *items[] = {"One", "Two"};
size_t selected = 0;
OPSTATUS status = GUI_FUNC(List)(&context, region, items, 2, &selected);
```

# ListEx

Draws a struct-configured list.

### Syntax

#### Macro form

```c
OPSTATUS GUI_FUNC(ListEx)(GUI_TYPE(TContext) * context, GUI_TYPE(TList) * list);
```

#### Direct form

```c
OPSTATUS GUI_ListEx(GUI_TContext * context, GUI_TList * list);
```

### Parameters

- `context`: UI or GUI context, initialized and inside its frame lifecycle as required.
- `list`: list widget configuration.

### Return value

`OPSTATUS`: SUCCESS if the operation completes; invalid frame/lifecycle, invalid range or incompatible input returns the corresponding status.

### Remarks

Pixel GUI widgets draw onto a Surface in an explicit Begin/End frame; Window presentation is a separate step. The struct configuration form shares the same interaction implementation as the direct form.

### Example

```c
/* Usage fragment: context, geometry and widget structs are initialized by the caller. */

OPSTATUS status = GUI_FUNC(ListEx)(&context, &list);
```

# Menu

Draws and interacts with a list of menu items.

### Syntax

#### Macro form

```c
OPSTATUS GUI_FUNC(Menu)(GUI_TYPE(TContext) * context, TQUAD_TYPE(float) region, const char *const *items, size_t count, size_t *selected, bool *outActivated);
```

#### Direct form

```c
OPSTATUS GUI_Menu(GUI_TContext * context, TQuad_float region, const char *const *items, size_t count, size_t *selected, bool *outActivated);
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

Pixel GUI widgets draw onto a Surface in an explicit Begin/End frame; Window presentation is a separate step.

### Example

```c
/* Usage fragment: context, geometry and widget structs are initialized by the caller. */
const char *items[] = {"One", "Two"};
size_t selected = 0;
bool activated = false;
OPSTATUS status = GUI_FUNC(Menu)(&context, region, items, 2, &selected, &activated);
```

# MenuEx

Draws a menu with struct configuration.

### Syntax

#### Macro form

```c
OPSTATUS GUI_FUNC(MenuEx)(GUI_TYPE(TContext) * context, GUI_TYPE(TMenu) * menu);
```

#### Direct form

```c
OPSTATUS GUI_MenuEx(GUI_TContext * context, GUI_TMenu * menu);
```

### Parameters

- `context`: UI or GUI context, initialized and inside its frame lifecycle as required.
- `menu`: menu widget configuration.

### Return value

`OPSTATUS`: SUCCESS if the operation completes; invalid frame/lifecycle, invalid range or incompatible input returns the corresponding status.

### Remarks

Pixel GUI widgets draw onto a Surface in an explicit Begin/End frame; Window presentation is a separate step. The struct configuration form shares the same interaction implementation as the direct form.

### Example

```c
/* Usage fragment: context, geometry and widget structs are initialized by the caller. */

OPSTATUS status = GUI_FUNC(MenuEx)(&context, &menu);
```

# Panel_Begin

Begins a panel using direct geometry and title.

### Syntax

#### Macro form

```c
OPSTATUS GUI_FUNC(Panel_Begin)(GUI_TYPE(TContext) * context, TQUAD_TYPE(float) region, const char *title);
```

#### Direct form

```c
OPSTATUS GUI_Panel_Begin(GUI_TContext * context, TQuad_float region, const char *title);
```

### Parameters

- `context`: UI or GUI context, initialized and inside its frame lifecycle as required.
- `region`: rectangle relative to the current Canvas/Surface or layout.
- `title`: caption for a panel or dialog.

### Return value

`OPSTATUS`: SUCCESS if the operation completes; invalid frame/lifecycle, invalid range or incompatible input returns the corresponding status.

### Remarks

Pixel GUI widgets draw onto a Surface in an explicit Begin/End frame; Window presentation is a separate step.

### Example

```c
/* Usage fragment: context, geometry and widget structs are initialized by the caller. */

OPSTATUS status = GUI_FUNC(Panel_Begin)(&context, region, "Title");
```

# Panel_BeginEx

Begins a panel using a struct configuration.

### Syntax

#### Macro form

```c
OPSTATUS GUI_FUNC(Panel_BeginEx)(GUI_TYPE(TContext) * context, const GUI_TYPE(TPanel) * panel);
```

#### Direct form

```c
OPSTATUS GUI_Panel_BeginEx(GUI_TContext * context, const GUI_TPanel * panel);
```

### Parameters

- `context`: UI or GUI context, initialized and inside its frame lifecycle as required.
- `panel`: panel widget configuration.

### Return value

`OPSTATUS`: SUCCESS if the operation completes; invalid frame/lifecycle, invalid range or incompatible input returns the corresponding status.

### Remarks

Pixel GUI widgets draw onto a Surface in an explicit Begin/End frame; Window presentation is a separate step. The struct configuration form shares the same interaction implementation as the direct form.

### Example

```c
/* Usage fragment: context, geometry and widget structs are initialized by the caller. */

OPSTATUS status = GUI_FUNC(Panel_BeginEx)(&context, &panel);
```

# Panel_End

Closes a panel scope.

### Syntax

#### Macro form

```c
OPSTATUS GUI_FUNC(Panel_End)(GUI_TYPE(TContext) * context);
```

#### Direct form

```c
OPSTATUS GUI_Panel_End(GUI_TContext * context);
```

### Parameters

- `context`: UI or GUI context, initialized and inside its frame lifecycle as required.

### Return value

`OPSTATUS`: SUCCESS if the operation completes; invalid frame/lifecycle, invalid range or incompatible input returns the corresponding status.

### Remarks

Pixel GUI widgets draw onto a Surface in an explicit Begin/End frame; Window presentation is a separate step.

### Example

```c
/* Usage fragment: context, geometry and widget structs are initialized by the caller. */

OPSTATUS status = GUI_FUNC(Panel_End)(&context);
```

# Modal_Begin

Begins a focus-modal region.

### Syntax

#### Macro form

```c
OPSTATUS GUI_FUNC(Modal_Begin)(GUI_TYPE(TContext) * context, GUI_TYPE(WidgetId) id);
```

#### Direct form

```c
OPSTATUS GUI_Modal_Begin(GUI_TContext * context, GUI_WidgetId id);
```

### Parameters

- `context`: UI or GUI context, initialized and inside its frame lifecycle as required.
- `id`: stable widget ID used by modal API.

### Return value

`OPSTATUS`: SUCCESS if the operation completes; invalid frame/lifecycle, invalid range or incompatible input returns the corresponding status.

### Remarks

Pixel GUI widgets draw onto a Surface in an explicit Begin/End frame; Window presentation is a separate step.

### Example

```c
/* Usage fragment: context, geometry and widget structs are initialized by the caller. */

OPSTATUS status = GUI_FUNC(Modal_Begin)(&context, id);
```

# Modal_End

Ends a focus-modal region.

### Syntax

#### Macro form

```c
OPSTATUS GUI_FUNC(Modal_End)(GUI_TYPE(TContext) * context);
```

#### Direct form

```c
OPSTATUS GUI_Modal_End(GUI_TContext * context);
```

### Parameters

- `context`: UI or GUI context, initialized and inside its frame lifecycle as required.

### Return value

`OPSTATUS`: SUCCESS if the operation completes; invalid frame/lifecycle, invalid range or incompatible input returns the corresponding status.

### Remarks

Pixel GUI widgets draw onto a Surface in an explicit Begin/End frame; Window presentation is a separate step.

### Example

```c
/* Usage fragment: context, geometry and widget structs are initialized by the caller. */

OPSTATUS status = GUI_FUNC(Modal_End)(&context);
```

# Confirm

Draws a confirm/reject dialog, reporting selected action.

### Syntax

#### Macro form

```c
OPSTATUS GUI_FUNC(Confirm)(GUI_TYPE(TContext) * context, TQUAD_TYPE(float) region, const char *message, bool *outAccepted, bool *outRejected);
```

#### Direct form

```c
OPSTATUS GUI_Confirm(GUI_TContext * context, TQuad_float region, const char *message, bool *outAccepted, bool *outRejected);
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

Pixel GUI widgets draw onto a Surface in an explicit Begin/End frame; Window presentation is a separate step.

### Example

```c
/* Usage fragment: context, geometry and widget structs are initialized by the caller. */
bool accepted = false;
bool rejected = false;
OPSTATUS status = GUI_FUNC(Confirm)(&context, region, "Message", &accepted, &rejected);
```

# ConfirmEx

Draws a confirmation configured by struct.

### Syntax

#### Macro form

```c
OPSTATUS GUI_FUNC(ConfirmEx)(GUI_TYPE(TContext) * context, GUI_TYPE(TDialog) * dialog);
```

#### Direct form

```c
OPSTATUS GUI_ConfirmEx(GUI_TContext * context, GUI_TDialog * dialog);
```

### Parameters

- `context`: UI or GUI context, initialized and inside its frame lifecycle as required.
- `dialog`: dialog or confirmation descriptor.

### Return value

`OPSTATUS`: SUCCESS if the operation completes; invalid frame/lifecycle, invalid range or incompatible input returns the corresponding status.

### Remarks

Pixel GUI widgets draw onto a Surface in an explicit Begin/End frame; Window presentation is a separate step. The struct configuration form shares the same interaction implementation as the direct form.

### Example

```c
/* Usage fragment: context, geometry and widget structs are initialized by the caller. */

OPSTATUS status = GUI_FUNC(ConfirmEx)(&context, &dialog);
```

# Dialog

Draws a dialog and reports dismissal.

### Syntax

#### Macro form

```c
OPSTATUS GUI_FUNC(Dialog)(GUI_TYPE(TContext) * context, TQUAD_TYPE(float) region, const char *title, const char *message, bool *outDismissed);
```

#### Direct form

```c
OPSTATUS GUI_Dialog(GUI_TContext * context, TQuad_float region, const char *title, const char *message, bool *outDismissed);
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

Pixel GUI widgets draw onto a Surface in an explicit Begin/End frame; Window presentation is a separate step.

### Example

```c
/* Usage fragment: context, geometry and widget structs are initialized by the caller. */
bool dismissed = false;
OPSTATUS status = GUI_FUNC(Dialog)(&context, region, "Title", "Message", &dismissed);
```

# DialogEx

Draws a dialog from its full configuration.

### Syntax

#### Macro form

```c
OPSTATUS GUI_FUNC(DialogEx)(GUI_TYPE(TContext) * context, GUI_TYPE(TDialog) * dialog);
```

#### Direct form

```c
OPSTATUS GUI_DialogEx(GUI_TContext * context, GUI_TDialog * dialog);
```

### Parameters

- `context`: UI or GUI context, initialized and inside its frame lifecycle as required.
- `dialog`: dialog or confirmation descriptor.

### Return value

`OPSTATUS`: SUCCESS if the operation completes; invalid frame/lifecycle, invalid range or incompatible input returns the corresponding status.

### Remarks

Pixel GUI widgets draw onto a Surface in an explicit Begin/End frame; Window presentation is a separate step. The struct configuration form shares the same interaction implementation as the direct form.

### Example

```c
/* Usage fragment: context, geometry and widget structs are initialized by the caller. */

OPSTATUS status = GUI_FUNC(DialogEx)(&context, &dialog);
```

# StatusBar

Draws status text in a bar.

### Syntax

#### Macro form

```c
OPSTATUS GUI_FUNC(StatusBar)(GUI_TYPE(TContext) * context, TQUAD_TYPE(float) region, const char *left, const char *right);
```

#### Direct form

```c
OPSTATUS GUI_StatusBar(GUI_TContext * context, TQuad_float region, const char *left, const char *right);
```

### Parameters

- `context`: UI or GUI context, initialized and inside its frame lifecycle as required.
- `region`: rectangle relative to the current Canvas/Surface or layout.
- `left`: left status bar text.
- `right`: right status bar text.

### Return value

`OPSTATUS`: SUCCESS if the operation completes; invalid frame/lifecycle, invalid range or incompatible input returns the corresponding status.

### Remarks

Pixel GUI widgets draw onto a Surface in an explicit Begin/End frame; Window presentation is a separate step.

### Example

```c
/* Usage fragment: context, geometry and widget structs are initialized by the caller. */

OPSTATUS status = GUI_FUNC(StatusBar)(&context, region, &left, &right);
```

# Toast

Draws a conditional non-modal notification.

### Syntax

#### Macro form

```c
OPSTATUS GUI_FUNC(Toast)(GUI_TYPE(TContext) * context, TQUAD_TYPE(float) region, const char *message, bool visible);
```

#### Direct form

```c
OPSTATUS GUI_Toast(GUI_TContext * context, TQuad_float region, const char *message, bool visible);
```

### Parameters

- `context`: UI or GUI context, initialized and inside its frame lifecycle as required.
- `region`: rectangle relative to the current Canvas/Surface or layout.
- `message`: message displayed to users.
- `visible`: controls visibility of toast or scrollbar content.

### Return value

`OPSTATUS`: SUCCESS if the operation completes; invalid frame/lifecycle, invalid range or incompatible input returns the corresponding status.

### Remarks

Pixel GUI widgets draw onto a Surface in an explicit Begin/End frame; Window presentation is a separate step.

### Example

```c
/* Usage fragment: context, geometry and widget structs are initialized by the caller. */

OPSTATUS status = GUI_FUNC(Toast)(&context, region, "Message", true);
```

# Spinner

Draws an animated spinner using caller-supplied phase.

### Syntax

#### Macro form

```c
OPSTATUS GUI_FUNC(Spinner)(GUI_TYPE(TContext) * context, TQUAD_TYPE(float) region, const char *label, uint64_t phase);
```

#### Direct form

```c
OPSTATUS GUI_Spinner(GUI_TContext * context, TQuad_float region, const char *label, uint64_t phase);
```

### Parameters

- `context`: UI or GUI context, initialized and inside its frame lifecycle as required.
- `region`: rectangle relative to the current Canvas/Surface or layout.
- `label`: widget label and stable identification text.
- `phase`: caller-supplied spinner animation phase.

### Return value

`OPSTATUS`: SUCCESS if the operation completes; invalid frame/lifecycle, invalid range or incompatible input returns the corresponding status.

### Remarks

Pixel GUI widgets draw onto a Surface in an explicit Begin/End frame; Window presentation is a separate step.

### Example

```c
/* Usage fragment: context, geometry and widget structs are initialized by the caller. */

OPSTATUS status = GUI_FUNC(Spinner)(&context, region, "OK", 0);
```

# RenderSoftware

Rasterizes a sequence of GUI drawing commands to a pixel Surface.

### Syntax

#### Macro form

```c
OPSTATUS GUI_FUNC(RenderSoftware)(GRAPHICS_TYPE(TSurface) * surface, const GUI_TYPE(TCommand) * commands, size_t count, float scale);
```

#### Direct form

```c
OPSTATUS GUI_RenderSoftware(Graphics_TSurface * surface, const GUI_TCommand * commands, size_t count, float scale);
```

### Parameters

- `surface`: pixel Surface for GUI drawing, image display or rendering.
- `commands`: readable rendering-command array produced by the GUI pipeline.
- `count`: number of items or render commands.
- `scale`: positive drawing or font scale.

### Return value

`OPSTATUS`: SUCCESS if the operation completes; invalid frame/lifecycle, invalid range or incompatible input returns the corresponding status.

### Remarks

Pixel GUI widgets draw onto a Surface in an explicit Begin/End frame; Window presentation is a separate step.

### Example

```c
/* Usage fragment: context, geometry and widget structs are initialized by the caller. */

OPSTATUS status = GUI_FUNC(RenderSoftware)(&surface, commands, 2, 1.0f);
```

# ImageButton

Draws a clickable image.

### Syntax

#### Macro form

```c
OPSTATUS GUI_FUNC(ImageButton)(GUI_TYPE(TContext) * context, TQUAD_TYPE(float) region, const GRAPHICS_TYPE(TSurface) * surface, bool *outPressed);
```

#### Direct form

```c
OPSTATUS GUI_ImageButton(GUI_TContext * context, TQuad_float region, const Graphics_TSurface * surface, bool *outPressed);
```

### Parameters

- `context`: UI or GUI context, initialized and inside its frame lifecycle as required.
- `region`: rectangle relative to the current Canvas/Surface or layout.
- `surface`: pixel Surface for GUI drawing, image display or rendering.
- `outPressed`: receives whether button activated.

### Return value

`OPSTATUS`: SUCCESS if the operation completes; invalid frame/lifecycle, invalid range or incompatible input returns the corresponding status.

### Remarks

Pixel GUI widgets draw onto a Surface in an explicit Begin/End frame; Window presentation is a separate step.

### Example

```c
/* Usage fragment: context, geometry and widget structs are initialized by the caller. */
bool pressed = false;
OPSTATUS status = GUI_FUNC(ImageButton)(&context, region, &surface, &pressed);
```

# ImageButtonEx

Draws an image button configured by struct.

### Syntax

#### Macro form

```c
OPSTATUS GUI_FUNC(ImageButtonEx)(GUI_TYPE(TContext) * context, GUI_TYPE(TImageButton) * button);
```

#### Direct form

```c
OPSTATUS GUI_ImageButtonEx(GUI_TContext * context, GUI_TImageButton * button);
```

### Parameters

- `context`: UI or GUI context, initialized and inside its frame lifecycle as required.
- `button`: button widget configuration.

### Return value

`OPSTATUS`: SUCCESS if the operation completes; invalid frame/lifecycle, invalid range or incompatible input returns the corresponding status.

### Remarks

Pixel GUI widgets draw onto a Surface in an explicit Begin/End frame; Window presentation is a separate step. The struct configuration form shares the same interaction implementation as the direct form.

### Example

```c
/* Usage fragment: context, geometry and widget structs are initialized by the caller. */

OPSTATUS status = GUI_FUNC(ImageButtonEx)(&context, &button);
```

# ComboBox

Draws a list selection widget with open/closed state.

### Syntax

#### Macro form

```c
OPSTATUS GUI_FUNC(ComboBox)(GUI_TYPE(TContext) * context, TQUAD_TYPE(float) region, const char *const *items, size_t count, size_t *selected, bool *open);
```

#### Direct form

```c
OPSTATUS GUI_ComboBox(GUI_TContext * context, TQuad_float region, const char *const *items, size_t count, size_t *selected, bool *open);
```

### Parameters

- `context`: UI or GUI context, initialized and inside its frame lifecycle as required.
- `region`: rectangle relative to the current Canvas/Surface or layout.
- `items`: borrowed array of display-item strings.
- `count`: number of items or render commands.
- `selected`: caller-owned selected index or option.
- `open`: caller-managed expanded/open state.

### Return value

`OPSTATUS`: SUCCESS if the operation completes; invalid frame/lifecycle, invalid range or incompatible input returns the corresponding status.

### Remarks

Pixel GUI widgets draw onto a Surface in an explicit Begin/End frame; Window presentation is a separate step.

### Example

```c
/* Usage fragment: context, geometry and widget structs are initialized by the caller. */
const char *items[] = {"One", "Two"};
size_t selected = 0;
bool open = false;
OPSTATUS status = GUI_FUNC(ComboBox)(&context, region, items, 2, &selected, &open);
```

# ComboBoxEx

Draws a combo box configured by struct.

### Syntax

#### Macro form

```c
OPSTATUS GUI_FUNC(ComboBoxEx)(GUI_TYPE(TContext) * context, GUI_TYPE(TComboBox) * combo);
```

#### Direct form

```c
OPSTATUS GUI_ComboBoxEx(GUI_TContext * context, GUI_TComboBox * combo);
```

### Parameters

- `context`: UI or GUI context, initialized and inside its frame lifecycle as required.
- `combo`: combobox widget configuration.

### Return value

`OPSTATUS`: SUCCESS if the operation completes; invalid frame/lifecycle, invalid range or incompatible input returns the corresponding status.

### Remarks

Pixel GUI widgets draw onto a Surface in an explicit Begin/End frame; Window presentation is a separate step. The struct configuration form shares the same interaction implementation as the direct form.

### Example

```c
/* Usage fragment: context, geometry and widget structs are initialized by the caller. */

OPSTATUS status = GUI_FUNC(ComboBoxEx)(&context, &combo);
```

# Tabs

Draws tabs and updates the selected tab.

### Syntax

#### Macro form

```c
OPSTATUS GUI_FUNC(Tabs)(GUI_TYPE(TContext) * context, TQUAD_TYPE(float) region, const char *const *items, size_t count, size_t *selected);
```

#### Direct form

```c
OPSTATUS GUI_Tabs(GUI_TContext * context, TQuad_float region, const char *const *items, size_t count, size_t *selected);
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

Pixel GUI widgets draw onto a Surface in an explicit Begin/End frame; Window presentation is a separate step.

### Example

```c
/* Usage fragment: context, geometry and widget structs are initialized by the caller. */
const char *items[] = {"One", "Two"};
size_t selected = 0;
OPSTATUS status = GUI_FUNC(Tabs)(&context, region, items, 2, &selected);
```

# TabsEx

Draws a tab set configured by struct.

### Syntax

#### Macro form

```c
OPSTATUS GUI_FUNC(TabsEx)(GUI_TYPE(TContext) * context, GUI_TYPE(TTabs) * tabs);
```

#### Direct form

```c
OPSTATUS GUI_TabsEx(GUI_TContext * context, GUI_TTabs * tabs);
```

### Parameters

- `context`: UI or GUI context, initialized and inside its frame lifecycle as required.
- `tabs`: tabs widget configuration.

### Return value

`OPSTATUS`: SUCCESS if the operation completes; invalid frame/lifecycle, invalid range or incompatible input returns the corresponding status.

### Remarks

Pixel GUI widgets draw onto a Surface in an explicit Begin/End frame; Window presentation is a separate step. The struct configuration form shares the same interaction implementation as the direct form.

### Example

```c
/* Usage fragment: context, geometry and widget structs are initialized by the caller. */

OPSTATUS status = GUI_FUNC(TabsEx)(&context, &tabs);
```

# TreeNode

Draws a foldable, optionally selectable tree node.

### Syntax

#### Macro form

```c
OPSTATUS GUI_FUNC(TreeNode)(GUI_TYPE(TContext) * context, TQUAD_TYPE(float) region, const char *label, bool *expanded, bool *selected);
```

#### Direct form

```c
OPSTATUS GUI_TreeNode(GUI_TContext * context, TQuad_float region, const char *label, bool *expanded, bool *selected);
```

### Parameters

- `context`: UI or GUI context, initialized and inside its frame lifecycle as required.
- `region`: rectangle relative to the current Canvas/Surface or layout.
- `label`: widget label and stable identification text.
- `expanded`: whether tree node is expanded.
- `selected`: caller-owned selected index or option.

### Return value

`OPSTATUS`: SUCCESS if the operation completes; invalid frame/lifecycle, invalid range or incompatible input returns the corresponding status.

### Remarks

Pixel GUI widgets draw onto a Surface in an explicit Begin/End frame; Window presentation is a separate step.

### Example

```c
/* Usage fragment: context, geometry and widget structs are initialized by the caller. */
bool expanded = false;
size_t selected = 0;
OPSTATUS status = GUI_FUNC(TreeNode)(&context, region, "OK", &expanded, &selected);
```

# TreeNodeEx

Draws a tree node using a struct.

### Syntax

#### Macro form

```c
OPSTATUS GUI_FUNC(TreeNodeEx)(GUI_TYPE(TContext) * context, GUI_TYPE(TTreeNode) * node);
```

#### Direct form

```c
OPSTATUS GUI_TreeNodeEx(GUI_TContext * context, GUI_TTreeNode * node);
```

### Parameters

- `context`: UI or GUI context, initialized and inside its frame lifecycle as required.
- `node`: tree-node widget configuration.

### Return value

`OPSTATUS`: SUCCESS if the operation completes; invalid frame/lifecycle, invalid range or incompatible input returns the corresponding status.

### Remarks

Pixel GUI widgets draw onto a Surface in an explicit Begin/End frame; Window presentation is a separate step. The struct configuration form shares the same interaction implementation as the direct form.

### Example

```c
/* Usage fragment: context, geometry and widget structs are initialized by the caller. */

OPSTATUS status = GUI_FUNC(TreeNodeEx)(&context, &node);
```

# Table

Draws tabular entries and updates selected row.

### Syntax

#### Macro form

```c
OPSTATUS GUI_FUNC(Table)(GUI_TYPE(TContext) * context, TQUAD_TYPE(float) region, const char *const *cells, size_t rows, size_t columns, size_t *selectedRow);
```

#### Direct form

```c
OPSTATUS GUI_Table(GUI_TContext * context, TQuad_float region, const char *const *cells, size_t rows, size_t columns, size_t *selectedRow);
```

### Parameters

- `context`: UI or GUI context, initialized and inside its frame lifecycle as required.
- `region`: rectangle relative to the current Canvas/Surface or layout.
- `cells`: flat array of table cell strings.
- `rows`: number of table rows.
- `columns`: number of table columns.
- `selectedRow`: receives selected row.

### Return value

`OPSTATUS`: SUCCESS if the operation completes; invalid frame/lifecycle, invalid range or incompatible input returns the corresponding status.

### Remarks

Pixel GUI widgets draw onto a Surface in an explicit Begin/End frame; Window presentation is a separate step.

### Example

```c
/* Usage fragment: context, geometry and widget structs are initialized by the caller. */
const char *cells[] = {"A", "B"};
size_t selectedRow = 0;
OPSTATUS status = GUI_FUNC(Table)(&context, region, cells, 1, 2, &selectedRow);
```

# TableEx

Draws a table from configuration.

### Syntax

#### Macro form

```c
OPSTATUS GUI_FUNC(TableEx)(GUI_TYPE(TContext) * context, GUI_TYPE(TTable) * table);
```

#### Direct form

```c
OPSTATUS GUI_TableEx(GUI_TContext * context, GUI_TTable * table);
```

### Parameters

- `context`: UI or GUI context, initialized and inside its frame lifecycle as required.
- `table`: table widget configuration.

### Return value

`OPSTATUS`: SUCCESS if the operation completes; invalid frame/lifecycle, invalid range or incompatible input returns the corresponding status.

### Remarks

Pixel GUI widgets draw onto a Surface in an explicit Begin/End frame; Window presentation is a separate step. The struct configuration form shares the same interaction implementation as the direct form.

### Example

```c
/* Usage fragment: context, geometry and widget structs are initialized by the caller. */

OPSTATUS status = GUI_FUNC(TableEx)(&context, &table);
```

# ColorPicker

Draws a color picker that changes a caller-owned ARGB color.

### Syntax

#### Macro form

```c
OPSTATUS GUI_FUNC(ColorPicker)(GUI_TYPE(TContext) * context, TQUAD_TYPE(float) region, uint32_t * color);
```

#### Direct form

```c
OPSTATUS GUI_ColorPicker(GUI_TContext * context, TQuad_float region, uint32_t * color);
```

### Parameters

- `context`: UI or GUI context, initialized and inside its frame lifecycle as required.
- `region`: rectangle relative to the current Canvas/Surface or layout.
- `color`: ARGB drawing color or pointer to editable color.

### Return value

`OPSTATUS`: SUCCESS if the operation completes; invalid frame/lifecycle, invalid range or incompatible input returns the corresponding status.

### Remarks

Pixel GUI widgets draw onto a Surface in an explicit Begin/End frame; Window presentation is a separate step.

### Example

```c
/* Usage fragment: context, geometry and widget structs are initialized by the caller. */
uint32_t color = UINT32_C(0xFF3399CC);
OPSTATUS status = GUI_FUNC(ColorPicker)(&context, region, &color);
```

# ColorPickerEx

Draws the configured color picker.

### Syntax

#### Macro form

```c
OPSTATUS GUI_FUNC(ColorPickerEx)(GUI_TYPE(TContext) * context, GUI_TYPE(TColorPicker) * picker);
```

#### Direct form

```c
OPSTATUS GUI_ColorPickerEx(GUI_TContext * context, GUI_TColorPicker * picker);
```

### Parameters

- `context`: UI or GUI context, initialized and inside its frame lifecycle as required.
- `picker`: color picker descriptor.

### Return value

`OPSTATUS`: SUCCESS if the operation completes; invalid frame/lifecycle, invalid range or incompatible input returns the corresponding status.

### Remarks

Pixel GUI widgets draw onto a Surface in an explicit Begin/End frame; Window presentation is a separate step. The struct configuration form shares the same interaction implementation as the direct form.

### Example

```c
/* Usage fragment: context, geometry and widget structs are initialized by the caller. */

OPSTATUS status = GUI_FUNC(ColorPickerEx)(&context, &picker);
```

# Number

Draws a floating-point numeric adjustment widget.

### Syntax

#### Macro form

```c
OPSTATUS GUI_FUNC(Number)(GUI_TYPE(TContext) * context, TQUAD_TYPE(float) region, float minimum, float maximum, float step, float *value);
```

#### Direct form

```c
OPSTATUS GUI_Number(GUI_TContext * context, TQuad_float region, float minimum, float maximum, float step, float *value);
```

### Parameters

- `context`: UI or GUI context, initialized and inside its frame lifecycle as required.
- `region`: rectangle relative to the current Canvas/Surface or layout.
- `minimum`: lower value bound.
- `maximum`: upper value bound.
- `step`: increment step for numeric input.
- `value`: widget value to show or change.

### Return value

`OPSTATUS`: SUCCESS if the operation completes; invalid frame/lifecycle, invalid range or incompatible input returns the corresponding status.

### Remarks

Pixel GUI widgets draw onto a Surface in an explicit Begin/End frame; Window presentation is a separate step.

### Example

```c
/* Usage fragment: context, geometry and widget structs are initialized by the caller. */
float value = 0.5f;
OPSTATUS status = GUI_FUNC(Number)(&context, region, 0, 100, 1.0f, &value);
```

# NumberEx

Draws a numeric editor configured by struct.

### Syntax

#### Macro form

```c
OPSTATUS GUI_FUNC(NumberEx)(GUI_TYPE(TContext) * context, GUI_TYPE(TNumber) * number);
```

#### Direct form

```c
OPSTATUS GUI_NumberEx(GUI_TContext * context, GUI_TNumber * number);
```

### Parameters

- `context`: UI or GUI context, initialized and inside its frame lifecycle as required.
- `number`: numeric editor descriptor.

### Return value

`OPSTATUS`: SUCCESS if the operation completes; invalid frame/lifecycle, invalid range or incompatible input returns the corresponding status.

### Remarks

Pixel GUI widgets draw onto a Surface in an explicit Begin/End frame; Window presentation is a separate step. The struct configuration form shares the same interaction implementation as the direct form.

### Example

```c
/* Usage fragment: context, geometry and widget structs are initialized by the caller. */

OPSTATUS status = GUI_FUNC(NumberEx)(&context, &number);
```

# Scrollbar

Draws a scrollbar and modifies its offset.

### Syntax

#### Macro form

```c
OPSTATUS GUI_FUNC(Scrollbar)(GUI_TYPE(TContext) * context, TQUAD_TYPE(float) region, float maximum, float visible, float *offset, bool vertical);
```

#### Direct form

```c
OPSTATUS GUI_Scrollbar(GUI_TContext * context, TQuad_float region, float maximum, float visible, float *offset, bool vertical);
```

### Parameters

- `context`: UI or GUI context, initialized and inside its frame lifecycle as required.
- `region`: rectangle relative to the current Canvas/Surface or layout.
- `maximum`: upper value bound.
- `visible`: controls visibility of toast or scrollbar content.
- `offset`: editable scroll offset.
- `vertical`: whether scrollbar is vertical.

### Return value

`OPSTATUS`: SUCCESS if the operation completes; invalid frame/lifecycle, invalid range or incompatible input returns the corresponding status.

### Remarks

Pixel GUI widgets draw onto a Surface in an explicit Begin/End frame; Window presentation is a separate step.

### Example

```c
/* Usage fragment: context, geometry and widget structs are initialized by the caller. */
float offset = 0.0f;
OPSTATUS status = GUI_FUNC(Scrollbar)(&context, region, 100, true, &offset, true);
```

# ScrollbarEx

Draws a scrollbar from its configuration.

### Syntax

#### Macro form

```c
OPSTATUS GUI_FUNC(ScrollbarEx)(GUI_TYPE(TContext) * context, GUI_TYPE(TScrollbar) * scrollbar);
```

#### Direct form

```c
OPSTATUS GUI_ScrollbarEx(GUI_TContext * context, GUI_TScrollbar * scrollbar);
```

### Parameters

- `context`: UI or GUI context, initialized and inside its frame lifecycle as required.
- `scrollbar`: scrollbar widget configuration.

### Return value

`OPSTATUS`: SUCCESS if the operation completes; invalid frame/lifecycle, invalid range or incompatible input returns the corresponding status.

### Remarks

Pixel GUI widgets draw onto a Surface in an explicit Begin/End frame; Window presentation is a separate step. The struct configuration form shares the same interaction implementation as the direct form.

### Example

```c
/* Usage fragment: context, geometry and widget structs are initialized by the caller. */

OPSTATUS status = GUI_FUNC(ScrollbarEx)(&context, &scrollbar);
```

# Integration and requirements

- C11, header-only; the library's namespace and status patterns are retained.
- `Begin` and `End` establish the UI frame; widget values and arrays remain owned by the caller.
- The direct functions and `*Ex` struct wrappers share interaction semantics.
- See [Canvas](Canvas.md), [Terminal](Terminal.md), [Surface](Surface.md), [Window](Window.md), [Input](Input.md) and [Bit](Bit.md).
