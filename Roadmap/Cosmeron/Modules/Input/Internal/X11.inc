#pragma once
#include "Dynamic.inc"
#include "PhysicalKeys.inc"
/* Xlib ABI uses an opaque Display and unsigned-long XIDs/KeySyms. No X11
 * development headers or link flags are needed for these stable entry points. */
typedef struct INPUT_INS(XDisplay) INPUT_INS(XDisplay);
typedef struct INPUT_INS(XState) {
  void *library;
  INPUT_INS(XDisplay) *display;
  INPUT_INS(XDisplay) *(*open)(const char *);
  int (*close)(INPUT_INS(XDisplay) *);
  unsigned long (*root)(INPUT_INS(XDisplay) *);
  int (*pointer)(INPUT_INS(XDisplay) *, unsigned long, unsigned long *, unsigned long *, int *, int *, int *, int *, unsigned *);
  int (*keymap)(INPUT_INS(XDisplay) *, char *);
  unsigned long (*keysym)(const char *);
  unsigned char (*keycode)(INPUT_INS(XDisplay) *, unsigned long);
  bool started;
} INPUT_INS(XState);
#if defined(INPUT_EXTERNAL_STORAGE) && !defined(INPUT_DEFINE_STORAGE)
COSMERON_INPUT_STORAGE INPUT_INS(XState) INPUT_INS(xState);
#else
COSMERON_INPUT_STORAGE INPUT_INS(XState) INPUT_INS(xState) = {0};
#endif
static void INPUT_INS(XCleanup)(void) {
  if (INPUT_INS(xState).display) INPUT_INS(xState).close(INPUT_INS(xState).display);
  if (INPUT_INS(xState).library) dlclose(INPUT_INS(xState).library);
}
static inline bool INPUT_INS(XStart)(void) {
  INPUT_INS(XState) *s = &INPUT_INS(xState);
  if (s->started) return s->display != NULL;
  /* Never mistake XWayland coordinates for the compositor's global pointer. */
  if (getenv("WAYLAND_DISPLAY") || !getenv("DISPLAY")) return false;
  s->library = dlopen("libX11.so.6", RTLD_LOCAL | RTLD_LAZY);
  if (!s->library) s->library = dlopen("libX11.so", RTLD_LOCAL | RTLD_LAZY);
  if (!s->library) return false;
  COSMERON_INPUT_DLSYM(s->library, s->open, "XOpenDisplay");
  COSMERON_INPUT_DLSYM(s->library, s->close, "XCloseDisplay");
  COSMERON_INPUT_DLSYM(s->library, s->root, "XDefaultRootWindow");
  COSMERON_INPUT_DLSYM(s->library, s->pointer, "XQueryPointer");
  COSMERON_INPUT_DLSYM(s->library, s->keymap, "XQueryKeymap");
  COSMERON_INPUT_DLSYM(s->library, s->keysym, "XStringToKeysym");
  COSMERON_INPUT_DLSYM(s->library, s->keycode, "XKeysymToKeycode");
  if (!s->open || !s->close || !s->root || !s->pointer || !s->keymap || !s->keysym || !s->keycode) {
    dlclose(s->library); memset(s, 0, sizeof(*s)); return false;
  }
  s->display = s->open(NULL);
  if (!s->display) { dlclose(s->library); memset(s, 0, sizeof(*s)); return false; }
  if (atexit(INPUT_INS(XCleanup)) != 0) { INPUT_INS(XCleanup)(); memset(s, 0, sizeof(*s)); return false; }
  s->started = true; return true;
}
static inline bool INPUT_INS(XUpdate)(bool keyboard, bool mouse) {
  INPUT_INS(XState) *s = &INPUT_INS(xState);
  unsigned long root, child;
  int rx, ry, wx, wy;
  unsigned mask;
  if (INPUT_INS(state).captureMode == INPUT_CONST(CAPTURE_APPLICATION) || !INPUT_INS(XStart)()) return false;
  if (keyboard) {
    char keys[32] = {0};
    if (s->keymap(s->display, keys)) {
#define COSMERON_INPUT_XKEY(NAME, SCAN, MAC, XNAME) do { \
  unsigned char code = s->keycode(s->display, s->keysym(XNAME)); \
  if (code) INPUT_INS(Key)(LIB_PREFIX_CONST(NAME), (keys[code / 8] & (1u << (code % 8))) != 0, false); \
} while (0);
      COSMERON_INPUT_PHYSICAL_TABLE(COSMERON_INPUT_XKEY)
#undef COSMERON_INPUT_XKEY
      INPUT_INS(state).capabilities |= INPUT_CONST(CAPABILITY_KEYBOARD) | INPUT_CONST(CAPABILITY_LOGICAL_KEYS);
    }
  }
  if (s->pointer(s->display, s->root(s->display), &root, &child, &rx, &ry, &wx, &wy, &mask)) {
    INPUT_INS(state).x = (float)rx; INPUT_INS(state).y = (float)ry;
    INPUT_INS(state).capabilities |= INPUT_CONST(CAPABILITY_POINTER_POSITION);
    if (mouse) {
      /* X core button masks 1/2/3 are left/middle/right. */
      INPUT_INS(MouseButton)(INPUT_CONST(MOUSE_BUTTON_LEFT), (mask & (1u << 8)) != 0);
      INPUT_INS(MouseButton)(INPUT_CONST(MOUSE_BUTTON_MIDDLE), (mask & (1u << 9)) != 0);
      INPUT_INS(MouseButton)(INPUT_CONST(MOUSE_BUTTON_RIGHT), (mask & (1u << 10)) != 0);
      INPUT_INS(state).capabilities |= INPUT_CONST(CAPABILITY_MOUSE_BUTTONS);
    }
  }
  return true;
}
