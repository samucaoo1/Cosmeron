#pragma once
#include <linux/input.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <dirent.h>
#include <stdio.h>
#include <time.h>
#include <limits.h>

enum { INPUT_INS(BitsPerByte) = CHAR_BIT, INPUT_INS(PathCapacity) = 256,
       INPUT_INS(EffectNone) = -1, INPUT_INS(ReadBudget) = 4096 };
typedef struct INPUT_INS(LinuxDevice) {
  int fd, effect;
  bool used, writable, controller, dropped;
  dev_t deviceNumber;
  ino_t inode;
  char path[INPUT_INS(PathCapacity)];
  unsigned char keys[(KEY_CNT + CHAR_BIT - 1) / CHAR_BIT];
  int buttonMap[KEY_CNT], axisMap[ABS_CNT];
  struct input_absinfo abs[ABS_CNT];
  int hatX[INPUT_CONST(MAX_HATS)], hatY[INPUT_CONST(MAX_HATS)];
} INPUT_INS(LinuxDevice);
typedef struct INPUT_INS(LinuxState) {
  INPUT_INS(LinuxDevice) devices[INPUT_DEVICE_CAPACITY];
  time_t scanTime;
  bool started;
} INPUT_INS(LinuxState);
#if defined(INPUT_EXTERNAL_STORAGE) && !defined(INPUT_DEFINE_STORAGE)
COSMERON_INPUT_STORAGE INPUT_INS(LinuxState) INPUT_INS(linuxState);
#else
COSMERON_INPUT_STORAGE INPUT_INS(LinuxState) INPUT_INS(linuxState) = {0};
#endif
static inline bool INPUT_INS(Bit)(const unsigned char *bits, unsigned index) {
  return (bits[index / CHAR_BIT] & (1u << (index % CHAR_BIT))) != 0;
}
static inline void INPUT_INS(SetBit)(unsigned char *bits, unsigned index, bool down) {
  unsigned char mask = (unsigned char)(1u << (index % CHAR_BIT));
  if (down) bits[index / CHAR_BIT] |= mask; else bits[index / CHAR_BIT] &= (unsigned char)~mask;
}
static inline INPUT_TYPE(Key) INPUT_INS(LinuxKey)(unsigned code) {
  switch (code) {
#define COSMERON_INPUT_KEY_CASE(NAME, LINUX, WINDOWS) case LINUX: return LIB_PREFIX_CONST(NAME);
    COSMERON_INPUT_KEY_TABLE(COSMERON_INPUT_KEY_CASE)
#undef COSMERON_INPUT_KEY_CASE
  default: return LIB_PREFIX_CONST(INPUT_KEY_UNKNOWN);
  }
}
static inline bool INPUT_INS(Held)(unsigned code) {
  size_t i;
  for (i = 0; i < INPUT_DEVICE_CAPACITY; ++i) {
    INPUT_INS(LinuxDevice) *n = &INPUT_INS(linuxState).devices[i];
    if (n->used && !n->controller && INPUT_INS(Bit)(n->keys, code)) return true;
  }
  return false;
}
static inline void INPUT_INS(Aggregate)(unsigned code, bool repeat) {
  bool down = INPUT_INS(Held)(code);
  if (code >= BTN_LEFT && code <= BTN_EXTRA)
    INPUT_INS(MouseButton)(code - BTN_LEFT, down);
  else INPUT_INS(Key)(INPUT_INS(LinuxKey)(code), down, repeat && down);
}
static inline void INPUT_INS(LinuxClose)(size_t slot) {
  INPUT_INS(LinuxDevice) *n = &INPUT_INS(linuxState).devices[slot];
  unsigned code;
  if (!n->used) return;
  if (n->effect >= 0) (void)ioctl(n->fd, EVIOCRMFF, n->effect);
  close(n->fd); n->used = false;
  if (n->controller) INPUT_INS(Disconnect)(slot);
  else for (code = 0; code < KEY_CNT; ++code)
    if (INPUT_INS(Bit)(n->keys, code)) INPUT_INS(Aggregate)(code, false);
}
static void INPUT_INS(LinuxCleanup)(void) {
  size_t i;
  for (i = 0; i < INPUT_DEVICE_CAPACITY; ++i) INPUT_INS(LinuxClose)(i);
}
static inline float INPUT_INS(Normalize)(int value, const struct input_absinfo *a) {
  double span = (double)a->maximum - a->minimum;
  double v;
  if (span <= 0) return 0;
  v = 2.0 * ((double)value - a->minimum) / span - 1.0;
  return (float)(v < -1 ? -1 : (v > 1 ? 1 : v));
}
static inline void INPUT_INS(LinuxHat)(size_t slot, unsigned hat) {
  INPUT_INS(LinuxDevice) *n = &INPUT_INS(linuxState).devices[slot];
  INPUT_INS(Device) *d = &INPUT_INS(state).devices[slot];
  unsigned v = 0;
  if (n->hatX[hat] < 0) v |= INPUT_CONST(HAT_LEFT);
  if (n->hatX[hat] > 0) v |= INPUT_CONST(HAT_RIGHT);
  if (n->hatY[hat] < 0) v |= INPUT_CONST(HAT_UP);
  if (n->hatY[hat] > 0) v |= INPUT_CONST(HAT_DOWN);
  INPUT_INS(Hat)(d, hat, (INPUT_TYPE(Hat))v);
  if (hat == 0 && d->mapped) {
    const unsigned masks[] = { INPUT_CONST(HAT_UP), INPUT_CONST(HAT_DOWN), INPUT_CONST(HAT_LEFT), INPUT_CONST(HAT_RIGHT) };
    unsigned k;
    for (k = 0; k < 4; ++k) {
      int b = d->gamepadButtons[INPUT_CONST(GAMEPAD_BUTTON_UP) + k];
      if (b >= 0) INPUT_INS(ControllerButton)(d, (unsigned)b, (v & masks[k]) != 0);
    }
  }
}
static inline void INPUT_INS(LinuxAbs)(size_t slot, unsigned code, int value) {
  INPUT_INS(LinuxDevice) *n = &INPUT_INS(linuxState).devices[slot];
  if (code >= ABS_CNT) return;
  if (code >= ABS_HAT0X && code <= ABS_HAT3Y) {
    unsigned hat = (code - ABS_HAT0X) / 2;
    if ((code - ABS_HAT0X) % 2) n->hatY[hat] = value; else n->hatX[hat] = value;
    INPUT_INS(LinuxHat)(slot, hat);
  } else if (n->axisMap[code] >= 0)
    INPUT_INS(Axis)(&INPUT_INS(state).devices[slot], (unsigned)n->axisMap[code], INPUT_INS(Normalize)(value, &n->abs[code]));
}
static inline bool INPUT_INS(LinuxSync)(size_t slot) {
  INPUT_INS(LinuxDevice) *n = &INPUT_INS(linuxState).devices[slot];
  unsigned code;
  if (ioctl(n->fd, EVIOCGKEY(sizeof(n->keys)), n->keys) < 0) return false;
  for (code = 0; code < KEY_CNT; ++code) {
    if (n->controller) {
      if (n->buttonMap[code] >= 0)
        INPUT_INS(ControllerButton)(&INPUT_INS(state).devices[slot], (unsigned)n->buttonMap[code], INPUT_INS(Bit)(n->keys, code));
    } else INPUT_INS(Aggregate)(code, false);
  }
  if (n->controller) for (code = 0; code < ABS_CNT; ++code) {
    if (n->axisMap[code] >= 0 || (code >= ABS_HAT0X && code <= ABS_HAT3Y)) {
      struct input_absinfo a;
      if (ioctl(n->fd, EVIOCGABS(code), &a) == 0) INPUT_INS(LinuxAbs)(slot, code, a.value);
    }
  }
  return true;
}
static inline void INPUT_INS(LinuxOpen)(const char *path, size_t slot) {
  unsigned char bits[(KEY_CNT + CHAR_BIT - 1) / CHAR_BIT] = {0};
  unsigned char absBits[(ABS_CNT + CHAR_BIT - 1) / CHAR_BIT] = {0};
  unsigned char ffBits[(FF_CNT + CHAR_BIT - 1) / CHAR_BIT] = {0};
  INPUT_INS(LinuxDevice) *n = &INPUT_INS(linuxState).devices[slot];
  INPUT_INS(Device) *d;
  struct stat st;
  unsigned code;
  char name[INPUT_CONST(NAME_CAPACITY)] = {0};
  int fd = open(path, O_RDWR | O_NONBLOCK);
  bool writable = fd >= 0;
  if (fd < 0) fd = open(path, O_RDONLY | O_NONBLOCK);
  if (fd < 0) return;
  if (fcntl(fd, F_SETFD, FD_CLOEXEC) < 0 || fstat(fd, &st) < 0 ||
      ioctl(fd, EVIOCGBIT(EV_KEY, sizeof(bits)), bits) < 0) { close(fd); return; }
  memset(n, 0, sizeof(*n));
  n->controller = INPUT_INS(Bit)(bits, BTN_GAMEPAD) || INPUT_INS(Bit)(bits, BTN_JOYSTICK);
  if (!n->controller && !INPUT_INS(Bit)(bits, KEY_A) && !INPUT_INS(Bit)(bits, BTN_LEFT)) { close(fd); return; }
  n->fd = fd; n->used = true; n->writable = writable; n->effect = INPUT_INS(EffectNone);
  n->deviceNumber = st.st_rdev; n->inode = st.st_ino;
  strcpy(n->path, path);
  for (code = 0; code < KEY_CNT; ++code) n->buttonMap[code] = -1;
  for (code = 0; code < ABS_CNT; ++code) n->axisMap[code] = -1;
  if (n->controller) {
    (void)ioctl(fd, EVIOCGNAME(sizeof(name)), name); name[sizeof(name)-1] = '\0';
    d = INPUT_INS(Connect)(slot, name);
    if (!d) { close(fd); n->used = false; return; }
    for (code = BTN_MISC; code < KEY_CNT && d->buttonCount < INPUT_CONST(MAX_BUTTONS); ++code)
      if (INPUT_INS(Bit)(bits, code)) n->buttonMap[code] = (int)d->buttonCount++;
    (void)ioctl(fd, EVIOCGBIT(EV_ABS, sizeof(absBits)), absBits);
    for (code = 0; code < ABS_CNT; ++code) if (INPUT_INS(Bit)(absBits, code)) {
      if (ioctl(fd, EVIOCGABS(code), &n->abs[code]) < 0) continue;
      if (code >= ABS_HAT0X && code <= ABS_HAT3Y) {
        unsigned count = (code - ABS_HAT0X) / 2 + 1;
        if (count > d->hatCount) d->hatCount = count;
      } else if (d->axisCount < INPUT_CONST(MAX_AXES)) n->axisMap[code] = (int)d->axisCount++;
    }
    /* Only accept the Linux standard layout when its essential axes/buttons exist. */
    d->mapped = INPUT_INS(Bit)(bits, BTN_SOUTH) && INPUT_INS(Bit)(bits, BTN_EAST) &&
      n->axisMap[ABS_X] >= 0 && n->axisMap[ABS_Y] >= 0;
    if (d->mapped) {
      const unsigned b[] = { BTN_SOUTH, BTN_EAST, BTN_WEST, BTN_NORTH, BTN_TL, BTN_TR,
        BTN_THUMBL, BTN_THUMBR, BTN_SELECT, BTN_START, BTN_MODE, BTN_DPAD_UP, BTN_DPAD_DOWN, BTN_DPAD_LEFT, BTN_DPAD_RIGHT };
      const unsigned a[] = { ABS_X, ABS_Y, ABS_RX, ABS_RY, ABS_Z, ABS_RZ };
      for (code = 0; code < INPUT_CONST(GAMEPAD_BUTTON_COUNT); ++code) d->gamepadButtons[code] = n->buttonMap[b[code]];
      for (code = 0; code < INPUT_CONST(GAMEPAD_AXIS_COUNT); ++code) d->gamepadAxes[code] = n->axisMap[a[code]];
      if (INPUT_INS(Bit)(absBits, ABS_HAT0X) && INPUT_INS(Bit)(absBits, ABS_HAT0Y) && d->buttonCount + 4 <= INPUT_CONST(MAX_BUTTONS))
        for (code = INPUT_CONST(GAMEPAD_BUTTON_UP); code <= INPUT_CONST(GAMEPAD_BUTTON_RIGHT); ++code)
          d->gamepadButtons[code] = (int)d->buttonCount++;
    }
    (void)ioctl(fd, EVIOCGBIT(EV_FF, sizeof(ffBits)), ffBits);
    d->rumble = writable && INPUT_INS(Bit)(ffBits, FF_RUMBLE);
  }
  if (!INPUT_INS(LinuxSync)(slot)) INPUT_INS(LinuxClose)(slot);
}
static inline OPSTATUS INPUT_INS(BackendUpdate)(void) {
  INPUT_INS(LinuxState) *s = &INPUT_INS(linuxState);
  size_t i;
  bool available = false;
  time_t now = time(NULL);
  if (!s->started) {
    if (atexit(INPUT_INS(LinuxCleanup)) != 0) return STATUS_CONST(OUT_OF_MEMORY);
    s->started = true;
  }
  if (s->scanTime != now || !s->scanTime) {
    DIR *dir;
    struct dirent *entry;
    s->scanTime = now;
    for (i = 0; i < INPUT_DEVICE_CAPACITY; ++i) if (s->devices[i].used) {
      struct stat st;
      if (stat(s->devices[i].path, &st) < 0 || st.st_rdev != s->devices[i].deviceNumber || st.st_ino != s->devices[i].inode)
        INPUT_INS(LinuxClose)(i);
    }
    dir = opendir("/dev/input");
    if (dir) {
      while ((entry = readdir(dir)) != NULL) {
        char path[INPUT_INS(PathCapacity)];
        size_t freeSlot = INPUT_DEVICE_CAPACITY;
        bool exists = false;
        int length;
        if (strncmp(entry->d_name, "event", 5)) continue;
        length = snprintf(path, sizeof(path), "/dev/input/%s", entry->d_name);
        if (length < 0 || (size_t)length >= sizeof(path)) continue;
        for (i = 0; i < INPUT_DEVICE_CAPACITY; ++i) {
          if (!s->devices[i].used) { if (freeSlot == INPUT_DEVICE_CAPACITY) freeSlot = i; }
          else if (!strcmp(s->devices[i].path, path)) exists = true;
        }
        if (!exists && freeSlot < INPUT_DEVICE_CAPACITY) INPUT_INS(LinuxOpen)(path, freeSlot);
      }
      closedir(dir);
    }
  }
  for (i = 0; i < INPUT_DEVICE_CAPACITY; ++i) if (s->devices[i].used) {
    INPUT_INS(LinuxDevice) *n = &s->devices[i];
    struct input_event e;
    unsigned budget = INPUT_INS(ReadBudget);
    ssize_t bytes = -1;
    while (budget-- && (bytes = read(n->fd, &e, sizeof(e))) == (ssize_t)sizeof(e)) {
      if (e.type == EV_SYN && e.code == SYN_DROPPED) { n->dropped = true; ++INPUT_INS(state).dropped; continue; }
      if (n->dropped) {
        if (e.type == EV_SYN && e.code == SYN_REPORT) {
          INPUT_TYPE(TEvent) reset = {0};
          n->dropped = false;
          if (!INPUT_INS(LinuxSync)(i)) { INPUT_INS(LinuxClose)(i); break; }
          reset.type = INPUT_CONST(EVENT_DEVICE_RESET);
          reset.controller = n->controller ? INPUT_INS(state).devices[i].id : 0;
          INPUT_INS(Push)(reset);
        }
        continue;
      }
      if (e.type == EV_KEY && e.code < KEY_CNT) {
        INPUT_INS(SetBit)(n->keys, e.code, e.value != 0);
        if (n->controller && n->buttonMap[e.code] >= 0)
          INPUT_INS(ControllerButton)(&INPUT_INS(state).devices[i], (unsigned)n->buttonMap[e.code], e.value != 0);
        else if (!n->controller) INPUT_INS(Aggregate)(e.code, e.value == 2);
      } else if (n->controller && e.type == EV_ABS) INPUT_INS(LinuxAbs)(i, e.code, e.value);
      else if (!n->controller && e.type == EV_REL) {
        if (e.code == REL_X) { INPUT_INS(state).x += (float)e.value; INPUT_INS(Motion)((float)e.value, 0, false); }
        if (e.code == REL_Y) { INPUT_INS(state).y += (float)e.value; INPUT_INS(Motion)(0, (float)e.value, false); }
        if (e.code == REL_HWHEEL) INPUT_INS(Motion)((float)e.value, 0, true);
        if (e.code == REL_WHEEL) INPUT_INS(Motion)(0, (float)e.value, true);
      }
    }
    if (n->used && (bytes == 0 || (bytes < 0 && errno != EAGAIN && errno != EWOULDBLOCK && errno != EINTR))) INPUT_INS(LinuxClose)(i);
    available |= n->used;
  }
  return available ? STATUS_CONST(SUCCESS) : STATUS_CONST(NOT_AVAILABLE);
}
static inline OPSTATUS INPUT_INS(BackendRumble)(INPUT_INS(Device) *d, float low, float high, uint32_t ms) {
  size_t slot = (size_t)(d - INPUT_INS(state).devices);
  INPUT_INS(LinuxDevice) *n = &INPUT_INS(linuxState).devices[slot];
  struct ff_effect effect;
  struct input_event event;
  if (!d->rumble) return STATUS_CONST(NOT_SUPPORTED);
  if (ms > UINT16_MAX) return STATUS_CONST(OUT_OF_RANGE);
  memset(&event, 0, sizeof(event)); event.type = EV_FF;
  if ((!ms || (low == 0 && high == 0))) {
    if (n->effect < 0) return STATUS_CONST(SUCCESS);
    event.code = (uint16_t)n->effect;
    return write(n->fd, &event, sizeof(event)) == (ssize_t)sizeof(event) ? STATUS_CONST(SUCCESS) : STATUS_CONST(GENERIC_ERROR);
  }
  memset(&effect, 0, sizeof(effect)); effect.type = FF_RUMBLE;
  effect.id = (int16_t)n->effect;
  effect.u.rumble.strong_magnitude = (uint16_t)(low * UINT16_MAX);
  effect.u.rumble.weak_magnitude = (uint16_t)(high * UINT16_MAX);
  effect.replay.length = (uint16_t)ms;
  if (ioctl(n->fd, EVIOCSFF, &effect) < 0) return STATUS_CONST(GENERIC_ERROR);
  n->effect = effect.id; event.code = (uint16_t)effect.id; event.value = 1;
  return write(n->fd, &event, sizeof(event)) == (ssize_t)sizeof(event) ? STATUS_CONST(SUCCESS) : STATUS_CONST(GENERIC_ERROR);
}
