#define INPUT_NO_NATIVE
#define INPUT_EVENT_CAPACITY 8
#include "../../Cosmeron/Modules/Input/Input.h"
#include <assert.h>
#include <math.h>

int main(void) {
  Input_TEvent event;
  Input_Internal_Device *d;
  Input_ControllerId ids[2] = {0}, oldId;
  size_t count = 123, required;
  char name[32], small[2] = {'x','y'};
  float x, y;
  unsigned i;
  uint64_t previous = 0;

  assert(Input_Update() == STATUS_NOT_SUPPORTED);
  Input_Internal_Key(INPUT_KEY_W, true, false);
  Input_Internal_Key(INPUT_KEY_W, true, false);
  assert(Input_Keyboard_IsDown(INPUT_KEY_W));
  assert(Input_Keyboard_IsPressed(INPUT_KEY_W));
  assert(!Input_Keyboard_IsReleased(INPUT_KEY_W));
  Input_Internal_Key(INPUT_KEY_W, true, true);
  assert(Input_Keyboard_IsRepeated(INPUT_KEY_W));
  Input_Internal_Key(INPUT_KEY_W, false, false);
  assert(!Input_Keyboard_IsDown(INPUT_KEY_W));
  assert(Input_Keyboard_IsPressed(INPUT_KEY_W));
  assert(Input_Keyboard_IsReleased(INPUT_KEY_W));
  assert(Input_Event_Poll(&event) && event.type == INPUT_EVENT_KEY_PRESS);
  assert(Input_Event_Poll(&event) && event.type == INPUT_EVENT_KEY_REPEAT);
  assert(Input_Event_Poll(&event) && event.type == INPUT_EVENT_KEY_RELEASE);
  assert(!Input_Event_Poll(&event));
  assert(Input_Keyboard_IsReleased(INPUT_KEY_W));
  assert(Input_Update() == STATUS_NOT_SUPPORTED);
  assert(!Input_Keyboard_IsPressed(INPUT_KEY_W));
  assert(!Input_Keyboard_IsReleased(INPUT_KEY_W));
  assert(!Input_Keyboard_IsRepeated(INPUT_KEY_W));
  assert(!Input_Keyboard_IsDown((Input_Key)-1));
  assert(!Input_Keyboard_IsDown(INPUT_KEY_COUNT));
  Input_Internal_Key(INPUT_KEY_LEFT_CONTROL, true, false);
  Input_Internal_Key(INPUT_KEY_RIGHT_SHIFT, true, false);
  Input_Internal_Key(INPUT_KEY_LEFT_ALT, true, false);
  Input_Internal_Key(INPUT_KEY_RIGHT_SUPER, true, false);
  assert(Input_Keyboard_GetModifiers() == (INPUT_MODIFIER_CONTROL | INPUT_MODIFIER_SHIFT | INPUT_MODIFIER_ALT | INPUT_MODIFIER_SUPER));

  Input_Internal_MouseButton(INPUT_MOUSE_BUTTON_LEFT, true);
  Input_Internal_MouseButton(INPUT_MOUSE_BUTTON_LEFT, false);
  assert(!Input_Mouse_IsDown(INPUT_MOUSE_BUTTON_LEFT));
  assert(Input_Mouse_IsPressed(INPUT_MOUSE_BUTTON_LEFT));
  assert(Input_Mouse_IsReleased(INPUT_MOUSE_BUTTON_LEFT));
  assert(!Input_Mouse_IsDown((Input_MouseButton)-1));
  Input_Internal_Motion(2, -3, false);
  Input_Internal_Motion(4, 1, false);
  Input_Internal_Motion(1, -1, true);
  Input_Mouse_GetDelta(&x, &y); assert(x == 6 && y == -2);
  Input_Mouse_GetScroll(&x, &y); assert(x == 1 && y == -1);
  Input_Mouse_GetPosition(&x, &y); assert(x == 0 && y == 0);
  Input_Mouse_GetPosition(NULL, NULL);
  Input_Mouse_GetDelta(NULL, NULL); Input_Mouse_GetScroll(NULL, NULL);
  Input_Internal_Begin(); Input_Mouse_GetDelta(&x, &y); assert(x == 0 && y == 0);
  Input_Mouse_GetScroll(&x, &y); assert(x == 0 && y == 0);

  Input_Event_Clear();
  d = Input_Internal_Connect(0, "Test controller"); assert(d);
  oldId = d->id; d->buttonCount = 2; d->axisCount = 2; d->hatCount = 1;
  assert(Input_Controller_Enumerate(NULL, 0, &count) == STATUS_SUCCESS && count == 1);
  assert(Input_Controller_Enumerate(ids, 0, &count) == STATUS_INSUFFICIENT_SPACE && ids[0] == 0);
  assert(Input_Controller_Enumerate(ids, 2, &count) == STATUS_SUCCESS && ids[0] == oldId);
  assert(Input_Controller_Enumerate(NULL, 2, &count) == STATUS_INVALID_ARGUMENT);
  assert(Input_Controller_Enumerate(ids, 2, NULL) == STATUS_INVALID_ARGUMENT);
  assert(Input_Controller_IsConnected(oldId));
  assert(Input_Controller_GetName(oldId, NULL, 0, &required) == STATUS_SUCCESS && required == 16);
  assert(Input_Controller_GetName(oldId, small, sizeof(small), &required) == STATUS_INSUFFICIENT_SPACE && small[0] == 'x');
  assert(Input_Controller_GetName(oldId, name, sizeof(name), &required) == STATUS_SUCCESS);
  assert(!strcmp(name, "Test controller"));
  assert(Input_Controller_GetName(oldId, NULL, 1, &required) == STATUS_INVALID_ARGUMENT);
  assert(Input_Controller_GetName(oldId, name, sizeof(name), NULL) == STATUS_INVALID_ARGUMENT);
  assert(Input_Controller_GetButtonCount(oldId) == 2);
  assert(Input_Controller_GetAxisCount(oldId) == 2);
  assert(Input_Controller_GetHatCount(oldId) == 1);
  assert(!Input_Controller_HasRumble(oldId));
  assert(Input_Controller_Rumble(oldId, NAN, 0, 10) == STATUS_INVALID_ARGUMENT);
  assert(Input_Controller_Rumble(oldId, 0, 2, 10) == STATUS_INVALID_ARGUMENT);
  assert(Input_Controller_Rumble(oldId, 1, 1, 10) == STATUS_NOT_SUPPORTED);
  assert(Input_Controller_StopRumble(oldId) == STATUS_NOT_SUPPORTED);
  assert(!Input_Gamepad_IsMapped(oldId));
  d->mapped = true; d->gamepadButtons[INPUT_GAMEPAD_BUTTON_SOUTH] = 0;
  d->gamepadAxes[INPUT_GAMEPAD_AXIS_LEFT_X] = 0;
  d->gamepadAxes[INPUT_GAMEPAD_AXIS_LEFT_TRIGGER] = 1;
  Input_Internal_ControllerButton(d, 0, true);
  assert(Input_Controller_IsDown(oldId, 0) && Input_Gamepad_IsDown(oldId, INPUT_GAMEPAD_BUTTON_SOUTH));
  assert(Input_Controller_IsPressed(oldId, 0) && Input_Gamepad_IsPressed(oldId, INPUT_GAMEPAD_BUTTON_SOUTH));
  Input_Internal_ControllerButton(d, 0, false);
  assert(Input_Controller_IsReleased(oldId, 0) && Input_Gamepad_IsReleased(oldId, INPUT_GAMEPAD_BUTTON_SOUTH));
  Input_Internal_Axis(d, 0, -0.5f); Input_Internal_Axis(d, 1, -1);
  assert(Input_Controller_GetAxis(oldId, 0) == -0.5f);
  assert(Input_Gamepad_GetAxis(oldId, INPUT_GAMEPAD_AXIS_LEFT_X) == -0.5f);
  assert(Input_Gamepad_GetAxis(oldId, INPUT_GAMEPAD_AXIS_LEFT_TRIGGER) == 0);
  Input_Internal_Axis(d, 1, 1);
  assert(Input_Gamepad_GetAxis(oldId, INPUT_GAMEPAD_AXIS_LEFT_TRIGGER) == 1);
  Input_Internal_Hat(d, 0, (Input_Hat)(INPUT_HAT_UP | INPUT_HAT_LEFT));
  assert(Input_Controller_GetHat(oldId, 0) == (INPUT_HAT_UP | INPUT_HAT_LEFT));
  assert(!Input_Controller_IsDown(oldId, UINT32_MAX));
  assert(!Input_Gamepad_IsDown(oldId, (Input_GamepadButton)-1));
  assert(Input_Controller_GetAxis(oldId, UINT32_MAX) == 0);
  assert(Input_Controller_GetHat(oldId, UINT32_MAX) == INPUT_HAT_CENTERED);
  assert(Input_Gamepad_GetAxis(oldId, (Input_GamepadAxis)-1) == 0);
  assert(Input_Gamepad_GetAxis(oldId, INPUT_GAMEPAD_AXIS_RIGHT_X) == 0);
  Input_Internal_Disconnect(0);
  assert(!Input_Controller_IsConnected(oldId));
  assert(!Input_Controller_IsDown(oldId, 0));
  assert(Input_Controller_GetButtonCount(oldId) == 0);
  assert(Input_Controller_GetName(oldId, name, sizeof(name), &required) == STATUS_NOT_FOUND);
  assert(Input_Controller_StopRumble(oldId) == STATUS_NOT_FOUND);
  d = Input_Internal_Connect(0, "New controller"); assert(d && d->id != oldId);

  Input_Event_Clear(); Input_Internal_state.dropped = 0;
  for (i = 0; i < INPUT_EVENT_CAPACITY + 2; ++i) Input_Internal_Motion(1, 0, false);
  assert(Input_Event_GetDroppedCount() == 2);
  for (i = 0; i < INPUT_EVENT_CAPACITY; ++i) {
    assert(Input_Event_Poll(&event)); assert(event.sequence > previous); previous = event.sequence;
  }
  assert(!Input_Event_Poll(&event));
  assert(!Input_Event_Poll(NULL));
  /* Overflow affects the event history, never the final state. */
  Input_Event_Clear();
  for (i = 0; i < INPUT_EVENT_CAPACITY + 4; ++i) Input_Internal_Key(INPUT_KEY_A, (i % 2) == 0, false);
  assert(!Input_Keyboard_IsDown(INPUT_KEY_A));
  assert(Input_Keyboard_IsPressed(INPUT_KEY_A) && Input_Keyboard_IsReleased(INPUT_KEY_A));
  return 0;
}
