#define INPUT_NO_NATIVE
#include "../../Cosmeron/Modules/Input/Input.h"
#include <assert.h>
#include <math.h>
static void key(uint32_t source, Input_EventType type, Input_Key logical, Input_Key physical) {
  Input_TEvent e = {0}; e.type = type; e.source = source; e.data.key = logical; e.physicalKey = physical;
  assert(Input_Event_Submit(&e) == STATUS_SUCCESS);
}
int main(void) {
  Input_TEvent e = {0};
  Input_Internal_Device *d;
  float x, y;
  unsigned i;
  assert(Input_Event_Submit(NULL) == STATUS_INVALID_ARGUMENT);
  assert(Input_Event_Submit(&e) == STATUS_NOT_SUPPORTED);
  assert(Input_SetCaptureMode((Input_CaptureMode)99) == STATUS_INVALID_ARGUMENT);
  assert(Input_SetCaptureMode(INPUT_CAPTURE_APPLICATION) == STATUS_SUCCESS);
  key(0, INPUT_EVENT_KEY_PRESS, INPUT_KEY_Z, INPUT_KEY_W);
  assert(!Input_Keyboard_IsDown(INPUT_KEY_Z));
  assert(Input_Update() == STATUS_SUCCESS);
  assert(Input_Keyboard_IsDown(INPUT_KEY_Z) && !Input_Keyboard_IsDown(INPUT_KEY_W));
  assert(Input_Keyboard_IsPhysicalDown(INPUT_KEY_W) && Input_Keyboard_IsPhysicalPressed(INPUT_KEY_W));
  assert(Input_GetCapabilities() & INPUT_CAPABILITY_LOGICAL_KEYS);
  assert(Input_GetCapabilities() & INPUT_CAPABILITY_PHYSICAL_KEYS);
  assert(Input_Event_Poll(&e) && e.data.key == INPUT_KEY_Z && e.physicalKey == INPUT_KEY_W);
  key(1, INPUT_EVENT_KEY_PRESS, INPUT_KEY_Z, INPUT_KEY_W);
  key(0, INPUT_EVENT_KEY_RELEASE, INPUT_KEY_Z, INPUT_KEY_W);
  assert(Input_Update() == STATUS_SUCCESS);
  assert(Input_Keyboard_IsDown(INPUT_KEY_Z) && Input_Keyboard_IsPhysicalDown(INPUT_KEY_W));
  assert(!Input_Keyboard_IsReleased(INPUT_KEY_Z));
  e.type = INPUT_EVENT_SOURCE_REMOVED; e.source = 1;
  assert(Input_Event_Submit(&e) == STATUS_SUCCESS);
  assert(Input_Update() == STATUS_SUCCESS);
  assert(!Input_Keyboard_IsDown(INPUT_KEY_Z) && Input_Keyboard_IsPhysicalReleased(INPUT_KEY_W));
  key(0, INPUT_EVENT_KEY_PRESS, INPUT_KEY_A, INPUT_KEY_A);
  e.type = INPUT_EVENT_FOCUS_LOST; e.source = 0;
  assert(Input_Event_Submit(&e) == STATUS_SUCCESS);
  key(0, INPUT_EVENT_KEY_PRESS, INPUT_KEY_B, INPUT_KEY_B);
  assert(Input_Update() == STATUS_SUCCESS);
  assert(!Input_Keyboard_IsDown(INPUT_KEY_A) && !Input_Keyboard_IsDown(INPUT_KEY_B));
  assert(Input_Keyboard_IsReleased(INPUT_KEY_A));
  e.type = INPUT_EVENT_FOCUS_GAINED; assert(Input_Event_Submit(&e) == STATUS_SUCCESS);
  key(0, INPUT_EVENT_KEY_PRESS, INPUT_KEY_B, INPUT_KEY_B);
  e.type = INPUT_EVENT_MOUSE_POSITION; e.data.motion.x = 123.5f; e.data.motion.y = 45;
  assert(Input_Event_Submit(&e) == STATUS_SUCCESS);
  e.type = INPUT_EVENT_MOUSE_SCROLL; e.data.motion.x = 0.25f; e.data.motion.y = -0.5f;
  assert(Input_Event_Submit(&e) == STATUS_SUCCESS);
  e.type = INPUT_EVENT_MOUSE_BUTTON_PRESS; e.source = 1; e.data.mouseButton = INPUT_MOUSE_BUTTON_LEFT;
  assert(Input_Event_Submit(&e) == STATUS_SUCCESS);
  e.source = 0; assert(Input_Event_Submit(&e) == STATUS_SUCCESS);
  e.type = INPUT_EVENT_MOUSE_BUTTON_RELEASE; assert(Input_Event_Submit(&e) == STATUS_SUCCESS);
  assert(Input_Update() == STATUS_SUCCESS);
  assert(Input_Mouse_IsDown(INPUT_MOUSE_BUTTON_LEFT));
  Input_Mouse_GetPosition(&x, &y); assert(x == 123.5f && y == 45);
  Input_Mouse_GetScroll(&x, &y); assert(x == 0.25f && y == -0.5f);
  assert(Input_GetCapabilities() & INPUT_CAPABILITY_POINTER_POSITION);
  assert(Input_Keyboard_IsDown(INPUT_KEY_B));
  assert(Input_SetCaptureMode(INPUT_CAPTURE_NATIVE) == STATUS_BUSY);
  assert(Input_SetCaptureMode(INPUT_CAPTURE_APPLICATION) == STATUS_SUCCESS);
  e.type = INPUT_EVENT_MOUSE_MOVE; e.data.motion.x = NAN;
  assert(Input_Event_Submit(&e) == STATUS_INVALID_ARGUMENT);
  e.type = INPUT_EVENT_KEY_PRESS; e.data.key = (Input_Key)-1;
  assert(Input_Event_Submit(&e) == STATUS_OUT_OF_RANGE);
  e.type = INPUT_EVENT_FOCUS_GAINED; e.source = INPUT_DEVICE_CAPACITY;
  assert(Input_Event_Submit(&e) == STATUS_INVALID_ARGUMENT);
  e.source = 0; e.type = INPUT_EVENT_DEVICE_CONNECTED;
  assert(Input_Event_Submit(&e) == STATUS_NOT_SUPPORTED);
  e.type = INPUT_EVENT_FOCUS_GAINED;
  for (i = 0; i < INPUT_EVENT_CAPACITY; ++i) assert(Input_Event_Submit(&e) == STATUS_SUCCESS);
  assert(Input_Event_Submit(&e) == STATUS_INSUFFICIENT_SPACE);
  assert(Input_Update() == STATUS_SUCCESS);

  d = Input_Internal_Connect(0, "Custom"); assert(d);
  d->buttonCount = 3; d->axisCount = 2; d->vendor = 0x1234; d->product = 0x5678;
  assert(Input_Controller_GetVendor(d->id) == 0x1234 && Input_Controller_GetProduct(d->id) == 0x5678);
  assert(Input_Controller_GetVendor(0) == 0 && Input_Controller_GetProduct(0) == 0);
  assert(Input_Gamepad_MapButton(d->id, INPUT_GAMEPAD_BUTTON_SOUTH, 2) == STATUS_SUCCESS);
  Input_Internal_ControllerButton(d, 2, true);
  assert(Input_Gamepad_IsDown(d->id, INPUT_GAMEPAD_BUTTON_SOUTH));
  assert(Input_Gamepad_MapAxis(d->id, INPUT_GAMEPAD_AXIS_LEFT_TRIGGER, 0, 0.5f, 0.5f) == STATUS_SUCCESS);
  Input_Internal_Axis(d, 0, -1); assert(Input_Gamepad_GetAxis(d->id, INPUT_GAMEPAD_AXIS_LEFT_TRIGGER) == 0);
  Input_Internal_Axis(d, 0, 1); assert(Input_Gamepad_GetAxis(d->id, INPUT_GAMEPAD_AXIS_LEFT_TRIGGER) == 1);
  assert(Input_Gamepad_MapAxis(d->id, INPUT_GAMEPAD_AXIS_LEFT_Y, 1, -1, 0) == STATUS_SUCCESS);
  Input_Internal_Axis(d, 1, -0.75f); assert(Input_Gamepad_GetAxis(d->id, INPUT_GAMEPAD_AXIS_LEFT_Y) == 0.75f);
  assert(Input_Gamepad_MapAxis(d->id, INPUT_GAMEPAD_AXIS_LEFT_X, 1, NAN, 0) == STATUS_INVALID_ARGUMENT);
  assert(Input_Gamepad_MapAxis(d->id, (Input_GamepadAxis)-1, 0, 1, 0) == STATUS_OUT_OF_RANGE);
  assert(Input_Gamepad_MapButton(d->id, INPUT_GAMEPAD_BUTTON_SOUTH, 3) == STATUS_OUT_OF_RANGE);
  assert(Input_Gamepad_MapButton(0, INPUT_GAMEPAD_BUTTON_SOUTH, 0) == STATUS_NOT_FOUND);
  assert(Input_Gamepad_MapButton(d->id, INPUT_GAMEPAD_BUTTON_SOUTH, -1) == STATUS_SUCCESS);
  assert(!Input_Gamepad_IsDown(d->id, INPUT_GAMEPAD_BUTTON_SOUTH));
  assert(Input_Gamepad_ClearMapping(d->id) == STATUS_SUCCESS && !Input_Gamepad_IsMapped(d->id));
  assert(Input_Gamepad_ClearMapping(0) == STATUS_NOT_FOUND);
  return 0;
}
