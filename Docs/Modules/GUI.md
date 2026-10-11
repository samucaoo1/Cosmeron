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
