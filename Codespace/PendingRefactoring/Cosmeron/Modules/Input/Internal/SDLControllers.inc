#pragma once
#include "Dynamic.inc"
/* Optional SDL 3 runtime transport for macOS/BSD controllers. These are the
 * stable SDL 3 C ABI signatures; objects stay opaque and no SDL structs are
 * re-created or dereferenced. No SDL headers or linker flags are required.
 * API reference: https://wiki.libsdl.org/SDL3/CategoryJoystick */
typedef struct INPUT_INS(SDLJoystick) INPUT_INS(SDLJoystick);
typedef struct INPUT_INS(SDLGamepad) INPUT_INS(SDLGamepad);
typedef struct INPUT_INS(SDLState) {
  void *library;
  bool ready;
  bool (*init)(uint32_t);
  uint32_t (*wasInit)(uint32_t);
  void (*quit)(uint32_t);
  void (*freeMemory)(void *);
  uint32_t *(*list)(int *);
  void (*update)(void);
  INPUT_INS(SDLJoystick) *(*open)(uint32_t);
  void (*close)(INPUT_INS(SDLJoystick) *);
  const char *(*name)(INPUT_INS(SDLJoystick) *);
  int (*buttons)(INPUT_INS(SDLJoystick) *);
  int (*axes)(INPUT_INS(SDLJoystick) *);
  int (*hats)(INPUT_INS(SDLJoystick) *);
  bool (*button)(INPUT_INS(SDLJoystick) *, int);
  int16_t (*axis)(INPUT_INS(SDLJoystick) *, int);
  uint8_t (*hat)(INPUT_INS(SDLJoystick) *, int);
  uint16_t (*vendor)(INPUT_INS(SDLJoystick) *);
  uint16_t (*product)(INPUT_INS(SDLJoystick) *);
  bool (*rumble)(INPUT_INS(SDLJoystick) *, uint16_t, uint16_t, uint32_t);
  uint32_t (*properties)(INPUT_INS(SDLJoystick) *);
  bool (*propertyBool)(uint32_t, const char *, bool);
  bool (*isGamepad)(uint32_t);
  INPUT_INS(SDLGamepad) *(*openGamepad)(uint32_t);
  void (*closeGamepad)(INPUT_INS(SDLGamepad) *);
  bool (*gamepadButton)(INPUT_INS(SDLGamepad) *, int);
  int16_t (*gamepadAxis)(INPUT_INS(SDLGamepad) *, int);
  void (*joystickEvents)(bool);
  void (*gamepadEvents)(bool);
  INPUT_INS(SDLJoystick) *joysticks[INPUT_DEVICE_CAPACITY];
  INPUT_INS(SDLGamepad) *gamepads[INPUT_DEVICE_CAPACITY];
  uint32_t ids[INPUT_DEVICE_CAPACITY];
} INPUT_INS(SDLState);
enum { INPUT_INS(SDLSubsystems) = 0x00000200u | 0x00002000u };
#if defined(INPUT_EXTERNAL_STORAGE) && !defined(INPUT_DEFINE_STORAGE)
COSMERON_INPUT_STORAGE INPUT_INS(SDLState) INPUT_INS(sdlState);
#else
COSMERON_INPUT_STORAGE INPUT_INS(SDLState) INPUT_INS(sdlState) = {0};
#endif
static inline void INPUT_INS(SDLClose)(size_t slot) {
  INPUT_INS(SDLState) *s = &INPUT_INS(sdlState);
  if (!s->ids[slot] && !s->joysticks[slot] && !s->gamepads[slot]) return;
  if (s->gamepads[slot]) s->closeGamepad(s->gamepads[slot]);
  if (s->joysticks[slot]) s->close(s->joysticks[slot]);
  s->gamepads[slot] = NULL; s->joysticks[slot] = NULL; s->ids[slot] = 0;
  /* Empty SDL slots may belong to a native transport. */
  INPUT_INS(Disconnect)(slot);
}
static void INPUT_INS(SDLCleanup)(void) {
  INPUT_INS(SDLState) *s = &INPUT_INS(sdlState);
  size_t i;
  for (i = 0; i < INPUT_DEVICE_CAPACITY; ++i) INPUT_INS(SDLClose)(i);
  if (s->ready) s->quit(INPUT_INS(SDLSubsystems));
  if (s->library) dlclose(s->library);
}
static inline bool INPUT_INS(SDLStart)(void) {
  INPUT_INS(SDLState) *s = &INPUT_INS(sdlState);
  uint32_t initialized;
  if (s->ready) return true;
#if defined(__APPLE__)
  s->library = dlopen("libSDL3.0.dylib", RTLD_LOCAL | RTLD_LAZY);
  if (!s->library) s->library = dlopen("/opt/homebrew/lib/libSDL3.0.dylib", RTLD_LOCAL | RTLD_LAZY);
  if (!s->library) s->library = dlopen("/usr/local/lib/libSDL3.0.dylib", RTLD_LOCAL | RTLD_LAZY);
  if (!s->library) s->library = dlopen("/Library/Frameworks/SDL3.framework/SDL3", RTLD_LOCAL | RTLD_LAZY);
#else
  s->library = dlopen("libSDL3.so.0", RTLD_LOCAL | RTLD_LAZY);
  if (!s->library) s->library = dlopen("libSDL3.so", RTLD_LOCAL | RTLD_LAZY);
#endif
  if (!s->library) return false;
#define COSMERON_INPUT_SDL_LOAD(FIELD, NAME) COSMERON_INPUT_DLSYM(s->library, s->FIELD, NAME); if (!s->FIELD) goto unavailable
  COSMERON_INPUT_SDL_LOAD(init, "SDL_InitSubSystem");
  COSMERON_INPUT_SDL_LOAD(wasInit, "SDL_WasInit");
  COSMERON_INPUT_SDL_LOAD(quit, "SDL_QuitSubSystem");
  COSMERON_INPUT_SDL_LOAD(freeMemory, "SDL_free");
  COSMERON_INPUT_SDL_LOAD(list, "SDL_GetJoysticks");
  COSMERON_INPUT_SDL_LOAD(update, "SDL_UpdateJoysticks");
  COSMERON_INPUT_SDL_LOAD(open, "SDL_OpenJoystick");
  COSMERON_INPUT_SDL_LOAD(close, "SDL_CloseJoystick");
  COSMERON_INPUT_SDL_LOAD(name, "SDL_GetJoystickName");
  COSMERON_INPUT_SDL_LOAD(buttons, "SDL_GetNumJoystickButtons");
  COSMERON_INPUT_SDL_LOAD(axes, "SDL_GetNumJoystickAxes");
  COSMERON_INPUT_SDL_LOAD(hats, "SDL_GetNumJoystickHats");
  COSMERON_INPUT_SDL_LOAD(button, "SDL_GetJoystickButton");
  COSMERON_INPUT_SDL_LOAD(axis, "SDL_GetJoystickAxis");
  COSMERON_INPUT_SDL_LOAD(hat, "SDL_GetJoystickHat");
  COSMERON_INPUT_SDL_LOAD(vendor, "SDL_GetJoystickVendor");
  COSMERON_INPUT_SDL_LOAD(product, "SDL_GetJoystickProduct");
  COSMERON_INPUT_SDL_LOAD(rumble, "SDL_RumbleJoystick");
  COSMERON_INPUT_SDL_LOAD(properties, "SDL_GetJoystickProperties");
  COSMERON_INPUT_SDL_LOAD(propertyBool, "SDL_GetBooleanProperty");
  COSMERON_INPUT_SDL_LOAD(isGamepad, "SDL_IsGamepad");
  COSMERON_INPUT_SDL_LOAD(openGamepad, "SDL_OpenGamepad");
  COSMERON_INPUT_SDL_LOAD(closeGamepad, "SDL_CloseGamepad");
  COSMERON_INPUT_SDL_LOAD(gamepadButton, "SDL_GetGamepadButton");
  COSMERON_INPUT_SDL_LOAD(gamepadAxis, "SDL_GetGamepadAxis");
  COSMERON_INPUT_SDL_LOAD(joystickEvents, "SDL_SetJoystickEventsEnabled");
  COSMERON_INPUT_SDL_LOAD(gamepadEvents, "SDL_SetGamepadEventsEnabled");
#undef COSMERON_INPUT_SDL_LOAD
  initialized = s->wasInit(INPUT_INS(SDLSubsystems));
  if (!s->init(INPUT_INS(SDLSubsystems))) goto unavailable;
  s->ready = true;
  if (atexit(INPUT_INS(SDLCleanup)) != 0) { s->quit(INPUT_INS(SDLSubsystems)); s->ready = false; goto unavailable; }
  /* When we own the first subsystem initialization, use polling without
   * accumulating an unconsumed SDL event queue. Never change an existing host. */
  if (!initialized) { s->joystickEvents(false); s->gamepadEvents(false); }
  return true;
unavailable:
  dlclose(s->library); memset(s, 0, sizeof(*s)); return false;
}
static inline bool INPUT_INS(SDLUpdate)(void) {
  INPUT_INS(SDLState) *s = &INPUT_INS(sdlState);
  uint32_t *ids;
  int count = 0, k;
  size_t i;
  if (!INPUT_INS(SDLStart)()) return false;
  s->update(); ids = s->list(&count);
  if (!ids && count != 0) return false;
  for (i = 0; i < INPUT_DEVICE_CAPACITY; ++i) if (s->ids[i]) {
    bool found = false;
    for (k = 0; k < count; ++k) if (ids[k] == s->ids[i]) found = true;
    if (!found) INPUT_INS(SDLClose)(i);
  }
  for (k = 0; k < count; ++k) {
    bool found = false;
    for (i = 0; i < INPUT_DEVICE_CAPACITY; ++i) if (s->ids[i] == ids[k]) found = true;
    if (found) continue;
    for (i = 0; i < INPUT_DEVICE_CAPACITY; ++i) if (!s->ids[i] && !INPUT_INS(state).devices[i].connected) {
      INPUT_INS(Device) *d;
      int n;
      s->joysticks[i] = s->open(ids[k]); if (!s->joysticks[i]) break;
      d = INPUT_INS(Connect)(i, s->name(s->joysticks[i]));
      if (!d) { s->close(s->joysticks[i]); s->joysticks[i] = NULL; break; }
      s->ids[i] = ids[k];
      n = s->buttons(s->joysticks[i]); d->buttonCount = n > 0 ? (uint32_t)n : 0;
      if (d->buttonCount > INPUT_CONST(MAX_BUTTONS)) d->buttonCount = INPUT_CONST(MAX_BUTTONS);
      n = s->axes(s->joysticks[i]); d->axisCount = n > 0 ? (uint32_t)n : 0;
      if (d->axisCount > INPUT_CONST(MAX_AXES)) d->axisCount = INPUT_CONST(MAX_AXES);
      n = s->hats(s->joysticks[i]); d->hatCount = n > 0 ? (uint32_t)n : 0;
      if (d->hatCount > INPUT_CONST(MAX_HATS)) d->hatCount = INPUT_CONST(MAX_HATS);
      d->vendor = s->vendor(s->joysticks[i]); d->product = s->product(s->joysticks[i]);
      d->rumble = s->propertyBool(s->properties(s->joysticks[i]), "SDL.joystick.cap.rumble", false);
      if (s->isGamepad(ids[k])) s->gamepads[i] = s->openGamepad(ids[k]);
      d->mapped = d->standardProvided = s->gamepads[i] != NULL;
      break;
    }
  }
  s->freeMemory(ids);
  for (i = 0; i < INPUT_DEVICE_CAPACITY; ++i) if (s->ids[i]) {
    INPUT_INS(Device) *d = &INPUT_INS(state).devices[i];
    unsigned b;
    for (b = 0; b < d->buttonCount; ++b) INPUT_INS(ControllerButton)(d, b, s->button(s->joysticks[i], (int)b));
    for (b = 0; b < d->axisCount; ++b) {
      int16_t value = s->axis(s->joysticks[i], (int)b);
      INPUT_INS(Axis)(d, b, value / (value < 0 ? 32768.0f : 32767.0f));
    }
    for (b = 0; b < d->hatCount; ++b) {
      uint8_t v = s->hat(s->joysticks[i], (int)b);
      unsigned mask = (v & 1 ? INPUT_CONST(HAT_UP) : 0) | (v & 2 ? INPUT_CONST(HAT_RIGHT) : 0) | (v & 4 ? INPUT_CONST(HAT_DOWN) : 0) | (v & 8 ? INPUT_CONST(HAT_LEFT) : 0);
      INPUT_INS(Hat)(d, b, (INPUT_TYPE(Hat))mask);
    }
    if (s->gamepads[i]) {
      /* SDL3 standard button ordering (SDL_gamepad.h). */
      const int buttons[] = { 0,1,2,3,9,10,7,8,4,6,5,11,12,13,14 };
      for (b = 0; b < INPUT_CONST(GAMEPAD_BUTTON_COUNT); ++b)
        INPUT_INS(Transition)(&d->standardButtons[b], !INPUT_INS(state).unfocused && s->gamepadButton(s->gamepads[i], buttons[b]));
      for (b = 0; b < INPUT_CONST(GAMEPAD_AXIS_COUNT); ++b) {
        int16_t value = s->gamepadAxis(s->gamepads[i], (int)b);
        float axis = value / (value < 0 ? 32768.0f : 32767.0f);
        if (b == INPUT_CONST(GAMEPAD_AXIS_LEFT_Y) || b == INPUT_CONST(GAMEPAD_AXIS_RIGHT_Y)) axis = -axis;
        d->standardAxes[b] = INPUT_INS(state).unfocused ? 0 : axis;
      }
    }
  }
  INPUT_INS(state).capabilities |= INPUT_CONST(CAPABILITY_CONTROLLERS);
  return true;
}
static inline OPSTATUS INPUT_INS(SDLRumble)(INPUT_INS(Device) *d, float low, float high, uint32_t ms) {
  INPUT_INS(SDLState) *s = &INPUT_INS(sdlState);
  size_t slot = (size_t)(d - INPUT_INS(state).devices);
  if (!d->rumble || !s->joysticks[slot]) return STATUS_CONST(NOT_SUPPORTED);
  return s->rumble(s->joysticks[slot], ms ? (uint16_t)(low * UINT16_MAX) : 0, ms ? (uint16_t)(high * UINT16_MAX) : 0, ms) ? STATUS_CONST(SUCCESS) : STATUS_CONST(GENERIC_ERROR);
}
