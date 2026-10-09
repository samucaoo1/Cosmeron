# Input

General device input with direct state queries, a bounded event queue, controller discovery and a standardized gamepad view. Terminal input remains independent: this module never reads stdin or parses terminal escape sequences.

```c
#include "Cosmeron/Modules/Input/Input.h"
```

## Overview

There is no public `Init`, `Free`, context object, or required implementation file on GCC, Clang and MSVC. `Input_Update()` prepares native resources automatically and samples/drains input. Queries read its snapshot and do not acquire resources. Call Update and all other operations on one thread, normally the main thread. The module is not thread-safe.

```c
OPSTATUS status = Input_Update();
if (status == STATUS_CONST(SUCCESS)) {
    if (Input_Keyboard_IsPressed(INPUT_KEY_SPACE)) {
        /* One action on the down transition. */
    }
    if (Input_Keyboard_IsDown(INPUT_KEY_W)) {
        /* Continuous action while held. */
    }
}
```

### Macro form

```c
INPUT_FUNC(Update)();
INPUT_KEYBOARD_FUNC(IsDown)(LIB_PREFIX_CONST(INPUT_KEY_W));
```

### Direct form

```c
Input_Update();
Input_Keyboard_IsDown(INPUT_KEY_W);
```

`COSMERON_NAMESPACE` changes function/type names. `COSMERON_NAMESPACE_CONST` changes constants. Use `LIB_PREFIX_CONST(INPUT_KEY_W)` for key constants in reusable code: OS headers can define unprefixed `KEY_W`, making `INPUT_CONST(KEY_W)` unsafe. All signatures have `*_PROTOTYPE` macros; no redundant implementation-generation aliases are provided.

| Package header | Operations |
| --- | --- |
| `Keyboard/Keyboard.h` | Down, pressed, released, repeated, modifiers |
| `Mouse/Mouse.h` | Buttons, position, raw delta, scroll |
| `Controller/Controller.h` | Enumeration, names, raw buttons/axes/hats, rumble |
| `Gamepad/Gamepad.h` | Standardized view using the same controller ID |
| `Event/Event.h` | Ordered events and overflow accounting |

Package headers expose the same shared implementation; include order does not create separate snapshots. Event coordinate pairs reuse `Struct_TPair_float`.

## Backend scope and limitations

This first native implementation is **device/desktop scoped, not window-focus scoped**. It does not create a visible application window, capture exclusively, manage cursor appearance, or provide text/IME input. Do not assume input stops when your application loses focus.

| Feature | Linux | Windows |
| --- | --- | --- |
| Backend | evdev (`/dev/input/event*`) | Raw Input, async state reconciliation, XInput |
| Access | Existing device-file permissions | Current interactive desktop access |
| Keyboard | Linux key-code positions; no layout/text translation | Windows virtual-key semantics |
| Mouse position | Raw relative displacement integrated from `(0,0)` | Desktop cursor coordinates from GetCursorPos |
| Mouse delta | Raw device counts | Raw relative mouse counts |
| Scroll | Legacy horizontal/vertical wheel steps | Wheel deltas divided by WHEEL_DELTA |
| Controllers | Generic evdev buttons, absolute axes and hats | XInput-compatible controllers, up to four |
| Hotplug | Rescan approximately once per wall-clock second | XInput polled on each update |
| Rumble duration | Kernel-timed, maximum 65535 ms | Stopped by subsequent Update or normal process exit |

**Linux `GetPosition` is not the desktop cursor location.** The module has no X11/Wayland window integration. Absolute pointing devices/touchpads, high-resolution wheel events, relative controller axes, a physical-vs-logical keyboard API, keypad/international key completeness and custom gamepad mapping databases are not implemented. Letters on Linux follow the Linux key-code positions; do not use them for localized text or assume parity with Windows layout-sensitive virtual keys.

On macOS/BSD and when `INPUT_NO_NATIVE` is defined, the module compiles but Update returns `NOT_SUPPORTED`. Native support is not claimed for those platforms. `INPUT_NO_NATIVE` is useful for build/test isolation, not a public event injection interface.

Windows uses a private message-only window for Raw Input. It checks for existing keyboard/mouse Raw Input registrations and returns `BUSY` instead of replacing them. This backend must own these registrations; do not register another Raw Input target later or run a separate message pump that consumes its messages. Integration with a host window's existing input loop requires a future adapter. Multiple physical keyboards/mice are aggregated; individual device identity is not exposed, and overlapping Windows raw transitions may be reconciled by the final aggregate snapshot.

No extra link libraries are required. Windows loads User32/XInput dynamically. Linux uses libc/syscalls and Linux UAPI headers. Fixed storage avoids heap allocation; resources close automatically at normal process exit and on device disconnect. Do not unload a DLL/shared object containing this state while its exit callback is registered.

## Update and state contracts

```c
OPSTATUS Input_Update(void);
```

- Clears pressed/released/repeated flags and mouse delta/scroll, then collects pending input.
- Never waits for a new input event; drains a bounded number of available native events. Initial discovery and periodic rescans perform filesystem/OS calls, so this is not a hard-real-time operation.
- Queries before the first Update return neutral values/empty enumeration.
- A successful update with no changes is normal. Linux returns `NOT_AVAILABLE` if it has no readable supported source; missing permissions are never bypassed.
- On failure, the snapshot may be partially updated. Check the status before relying on fresh data; there is no hidden LastError.
- `IsDown`: final held state. `IsPressed`: at least one down transition this update. `IsReleased`: at least one up transition. `IsRepeated`: a native repeat was observed.
- Press and release in one update set both transient flags. Repeated queries do not clear them. Consuming events does not change them.
- XInput is sampled: a press/release completed entirely between updates can be missed. An event queue cannot reconstruct transitions the backend never received.
- No automatic dead zone, response curve, or action-binding layer.

## Keyboard

```c
bool Input_Keyboard_IsDown(Input_Key key);
bool Input_Keyboard_IsPressed(Input_Key key);
bool Input_Keyboard_IsReleased(Input_Key key);
bool Input_Keyboard_IsRepeated(Input_Key key);
Input_Modifiers Input_Keyboard_GetModifiers(void);
```

Keys include A–Z, digits (`INPUT_KEY_DIGIT_0` etc.), F1–F12, navigation, punctuation, and sided modifiers. Invalid/unknown keys return false. Modifier bits represent Shift, Control, Alt and Super; lock-toggle state is not exposed. Native repeat generates `KEY_REPEAT`, not another `KEY_PRESS`.

## Mouse

```c
bool Input_Mouse_IsDown(Input_MouseButton button);
bool Input_Mouse_IsPressed(Input_MouseButton button);
bool Input_Mouse_IsReleased(Input_MouseButton button);
void Input_Mouse_GetPosition(float *x, float *y);
void Input_Mouse_GetDelta(float *x, float *y);
void Input_Mouse_GetScroll(float *x, float *y);
```

Buttons: `INPUT_MOUSE_BUTTON_LEFT`, `RIGHT`, `MIDDLE`, `SIDE`, `EXTRA`. Either coordinate output can be NULL. Position persists; delta/scroll accumulate during Update and reset at the next Update. Delta and position need not share units. See the backend table before interpreting coordinates.

## Controllers

```c
OPSTATUS Input_Controller_Enumerate(Input_ControllerId *controllers,
                                   size_t capacity, size_t *count);
bool Input_Controller_IsConnected(Input_ControllerId controller);
OPSTATUS Input_Controller_GetName(Input_ControllerId controller,
                                 char *buffer, size_t capacity,
                                 size_t *required);
uint32_t Input_Controller_GetButtonCount(Input_ControllerId controller);
uint32_t Input_Controller_GetAxisCount(Input_ControllerId controller);
uint32_t Input_Controller_GetHatCount(Input_ControllerId controller);
bool Input_Controller_HasRumble(Input_ControllerId controller);
bool Input_Controller_IsDown(Input_ControllerId controller, uint32_t button);
bool Input_Controller_IsPressed(Input_ControllerId controller, uint32_t button);
bool Input_Controller_IsReleased(Input_ControllerId controller, uint32_t button);
float Input_Controller_GetAxis(Input_ControllerId controller, uint32_t axis);
Input_Hat Input_Controller_GetHat(Input_ControllerId controller, uint32_t hat);
```

- IDs are nonzero, monotonically allocated for observed connections and never recycled. Rapid disconnect/reconnect between native observations may be indistinguishable, especially on XInput.
- Call Update before enumeration. Enumeration and GetName accept NULL storage with zero capacity to query required size. Name size includes the terminating NUL.
- On `INSUFFICIENT_SPACE`, the required count/size is produced, while the destination buffer is untouched. This is an explicit exception to the usual output-only-on-success contract.
- Invalid pointers return `INVALID_ARGUMENT`; GetName with a stale ID returns `NOT_FOUND`.
- Direct state/count queries intentionally return neutral values for stale IDs or invalid indices, following the compact API contract. IsConnected distinguishes a disconnected device from idle state; count queries validate indices.
- Generic axes use `[-1,1]`; even unidirectional triggers occupy that interval in Controller. Hat values combine `UP`, `DOWN`, `LEFT`, `RIGHT`; zero is `CENTERED`.
- Linux generic button/axis ordering follows ascending supported native codes. The standard-gamepad hat mapping may append four synthetic d-pad buttons. Do not persist raw indices as portable bindings.

### Rumble

```c
OPSTATUS Input_Controller_Rumble(Input_ControllerId controller,
                                float lowFrequency, float highFrequency,
                                uint32_t durationMs);
OPSTATUS Input_Controller_StopRumble(Input_ControllerId controller);
```

Intensities are finite values in `[0,1]`; invalid values (including NaN) return `INVALID_ARGUMENT`. A zero duration or two zero intensities stops the effect. A new effect replaces the previous one. Unsupported hardware returns `NOT_SUPPORTED`; stale IDs return `NOT_FOUND`; Linux durations above 65535 ms return `OUT_OF_RANGE`. Keep updating on Windows to enforce the requested stop time.

## Gamepad

```c
bool Input_Gamepad_IsMapped(Input_ControllerId controller);
bool Input_Gamepad_IsDown(Input_ControllerId controller, Input_GamepadButton button);
bool Input_Gamepad_IsPressed(Input_ControllerId controller, Input_GamepadButton button);
bool Input_Gamepad_IsReleased(Input_ControllerId controller, Input_GamepadButton button);
float Input_Gamepad_GetAxis(Input_ControllerId controller, Input_GamepadAxis axis);
```

The same controller ID is used; no second handle or duplicated event stream is created. Buttons are named by position (`SOUTH`, `EAST`, `WEST`, `NORTH`), shoulders, stick clicks, Back/Start/Guide and d-pad directions. Axes: `LEFT_X/Y`, `RIGHT_X/Y`, `LEFT_TRIGGER`, `RIGHT_TRIGGER`.

Stick components use `[-1,1]`, positive X right and Y up. Triggers use `[0,1]`. Missing elements and unmapped devices return neutral values. The Windows Guide button is not reported by standard XInput. Linux supports the kernel standard gamepad layout, not arbitrary vendor remappings; `IsMapped` indicates the recognized base layout, not availability of every optional control.

## Events

```c
bool Input_Event_Poll(Input_TEvent *event);
void Input_Event_Clear(void);
uint64_t Input_Event_GetDroppedCount(void);
```

Poll copies the oldest event and removes it; false means empty or NULL output. Clear discards queued events only, preserving states and the cumulative dropped count. Events are fixed-size values with no borrowed strings or allocation.

| Event type suffix (`INPUT_EVENT_...`) | Payload |
| --- | --- |
| `KEY_PRESS`, `KEY_RELEASE`, `KEY_REPEAT` | `data.key` |
| `MOUSE_BUTTON_PRESS`, `MOUSE_BUTTON_RELEASE` | `data.mouseButton` |
| `MOUSE_MOVE`, `MOUSE_SCROLL` | `data.motion.x/y` (delta) |
| `CONTROLLER_BUTTON_PRESS`, `CONTROLLER_BUTTON_RELEASE` | `data.button` |
| `CONTROLLER_AXIS` | `data.axis.index/value` (generic units) |
| `CONTROLLER_HAT` | `data.hat.index/value` |
| `DEVICE_CONNECTED`, `DEVICE_DISCONNECTED`, `DEVICE_RESET` | `controller` |

`controller` is zero for keyboard/mouse events; sequence is monotonically increasing processing order, not a physical timestamp. Disconnect events invalidate the ID and future queries return neutral values. Linux SYN_DROPPED handling ignores packets until SYN_REPORT, resynchronizes through ioctls and emits a reset; an unsuccessful resync closes the source.

The queue retains older events and drops new ones when full. State updates still apply. `GetDroppedCount` counts discarded queued events plus detected native loss incidents; native loss incidents have unknown event counts. Clearing the queue does not reset it.

## Configuration and multiple translation units

- `INPUT_EVENT_CAPACITY`: queued event slots, default 256.
- `INPUT_DEVICE_CAPACITY`: native source slots, default 32; on Linux keyboards/mice also consume slots.
- Fixed per-controller limits: 128 buttons, 64 absolute axes, 4 hats.
- All translation units must use identical Input capacity, namespace and backend settings.
- GCC/Clang weak storage and MSVC selectany storage make the default state shared across translation units rather than accidentally giving each `.c` file a separate input state.
- For compilers without these extensions, define `INPUT_EXTERNAL_STORAGE` in all translation units and `INPUT_DEFINE_STORAGE` in exactly one. This defines storage, not a runtime lifecycle function.
- State is scoped to one linked application image; sharing across independently loaded shared libraries is not promised.

## Validation

`make -C Codespace/Tests/Input run` checks state transitions, repeat, overflow, invalid arguments, controller enumeration, stale IDs, gamepad mapping, namespace isolation and shared state across multiple translation units. Linux parser tests feed native-format events through a pipe, including loss recovery, without opening real devices. Native header compilation is also covered. Physical-device behavior and rumble require hardware validation.

Backend references: [Linux evdev](https://www.kernel.org/doc/html/latest/input/input_uapi.html), [Linux force feedback](https://www.kernel.org/doc/html/latest/input/ff.html), [Raw Input registration](https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-registerrawinputdevices), [XInputGetState](https://learn.microsoft.com/en-us/windows/win32/api/xinput/nf-xinput-xinputgetstate).
