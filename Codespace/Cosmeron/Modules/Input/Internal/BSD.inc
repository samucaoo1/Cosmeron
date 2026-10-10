#pragma once
#include "X11.inc"
#ifndef INPUT_NO_SDL_CONTROLLERS
#include "SDLControllers.inc"
#endif
static inline OPSTATUS INPUT_INS(BackendUpdate)(void) {
  bool available = INPUT_INS(XUpdate)(true, true);
#ifndef INPUT_NO_SDL_CONTROLLERS
  available |= INPUT_INS(SDLUpdate)();
#endif
  return available ? STATUS_CONST(SUCCESS) : STATUS_CONST(NOT_AVAILABLE);
}
static inline OPSTATUS INPUT_INS(BackendRumble)(INPUT_INS(Device) *d, float low, float high, uint32_t ms) {
#ifndef INPUT_NO_SDL_CONTROLLERS
  return INPUT_INS(SDLRumble)(d, low, high, ms);
#else
  (void)d; (void)low; (void)high; (void)ms; return STATUS_CONST(NOT_SUPPORTED);
#endif
}
