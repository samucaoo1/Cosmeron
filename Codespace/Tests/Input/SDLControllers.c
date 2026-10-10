#define INPUT_NO_NATIVE
#include "../../Cosmeron/Modules/Input/Input.h"
#if !defined(_WIN32)
#include "../../Cosmeron/Modules/Input/Internal/SDLControllers.inc"
#include <assert.h>
static uint32_t deviceIds[] = {42};
static int connected = 1, closes;
static char joystickObject, gamepadObject;
static void noop(void) { }
static void mockFree(void *p) { (void)p; }
static uint32_t *mockList(int *count) { *count = connected; return deviceIds; }
static INPUT_INS(SDLJoystick) *mockOpen(uint32_t id) { assert(id == 42); return (INPUT_INS(SDLJoystick) *)&joystickObject; }
static void mockClose(INPUT_INS(SDLJoystick) *p) { (void)p; ++closes; }
static const char *mockName(INPUT_INS(SDLJoystick) *p) { (void)p; return "Mock SDL controller"; }
static int mockButtons(INPUT_INS(SDLJoystick) *p) { (void)p; return 4; }
static int mockAxes(INPUT_INS(SDLJoystick) *p) { (void)p; return 2; }
static int mockHats(INPUT_INS(SDLJoystick) *p) { (void)p; return 1; }
static bool mockButton(INPUT_INS(SDLJoystick) *p, int i) { (void)p; return i == 1; }
static int16_t mockAxis(INPUT_INS(SDLJoystick) *p, int i) { (void)p; return i ? INT16_MAX : INT16_MIN; }
static uint8_t mockHat(INPUT_INS(SDLJoystick) *p, int i) { (void)p; (void)i; return 3; }
static uint16_t mockVendor(INPUT_INS(SDLJoystick) *p) { (void)p; return 123; }
static uint16_t mockProduct(INPUT_INS(SDLJoystick) *p) { (void)p; return 456; }
static bool mockRumble(INPUT_INS(SDLJoystick) *p, uint16_t low, uint16_t high, uint32_t ms) { (void)p; (void)low; (void)high; return ms == 100; }
static uint32_t mockProperties(INPUT_INS(SDLJoystick) *p) { (void)p; return 1; }
static bool mockBool(uint32_t p, const char *name, bool fallback) { (void)p; (void)name; (void)fallback; return true; }
static bool mockIsGamepad(uint32_t id) { (void)id; return true; }
static INPUT_INS(SDLGamepad) *mockOpenGamepad(uint32_t id) { (void)id; return (INPUT_INS(SDLGamepad) *)&gamepadObject; }
static void mockCloseGamepad(INPUT_INS(SDLGamepad) *p) { (void)p; ++closes; }
static bool mockGamepadButton(INPUT_INS(SDLGamepad) *p, int i) { (void)p; return i == 0; }
static int16_t mockGamepadAxis(INPUT_INS(SDLGamepad) *p, int i) { (void)p; return i == 1 ? INT16_MIN : 0; }
#endif
int main(void) {
#if !defined(_WIN32)
  INPUT_INS(SDLState) *s = &INPUT_INS(sdlState);
  Input_ControllerId ids[1], old;
  size_t count;
  s->ready = true; s->update = noop; s->freeMemory = mockFree; s->list = mockList;
  s->open = mockOpen; s->close = mockClose; s->name = mockName;
  s->buttons = mockButtons; s->axes = mockAxes; s->hats = mockHats;
  s->button = mockButton; s->axis = mockAxis; s->hat = mockHat;
  s->vendor = mockVendor; s->product = mockProduct; s->rumble = mockRumble;
  s->properties = mockProperties; s->propertyBool = mockBool; s->isGamepad = mockIsGamepad;
  s->openGamepad = mockOpenGamepad; s->closeGamepad = mockCloseGamepad;
  s->gamepadButton = mockGamepadButton; s->gamepadAxis = mockGamepadAxis;
  assert(INPUT_INS(SDLUpdate)());
  assert(Input_Controller_Enumerate(ids, 1, &count) == STATUS_SUCCESS && count == 1);
  old = ids[0];
  assert(Input_Controller_GetVendor(old) == 123 && Input_Controller_GetProduct(old) == 456);
  assert(Input_Controller_IsDown(old, 1));
  assert(Input_Controller_GetAxis(old, 0) == -1 && Input_Controller_GetAxis(old, 1) == 1);
  assert(Input_Controller_GetHat(old, 0) == (INPUT_HAT_UP | INPUT_HAT_RIGHT));
  assert(Input_Gamepad_IsMapped(old) && Input_Gamepad_IsDown(old, INPUT_GAMEPAD_BUTTON_SOUTH));
  assert(Input_Gamepad_GetAxis(old, INPUT_GAMEPAD_AXIS_LEFT_Y) == 1);
  assert(Input_Gamepad_MapButton(old, INPUT_GAMEPAD_BUTTON_EAST, 1) == STATUS_SUCCESS);
  assert(Input_Gamepad_IsDown(old, INPUT_GAMEPAD_BUTTON_EAST));
  assert(Input_Gamepad_IsDown(old, INPUT_GAMEPAD_BUTTON_SOUTH)); /* preserve other native bindings */
  assert(Input_Gamepad_GetAxis(old, INPUT_GAMEPAD_AXIS_LEFT_Y) == 1);
  assert(INPUT_INS(SDLRumble)(INPUT_INS(Find)(old), 1, 1, 100) == STATUS_SUCCESS);
  assert(INPUT_INS(SDLRumble)(INPUT_INS(Find)(old), 1, 1, 200) == STATUS_GENERIC_ERROR);
  connected = 0; assert(INPUT_INS(SDLUpdate)()); assert(!Input_Controller_IsConnected(old) && closes == 2);
  connected = 1; assert(INPUT_INS(SDLUpdate)());
  assert(Input_Controller_Enumerate(ids, 1, &count) == STATUS_SUCCESS && ids[0] != old);
#endif
  return 0;
}
