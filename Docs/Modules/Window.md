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
