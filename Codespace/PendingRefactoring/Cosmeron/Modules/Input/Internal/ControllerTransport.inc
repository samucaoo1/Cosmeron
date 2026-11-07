#pragma once
/* Select the first transport that actually finds a controller. Keep that
 * transport for the process lifetime, including hot-unplug, to preserve IDs
 * and prevent double enumeration. No SDL initialization when native succeeds. */
#if defined(INPUT_EXTERNAL_STORAGE) && !defined(INPUT_DEFINE_STORAGE)
COSMERON_INPUT_STORAGE unsigned INPUT_INS(controllerTransport);
#else
COSMERON_INPUT_STORAGE unsigned INPUT_INS(controllerTransport) = 0;
#endif
static inline bool INPUT_INS(ControllersUpdate)(void) {
  if (INPUT_INS(controllerTransport) != 2) {
    if (INPUT_INS(NativeControllersUpdate)()) { INPUT_INS(controllerTransport) = 1; return true; }
    if (INPUT_INS(controllerTransport) == 1) return false;
  }
#ifndef INPUT_NO_SDL_CONTROLLERS
  {
    bool available = INPUT_INS(SDLUpdate)();
    size_t i;
    for (i = 0; i < INPUT_DEVICE_CAPACITY; ++i)
      if (INPUT_INS(sdlState).ids[i]) { INPUT_INS(controllerTransport) = 2; break; }
    return available;
  }
#else
  return false;
#endif
}
static inline OPSTATUS INPUT_INS(ControllersRumble)(INPUT_INS(Device) *d, float low, float high, uint32_t ms) {
#ifndef INPUT_NO_SDL_CONTROLLERS
  if (INPUT_INS(controllerTransport) == 2) return INPUT_INS(SDLRumble)(d, low, high, ms);
#endif
  (void)d; (void)low; (void)high; (void)ms;
  /* Generic HID defines no universal two-motor output report. */
  return STATUS_CONST(NOT_SUPPORTED);
}
