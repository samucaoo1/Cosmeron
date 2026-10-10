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
  float gamepadScale[INPUT_CONST(GAMEPAD_AXIS_COUNT)];
  float gamepadOffset[INPUT_CONST(GAMEPAD_AXIS_COUNT)];
  bool customMapping, standardProvided;
  bool customButtons[INPUT_CONST(GAMEPAD_BUTTON_COUNT)], customAxes[INPUT_CONST(GAMEPAD_AXIS_COUNT)];
  INPUT_INS(Button) standardButtons[INPUT_CONST(GAMEPAD_BUTTON_COUNT)];
  float standardAxes[INPUT_CONST(GAMEPAD_AXIS_COUNT)];
  uint16_t vendor, product;

} INPUT_INS(Device);
typedef struct INPUT_INS(State) {
  INPUT_INS(Button) keys[LIB_PREFIX_CONST(INPUT_KEY_COUNT)];
  INPUT_INS(Button) mouse[INPUT_CONST(MOUSE_BUTTON_COUNT)];
  INPUT_INS(Button) physical[LIB_PREFIX_CONST(INPUT_KEY_COUNT)];
  bool sourceKeys[INPUT_DEVICE_CAPACITY][LIB_PREFIX_CONST(INPUT_KEY_COUNT)];
  bool sourcePhysical[INPUT_DEVICE_CAPACITY][LIB_PREFIX_CONST(INPUT_KEY_COUNT)];
  bool sourceMouse[INPUT_DEVICE_CAPACITY][INPUT_CONST(MOUSE_BUTTON_COUNT)];
  INPUT_TYPE(TEvent) pending[INPUT_EVENT_CAPACITY];
  size_t pendingCount;
  bool updated, unfocused;
  INPUT_TYPE(CaptureMode) captureMode;
  INPUT_TYPE(Capabilities) capabilities;
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
static inline void INPUT_INS(KeyEvent)(INPUT_TYPE(Key) key, bool down, bool repeat, uint32_t source, INPUT_TYPE(Key) physical) {
  INPUT_TYPE(TEvent) e = {0};
  INPUT_INS(Button) *b;
  if (key <= LIB_PREFIX_CONST(INPUT_KEY_UNKNOWN) || key >= LIB_PREFIX_CONST(INPUT_KEY_COUNT)) return;
  b = &INPUT_INS(state).keys[key];
  if (!repeat && b->down == down) return;
  INPUT_INS(Transition)(b, down);
  b->repeated |= repeat;
  e.type = repeat ? INPUT_CONST(EVENT_KEY_REPEAT) :
    (down ? INPUT_CONST(EVENT_KEY_PRESS) : INPUT_CONST(EVENT_KEY_RELEASE));
  e.data.key = key; e.source = source; e.physicalKey = physical;
  INPUT_INS(Push)(e);
}
static inline void INPUT_INS(Key)(INPUT_TYPE(Key) key, bool down, bool repeat) {
  INPUT_INS(KeyEvent)(key, down, repeat, 0, LIB_PREFIX_CONST(INPUT_KEY_UNKNOWN));
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
  for (i = 0; i < INPUT_CONST(GAMEPAD_AXIS_COUNT); ++i) {
    d->gamepadAxes[i] = -1; d->gamepadScale[i] = 1;
  }
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
  if (INPUT_INS(state).unfocused) down = false;
  if (index >= d->buttonCount || d->buttons[index].down == down) return;
  INPUT_INS(Transition)(&d->buttons[index], down);
  e.type = down ? INPUT_CONST(EVENT_CONTROLLER_BUTTON_PRESS) : INPUT_CONST(EVENT_CONTROLLER_BUTTON_RELEASE);
  e.controller = d->id; e.data.button = index;
  INPUT_INS(Push)(e);
}
static inline void INPUT_INS(Axis)(INPUT_INS(Device) *d, unsigned index, float value) {
  INPUT_TYPE(TEvent) e = {0};
  if (INPUT_INS(state).unfocused) value = 0;
  if (index >= d->axisCount || d->axes[index] == value) return;
  d->axes[index] = value;
  e.type = INPUT_CONST(EVENT_CONTROLLER_AXIS); e.controller = d->id;
  e.data.axis.index = index; e.data.axis.value = value;
  INPUT_INS(Push)(e);
}
static inline void INPUT_INS(Hat)(INPUT_INS(Device) *d, unsigned index, INPUT_TYPE(Hat) value) {
  INPUT_TYPE(TEvent) e = {0};
  if (INPUT_INS(state).unfocused) value = INPUT_CONST(HAT_CENTERED);
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
  for (i = 0; i < LIB_PREFIX_CONST(INPUT_KEY_COUNT); ++i) INPUT_INS(ClearButton)(&INPUT_INS(state).physical[i]);
  for (i = 0; i < INPUT_DEVICE_CAPACITY; ++i)
    for (j = 0; j < INPUT_CONST(GAMEPAD_BUTTON_COUNT); ++j)
      INPUT_INS(ClearButton)(&INPUT_INS(state).devices[i].standardButtons[j]);
  INPUT_INS(state).dx = INPUT_INS(state).dy = INPUT_INS(state).sx = INPUT_INS(state).sy = 0;
}

static inline void INPUT_INS(Physical)(INPUT_TYPE(Key) key, bool down, bool repeat) {
  if (key <= LIB_PREFIX_CONST(INPUT_KEY_UNKNOWN) || key >= LIB_PREFIX_CONST(INPUT_KEY_COUNT)) return;
  INPUT_INS(Transition)(&INPUT_INS(state).physical[key], down);
  INPUT_INS(state).physical[key].repeated |= repeat;
}
static inline bool INPUT_INS(SourceHeld)(bool physical, unsigned key) {
  size_t i;
  for (i = 0; i < INPUT_DEVICE_CAPACITY; ++i)
    if (physical ? INPUT_INS(state).sourcePhysical[i][key] : INPUT_INS(state).sourceKeys[i][key]) return true;
  return false;
}
static inline void INPUT_INS(SourceKey)(uint32_t source, INPUT_TYPE(Key) key,
    INPUT_TYPE(Key) physical, bool down, bool repeat) {
  INPUT_INS(State) *s = &INPUT_INS(state);
  if (source >= INPUT_DEVICE_CAPACITY || s->unfocused) return;
  if (key > LIB_PREFIX_CONST(INPUT_KEY_UNKNOWN) && key < LIB_PREFIX_CONST(INPUT_KEY_COUNT)) {
    s->sourceKeys[source][key] = down;
    INPUT_INS(KeyEvent)(key, INPUT_INS(SourceHeld)(false, (unsigned)key), repeat, source, physical);
  }
  if (physical > LIB_PREFIX_CONST(INPUT_KEY_UNKNOWN) && physical < LIB_PREFIX_CONST(INPUT_KEY_COUNT)) {
    s->sourcePhysical[source][physical] = down;
    {
      bool held = INPUT_INS(SourceHeld)(true, (unsigned)physical);
      bool changed = INPUT_INS(state).physical[physical].down != held;
      INPUT_INS(Physical)(physical, held, repeat);
      if (key == LIB_PREFIX_CONST(INPUT_KEY_UNKNOWN) && (changed || repeat)) {
        INPUT_TYPE(TEvent) e = {0}; e.source = source; e.physicalKey = physical;
        e.type = repeat ? INPUT_CONST(EVENT_KEY_REPEAT) : (held ? INPUT_CONST(EVENT_KEY_PRESS) : INPUT_CONST(EVENT_KEY_RELEASE));
        e.data.key = key; INPUT_INS(Push)(e);
      }
    }
  }
}
static inline void INPUT_INS(SourceMouse)(uint32_t source, unsigned button, bool down) {
  size_t i;
  bool held = false;
  if (source >= INPUT_DEVICE_CAPACITY || button >= INPUT_CONST(MOUSE_BUTTON_COUNT) || INPUT_INS(state).unfocused) return;
  INPUT_INS(state).sourceMouse[source][button] = down;
  for (i = 0; i < INPUT_DEVICE_CAPACITY; ++i) held |= INPUT_INS(state).sourceMouse[i][button];
  {
    size_t count = INPUT_INS(state).count;
    INPUT_INS(MouseButton)(button, held);
    if (INPUT_INS(state).count > count)
      INPUT_INS(state).events[(INPUT_INS(state).head + INPUT_INS(state).count - 1) % INPUT_EVENT_CAPACITY].source = source;
  }
}
static inline void INPUT_INS(RemoveSource)(uint32_t source) {
  unsigned k;
  if (source >= INPUT_DEVICE_CAPACITY) return;
  for (k = 1; k < LIB_PREFIX_CONST(INPUT_KEY_COUNT); ++k) {
    INPUT_INS(SourceKey)(source, (INPUT_TYPE(Key))k, (INPUT_TYPE(Key))k, false, false);
  }
  for (k = 0; k < INPUT_CONST(MOUSE_BUTTON_COUNT); ++k) INPUT_INS(SourceMouse)(source, k, false);
}
static inline void INPUT_INS(Focus)(bool focused) {
  size_t i, j;
  INPUT_TYPE(TEvent) e = {0};
  if (INPUT_INS(state).unfocused == !focused) return;
  if (!focused) {
    for (i = 0; i < INPUT_DEVICE_CAPACITY; ++i) INPUT_INS(RemoveSource)((uint32_t)i);
    for (i = 1; i < LIB_PREFIX_CONST(INPUT_KEY_COUNT); ++i) {
      INPUT_INS(Key)((INPUT_TYPE(Key))i, false, false);
      INPUT_INS(Physical)((INPUT_TYPE(Key))i, false, false);
    }
    for (i = 0; i < INPUT_CONST(MOUSE_BUTTON_COUNT); ++i) INPUT_INS(MouseButton)((unsigned)i, false);
    for (i = 0; i < INPUT_DEVICE_CAPACITY; ++i) {
      INPUT_INS(Device) *d = &INPUT_INS(state).devices[i];
      for (j = 0; j < d->buttonCount; ++j) INPUT_INS(ControllerButton)(d, (unsigned)j, false);
      for (j = 0; j < d->axisCount; ++j) INPUT_INS(Axis)(d, (unsigned)j, 0);
    }
  }
  INPUT_INS(state).unfocused = !focused;
  e.type = focused ? INPUT_CONST(EVENT_FOCUS_GAINED) : INPUT_CONST(EVENT_FOCUS_LOST);
  INPUT_INS(Push)(e);
}

static inline void INPUT_INS(ApplyPending)(void) {
  INPUT_INS(State) *s = &INPUT_INS(state);
  size_t i;
  for (i = 0; i < s->pendingCount; ++i) {
    INPUT_TYPE(TEvent) *e = &s->pending[i];
    if (e->type == INPUT_CONST(EVENT_FOCUS_GAINED)) { INPUT_INS(Focus)(true); continue; }
    if (e->type == INPUT_CONST(EVENT_FOCUS_LOST)) { INPUT_INS(Focus)(false); continue; }
    if (e->type == INPUT_CONST(EVENT_SOURCE_REMOVED)) { INPUT_INS(RemoveSource)(e->source); continue; }
    if (s->unfocused) continue;
    switch (e->type) {
    case INPUT_CONST(EVENT_KEY_PRESS): case INPUT_CONST(EVENT_KEY_RELEASE): case INPUT_CONST(EVENT_KEY_REPEAT):
      INPUT_INS(SourceKey)(e->source, e->data.key, e->physicalKey, e->type != INPUT_CONST(EVENT_KEY_RELEASE), e->type == INPUT_CONST(EVENT_KEY_REPEAT));
      if (e->data.key != LIB_PREFIX_CONST(INPUT_KEY_UNKNOWN)) s->capabilities |= INPUT_CONST(CAPABILITY_LOGICAL_KEYS);
      if (e->physicalKey != LIB_PREFIX_CONST(INPUT_KEY_UNKNOWN)) s->capabilities |= INPUT_CONST(CAPABILITY_PHYSICAL_KEYS);
      s->capabilities |= INPUT_CONST(CAPABILITY_KEYBOARD); break;
    case INPUT_CONST(EVENT_MOUSE_BUTTON_PRESS): case INPUT_CONST(EVENT_MOUSE_BUTTON_RELEASE):
      INPUT_INS(SourceMouse)(e->source, (unsigned)e->data.mouseButton, e->type == INPUT_CONST(EVENT_MOUSE_BUTTON_PRESS));
      s->capabilities |= INPUT_CONST(CAPABILITY_MOUSE_BUTTONS); break;
    case INPUT_CONST(EVENT_MOUSE_MOVE): case INPUT_CONST(EVENT_MOUSE_SCROLL):
      INPUT_INS(Motion)(e->data.motion.x, e->data.motion.y, e->type == INPUT_CONST(EVENT_MOUSE_SCROLL));
      s->capabilities |= e->type == INPUT_CONST(EVENT_MOUSE_SCROLL) ? INPUT_CONST(CAPABILITY_SCROLL) | INPUT_CONST(CAPABILITY_PRECISE_SCROLL) : INPUT_CONST(CAPABILITY_RAW_MOTION); break;
    case INPUT_CONST(EVENT_MOUSE_POSITION):
      s->x = e->data.motion.x; s->y = e->data.motion.y;
      s->capabilities |= INPUT_CONST(CAPABILITY_POINTER_POSITION); INPUT_INS(Push)(*e); break;
    default: break;
    }
  }
  s->pendingCount = 0;
}
