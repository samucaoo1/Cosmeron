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

Native capture is desktop/device scoped. For a host window (including Wayland), select `INPUT_CAPTURE_APPLICATION` before the first Update and submit translated events from its event loop. Focus is then explicit. Neither mode provides text/IME input or cursor appearance management.

| Platform | Keyboard / mouse | Controllers |
| --- | --- | --- |
| Linux | evdev physical keys, raw motion, absolute device coordinates and high-resolution wheels; optional X11 logical keys and desktop pointer | evdev, kernel-timed rumble |
| Windows | Raw Input physical/logical keys and per-device aggregation; desktop pointer | XInput plus WinMM generic joysticks; XInput rumble uses an OS timer |
| macOS | CoreGraphics event tap, layout translation through HIToolbox, desktop pointer | Native IOHIDManager; optional SDL 3 fallback |
| FreeBSD | evdev and optional X11 | evdev |
| OpenBSD / NetBSD / DragonFly | optional X11 polling | Native ujoy/uhid through system libusbhid; optional SDL 3 fallback |
| Other / INPUT_NO_NATIVE | application-submitted events | no native backend |

Check `Input_GetCapabilities()` after Update. Capabilities describe currently available native sources; application capabilities accumulate as corresponding events are submitted. A successful application-mode update does not imply controller availability.

Linux/FreeBSD device access respects existing file permissions. X11 provides a keyboard/pointer alternative without evdev access, but polling can miss short taps and does not supply raw motion or wheel events. X11 is deliberately disabled when WAYLAND_DISPLAY is present: Wayland applications must submit their own window events. Without a desktop position source, GetPosition is integrated raw displacement or absolute device units, not desktop pixels. Absolute tablet/touchpad coordinates are not calibrated to screens; multitouch gestures are outside this API.

Windows checks existing Raw Input registrations and returns BUSY rather than replacing another target. Application mode avoids these registrations. Native key transitions begin when observed; keys held before capture starts are not guaranteed to be seeded. WinMM can expose duplicate XInput devices; generic bindings require explicit mapping and WinMM rumble is unsupported. WinMM manufacturer/product numbers are not presented as USB IDs.

macOS event capture requires OS Input Monitoring permission; the module does not prompt or bypass it. Its event tap aggregates devices. Layout translation covers the key enum, not Unicode text. Native HID is attempted first on macOS/non-evdev BSD. SDL 3 is optional and attempted only while no native controller has been acquired; define INPUT_NO_SDL_CONTROLLERS to disable this optional transport. SDL performs its own allocations and device management; applications already using SDL must continue their normal event pumping.

No extra link flags are needed on Windows, macOS, BSD or modern Linux/glibc (2.34+). Older Linux libcs may require their usual dynamic-loader link library. Native libraries load at runtime. Module state uses bounded storage; native libraries may allocate internally. Resources close automatically at normal process exit and disconnect. Do not unload a shared object containing registered exit callbacks.

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

Intensities are finite values in `[0,1]`; invalid values (including NaN) return `INVALID_ARGUMENT`. A zero duration or two zero intensities stops the effect. A new effect replaces the previous one. Unsupported hardware returns `NOT_SUPPORTED`; stale IDs return `NOT_FOUND`; Linux durations above 65535 ms return `OUT_OF_RANGE`. Windows stops XInput vibration using a system timer, independently of Update. SDL-backed duration follows the SDL driver contract.

## Gamepad

```c
bool Input_Gamepad_IsMapped(Input_ControllerId controller);
bool Input_Gamepad_IsDown(Input_ControllerId controller, Input_GamepadButton button);
bool Input_Gamepad_IsPressed(Input_ControllerId controller, Input_GamepadButton button);
bool Input_Gamepad_IsReleased(Input_ControllerId controller, Input_GamepadButton button);
float Input_Gamepad_GetAxis(Input_ControllerId controller, Input_GamepadAxis axis);
```

The same controller ID is used; no second handle or duplicated event stream is created. Buttons are named by position (`SOUTH`, `EAST`, `WEST`, `NORTH`), shoulders, stick clicks, Back/Start/Guide and d-pad directions. Axes: `LEFT_X/Y`, `RIGHT_X/Y`, `LEFT_TRIGGER`, `RIGHT_TRIGGER`.

Stick components use `[-1,1]`, positive X right and Y up. Triggers use `[0,1]`. Missing elements and unmapped devices return neutral values. The Windows Guide button is not reported by standard XInput. Linux recognizes the kernel standard layout; SDL supplies its standard mappings. MapButton and MapAxis override individual bindings; IsMapped does not guarantee every optional control exists.

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
- GCC/Clang weak storage on POSIX and selectany/COMDAT storage on Windows make the default state shared across translation units rather than accidentally giving each `.c` file a separate input state.
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

Windows/macOS and an available X11 source report desktop coordinates. Check CAPABILITY_POINTER_POSITION; otherwise Linux reports integrated displacement or absolute device coordinates. This function does not consume delta or update native input.

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

Values use wheel-step units. Fractional Windows wheel steps are preserved. Linux high-resolution wheel values are divided by 120; matching legacy values are ignored to prevent double counting. The next Update clears the accumulation.

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

Zero duration or zero intensities stops the effect, but Linux validates the maximum duration first. Linux times effects in the kernel. Windows uses a system timer and does not require continued Update calls to stop on schedule. NaN and infinity are rejected.

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


## Host events and capability queries

```c
OPSTATUS Input_SetCaptureMode(Input_CaptureMode mode);
Input_Capabilities Input_GetCapabilities(void);
OPSTATUS Input_Event_Submit(const Input_TEvent *event);
```

SetCaptureMode accepts NATIVE (default) or APPLICATION. Changing mode after the first Update returns BUSY; repeating the current mode succeeds. An invalid mode returns INVALID_ARGUMENT. There is no Init/Free lifecycle. Submit copies an event into a bounded pending queue; Update applies it to the same snapshot and output queue used by native capture. Submit does not inject events into the operating system.

Submit requires application capture and accepts key press/release/repeat, mouse button press/release, movement, position, scroll, focus gained/lost and source removed. Unsupported types return NOT_SUPPORTED; invalid pointers, source slots or nonfinite motion return INVALID_ARGUMENT; invalid key/button codes return OUT_OF_RANGE; a full pending queue returns INSUFFICIENT_SPACE. Controller events remain native. Event_Clear clears output events, not pending submissions.

`source` is a caller-assigned slot below INPUT_DEVICE_CAPACITY. Keep it stable while a device is connected. Releases only clear the aggregate key/button when no source holds it. SOURCE_REMOVED releases that source. FOCUS_LOST releases input and suppresses held state until FOCUS_GAINED. Submit these from the host's actual focus notifications. `physicalKey` identifies position while `data.key` identifies the logical key; UNKNOWN represents unavailable information. Sequence values are assigned by the module.

```c
Input_SetCaptureMode(INPUT_CAPTURE_APPLICATION); /* before first Update */
Input_TEvent event = {0};
event.type = INPUT_EVENT_KEY_PRESS;
event.source = 0;
event.data.key = INPUT_KEY_Z;
event.physicalKey = INPUT_KEY_W; /* example: host's layout translation */
Input_Event_Submit(&event);
Input_Update();
```

Capability flags are KEYBOARD, PHYSICAL_KEYS, LOGICAL_KEYS, MOUSE_BUTTONS, POINTER_POSITION, RAW_MOTION, SCROLL, PRECISE_SCROLL, CONTROLLERS and APPLICATION_EVENTS, each prefixed INPUT_CAPABILITY_. Test them with a bitwise AND. Flags do not imply exclusive access, text input, or availability of every possible key/device.

## Physical keyboard queries

```c
bool Input_Keyboard_IsPhysicalDown(Input_Key key);
bool Input_Keyboard_IsPhysicalPressed(Input_Key key);
bool Input_Keyboard_IsPhysicalReleased(Input_Key key);
```

These queries use physical positions and the same per-update transition semantics as logical queries. Invalid/UNKNOWN keys return false. Keypad, menu, non-US backslash and five international positions are now represented; actual support depends on the native source. X11-only polling has no physical-key capability. Use logical queries for shortcuts, physical queries for positional bindings, and a separate host text API for text entry.

## Device identity and custom mappings

```c
uint16_t Input_Controller_GetVendor(Input_ControllerId controller);
uint16_t Input_Controller_GetProduct(Input_ControllerId controller);
OPSTATUS Input_Gamepad_MapButton(Input_ControllerId controller, Input_GamepadButton button, int32_t index);
OPSTATUS Input_Gamepad_MapAxis(Input_ControllerId controller, Input_GamepadAxis axis, int32_t index, float scale, float offset);
OPSTATUS Input_Gamepad_ClearMapping(Input_ControllerId controller);
```

Vendor/Product return available USB identifiers or zero, including for stale IDs. They are not stable unique device identities. Bindings last for the current connection only. MapButton maps a standard gamepad button to a raw button; MapAxis maps an axis using `raw * scale + offset`, clamped to the standard range (sticks -1..1, triggers 0..1). Index -1 disables that control. Scale/offset must be finite within -16..16. These functions return NOT_FOUND for stale IDs, OUT_OF_RANGE for invalid controls/indices and INVALID_ARGUMENT for invalid transforms. A successful binding marks the controller mapped and preserves other existing bindings. ClearMapping removes all standard bindings, including native defaults; it does not disconnect the controller. Raw controller queries remain available.

## Expanded validation

Application tests cover focus, multiple sources, physical/logical separation, pending overflow, invalid events and custom bindings. Native decoder tests cover evdev high-resolution wheels and recovery, Windows scan codes and system-timed rumble, macOS event translation, and SDL transport using a simulated driver. CI compiles platform branches on Linux, macOS and Windows. These automated checks do not replace physical-device, desktop-permission validation with physical devices. A separate Input BSD workflow builds and runs the suite in FreeBSD, OpenBSD, NetBSD and DragonFly virtual machines.


The additional APIs also support the namespace macros:

```c
INPUT_FUNC(SetCaptureMode)(INPUT_CONST(CAPTURE_APPLICATION));
INPUT_TYPE(Capabilities) caps = INPUT_FUNC(GetCapabilities)();
bool held = INPUT_KEYBOARD_FUNC(IsPhysicalDown)(LIB_PREFIX_CONST(INPUT_KEY_W));
OPSTATUS status = INPUT_EVENT_FUNC(Submit)(&event);
uint16_t vendor = INPUT_CONTROLLER_FUNC(GetVendor)(controller);
uint16_t product = INPUT_CONTROLLER_FUNC(GetProduct)(controller);
INPUT_GAMEPAD_FUNC(MapButton)(controller, INPUT_CONST(GAMEPAD_BUTTON_SOUTH), 0);
INPUT_GAMEPAD_FUNC(MapAxis)(controller, INPUT_CONST(GAMEPAD_AXIS_LEFT_Y), 1, -1.0f, 0.0f);
INPUT_GAMEPAD_FUNC(ClearMapping)(controller);
```

Use the same `*_FUNC` form for IsPhysicalPressed and IsPhysicalReleased. Both forms call the identical implementation; the macro form follows configured namespace prefixes.


## Native controller priority on macOS and BSD

macOS reads joystick/gamepad/multi-axis HID devices through IOHIDManager. It enumerates elements, normalizes absolute axes using descriptor limits, reads buttons and hats, reports vendor/product IDs and detects device removal. It uses its own run-loop mode and loads IOKit/CoreFoundation dynamically, with no framework link flags or SDL requirement.

FreeBSD retains its native evdev backend. OpenBSD reads `/dev/ujoy/*`; NetBSD and DragonFly read `/dev/uhid*`. Those three systems use their base-system libusbhid to parse descriptors, then read bounded nonblocking input reports directly from the device. Discovery repeats approximately once per second. Device permissions must already allow reading; the code does not change them. Device paths serve as names on uhid/ujoy; USB IDs are currently unavailable there.

On macOS and non-evdev BSD, native discovery runs before SDL. The first transport to acquire a controller owns controller enumeration for the process lifetime, including disconnect/reconnect. This avoids duplicate devices and changing connection identity behind the caller. If native discovery finds none, an installed SDL 3 may supply fallback devices. This is a whole-transport fallback, not per-device mixing. Define INPUT_NO_SDL_CONTROLLERS to require native-only operation.

Generic HID button ordering is not a portable gamepad layout: these native devices expose raw controls and accept MapButton/MapAxis, but do not invent standard bindings. Native generic HID rumble returns NOT_SUPPORTED because motor output reports are device-specific. The SDL fallback retains its mappings and rumble support. FreeBSD evdev rumble is unchanged. Native-only does not mean universal vendor-protocol or wireless-driver coverage.


Native-controller tests include an IOHID device simulation on macOS (enumeration, polling, ranges, hats and disconnect), HID bit decoding including unsigned 32-bit ranges, fallback ownership, and a native-only runtime smoke test. FreeBSD, OpenBSD, NetBSD and DragonFly compile and execute these tests in the Input BSD CI workflow. Physical USB/Bluetooth controllers, vendor-specific mappings and motor protocols still require hardware validation.
