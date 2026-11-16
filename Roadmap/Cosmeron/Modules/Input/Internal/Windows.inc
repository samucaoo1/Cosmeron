#pragma once
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <xinput.h>
#include <mmsystem.h>
#include "PhysicalKeys.inc"

typedef struct INPUT_INS(WindowsState) {
  HMODULE user, xinput, winmm;
  HWND window;
  bool started;
  SHORT (WINAPI *key)(int);
  BOOL (WINAPI *cursor)(LPPOINT);
  HWND (WINAPI *create)(DWORD,LPCSTR,LPCSTR,DWORD,int,int,int,int,HWND,HMENU,HINSTANCE,LPVOID);
  BOOL (WINAPI *destroy)(HWND);
  BOOL (WINAPI *registerRaw)(PCRAWINPUTDEVICE,UINT,UINT);
  UINT (WINAPI *registered)(PRAWINPUTDEVICE,PUINT,UINT);
  BOOL (WINAPI *peek)(LPMSG,HWND,UINT,UINT,UINT);
  UINT (WINAPI *raw)(HRAWINPUT,UINT,LPVOID,PUINT,UINT);
  LRESULT (WINAPI *defproc)(HWND,UINT,WPARAM,LPARAM);
  DWORD (WINAPI *get)(DWORD,XINPUT_STATE *);
  DWORD (WINAPI *set)(DWORD,XINPUT_VIBRATION *);
  HANDLE timers[XUSER_MAX_COUNT];
  HANDLE sources[INPUT_DEVICE_CAPACITY];
  bool sourceUsed[INPUT_DEVICE_CAPACITY];
  UINT (WINAPI *joyCount)(void);
  MMRESULT (WINAPI *joyCaps)(UINT_PTR, LPJOYCAPSA, UINT);
  MMRESULT (WINAPI *joyState)(UINT, LPJOYINFOEX);
  JOYCAPSA joystickCaps[INPUT_DEVICE_CAPACITY];
} INPUT_INS(WindowsState);
#if defined(INPUT_EXTERNAL_STORAGE) && !defined(INPUT_DEFINE_STORAGE)
COSMERON_INPUT_STORAGE INPUT_INS(WindowsState) INPUT_INS(windowsState);
#else
COSMERON_INPUT_STORAGE INPUT_INS(WindowsState) INPUT_INS(windowsState) = {0};
#endif
static inline void INPUT_INS(WindowsCancelTimer)(DWORD slot) {
  HANDLE timer = INPUT_INS(windowsState).timers[slot];
  if (timer) {
    (void)DeleteTimerQueueTimer(NULL, timer, INVALID_HANDLE_VALUE);
    INPUT_INS(windowsState).timers[slot] = NULL;
  }
}
static VOID CALLBACK INPUT_INS(WindowsStopTimer)(PVOID context, BOOLEAN fired) {
  DWORD slot = (DWORD)(uintptr_t)context;
  XINPUT_VIBRATION vibration = {0};
  (void)fired;
  if (INPUT_INS(windowsState).set) (void)INPUT_INS(windowsState).set(slot, &vibration);
}
static void INPUT_INS(WindowsCleanup)(void) {
  INPUT_INS(WindowsState) *s = &INPUT_INS(windowsState);
  DWORD i;
  if (s->set) {
    XINPUT_VIBRATION v = {0};
    for (i = 0; i < XUSER_MAX_COUNT; ++i) { INPUT_INS(WindowsCancelTimer)(i); (void)s->set(i, &v); }
  }
  if (s->window && s->destroy) s->destroy(s->window);
  if (s->xinput) FreeLibrary(s->xinput);
  if (s->user) FreeLibrary(s->user);
  if (s->winmm) FreeLibrary(s->winmm);
}
#define COSMERON_INPUT_LOAD(MODULE, FIELD, NAME) do { \
  FARPROC address = GetProcAddress((MODULE), (NAME)); \
  _Static_assert(sizeof(address) == sizeof(s->FIELD), "Windows function pointer size"); \
  memcpy(&s->FIELD, &address, sizeof(address)); \
} while (0)
static inline OPSTATUS INPUT_INS(WindowsStart)(void) {
  INPUT_INS(WindowsState) *s = &INPUT_INS(windowsState);
  RAWINPUTDEVICE devices[2];
  if (s->started) return STATUS_CONST(SUCCESS);
  s->user = LoadLibraryA("user32.dll");
  if (!s->user) return STATUS_CONST(NOT_AVAILABLE);
  COSMERON_INPUT_LOAD(s->user, key, "GetAsyncKeyState");
  COSMERON_INPUT_LOAD(s->user, cursor, "GetCursorPos");
  COSMERON_INPUT_LOAD(s->user, create, "CreateWindowExA");
  COSMERON_INPUT_LOAD(s->user, destroy, "DestroyWindow");
  COSMERON_INPUT_LOAD(s->user, registerRaw, "RegisterRawInputDevices");
  COSMERON_INPUT_LOAD(s->user, registered, "GetRegisteredRawInputDevices");
  COSMERON_INPUT_LOAD(s->user, peek, "PeekMessageA");
  COSMERON_INPUT_LOAD(s->user, raw, "GetRawInputData");
  COSMERON_INPUT_LOAD(s->user, defproc, "DefWindowProcA");
  if (!s->key || !s->cursor || !s->create || !s->destroy || !s->registerRaw || !s->registered || !s->peek || !s->raw || !s->defproc) {
    FreeLibrary(s->user); s->user = NULL; return STATUS_CONST(NOT_AVAILABLE);
  }
  if (INPUT_INS(state).captureMode == INPUT_CONST(CAPTURE_NATIVE)) {
  {
    RAWINPUTDEVICE existing[INPUT_DEVICE_CAPACITY];
    UINT count = INPUT_DEVICE_CAPACITY, i;
    UINT result = s->registered(existing, &count, sizeof(existing[0]));
    if (result == (UINT)-1) {
      FreeLibrary(s->user); s->user = NULL; return STATUS_CONST(BUSY);
    }
    for (i = 0; i < result; ++i) {
      if (existing[i].usUsagePage == 0x01 && (existing[i].usUsage == 0x02 || existing[i].usUsage == 0x06)) {
        FreeLibrary(s->user); s->user = NULL; return STATUS_CONST(BUSY);
      }
    }
  }
  s->window = s->create(0, "STATIC", "Cosmeron Input", 0, 0,0,0,0, HWND_MESSAGE, NULL, GetModuleHandleA(NULL), NULL);
  if (!s->window) { FreeLibrary(s->user); s->user = NULL; return STATUS_CONST(NOT_AVAILABLE); }
  memset(devices, 0, sizeof(devices));
  /* USB HID Generic Desktop usage page, mouse and keyboard usages. */
  devices[0].usUsagePage = 0x01; devices[0].usUsage = 0x02;
  devices[1].usUsagePage = 0x01; devices[1].usUsage = 0x06;
  devices[0].dwFlags = devices[1].dwFlags = RIDEV_INPUTSINK | RIDEV_DEVNOTIFY;
  devices[0].hwndTarget = devices[1].hwndTarget = s->window;
  if (!s->registerRaw(devices, 2, sizeof(devices[0]))) {
    s->destroy(s->window); s->window = NULL; FreeLibrary(s->user); s->user = NULL; return STATUS_CONST(NOT_AVAILABLE);
  }
  }
  s->xinput = LoadLibraryA("xinput1_4.dll");
  if (!s->xinput) s->xinput = LoadLibraryA("xinput1_3.dll");
  if (!s->xinput) s->xinput = LoadLibraryA("xinput9_1_0.dll");
  if (s->xinput) {
    COSMERON_INPUT_LOAD(s->xinput, get, "XInputGetState");
    COSMERON_INPUT_LOAD(s->xinput, set, "XInputSetState");
  }
  s->winmm = LoadLibraryA("winmm.dll");
  if (s->winmm) {
    COSMERON_INPUT_LOAD(s->winmm, joyCount, "joyGetNumDevs");
    COSMERON_INPUT_LOAD(s->winmm, joyCaps, "joyGetDevCapsA");
    COSMERON_INPUT_LOAD(s->winmm, joyState, "joyGetPosEx");
  }
  if (atexit(INPUT_INS(WindowsCleanup)) != 0) {
    INPUT_INS(WindowsCleanup)(); memset(s, 0, sizeof(*s)); return STATUS_CONST(NOT_AVAILABLE);
  }
  s->started = true;
  return STATUS_CONST(SUCCESS);
}
#undef COSMERON_INPUT_LOAD
static inline INPUT_TYPE(Key) INPUT_INS(WindowsKey)(unsigned code) {
#define COSMERON_INPUT_KEY_CASE(NAME, LINUX, WINDOWS) if ((WINDOWS) != 0 && code == (WINDOWS)) return LIB_PREFIX_CONST(NAME);
  COSMERON_INPUT_KEY_TABLE(COSMERON_INPUT_KEY_CASE)
#undef COSMERON_INPUT_KEY_CASE
  return LIB_PREFIX_CONST(INPUT_KEY_UNKNOWN);
}
static inline INPUT_TYPE(Key) INPUT_INS(WindowsPhysical)(unsigned scan) {
#define COSMERON_INPUT_PHYSICAL_CASE(NAME, SCAN, MAC, XNAME) if ((SCAN) != 0 && scan == (SCAN)) return LIB_PREFIX_CONST(NAME);
  COSMERON_INPUT_PHYSICAL_TABLE(COSMERON_INPUT_PHYSICAL_CASE)
#undef COSMERON_INPUT_PHYSICAL_CASE
  return LIB_PREFIX_CONST(INPUT_KEY_UNKNOWN);
}
static inline uint32_t INPUT_INS(WindowsSource)(HANDLE device) {
  uint32_t i;
  INPUT_INS(WindowsState) *s = &INPUT_INS(windowsState);
  for (i = 0; i < INPUT_DEVICE_CAPACITY; ++i) if (s->sourceUsed[i] && s->sources[i] == device) return i;
  for (i = 0; i < INPUT_DEVICE_CAPACITY; ++i) if (!s->sourceUsed[i]) {
    s->sourceUsed[i] = true; s->sources[i] = device; return i;
  }
  ++INPUT_INS(state).dropped; return INPUT_DEVICE_CAPACITY;
}
static inline void INPUT_INS(WindowsGenericControllers)(void) {
  INPUT_INS(WindowsState) *s = &INPUT_INS(windowsState);
  UINT count, i;
  if (!s->joyCount || !s->joyCaps || !s->joyState) return;
  count = s->joyCount();
  for (i = 0; i < count && i + XUSER_MAX_COUNT < INPUT_DEVICE_CAPACITY; ++i) {
    size_t slot = i + XUSER_MAX_COUNT;
    INPUT_INS(Device) *d = &INPUT_INS(state).devices[slot];
    JOYCAPSA *c = &s->joystickCaps[slot];
    JOYINFOEX j = {0};
    unsigned b;
    j.dwSize = sizeof(j); j.dwFlags = JOY_RETURNALL;
    if (s->joyState(i, &j) != JOYERR_NOERROR) { INPUT_INS(Disconnect)(slot); continue; }
    if (!d->connected) {
      if (s->joyCaps(i, c, sizeof(*c)) != JOYERR_NOERROR) continue;
      d = INPUT_INS(Connect)(slot, c->szPname); if (!d) continue;
      /* WinMM manufacturer/product IDs are not USB VID/PID; leave them unknown. */
      d->buttonCount = c->wNumButtons > 32 ? 32 : c->wNumButtons;
      /* Preserve native X/Y/Z/R/U/V positions rather than compress optional axes. */
      d->axisCount = 6; d->hatCount = (c->wCaps & JOYCAPS_HASPOV) ? 1 : 0;
    }
    for (b = 0; b < d->buttonCount; ++b) INPUT_INS(ControllerButton)(d, b, (j.dwButtons & ((DWORD)1 << b)) != 0);
    {
      DWORD values[] = { j.dwXpos, j.dwYpos, j.dwZpos, j.dwRpos, j.dwUpos, j.dwVpos };
      UINT minima[] = { c->wXmin,c->wYmin,c->wZmin,c->wRmin,c->wUmin,c->wVmin };
      UINT maxima[] = { c->wXmax,c->wYmax,c->wZmax,c->wRmax,c->wUmax,c->wVmax };
      unsigned flags[] = { 0,0,JOYCAPS_HASZ,JOYCAPS_HASR,JOYCAPS_HASU,JOYCAPS_HASV };
      for (b = 0; b < d->axisCount; ++b) {
        double span = (double)maxima[b] - minima[b];
        float v = span > 0 && (!flags[b] || (c->wCaps & flags[b])) ? (float)(2 * ((double)values[b] - minima[b]) / span - 1) : 0;
        INPUT_INS(Axis)(d, b, v < -1 ? -1 : (v > 1 ? 1 : v));
      }
    }
    if (d->hatCount) {
      unsigned mask = 0;
      if (j.dwPOV != JOY_POVCENTERED && j.dwPOV < 36000) {
        unsigned sector = ((j.dwPOV + 2250) / 4500) % 8;
        if (sector == 7 || sector <= 1) mask |= INPUT_CONST(HAT_UP);
        if (sector >= 1 && sector <= 3) mask |= INPUT_CONST(HAT_RIGHT);
        if (sector >= 3 && sector <= 5) mask |= INPUT_CONST(HAT_DOWN);
        if (sector >= 5 && sector <= 7) mask |= INPUT_CONST(HAT_LEFT);
      }
      INPUT_INS(Hat)(d, 0, (INPUT_TYPE(Hat))mask);
    }
    INPUT_INS(state).capabilities |= INPUT_CONST(CAPABILITY_CONTROLLERS);
  }
}
static inline OPSTATUS INPUT_INS(BackendUpdate)(void) {
  INPUT_INS(WindowsState) *s = &INPUT_INS(windowsState);
  POINT point;
  MSG msg;
  unsigned budget = 4096;
  DWORD i;
  bool readFailed = false;
  OPSTATUS startStatus = INPUT_INS(WindowsStart)();
  if (startStatus != STATUS_CONST(SUCCESS)) return startStatus;
  while (s->window && budget-- && s->peek(&msg, s->window, 0, 0, PM_REMOVE)) {
    if (msg.message == WM_INPUT_DEVICE_CHANGE && msg.wParam == GIDC_REMOVAL) {
      uint32_t source;
      for (source = 0; source < INPUT_DEVICE_CAPACITY; ++source) if (s->sourceUsed[source] && s->sources[source] == (HANDLE)msg.lParam) {
        INPUT_INS(RemoveSource)(source); s->sourceUsed[source] = false;
      }
    }
    if (msg.message != WM_INPUT) { (void)s->defproc(msg.hwnd, msg.message, msg.wParam, msg.lParam); continue; }
    RAWINPUT r;
    UINT size = sizeof(r);
    if (s->raw((HRAWINPUT)msg.lParam, RID_INPUT, &r, &size, sizeof(RAWINPUTHEADER)) != (UINT)-1) {
      uint32_t source = INPUT_INS(WindowsSource)(r.header.hDevice);
      if (source >= INPUT_DEVICE_CAPACITY) { (void)s->defproc(msg.hwnd, msg.message, msg.wParam, msg.lParam); continue; }
      if (r.header.dwType == RIM_TYPEKEYBOARD) {
        unsigned vk = r.data.keyboard.VKey;
        bool down = !(r.data.keyboard.Flags & RI_KEY_BREAK);
        INPUT_TYPE(Key) key;
        if (vk == VK_SHIFT) vk = r.data.keyboard.MakeCode == 0x36 ? VK_RSHIFT : VK_LSHIFT;
        if (vk == VK_CONTROL) vk = (r.data.keyboard.Flags & RI_KEY_E0) ? VK_RCONTROL : VK_LCONTROL;
        if (vk == VK_MENU) vk = (r.data.keyboard.Flags & RI_KEY_E0) ? VK_RMENU : VK_LMENU;
        if (vk == VK_RETURN && (r.data.keyboard.Flags & RI_KEY_E0)) vk |= 0x100;
        key = INPUT_INS(WindowsKey)(vk);
        {
          unsigned scan = r.data.keyboard.MakeCode | ((r.data.keyboard.Flags & RI_KEY_E0) ? 0x100u : 0) | ((r.data.keyboard.Flags & RI_KEY_E1) ? 0x200u : 0);
          bool repeat = down && INPUT_INS(state).sourceKeys[source][key];
          INPUT_INS(SourceKey)(source, key, INPUT_INS(WindowsPhysical)(scan), down, repeat);
        }
      } else if (r.header.dwType == RIM_TYPEMOUSE) {
        USHORT f = r.data.mouse.usButtonFlags;
        const USHORT presses[] = { RI_MOUSE_LEFT_BUTTON_DOWN, RI_MOUSE_RIGHT_BUTTON_DOWN, RI_MOUSE_MIDDLE_BUTTON_DOWN, RI_MOUSE_BUTTON_4_DOWN, RI_MOUSE_BUTTON_5_DOWN };
        const USHORT releases[] = { RI_MOUSE_LEFT_BUTTON_UP, RI_MOUSE_RIGHT_BUTTON_UP, RI_MOUSE_MIDDLE_BUTTON_UP, RI_MOUSE_BUTTON_4_UP, RI_MOUSE_BUTTON_5_UP };
        unsigned b;
        for (b = 0; b < INPUT_CONST(MOUSE_BUTTON_COUNT); ++b) {
          if (f & presses[b]) INPUT_INS(SourceMouse)(source, b, true);
          if (f & releases[b]) INPUT_INS(SourceMouse)(source, b, false);
        }
        if (!(r.data.mouse.usFlags & MOUSE_MOVE_ABSOLUTE)) INPUT_INS(Motion)((float)r.data.mouse.lLastX, (float)r.data.mouse.lLastY, false);
        if (f & RI_MOUSE_WHEEL) INPUT_INS(Motion)(0, (float)(SHORT)r.data.mouse.usButtonData / WHEEL_DELTA, true);
        if (f & RI_MOUSE_HWHEEL) INPUT_INS(Motion)((float)(SHORT)r.data.mouse.usButtonData / WHEEL_DELTA, 0, true);
      }
    } else { readFailed = true; ++INPUT_INS(state).dropped; }
    (void)s->defproc(msg.hwnd, msg.message, msg.wParam, msg.lParam);
  }
  if (s->window) {
    INPUT_INS(state).capabilities |= INPUT_CONST(CAPABILITY_KEYBOARD) | INPUT_CONST(CAPABILITY_PHYSICAL_KEYS) | INPUT_CONST(CAPABILITY_LOGICAL_KEYS) | INPUT_CONST(CAPABILITY_MOUSE_BUTTONS) | INPUT_CONST(CAPABILITY_RAW_MOTION) | INPUT_CONST(CAPABILITY_SCROLL) | INPUT_CONST(CAPABILITY_PRECISE_SCROLL);
    if (s->cursor(&point)) {
      INPUT_INS(state).x = (float)point.x; INPUT_INS(state).y = (float)point.y;
      INPUT_INS(state).capabilities |= INPUT_CONST(CAPABILITY_POINTER_POSITION);
    } else readFailed = true;
  }
  for (i = 0; i < XUSER_MAX_COUNT && i < INPUT_DEVICE_CAPACITY; ++i) {
    XINPUT_STATE native;
    INPUT_INS(Device) *d = &INPUT_INS(state).devices[i];
    const WORD masks[] = { XINPUT_GAMEPAD_A, XINPUT_GAMEPAD_B, XINPUT_GAMEPAD_X, XINPUT_GAMEPAD_Y,
      XINPUT_GAMEPAD_LEFT_SHOULDER, XINPUT_GAMEPAD_RIGHT_SHOULDER, XINPUT_GAMEPAD_LEFT_THUMB,
      XINPUT_GAMEPAD_RIGHT_THUMB, XINPUT_GAMEPAD_BACK, XINPUT_GAMEPAD_START, 0,
      XINPUT_GAMEPAD_DPAD_UP, XINPUT_GAMEPAD_DPAD_DOWN, XINPUT_GAMEPAD_DPAD_LEFT, XINPUT_GAMEPAD_DPAD_RIGHT };
    unsigned b;
    if (!s->get || s->get(i, &native) != ERROR_SUCCESS) { INPUT_INS(WindowsCancelTimer)(i); INPUT_INS(Disconnect)(i); continue; }
    INPUT_INS(state).capabilities |= INPUT_CONST(CAPABILITY_CONTROLLERS);
    if (!d->connected) {
      d = INPUT_INS(Connect)(i, "XInput controller");
      if (!d) return STATUS_CONST(OUT_OF_RANGE);
      d->buttonCount = INPUT_CONST(GAMEPAD_BUTTON_COUNT); d->axisCount = INPUT_CONST(GAMEPAD_AXIS_COUNT);
      d->hatCount = 1; d->mapped = true; d->rumble = s->set != NULL;
      for (b = 0; b < d->buttonCount; ++b) d->gamepadButtons[b] = (int)b;
      for (b = 0; b < d->axisCount; ++b) d->gamepadAxes[b] = (int)b;
    }
    for (b = 0; b < d->buttonCount; ++b) INPUT_INS(ControllerButton)(d, b, (native.Gamepad.wButtons & masks[b]) != 0);
    {
      SHORT sticks[] = { native.Gamepad.sThumbLX, native.Gamepad.sThumbLY, native.Gamepad.sThumbRX, native.Gamepad.sThumbRY };
      for (b = 0; b < 4; ++b) INPUT_INS(Axis)(d, b, sticks[b] / (sticks[b] < 0 ? 32768.0f : 32767.0f));
      INPUT_INS(Axis)(d, INPUT_CONST(GAMEPAD_AXIS_LEFT_TRIGGER), native.Gamepad.bLeftTrigger / 255.0f * 2 - 1);
      INPUT_INS(Axis)(d, INPUT_CONST(GAMEPAD_AXIS_RIGHT_TRIGGER), native.Gamepad.bRightTrigger / 255.0f * 2 - 1);
    }
    {
      unsigned hat = 0;
      if (native.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP) hat |= INPUT_CONST(HAT_UP);
      if (native.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN) hat |= INPUT_CONST(HAT_DOWN);
      if (native.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT) hat |= INPUT_CONST(HAT_LEFT);
      if (native.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT) hat |= INPUT_CONST(HAT_RIGHT);
      INPUT_INS(Hat)(d, 0, (INPUT_TYPE(Hat))hat);
    }
  }
  INPUT_INS(WindowsGenericControllers)();
  return readFailed ? STATUS_CONST(NOT_AVAILABLE) : STATUS_CONST(SUCCESS);
}
static inline OPSTATUS INPUT_INS(BackendRumble)(INPUT_INS(Device) *d, float low, float high, uint32_t ms) {
  INPUT_INS(WindowsState) *s = &INPUT_INS(windowsState);
  DWORD slot = (DWORD)(d - INPUT_INS(state).devices);
  XINPUT_VIBRATION v;
  if (slot >= XUSER_MAX_COUNT || !s->set || !d->rumble) return STATUS_CONST(NOT_SUPPORTED);
  INPUT_INS(WindowsCancelTimer)(slot);
  v.wLeftMotorSpeed = ms ? (WORD)(low * UINT16_MAX) : 0;
  v.wRightMotorSpeed = ms ? (WORD)(high * UINT16_MAX) : 0;
  if (s->set(slot, &v) != ERROR_SUCCESS) return STATUS_CONST(NOT_AVAILABLE);
  if (ms && (low > 0 || high > 0) && !CreateTimerQueueTimer(&s->timers[slot], NULL, INPUT_INS(WindowsStopTimer), (PVOID)(uintptr_t)slot, ms, 0, WT_EXECUTEONLYONCE)) {
    v.wLeftMotorSpeed = v.wRightMotorSpeed = 0; (void)s->set(slot, &v);
    return STATUS_CONST(GENERIC_ERROR);
  }
  return STATUS_CONST(SUCCESS);
}
