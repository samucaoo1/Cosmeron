# Input

General device input with direct state queries, a bounded event queue, controller discovery and a standardized gamepad view. Terminal input remains independent: this module never reads stdin or parses terminal escape sequences.

```c
#include "Cosmeron/Modules/Input/Input.h"
```

[Overview](#overview) · [Backend support](#backend-scope-and-limitations) · [Function summary](#function-summary) · [Configuration](#configuration-and-multiple-translation-units)

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

Each package header declares its own API through prototype macros and includes its corresponding `Impl/<Package>.impl`. `Input.h` owns Update and aggregates all five package headers. Internal state and native backends remain shared; include order does not create separate snapshots. Event coordinate pairs reuse `Struct_TPair_float`.

## Public types

| Type | Representation and purpose |
| --- | --- |
| `Input_Key` | Enum identifying a supported keyboard key. |
| `Input_Modifiers` | `uint32_t` mask of held modifier groups. |
| `Input_MouseButton` | Enum identifying one of five mouse buttons. |
| `Input_ControllerId` | `uint64_t` connection identity; zero is invalid. |
| `Input_Hat` | Enum-valued direction mask; diagonals combine bits. |
| `Input_GamepadButton` | Enum identifying a standardized button position. |
| `Input_GamepadAxis` | Enum identifying a standardized analog component. |
| `Input_EventType` | Discriminant selecting the valid event union member. |
| `Input_TEvent` | Copyable event value: type, controller ID, sequence and payload union. |

These are identifiers and one event value, not separate owning objects. No public button-state or axis-state structures are required. Enum `COUNT` entries are bounds, not valid controls. `INPUT_KEY_UNKNOWN` is a neutral sentinel.

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

---

# API reference

## Function summary

| Function | Description | Modifies state |
| --- | --- | --- |
| [Update](#update) | Collects native input and publishes a new snapshot. | Yes |
| [Keyboard_IsDown](#keyboard_isdown) | Tests whether the input is currently held. | No |
| [Keyboard_IsPressed](#keyboard_ispressed) | Tests whether a down transition was observed during the latest update. | No |
| [Keyboard_IsReleased](#keyboard_isreleased) | Tests whether an up transition was observed during the latest update. | No |
| [Keyboard_IsRepeated](#keyboard_isrepeated) | Tests whether native keyboard repetition was observed during the latest update. | No |
| [Keyboard_GetModifiers](#keyboard_getmodifiers) | Returns the combined held modifier mask. | No |
| [Mouse_IsDown](#mouse_isdown) | Tests whether the input is currently held. | No |
| [Mouse_IsPressed](#mouse_ispressed) | Tests whether a down transition was observed during the latest update. | No |
| [Mouse_IsReleased](#mouse_isreleased) | Tests whether an up transition was observed during the latest update. | No |
| [Mouse_GetPosition](#mouse_getposition) | Copies the persistent mouse position into optional outputs. | No |
| [Mouse_GetDelta](#mouse_getdelta) | Copies accumulated raw relative movement for the latest update. | No |
| [Mouse_GetScroll](#mouse_getscroll) | Copies accumulated horizontal and vertical wheel movement. | No |
| [Controller_Enumerate](#controller_enumerate) | Copies IDs for currently observed connected controllers. | No |
| [Controller_IsConnected](#controller_isconnected) | Checks whether an ID still refers to an observed connected controller. | No |
| [Controller_GetName](#controller_getname) | Copies the device name into caller-provided storage. | No |
| [Controller_GetButtonCount](#controller_getbuttoncount) | Returns the number of generic button controls. | No |
| [Controller_GetAxisCount](#controller_getaxiscount) | Returns the number of generic axis controls. | No |
| [Controller_GetHatCount](#controller_gethatcount) | Returns the number of generic hat controls. | No |
| [Controller_HasRumble](#controller_hasrumble) | Checks whether this connection supports the implemented rumble operation. | No |
| [Controller_IsDown](#controller_isdown) | Tests whether the input is currently held. | No |
| [Controller_IsPressed](#controller_ispressed) | Tests whether a down transition was observed during the latest update. | No |
| [Controller_IsReleased](#controller_isreleased) | Tests whether an up transition was observed during the latest update. | No |
| [Controller_GetAxis](#controller_getaxis) | Reads a generic absolute controller axis. | No |
| [Controller_GetHat](#controller_gethat) | Reads a generic directional hat. | No |
| [Controller_Rumble](#controller_rumble) | Starts or replaces a simple two-motor vibration effect. | Yes |
| [Controller_StopRumble](#controller_stoprumble) | Stops the current simple vibration effect. | Yes |
| [Gamepad_IsMapped](#gamepad_ismapped) | Checks for a recognized standardized gamepad layout. | No |
| [Gamepad_IsDown](#gamepad_isdown) | Tests whether the input is currently held. | No |
| [Gamepad_IsPressed](#gamepad_ispressed) | Tests whether a down transition was observed during the latest update. | No |
| [Gamepad_IsReleased](#gamepad_isreleased) | Tests whether an up transition was observed during the latest update. | No |
| [Gamepad_GetAxis](#gamepad_getaxis) | Reads a standardized analog stick component or trigger. | No |
| [Event_Poll](#event_poll) | Copies and removes the oldest queued event. | Yes |
| [Event_Clear](#event_clear) | Discards all currently queued events. | Yes |
| [Event_GetDroppedCount](#event_getdroppedcount) | Returns cumulative event-loss accounting. | No |

---

# Update

Collects native input and publishes a new snapshot.

### Syntax

#### Macro form

```c
INPUT_FUNC(Update)();
```

#### Direct form

```c
OPSTATUS Input_Update(void);
```

### Parameters

None.

### Return value

`SUCCESS` on collection; `NOT_AVAILABLE` when native access is unavailable, `NOT_SUPPORTED` without a backend, `BUSY` for conflicting Windows Raw Input registrations. Linux exit-handler registration can return `OUT_OF_MEMORY`; exhausted Windows connection IDs return `OUT_OF_RANGE`.

### Remarks

No public Init/Free is needed. It clears transient flags before collection; errors can leave a partial snapshot. Call from the same thread throughout the process. Device discovery is bounded by configured storage, not a hard-real-time guarantee.

### Example

```c
OPSTATUS value = Input_Update();
(void)value;
```

---

# Keyboard_IsDown

Tests whether the input is currently held.

### Syntax

#### Macro form

```c
INPUT_KEYBOARD_FUNC(IsDown)(key);
```

#### Direct form

```c
bool Input_Keyboard_IsDown(Input_Key key);
```

### Parameters

| Parameter | Explanation |
| --- | --- |
| `key` | Key identifier, for example `INPUT_KEY_SPACE`. Unknown and out-of-domain values return false. |

### Return value

True when the condition holds; false for idle, invalid or unavailable input.

### Remarks

Reading the flag does not consume it. Update renews transient flags. A press followed by a release in one update can set both IsPressed and IsReleased.

### Example

```c
bool value = Input_Keyboard_IsDown(INPUT_KEY_SPACE);
(void)value;
```

---

# Keyboard_IsPressed

Tests whether a down transition was observed during the latest update.

### Syntax

#### Macro form

```c
INPUT_KEYBOARD_FUNC(IsPressed)(key);
```

#### Direct form

```c
bool Input_Keyboard_IsPressed(Input_Key key);
```

### Parameters

| Parameter | Explanation |
| --- | --- |
| `key` | Key identifier, for example `INPUT_KEY_SPACE`. Unknown and out-of-domain values return false. |

### Return value

True when the condition holds; false for idle, invalid or unavailable input.

### Remarks

Reading the flag does not consume it. Update renews transient flags. A press followed by a release in one update can set both IsPressed and IsReleased.

### Example

```c
bool value = Input_Keyboard_IsPressed(INPUT_KEY_SPACE);
(void)value;
```

---

# Keyboard_IsReleased

Tests whether an up transition was observed during the latest update.

### Syntax

#### Macro form

```c
INPUT_KEYBOARD_FUNC(IsReleased)(key);
```

#### Direct form

```c
bool Input_Keyboard_IsReleased(Input_Key key);
```

### Parameters

| Parameter | Explanation |
| --- | --- |
| `key` | Key identifier, for example `INPUT_KEY_SPACE`. Unknown and out-of-domain values return false. |

### Return value

True when the condition holds; false for idle, invalid or unavailable input.

### Remarks

Reading the flag does not consume it. Update renews transient flags. A press followed by a release in one update can set both IsPressed and IsReleased.

### Example

```c
bool value = Input_Keyboard_IsReleased(INPUT_KEY_SPACE);
(void)value;
```

---

# Keyboard_IsRepeated

Tests whether native keyboard repetition was observed during the latest update.

### Syntax

#### Macro form

```c
INPUT_KEYBOARD_FUNC(IsRepeated)(key);
```

#### Direct form

```c
bool Input_Keyboard_IsRepeated(Input_Key key);
```

### Parameters

| Parameter | Explanation |
| --- | --- |
| `key` | Key identifier, for example `INPUT_KEY_SPACE`. Unknown and out-of-domain values return false. |

### Return value

True when the condition holds; false for idle, invalid or unavailable input.

### Remarks

Reading the flag does not consume it. Update renews transient flags. A press followed by a release in one update can set both IsPressed and IsReleased.

### Example

```c
bool value = Input_Keyboard_IsRepeated(INPUT_KEY_SPACE);
(void)value;
```

---

# Keyboard_GetModifiers

Returns the combined held modifier mask.

### Syntax

#### Macro form

```c
INPUT_KEYBOARD_FUNC(GetModifiers)();
```

#### Direct form

```c
Input_Modifiers Input_Keyboard_GetModifiers(void);
```

### Parameters

None.

### Return value

Bitwise combination of `INPUT_MODIFIER_SHIFT`, `CONTROL`, `ALT`, and `SUPER`; zero if none are held.

### Remarks

Either side sets the corresponding aggregate bit. This does not expose Caps Lock, Num Lock or Scroll Lock toggle state.

### Example

```c
Input_Modifiers value = Input_Keyboard_GetModifiers();
(void)value;
```

---

# Mouse_IsDown

Tests whether the input is currently held.

### Syntax

#### Macro form

```c
INPUT_MOUSE_FUNC(IsDown)(button);
```

#### Direct form

```c
bool Input_Mouse_IsDown(Input_MouseButton button);
```

### Parameters

| Parameter | Explanation |
| --- | --- |
| `button` | Mouse button enum, such as `INPUT_MOUSE_BUTTON_LEFT`. |

### Return value

True when the condition holds; false for idle, invalid or unavailable input.

### Remarks

Reading the flag does not consume it. Update renews transient flags. A press followed by a release in one update can set both IsPressed and IsReleased.

### Example

```c
bool value = Input_Mouse_IsDown(INPUT_MOUSE_BUTTON_LEFT);
(void)value;
```

---

# Mouse_IsPressed

Tests whether a down transition was observed during the latest update.

### Syntax

#### Macro form

```c
INPUT_MOUSE_FUNC(IsPressed)(button);
```

#### Direct form

```c
bool Input_Mouse_IsPressed(Input_MouseButton button);
```

### Parameters

| Parameter | Explanation |
| --- | --- |
| `button` | Mouse button enum, such as `INPUT_MOUSE_BUTTON_LEFT`. |

### Return value

True when the condition holds; false for idle, invalid or unavailable input.

### Remarks

Reading the flag does not consume it. Update renews transient flags. A press followed by a release in one update can set both IsPressed and IsReleased.

### Example

```c
bool value = Input_Mouse_IsPressed(INPUT_MOUSE_BUTTON_LEFT);
(void)value;
```

---

# Mouse_IsReleased

Tests whether an up transition was observed during the latest update.

### Syntax

#### Macro form

```c
INPUT_MOUSE_FUNC(IsReleased)(button);
```

#### Direct form

```c
bool Input_Mouse_IsReleased(Input_MouseButton button);
```

### Parameters

| Parameter | Explanation |
| --- | --- |
| `button` | Mouse button enum, such as `INPUT_MOUSE_BUTTON_LEFT`. |

### Return value

True when the condition holds; false for idle, invalid or unavailable input.

### Remarks

Reading the flag does not consume it. Update renews transient flags. A press followed by a release in one update can set both IsPressed and IsReleased.

### Example

```c
bool value = Input_Mouse_IsReleased(INPUT_MOUSE_BUTTON_LEFT);
(void)value;
```

---

# Mouse_GetPosition

Copies the persistent mouse position into optional outputs.

### Syntax

#### Macro form

```c
INPUT_MOUSE_FUNC(GetPosition)(x, y);
```

#### Direct form

```c
void Input_Mouse_GetPosition(float *x, float *y);
```

### Parameters

| Parameter | Explanation |
| --- | --- |
| `x` | Optional horizontal output; NULL skips this component. |
| `y` | Optional vertical output; NULL skips this component. |

### Return value

None; each non-NULL output receives its component.

### Remarks

Windows reports desktop coordinates. Linux reports integrated raw displacement from zero, not the desktop cursor. This function does not consume delta or update native input.

### Example

```c
float x = 0, y = 0;
Input_Mouse_GetPosition(&x, &y);
```

---

# Mouse_GetDelta

Copies accumulated raw relative movement for the latest update.

### Syntax

#### Macro form

```c
INPUT_MOUSE_FUNC(GetDelta)(x, y);
```

#### Direct form

```c
void Input_Mouse_GetDelta(float *x, float *y);
```

### Parameters

| Parameter | Explanation |
| --- | --- |
| `x` | Optional horizontal output; NULL skips this component. |
| `y` | Optional vertical output; NULL skips this component. |

### Return value

None; each non-NULL output receives its component.

### Remarks

Values are device counts, not necessarily pixels. Calling twice returns the same values. The next Update clears the accumulation. Windows absolute mouse packets do not produce relative delta.

### Example

```c
float x = 0, y = 0;
Input_Mouse_GetDelta(&x, &y);
```

---

# Mouse_GetScroll

Copies accumulated horizontal and vertical wheel movement.

### Syntax

#### Macro form

```c
INPUT_MOUSE_FUNC(GetScroll)(x, y);
```

#### Direct form

```c
void Input_Mouse_GetScroll(float *x, float *y);
```

### Parameters

| Parameter | Explanation |
| --- | --- |
| `x` | Optional horizontal output; NULL skips this component. |
| `y` | Optional vertical output; NULL skips this component. |

### Return value

None; each non-NULL output receives its component.

### Remarks

Values use wheel-step units. Fractional Windows wheel steps are preserved. Linux high-resolution wheel events are not decoded. The next Update clears the accumulation.

### Example

```c
float x = 0, y = 0;
Input_Mouse_GetScroll(&x, &y);
```

---

# Controller_Enumerate

Copies IDs for currently observed connected controllers.

### Syntax

#### Macro form

```c
INPUT_CONTROLLER_FUNC(Enumerate)(controllers, capacity, count);
```

#### Direct form

```c
OPSTATUS Input_Controller_Enumerate(Input_ControllerId *controllers, size_t capacity, size_t *count);
```

### Parameters

| Parameter | Explanation |
| --- | --- |
| `controllers` | Destination array of controller IDs; NULL is allowed only when capacity is zero. |
| `capacity` | Destination capacity: elements for Enumerate, bytes for GetName. |
| `count` | Required non-NULL output for the connected-controller count. |

### Return value

`SUCCESS`; `INVALID_ARGUMENT` for NULL count or NULL controllers with nonzero capacity; `INSUFFICIENT_SPACE` for a small destination.

### Remarks

NULL plus zero capacity queries the required count. Count is also written on INSUFFICIENT_SPACE, while the array is untouched. On other failures outputs remain untouched. Enumeration does not discover new devices; call Update first.

### Example

```c
Input_ControllerId controllers[INPUT_DEVICE_CAPACITY];
size_t count = 0;
OPSTATUS value = Input_Controller_Enumerate(controllers, INPUT_DEVICE_CAPACITY, &count);
(void)value;
```

---

# Controller_IsConnected

Checks whether an ID still refers to an observed connected controller.

### Syntax

#### Macro form

```c
INPUT_CONTROLLER_FUNC(IsConnected)(controller);
```

#### Direct form

```c
bool Input_Controller_IsConnected(Input_ControllerId controller);
```

### Parameters

| Parameter | Explanation |
| --- | --- |
| `controller` | ID obtained from the latest successful controller enumeration; zero is invalid. |

### Return value

True for a live ID; false for zero, stale or unknown IDs.

### Remarks

Reads the most recent snapshot. Physical removal is reflected after the backend observes it. Reconnection allocates a new ID when disconnection was observed.

### Example

```c
/* controller is an ID obtained from Enumerate. */
bool value = Input_Controller_IsConnected(controller);
(void)value;
```

---

# Controller_GetName

Copies the device name into caller-provided storage.

### Syntax

#### Macro form

```c
INPUT_CONTROLLER_FUNC(GetName)(controller, buffer, capacity, required);
```

#### Direct form

```c
OPSTATUS Input_Controller_GetName(Input_ControllerId controller, char *buffer, size_t capacity, size_t *required);
```

### Parameters

| Parameter | Explanation |
| --- | --- |
| `controller` | ID obtained from the latest successful controller enumeration; zero is invalid. |
| `buffer` | Destination for a NUL-terminated device name; NULL is allowed only when capacity is zero. |
| `capacity` | Destination capacity: elements for Enumerate, bytes for GetName. |
| `required` | Required non-NULL output for the name size in bytes, including NUL. |

### Return value

`SUCCESS`; `INVALID_ARGUMENT` for invalid output pointers; `NOT_FOUND` for an unknown ID; `INSUFFICIENT_SPACE` when capacity is too small.

### Remarks

Required includes NUL. Query with NULL/zero first. Required is also produced on INSUFFICIENT_SPACE; the buffer stays untouched. Other errors do not modify outputs. XInput uses a generic name; a Linux device can have an empty name if its native name query fails.

### Example

```c
/* controller is an ID obtained from Enumerate. */
char name[INPUT_NAME_CAPACITY];
size_t required = 0;
OPSTATUS value = Input_Controller_GetName(controller, name, sizeof(name), &required);
(void)value;
```

---

# Controller_GetButtonCount

Returns the number of generic button controls.

### Syntax

#### Macro form

```c
INPUT_CONTROLLER_FUNC(GetButtonCount)(controller);
```

#### Direct form

```c
uint32_t Input_Controller_GetButtonCount(Input_ControllerId controller);
```

### Parameters

| Parameter | Explanation |
| --- | --- |
| `controller` | ID obtained from the latest successful controller enumeration; zero is invalid. |

### Return value

A uint32_t count, or zero for a stale or invalid ID.

### Remarks

Valid button indices are in `[0, count)`. The count describes the current connection; re-enumerate after hotplug.

### Example

```c
/* controller is an ID obtained from Enumerate. */
uint32_t value = Input_Controller_GetButtonCount(controller);
(void)value;
```

---

# Controller_GetAxisCount

Returns the number of generic axis controls.

### Syntax

#### Macro form

```c
INPUT_CONTROLLER_FUNC(GetAxisCount)(controller);
```

#### Direct form

```c
uint32_t Input_Controller_GetAxisCount(Input_ControllerId controller);
```

### Parameters

| Parameter | Explanation |
| --- | --- |
| `controller` | ID obtained from the latest successful controller enumeration; zero is invalid. |

### Return value

A uint32_t count, or zero for a stale or invalid ID.

### Remarks

Valid axis indices are in `[0, count)`. The count describes the current connection; re-enumerate after hotplug.

### Example

```c
/* controller is an ID obtained from Enumerate. */
uint32_t value = Input_Controller_GetAxisCount(controller);
(void)value;
```

---

# Controller_GetHatCount

Returns the number of generic hat controls.

### Syntax

#### Macro form

```c
INPUT_CONTROLLER_FUNC(GetHatCount)(controller);
```

#### Direct form

```c
uint32_t Input_Controller_GetHatCount(Input_ControllerId controller);
```

### Parameters

| Parameter | Explanation |
| --- | --- |
| `controller` | ID obtained from the latest successful controller enumeration; zero is invalid. |

### Return value

A uint32_t count, or zero for a stale or invalid ID.

### Remarks

Valid hat indices are in `[0, count)`. The count describes the current connection; re-enumerate after hotplug.

### Example

```c
/* controller is an ID obtained from Enumerate. */
uint32_t value = Input_Controller_GetHatCount(controller);
(void)value;
```

---

# Controller_HasRumble

Checks whether this connection supports the implemented rumble operation.

### Syntax

#### Macro form

```c
INPUT_CONTROLLER_FUNC(HasRumble)(controller);
```

#### Direct form

```c
bool Input_Controller_HasRumble(Input_ControllerId controller);
```

### Parameters

| Parameter | Explanation |
| --- | --- |
| `controller` | ID obtained from the latest successful controller enumeration; zero is invalid. |

### Return value

True for a supported writable connection; otherwise false.

### Remarks

A true result does not guarantee that a later native write succeeds. On Linux, read-only devices report false.

### Example

```c
/* controller is an ID obtained from Enumerate. */
bool value = Input_Controller_HasRumble(controller);
(void)value;
```

---

# Controller_IsDown

Tests whether the input is currently held.

### Syntax

#### Macro form

```c
INPUT_CONTROLLER_FUNC(IsDown)(controller, button);
```

#### Direct form

```c
bool Input_Controller_IsDown(Input_ControllerId controller, uint32_t button);
```

### Parameters

| Parameter | Explanation |
| --- | --- |
| `controller` | ID obtained from the latest successful controller enumeration; zero is invalid. |
| `button` | Zero-based generic button index, below GetButtonCount. |

### Return value

True when the condition holds; false for idle, invalid or unavailable input.

### Remarks

Reading the flag does not consume it. Update renews transient flags. A press followed by a release in one update can set both IsPressed and IsReleased.

### Example

```c
/* controller is an ID obtained from Enumerate. */
bool value = Input_Controller_IsDown(controller, 0);
(void)value;
```

---

# Controller_IsPressed

Tests whether a down transition was observed during the latest update.

### Syntax

#### Macro form

```c
INPUT_CONTROLLER_FUNC(IsPressed)(controller, button);
```

#### Direct form

```c
bool Input_Controller_IsPressed(Input_ControllerId controller, uint32_t button);
```

### Parameters

| Parameter | Explanation |
| --- | --- |
| `controller` | ID obtained from the latest successful controller enumeration; zero is invalid. |
| `button` | Zero-based generic button index, below GetButtonCount. |

### Return value

True when the condition holds; false for idle, invalid or unavailable input.

### Remarks

Reading the flag does not consume it. Update renews transient flags. A press followed by a release in one update can set both IsPressed and IsReleased.

### Example

```c
/* controller is an ID obtained from Enumerate. */
bool value = Input_Controller_IsPressed(controller, 0);
(void)value;
```

---

# Controller_IsReleased

Tests whether an up transition was observed during the latest update.

### Syntax

#### Macro form

```c
INPUT_CONTROLLER_FUNC(IsReleased)(controller, button);
```

#### Direct form

```c
bool Input_Controller_IsReleased(Input_ControllerId controller, uint32_t button);
```

### Parameters

| Parameter | Explanation |
| --- | --- |
| `controller` | ID obtained from the latest successful controller enumeration; zero is invalid. |
| `button` | Zero-based generic button index, below GetButtonCount. |

### Return value

True when the condition holds; false for idle, invalid or unavailable input.

### Remarks

Reading the flag does not consume it. Update renews transient flags. A press followed by a release in one update can set both IsPressed and IsReleased.

### Example

```c
/* controller is an ID obtained from Enumerate. */
bool value = Input_Controller_IsReleased(controller, 0);
(void)value;
```

---

# Controller_GetAxis

Reads a generic absolute controller axis.

### Syntax

#### Macro form

```c
INPUT_CONTROLLER_FUNC(GetAxis)(controller, axis);
```

#### Direct form

```c
float Input_Controller_GetAxis(Input_ControllerId controller, uint32_t axis);
```

### Parameters

| Parameter | Explanation |
| --- | --- |
| `controller` | ID obtained from the latest successful controller enumeration; zero is invalid. |
| `axis` | Zero-based generic absolute-axis index, below GetAxisCount. |

### Return value

A normalized float in `[-1,1]`; zero for an invalid ID or index.

### Remarks

Indices are backend-specific. Controller triggers also use `[-1,1]`; use Gamepad_GetAxis for standardized trigger units. No automatic dead zone is applied.

### Example

```c
/* controller is an ID obtained from Enumerate. */
float value = Input_Controller_GetAxis(controller, 0);
(void)value;
```

---

# Controller_GetHat

Reads a generic directional hat.

### Syntax

#### Macro form

```c
INPUT_CONTROLLER_FUNC(GetHat)(controller, hat);
```

#### Direct form

```c
Input_Hat Input_Controller_GetHat(Input_ControllerId controller, uint32_t hat);
```

### Parameters

| Parameter | Explanation |
| --- | --- |
| `controller` | ID obtained from the latest successful controller enumeration; zero is invalid. |
| `hat` | Zero-based generic directional-hat index, below GetHatCount. |

### Return value

An `Input_Hat` direction mask; `INPUT_HAT_CENTERED` for neutral or invalid input.

### Remarks

Diagonal directions combine two bits. Test directions with bitwise AND, not equality, when diagonals should match.

### Example

```c
/* controller is an ID obtained from Enumerate. */
Input_Hat value = Input_Controller_GetHat(controller, 0);
(void)value;
```

---

# Controller_Rumble

Starts or replaces a simple two-motor vibration effect.

### Syntax

#### Macro form

```c
INPUT_CONTROLLER_FUNC(Rumble)(controller, lowFrequency, highFrequency, durationMs);
```

#### Direct form

```c
OPSTATUS Input_Controller_Rumble(Input_ControllerId controller, float lowFrequency, float highFrequency, uint32_t durationMs);
```

### Parameters

| Parameter | Explanation |
| --- | --- |
| `controller` | ID obtained from the latest successful controller enumeration; zero is invalid. |
| `lowFrequency` | Low-frequency/strong-motor intensity, finite and in `[0,1]`. |
| `highFrequency` | High-frequency/weak-motor intensity, finite and in `[0,1]`. |
| `durationMs` | Duration in milliseconds; zero requests stop. Linux supports at most 65535. |

### Return value

`SUCCESS`; `INVALID_ARGUMENT` for invalid intensities; `NOT_FOUND` for a stale ID; `NOT_SUPPORTED` without support; Linux `OUT_OF_RANGE` for durations above 65535 and `GENERIC_ERROR` for native I/O failure; Windows `NOT_AVAILABLE` for native failure.

### Remarks

Zero duration or zero intensities stops the effect, but Linux validates the maximum duration first. Linux times effects in the kernel. Windows requires continued Update calls to stop on schedule. NaN and infinity are rejected.

### Example

```c
/* controller is an ID obtained from Enumerate. */
OPSTATUS value = Input_Controller_Rumble(controller, 0.4f, 0.2f, 200);
(void)value;
```

---

# Controller_StopRumble

Stops the current simple vibration effect.

### Syntax

#### Macro form

```c
INPUT_CONTROLLER_FUNC(StopRumble)(controller);
```

#### Direct form

```c
OPSTATUS Input_Controller_StopRumble(Input_ControllerId controller);
```

### Parameters

| Parameter | Explanation |
| --- | --- |
| `controller` | ID obtained from the latest successful controller enumeration; zero is invalid. |

### Return value

The result of Rumble with zero intensities and duration: SUCCESS, NOT_FOUND, NOT_SUPPORTED, or a backend failure.

### Remarks

Stopping an idle supported Linux controller succeeds. This does not disconnect or release the controller.

### Example

```c
/* controller is an ID obtained from Enumerate. */
OPSTATUS value = Input_Controller_StopRumble(controller);
(void)value;
```

---

# Gamepad_IsMapped

Checks for a recognized standardized gamepad layout.

### Syntax

#### Macro form

```c
INPUT_GAMEPAD_FUNC(IsMapped)(controller);
```

#### Direct form

```c
bool Input_Gamepad_IsMapped(Input_ControllerId controller);
```

### Parameters

| Parameter | Explanation |
| --- | --- |
| `controller` | ID obtained from the latest successful controller enumeration; zero is invalid. |

### Return value

True for a recognized base layout; false for unknown IDs and unmapped controllers.

### Remarks

It does not guarantee that every optional button or axis exists. Generic Controller queries remain available independently of this mapping.

### Example

```c
/* controller is an ID obtained from Enumerate. */
bool value = Input_Gamepad_IsMapped(controller);
(void)value;
```

---

# Gamepad_IsDown

Tests whether the input is currently held.

### Syntax

#### Macro form

```c
INPUT_GAMEPAD_FUNC(IsDown)(controller, button);
```

#### Direct form

```c
bool Input_Gamepad_IsDown(Input_ControllerId controller, Input_GamepadButton button);
```

### Parameters

| Parameter | Explanation |
| --- | --- |
| `controller` | ID obtained from the latest successful controller enumeration; zero is invalid. |
| `button` | Standardized button enum, such as `INPUT_GAMEPAD_BUTTON_SOUTH`. |

### Return value

True when the condition holds; false for idle, invalid or unavailable input.

### Remarks

Reading the flag does not consume it. Update renews transient flags. A press followed by a release in one update can set both IsPressed and IsReleased. The controller must have a recognized mapping; missing mapped buttons return false.

### Example

```c
/* controller is an ID obtained from Enumerate. */
bool value = Input_Gamepad_IsDown(controller, INPUT_GAMEPAD_BUTTON_SOUTH);
(void)value;
```

---

# Gamepad_IsPressed

Tests whether a down transition was observed during the latest update.

### Syntax

#### Macro form

```c
INPUT_GAMEPAD_FUNC(IsPressed)(controller, button);
```

#### Direct form

```c
bool Input_Gamepad_IsPressed(Input_ControllerId controller, Input_GamepadButton button);
```

### Parameters

| Parameter | Explanation |
| --- | --- |
| `controller` | ID obtained from the latest successful controller enumeration; zero is invalid. |
| `button` | Standardized button enum, such as `INPUT_GAMEPAD_BUTTON_SOUTH`. |

### Return value

True when the condition holds; false for idle, invalid or unavailable input.

### Remarks

Reading the flag does not consume it. Update renews transient flags. A press followed by a release in one update can set both IsPressed and IsReleased. The controller must have a recognized mapping; missing mapped buttons return false.

### Example

```c
/* controller is an ID obtained from Enumerate. */
bool value = Input_Gamepad_IsPressed(controller, INPUT_GAMEPAD_BUTTON_SOUTH);
(void)value;
```

---

# Gamepad_IsReleased

Tests whether an up transition was observed during the latest update.

### Syntax

#### Macro form

```c
INPUT_GAMEPAD_FUNC(IsReleased)(controller, button);
```

#### Direct form

```c
bool Input_Gamepad_IsReleased(Input_ControllerId controller, Input_GamepadButton button);
```

### Parameters

| Parameter | Explanation |
| --- | --- |
| `controller` | ID obtained from the latest successful controller enumeration; zero is invalid. |
| `button` | Standardized button enum, such as `INPUT_GAMEPAD_BUTTON_SOUTH`. |

### Return value

True when the condition holds; false for idle, invalid or unavailable input.

### Remarks

Reading the flag does not consume it. Update renews transient flags. A press followed by a release in one update can set both IsPressed and IsReleased. The controller must have a recognized mapping; missing mapped buttons return false.

### Example

```c
/* controller is an ID obtained from Enumerate. */
bool value = Input_Gamepad_IsReleased(controller, INPUT_GAMEPAD_BUTTON_SOUTH);
(void)value;
```

---

# Gamepad_GetAxis

Reads a standardized analog stick component or trigger.

### Syntax

#### Macro form

```c
INPUT_GAMEPAD_FUNC(GetAxis)(controller, axis);
```

#### Direct form

```c
float Input_Gamepad_GetAxis(Input_ControllerId controller, Input_GamepadAxis axis);
```

### Parameters

| Parameter | Explanation |
| --- | --- |
| `controller` | ID obtained from the latest successful controller enumeration; zero is invalid. |
| `axis` | Standardized `Input_GamepadAxis` identifier. |

### Return value

Stick components in `[-1,1]`; triggers in `[0,1]`; zero for unavailable, unmapped or invalid input.

### Remarks

Positive X points right; positive Y points up. No dead zone is applied. Generic controller-axis events remain in generic units; this accessor performs the standardized conversion.

### Example

```c
/* controller is an ID obtained from Enumerate. */
float value = Input_Gamepad_GetAxis(controller, INPUT_GAMEPAD_AXIS_LEFT_X);
(void)value;
```

---

# Event_Poll

Copies and removes the oldest queued event.

### Syntax

#### Macro form

```c
INPUT_EVENT_FUNC(Poll)(event);
```

#### Direct form

```c
bool Input_Event_Poll(Input_TEvent *event);
```

### Parameters

| Parameter | Explanation |
| --- | --- |
| `event` | Destination event value. NULL returns false without consuming anything. |

### Return value

True if copied; false for an empty queue or NULL output. False leaves the output unchanged.

### Remarks

The event is a value with no borrowed allocation. Poll does not alter keyboard, mouse or controller state. Inspect type before reading the corresponding union member.

### Example

```c
Input_TEvent event;
bool value = Input_Event_Poll(&event);
(void)value;
```

---

# Event_Clear

Discards all currently queued events.

### Syntax

#### Macro form

```c
INPUT_EVENT_FUNC(Clear)();
```

#### Direct form

```c
void Input_Event_Clear(void);
```

### Parameters

None.

### Return value

None.

### Remarks

Does not reset states, sequence numbers or the cumulative dropped count. This is intentional history removal, not overflow.

### Example

```c
Input_Event_Clear();
```

---

# Event_GetDroppedCount

Returns cumulative event-loss accounting.

### Syntax

#### Macro form

```c
INPUT_EVENT_FUNC(GetDroppedCount)();
```

#### Direct form

```c
uint64_t Input_Event_GetDroppedCount(void);
```

### Parameters

None.

### Return value

A uint64_t count of discarded queue entries plus detected native-loss incidents.

### Remarks

A native-loss incident can represent an unknown number of events. The count is not an exact total of physical events lost. Compare successive values to detect new loss; Clear does not reset it.

### Example

```c
uint64_t value = Input_Event_GetDroppedCount();
(void)value;
```
