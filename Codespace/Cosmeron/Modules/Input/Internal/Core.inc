#pragma once
/* Shared storage: identical namespace/configuration in every translation unit.
 * Explicit storage mode is available for other C11 compilers. */
typedef struct INPUT_INS(Button) {
  bool down, pressed, released, repeated;
} INPUT_INS(Button);
typedef struct INPUT_INS(Device) {
  INPUT_TYPE(ControllerId) id;
  bool connected, mapped, rumble;
  char name[INPUT_CONST(NAME_CAPACITY)];
  uint32_t buttonCount, axisCount, hatCount;
  INPUT_INS(Button) buttons[INPUT_CONST(MAX_BUTTONS)];
  float axes[INPUT_CONST(MAX_AXES)];
  INPUT_TYPE(Hat) hats[INPUT_CONST(MAX_HATS)];
  int gamepadButtons[INPUT_CONST(GAMEPAD_BUTTON_COUNT)];
  int gamepadAxes[INPUT_CONST(GAMEPAD_AXIS_COUNT)];
} INPUT_INS(Device);
typedef struct INPUT_INS(State) {
  INPUT_INS(Button) keys[LIB_PREFIX_CONST(INPUT_KEY_COUNT)];
  INPUT_INS(Button) mouse[INPUT_CONST(MOUSE_BUTTON_COUNT)];
  float x, y, dx, dy, sx, sy;
  INPUT_INS(Device) devices[INPUT_DEVICE_CAPACITY];
  INPUT_TYPE(TEvent) events[INPUT_EVENT_CAPACITY];
  size_t head, count;
  uint64_t sequence, dropped, nextId;
} INPUT_INS(State);
#if defined(INPUT_EXTERNAL_STORAGE)
# if defined(INPUT_DEFINE_STORAGE)
#  define COSMERON_INPUT_STORAGE
# else
#  define COSMERON_INPUT_STORAGE extern
# endif
#elif defined(_MSC_VER)
# define COSMERON_INPUT_STORAGE __declspec(selectany)
#elif defined(__GNUC__) || defined(__clang__)
# define COSMERON_INPUT_STORAGE __attribute__((weak))
#else
# error "Define INPUT_EXTERNAL_STORAGE in all TUs and INPUT_DEFINE_STORAGE in one TU"
#endif
#if defined(INPUT_EXTERNAL_STORAGE) && !defined(INPUT_DEFINE_STORAGE)
COSMERON_INPUT_STORAGE INPUT_INS(State) INPUT_INS(state);
#else
COSMERON_INPUT_STORAGE INPUT_INS(State) INPUT_INS(state) = {0};
#endif

static inline void INPUT_INS(Push)(INPUT_TYPE(TEvent) event) {
  INPUT_INS(State) *s = &INPUT_INS(state);
  event.sequence = ++s->sequence;
  if (s->count == INPUT_EVENT_CAPACITY) { ++s->dropped; return; }
  s->events[(s->head + s->count) % INPUT_EVENT_CAPACITY] = event;
  ++s->count;
}
static inline void INPUT_INS(Transition)(INPUT_INS(Button) *b, bool down) {
  if (b->down == down) return;
  b->down = down;
  if (down) b->pressed = true; else b->released = true;
}
static inline void INPUT_INS(Key)(INPUT_TYPE(Key) key, bool down, bool repeat) {
  INPUT_TYPE(TEvent) e = {0};
  INPUT_INS(Button) *b;
  if (key <= LIB_PREFIX_CONST(INPUT_KEY_UNKNOWN) || key >= LIB_PREFIX_CONST(INPUT_KEY_COUNT)) return;
  b = &INPUT_INS(state).keys[key];
  if (!repeat && b->down == down) return;
  INPUT_INS(Transition)(b, down);
  b->repeated |= repeat;
  e.type = repeat ? INPUT_CONST(EVENT_KEY_REPEAT) :
    (down ? INPUT_CONST(EVENT_KEY_PRESS) : INPUT_CONST(EVENT_KEY_RELEASE));
  e.data.key = key;
  INPUT_INS(Push)(e);
}
static inline void INPUT_INS(MouseButton)(unsigned button, bool down) {
  INPUT_TYPE(TEvent) e = {0};
  INPUT_INS(Button) *b;
  if (button >= INPUT_CONST(MOUSE_BUTTON_COUNT)) return;
  b = &INPUT_INS(state).mouse[button];
  if (b->down == down) return;
  INPUT_INS(Transition)(b, down);
  e.type = down ? INPUT_CONST(EVENT_MOUSE_BUTTON_PRESS) : INPUT_CONST(EVENT_MOUSE_BUTTON_RELEASE);
  e.data.mouseButton = (INPUT_TYPE(MouseButton))button;
  INPUT_INS(Push)(e);
}
static inline void INPUT_INS(Motion)(float x, float y, bool scroll) {
  INPUT_TYPE(TEvent) e = {0};
  if (x == 0 && y == 0) return;
  if (scroll) { INPUT_INS(state).sx += x; INPUT_INS(state).sy += y; }
  else { INPUT_INS(state).dx += x; INPUT_INS(state).dy += y; }
  e.type = scroll ? INPUT_CONST(EVENT_MOUSE_SCROLL) : INPUT_CONST(EVENT_MOUSE_MOVE);
  e.data.motion.x = x; e.data.motion.y = y;
  INPUT_INS(Push)(e);
}
static inline INPUT_INS(Device) *INPUT_INS(Find)(INPUT_TYPE(ControllerId) id) {
  size_t i;
  if (!id) return NULL;
  for (i = 0; i < INPUT_DEVICE_CAPACITY; ++i)
    if (INPUT_INS(state).devices[i].connected && INPUT_INS(state).devices[i].id == id)
      return &INPUT_INS(state).devices[i];
  return NULL;
}
static inline INPUT_INS(Device) *INPUT_INS(Connect)(size_t slot, const char *name) {
  INPUT_INS(Device) *d = &INPUT_INS(state).devices[slot];
  INPUT_TYPE(TEvent) e = {0};
  size_t i;
  memset(d, 0, sizeof(*d));
  /* Never reuse a stale ID, even after exhausting the counter. */
  if (INPUT_INS(state).nextId == UINT64_MAX) return NULL;
  d->id = ++INPUT_INS(state).nextId; d->connected = true;
  for (i = 0; i < INPUT_CONST(GAMEPAD_BUTTON_COUNT); ++i) d->gamepadButtons[i] = -1;
  for (i = 0; i < INPUT_CONST(GAMEPAD_AXIS_COUNT); ++i) d->gamepadAxes[i] = -1;
  if (name) {
    strncpy(d->name, name, sizeof(d->name) - 1);
    d->name[sizeof(d->name) - 1] = '\0';
  }
  e.type = INPUT_CONST(EVENT_DEVICE_CONNECTED); e.controller = d->id;
  INPUT_INS(Push)(e);
  return d;
}
static inline void INPUT_INS(Disconnect)(size_t slot) {
  INPUT_INS(Device) *d = &INPUT_INS(state).devices[slot];
  INPUT_TYPE(TEvent) e = {0};
  if (!d->connected) return;
  e.type = INPUT_CONST(EVENT_DEVICE_DISCONNECTED); e.controller = d->id;
  INPUT_INS(Push)(e);
  memset(d, 0, sizeof(*d));
}
static inline void INPUT_INS(ControllerButton)(INPUT_INS(Device) *d, unsigned index, bool down) {
  INPUT_TYPE(TEvent) e = {0};
  if (index >= d->buttonCount || d->buttons[index].down == down) return;
  INPUT_INS(Transition)(&d->buttons[index], down);
  e.type = down ? INPUT_CONST(EVENT_CONTROLLER_BUTTON_PRESS) : INPUT_CONST(EVENT_CONTROLLER_BUTTON_RELEASE);
  e.controller = d->id; e.data.button = index;
  INPUT_INS(Push)(e);
}
static inline void INPUT_INS(Axis)(INPUT_INS(Device) *d, unsigned index, float value) {
  INPUT_TYPE(TEvent) e = {0};
  if (index >= d->axisCount || d->axes[index] == value) return;
  d->axes[index] = value;
  e.type = INPUT_CONST(EVENT_CONTROLLER_AXIS); e.controller = d->id;
  e.data.axis.index = index; e.data.axis.value = value;
  INPUT_INS(Push)(e);
}
static inline void INPUT_INS(Hat)(INPUT_INS(Device) *d, unsigned index, INPUT_TYPE(Hat) value) {
  INPUT_TYPE(TEvent) e = {0};
  if (index >= d->hatCount || d->hats[index] == value) return;
  d->hats[index] = value;
  e.type = INPUT_CONST(EVENT_CONTROLLER_HAT); e.controller = d->id;
  e.data.hat.index = index; e.data.hat.value = value;
  INPUT_INS(Push)(e);
}
static inline void INPUT_INS(ClearButton)(INPUT_INS(Button) *b) {
  b->pressed = b->released = b->repeated = false;
}
static inline void INPUT_INS(Begin)(void) {
  size_t i, j;
  for (i = 0; i < LIB_PREFIX_CONST(INPUT_KEY_COUNT); ++i) INPUT_INS(ClearButton)(&INPUT_INS(state).keys[i]);
  for (i = 0; i < INPUT_CONST(MOUSE_BUTTON_COUNT); ++i) INPUT_INS(ClearButton)(&INPUT_INS(state).mouse[i]);
  for (i = 0; i < INPUT_DEVICE_CAPACITY; ++i)
    for (j = 0; j < INPUT_CONST(MAX_BUTTONS); ++j)
      INPUT_INS(ClearButton)(&INPUT_INS(state).devices[i].buttons[j]);
  INPUT_INS(state).dx = INPUT_INS(state).dy = INPUT_INS(state).sx = INPUT_INS(state).sy = 0;
}
