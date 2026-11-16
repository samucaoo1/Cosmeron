# Window — explicit GUI presentation

Header: `Codespace/Cosmeron/Modules/Graphics/Window/Window.h`.
Namespaces: `WINDOW_TYPE`, `WINDOW_FUNC`, `WINDOW_CONST`.

`Window_Open` creates a native window. `Window_OpenWith` uses supplied host
callbacks instead. Both require a zero-initialized output with no live resource.
The window owns its native backend; host adapter user data is borrowed.

| Function | Contract |
|---|---|
| Open / OpenWith | Create a native window or open a host adapter |
| Poll | Produce INPUT, RESIZE or CLOSE; WOULD_BLOCK leaves the output unchanged |
| Update | Present a valid surface whose dimensions match the current client size |
| GetSize | Produce the current client dimensions |
| IsOpen | Report whether a close event has been received |
| Close | Release resources and clear the struct; safe to repeat |

A close event changes IsOpen, but the application still calls Close to release
resources. On RESIZE, resize or replace the surface outside the GUI frame.
Native input positions are client-local surface coordinates. Forward only the
embedded Input event to GUI_Event; window resize/close remain application events.

| Platform | Native path |
|---|---|
| Windows | Win32 window/events, GDI presentation; User32/GDI32 loaded dynamically |
| Linux/BSD with X11 headers and runtime | Xlib window/events, TrueColor presentation; Xlib loaded dynamically |
| Wayland desktop with XWayland | X11 path through DISPLAY; these are window-local coordinates |
| Native Wayland, macOS AppKit, embedded displays | Supply a TAdapter; native backends are not bundled yet |

X11 auto-enables when Xlib/Xutil development headers are available. Define
WINDOW_DISABLE_X11 for a host-only build. Missing runtime/display returns
NOT_AVAILABLE; a build without a native backend returns NOT_SUPPORTED. The GUI
software renderer remains usable without windowing or those headers.

Native X11 requires the system's dlopen API (available without -ldl on modern
Linux and supported BSD desktops). On older Linux libc versions where dlopen
requires libdl, use a host-only build to preserve the no-extra-link profile.
Windows' additional APIs are loaded from the system DLLs.

Windows committed text comes from WM_CHAR, including UTF-16 surrogate pairs.
X11 uses XIM/Xutf8LookupString when available, with a Latin-1 fallback. The host
application owns its process locale; select a UTF-8 LC_CTYPE locale before Open
for XIM input. Full IME preedit, clipboard integration, native Wayland and AppKit
are host-adapter capabilities, not implied by the committed-text path.

Calls must stay on the UI thread that owns the native window. There is no hidden
worker thread or global default window. TAdapter has open/poll/update/close
callbacks, so an existing application can own all native resources itself.

---

# API reference

The entries below follow the [Bit.md](Bit.md) convention: direct and namespace-macro signatures, inputs, results, remarks and examples. Examples are illustrative fragments; variables not explicitly declared must already have the correct initialized state.

## Function summary

| Function | Return type | Description |
| --- | --- | --- |
| `WINDOW_FUNC(Open)` | `OPSTATUS` | Creates a native desktop window with given title and dimensions. |
| `WINDOW_FUNC(OpenWith)` | `OPSTATUS` | Uses supplied callbacks to open a host-managed window. |
| `WINDOW_FUNC(Poll)` | `OPSTATUS` | Retrieves the next input, resize or close event. |
| `WINDOW_FUNC(Update)` | `OPSTATUS` | Presents a valid pixel surface with dimensions matching client size. |
| `WINDOW_FUNC(Close)` | `void` | Closes the owned native or hosted presentation resource. |
| `WINDOW_FUNC(IsOpen)` | `bool` | Queries whether window remains open and has not received close. |
| `WINDOW_FUNC(GetSize)` | `OPSTATUS` | Obtains current client dimensions. |

# Open

Creates a native desktop window with given title and dimensions.

### Syntax

#### Macro form

```c
OPSTATUS WINDOW_FUNC(Open)(WINDOW_TYPE(TWindow) * outWindow, const char *title, TDUAL_TYPE(uint32) size);
```

#### Direct form

```c
OPSTATUS Window_Open(Window_TWindow * outWindow, const char *title, TDual_uint32 size);
```

### Parameters

- `outWindow`: zero-initialized output window.
- `title`: NUL-terminated title text.
- `size`: data length or dimensions as declared.

### Return value

Returns OPSTATUS: SUCCESS on completion, or INVALID_ARGUMENT, BUSY, NOT_SUPPORTED, NOT_AVAILABLE or native/size errors when applicable.

### Remarks

Window native events and presentation are UI-thread operations. The host adapter owns its callbacks and user data; the Window never retains borrowed surface pixels. A missing native runtime/display can return NOT_AVAILABLE or NOT_SUPPORTED.

### Example

```c
WINDOW_TYPE(TWindow) window = {0};
OPSTATUS status = WINDOW_FUNC(Open)(&window, "Cosmeron", (TDUAL_TYPE(uint32)){.x=640,.y=480});
WINDOW_FUNC(Close)(&window);
```

# OpenWith

Uses supplied callbacks to open a host-managed window.

### Syntax

#### Macro form

```c
OPSTATUS WINDOW_FUNC(OpenWith)(WINDOW_TYPE(TWindow) * outWindow, const char *title, TDUAL_TYPE(uint32) size, const WINDOW_TYPE(TAdapter) * adapter);
```

#### Direct form

```c
OPSTATUS Window_OpenWith(Window_TWindow * outWindow, const char *title, TDual_uint32 size, const Window_TAdapter * adapter);
```

### Parameters

- `outWindow`: zero-initialized output window.
- `title`: NUL-terminated title text.
- `size`: data length or dimensions as declared.
- `adapter`: host callback table, supplied by the caller.

### Return value

Returns OPSTATUS: SUCCESS on completion, or INVALID_ARGUMENT, BUSY, NOT_SUPPORTED, NOT_AVAILABLE or native/size errors when applicable.

### Remarks

Window native events and presentation are UI-thread operations. The host adapter owns its callbacks and user data; the Window never retains borrowed surface pixels.

### Example

```c
OPSTATUS status = WINDOW_FUNC(OpenWith)(&window, "Cosmeron", (TDUAL_TYPE(uint32)){.x=640,.y=480}, &adapter);
```

# Poll

Retrieves the next input, resize or close event.

### Syntax

#### Macro form

```c
OPSTATUS WINDOW_FUNC(Poll)(WINDOW_TYPE(TWindow) * window, WINDOW_TYPE(TEvent) * outEvent);
```

#### Direct form

```c
OPSTATUS Window_Poll(Window_TWindow * window, Window_TEvent * outEvent);
```

### Parameters

- `window`: previously opened or initialized window.
- `outEvent`: output for next event.

### Return value

Returns OPSTATUS: SUCCESS on completion, or INVALID_ARGUMENT, BUSY, NOT_SUPPORTED, NOT_AVAILABLE or native/size errors when applicable.

### Remarks

Window native events and presentation are UI-thread operations. The host adapter owns its callbacks and user data; the Window never retains borrowed surface pixels. On WOULD_BLOCK, outEvent is unchanged.

### Example

```c
WINDOW_TYPE(TEvent) event = {0};
OPSTATUS status = WINDOW_FUNC(Poll)(&window, &event);
```

# Update

Presents a valid pixel surface with dimensions matching client size.

### Syntax

#### Macro form

```c
OPSTATUS WINDOW_FUNC(Update)(WINDOW_TYPE(TWindow) * window, const GRAPHICS_TYPE(TSurface) * surface);
```

#### Direct form

```c
OPSTATUS Window_Update(Window_TWindow * window, const Graphics_TSurface * surface);
```

### Parameters

- `window`: previously opened or initialized window.
- `surface`: surface whose pixels may be owned or borrowed.

### Return value

Returns OPSTATUS: SUCCESS on completion, or INVALID_ARGUMENT, BUSY, NOT_SUPPORTED, NOT_AVAILABLE or native/size errors when applicable.

### Remarks

Window native events and presentation are UI-thread operations. The host adapter owns its callbacks and user data; the Window never retains borrowed surface pixels.

### Example

```c
OPSTATUS status = WINDOW_FUNC(Update)(&window, &surface);
```

# Close

Closes the owned native or hosted presentation resource.

### Syntax

#### Macro form

```c
void WINDOW_FUNC(Close)(WINDOW_TYPE(TWindow) * window);
```

#### Direct form

```c
void Window_Close(Window_TWindow * window);
```

### Parameters

- `window`: previously opened or initialized window.

### Return value

Does not return a value.

### Remarks

Window native events and presentation are UI-thread operations. The host adapter owns its callbacks and user data; the Window never retains borrowed surface pixels.

### Example

```c
WINDOW_FUNC(Close)(&window);
```

# IsOpen

Queries whether window remains open and has not received close.

### Syntax

#### Macro form

```c
bool WINDOW_FUNC(IsOpen)(const WINDOW_TYPE(TWindow) * window);
```

#### Direct form

```c
bool Window_IsOpen(const Window_TWindow * window);
```

### Parameters

- `window`: previously opened or initialized window.

### Return value

Returns `bool` by value.

### Remarks

Window native events and presentation are UI-thread operations. The host adapter owns its callbacks and user data; the Window never retains borrowed surface pixels.

### Example

```c
bool open = WINDOW_FUNC(IsOpen)(&window);
```

# GetSize

Obtains current client dimensions.

### Syntax

#### Macro form

```c
OPSTATUS WINDOW_FUNC(GetSize)(const WINDOW_TYPE(TWindow) * window, TDUAL_TYPE(uint32) * outSize);
```

#### Direct form

```c
OPSTATUS Window_GetSize(const Window_TWindow * window, TDual_uint32 * outSize);
```

### Parameters

- `window`: previously opened or initialized window.
- `outSize`: output dimensions in cells or pixels depending on module.

### Return value

Returns OPSTATUS: SUCCESS on completion, or INVALID_ARGUMENT, BUSY, NOT_SUPPORTED, NOT_AVAILABLE or native/size errors when applicable.

### Remarks

Window native events and presentation are UI-thread operations. The host adapter owns its callbacks and user data; the Window never retains borrowed surface pixels.

### Example

```c
TDUAL_TYPE(uint32) size = {0};
OPSTATUS status = WINDOW_FUNC(GetSize)(&window, &size);
```

