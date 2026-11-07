#pragma once
#include "X11.inc"
#include "BSDHID.inc"
#ifndef INPUT_NO_SDL_CONTROLLERS
#include "SDLControllers.inc"
#endif
#include "ControllerTransport.inc"
static inline OPSTATUS INPUT_INS(BackendUpdate)(void) {
  bool available = INPUT_INS(XUpdate)(true, true);
  available |= INPUT_INS(ControllersUpdate)();
  return available ? STATUS_CONST(SUCCESS) : STATUS_CONST(NOT_AVAILABLE);
}
static inline OPSTATUS INPUT_INS(BackendRumble)(INPUT_INS(Device) *d, float low, float high, uint32_t ms) {
  return INPUT_INS(ControllersRumble)(d, low, high, ms);
}
