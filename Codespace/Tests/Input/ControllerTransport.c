#define INPUT_NO_NATIVE
#include "../../Cosmeron/Modules/Input/Input.h"
#include <assert.h>
/* Isolate transport arbitration from hardware and external libraries. */
static struct { uint32_t ids[INPUT_DEVICE_CAPACITY]; } Input_Internal_sdlState;
static bool nativeAvailable, sdlAvailable;
static unsigned nativeCalls, sdlCalls;
static inline bool Input_Internal_NativeControllersUpdate(void) { ++nativeCalls; return nativeAvailable; }
static inline bool Input_Internal_SDLUpdate(void) { ++sdlCalls; return sdlAvailable; }
static inline OPSTATUS Input_Internal_SDLRumble(Input_Internal_Device *d, float l, float h, uint32_t ms) {
  (void)d; (void)l; (void)h; (void)ms; return STATUS_SUCCESS;
}
#include "../../Cosmeron/Modules/Input/Internal/ControllerTransport.inc"
int main(void) {
  nativeAvailable = true;
  assert(Input_Internal_ControllersUpdate());
  assert(nativeCalls == 1 && sdlCalls == 0 && Input_Internal_controllerTransport == 1);
  nativeAvailable = false;
  assert(!Input_Internal_ControllersUpdate());
  assert(sdlCalls == 0); /* hot-unplug does not change ownership */
  Input_Internal_controllerTransport = 0;
  sdlAvailable = true; Input_Internal_sdlState.ids[0] = 123;
  assert(Input_Internal_ControllersUpdate() && Input_Internal_controllerTransport == 2);
  nativeAvailable = true;
  nativeCalls = 0;
  assert(Input_Internal_ControllersUpdate() && nativeCalls == 0);
  assert(Input_Internal_ControllersRumble(NULL, 1, 1, 10) == STATUS_SUCCESS);
  Input_Internal_controllerTransport = 1;
  assert(Input_Internal_ControllersRumble(NULL, 1, 1, 10) == STATUS_NOT_SUPPORTED);
  return 0;
}
