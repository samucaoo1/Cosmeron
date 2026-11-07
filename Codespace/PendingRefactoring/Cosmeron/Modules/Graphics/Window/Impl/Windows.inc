#pragma once
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <wchar.h>
#define COSMERON_MACRO_INTERNAL_WINDOW_WIN_FUNCTIONS(X)                                            \
  X(HWND, CreateWindowExW,                                                                         \
    (DWORD, LPCWSTR, LPCWSTR, DWORD, int, int, int, int, HWND, HMENU, HINSTANCE, LPVOID))          \
  X(ATOM, RegisterClassW, (const WNDCLASSW *))                                                     \
  X(BOOL, UnregisterClassW, (LPCWSTR, HINSTANCE))                                                  \
  X(LRESULT, DefWindowProcW, (HWND, UINT, WPARAM, LPARAM))                                         \
  X(BOOL, DestroyWindow, (HWND))                                                                   \
  X(LONG_PTR, SetWindowLongPtrW, (HWND, int, LONG_PTR))                                            \
  X(LONG_PTR, GetWindowLongPtrW, (HWND, int))                                                      \
  X(BOOL, PeekMessageW, (LPMSG, HWND, UINT, UINT, UINT))                                           \
  X(BOOL, TranslateMessage, (const MSG *))                                                         \
  X(LRESULT, DispatchMessageW, (const MSG *))                                                      \
  X(BOOL, GetClientRect, (HWND, LPRECT))                                                           \
  X(BOOL, AdjustWindowRect, (LPRECT, DWORD, BOOL))                                                 \
  X(HDC, GetDC, (HWND))                                                                            \
  X(int, ReleaseDC, (HWND, HDC))                                                                   \
  X(SHORT, GetKeyState, (int))                                                                     \
  X(HWND, SetCapture, (HWND))                                                                      \
  X(BOOL, ReleaseCapture, (void))
typedef struct WINDOW_INS(WinState) {
  HMODULE user, gdi;
  HWND window;
  HINSTANCE instance;
  wchar_t className[64];
  WINDOW_TYPE(TEvent) events[WINDOW_EVENT_CAPACITY];
  size_t head, count;
  bool overflow;
  uint16_t highSurrogate;
#define COSMERON_MACRO_INTERNAL_WINDOW_WIN_FIELD(RET, NAME, ARGS) RET(WINAPI *NAME) ARGS;
  COSMERON_MACRO_INTERNAL_WINDOW_WIN_FUNCTIONS(COSMERON_MACRO_INTERNAL_WINDOW_WIN_FIELD)
#undef COSMERON_MACRO_INTERNAL_WINDOW_WIN_FIELD
  int(WINAPI *StretchDIBits)(HDC, int, int, int, int, int, int, int, int, const void *,
                             const BITMAPINFO *, UINT, DWORD);
} WINDOW_INS(WinState);
static inline INPUT_TYPE(Modifiers) WINDOW_INS(WinModifiers)(WINDOW_INS(WinState) * s) {
  INPUT_TYPE(Modifiers) m = 0;
  if (s->GetKeyState(VK_SHIFT) & 0x8000)
    m |= INPUT_CONST(MODIFIER_SHIFT);
  if (s->GetKeyState(VK_CONTROL) & 0x8000)
    m |= INPUT_CONST(MODIFIER_CONTROL);
  if (s->GetKeyState(VK_MENU) & 0x8000)
    m |= INPUT_CONST(MODIFIER_ALT);
  if ((s->GetKeyState(VK_LWIN) | s->GetKeyState(VK_RWIN)) & 0x8000)
    m |= INPUT_CONST(MODIFIER_SUPER);
  return m;
}
static inline INPUT_TYPE(Key) WINDOW_INS(WinKey)(WPARAM key) {
  if (key >= 'A' && key <= 'Z')
    return (INPUT_TYPE(Key))(LIB_PREFIX_CONST(INPUT_KEY_A) + key - 'A');
  if (key >= '0' && key <= '9')
    return (INPUT_TYPE(Key))(LIB_PREFIX_CONST(INPUT_KEY_DIGIT_0) + key - '0');
  if (key >= VK_F1 && key <= VK_F12)
    return (INPUT_TYPE(Key))(LIB_PREFIX_CONST(INPUT_KEY_F1) + key - VK_F1);
  switch (key) {
  case VK_RETURN:
    return LIB_PREFIX_CONST(INPUT_KEY_ENTER);
  case VK_TAB:
    return LIB_PREFIX_CONST(INPUT_KEY_TAB);
  case VK_ESCAPE:
    return LIB_PREFIX_CONST(INPUT_KEY_ESCAPE);
  case VK_SPACE:
    return LIB_PREFIX_CONST(INPUT_KEY_SPACE);
  case VK_BACK:
    return LIB_PREFIX_CONST(INPUT_KEY_BACKSPACE);
  case VK_DELETE:
    return LIB_PREFIX_CONST(INPUT_KEY_DELETE);
  case VK_LEFT:
    return LIB_PREFIX_CONST(INPUT_KEY_LEFT);
  case VK_RIGHT:
    return LIB_PREFIX_CONST(INPUT_KEY_RIGHT);
  case VK_UP:
    return LIB_PREFIX_CONST(INPUT_KEY_UP);
  case VK_DOWN:
    return LIB_PREFIX_CONST(INPUT_KEY_DOWN);
  case VK_HOME:
    return LIB_PREFIX_CONST(INPUT_KEY_HOME);
  case VK_END:
    return LIB_PREFIX_CONST(INPUT_KEY_END);
  case VK_PRIOR:
    return LIB_PREFIX_CONST(INPUT_KEY_PAGE_UP);
  case VK_NEXT:
    return LIB_PREFIX_CONST(INPUT_KEY_PAGE_DOWN);
  default:
    return LIB_PREFIX_CONST(INPUT_KEY_UNKNOWN);
  }
}
static inline void WINDOW_INS(WinPush)(WINDOW_INS(WinState) * s, WINDOW_TYPE(TEvent) e) {
  if (s->count == WINDOW_EVENT_CAPACITY) {
    s->overflow = true;
    return;
  }
  s->events[(s->head + s->count++) % WINDOW_EVENT_CAPACITY] = e;
}
static LRESULT CALLBACK WINDOW_INS(WinProc)(HWND hwnd, UINT message, WPARAM wp, LPARAM lp) {
  WINDOW_INS(WinState) *s = NULL;
  WINDOW_TYPE(TEvent) e = {0};
  LONG_PTR(WINAPI * get)(HWND, int) = NULL;
  LRESULT(WINAPI * def)(HWND, UINT, WPARAM, LPARAM) = NULL;
  FARPROC symbol = GetProcAddress(GetModuleHandleW(L"user32.dll"), PP_OP_STRING(GetWindowLongPtrW));
  memcpy(&get, &symbol, sizeof(get));
  if (get)
    s = (WINDOW_INS(WinState) *)(uintptr_t)get(hwnd, GWLP_USERDATA);
  if (message == WM_NCCREATE) {
    s = (WINDOW_INS(WinState) *)((CREATESTRUCTW *)lp)->lpCreateParams;
    s->SetWindowLongPtrW(hwnd, GWLP_USERDATA, (LONG_PTR)(uintptr_t)s);
    return TRUE;
  }
  if (!s) {
    symbol = GetProcAddress(GetModuleHandleW(L"user32.dll"), "DefWindowProcW");
    memcpy(&def, &symbol, sizeof(def));
    return def ? def(hwnd, message, wp, lp) : 0;
  }
  e.input.modifiers = WINDOW_INS(WinModifiers)(s);
  switch (message) {
  case WM_CLOSE:
    e.type = WINDOW_CONST(EVENT_CLOSE);
    WINDOW_INS(WinPush)(s, e);
    return 0;
  case WM_SIZE:
    if (LOWORD(lp) && HIWORD(lp)) {
      e.type = WINDOW_CONST(EVENT_RESIZE);
      e.size.x = LOWORD(lp);
      e.size.y = HIWORD(lp);
      WINDOW_INS(WinPush)(s, e);
    }
    return 0;
  case WM_SETFOCUS:
  case WM_KILLFOCUS:
    e.input.type =
        message == WM_SETFOCUS ? INPUT_CONST(EVENT_FOCUS_GAINED) : INPUT_CONST(EVENT_FOCUS_LOST);
    if (message == WM_KILLFOCUS)
      s->highSurrogate = 0;
    WINDOW_INS(WinPush)(s, e);
    return 0;
  case WM_MOUSEMOVE:
    e.input.type = INPUT_CONST(EVENT_MOUSE_POSITION);
    e.input.data.motion.x = (float)(short)LOWORD(lp);
    e.input.data.motion.y = (float)(short)HIWORD(lp);
    WINDOW_INS(WinPush)(s, e);
    return 0;
  case WM_LBUTTONDOWN:
  case WM_LBUTTONUP:
  case WM_RBUTTONDOWN:
  case WM_RBUTTONUP:
  case WM_MBUTTONDOWN:
  case WM_MBUTTONUP:
    e.input.type = INPUT_CONST(EVENT_MOUSE_POSITION);
    e.input.data.motion.x = (float)(short)LOWORD(lp);
    e.input.data.motion.y = (float)(short)HIWORD(lp);
    WINDOW_INS(WinPush)(s, e);
    memset(&e, 0, sizeof(e));
    e.input.modifiers = WINDOW_INS(WinModifiers)(s);
    e.input.type =
        (message == WM_LBUTTONDOWN || message == WM_RBUTTONDOWN || message == WM_MBUTTONDOWN)
            ? INPUT_CONST(EVENT_MOUSE_BUTTON_PRESS)
            : INPUT_CONST(EVENT_MOUSE_BUTTON_RELEASE);
    e.input.data.mouseButton =
        (message == WM_LBUTTONDOWN || message == WM_LBUTTONUP)   ? INPUT_CONST(MOUSE_BUTTON_LEFT)
        : (message == WM_RBUTTONDOWN || message == WM_RBUTTONUP) ? INPUT_CONST(MOUSE_BUTTON_RIGHT)
                                                                 : INPUT_CONST(MOUSE_BUTTON_MIDDLE);
    if (message == WM_LBUTTONDOWN)
      s->SetCapture(hwnd);
    if (message == WM_LBUTTONUP)
      s->ReleaseCapture();
    WINDOW_INS(WinPush)(s, e);
    return 0;
  case WM_MOUSEWHEEL:
    e.input.type = INPUT_CONST(EVENT_MOUSE_SCROLL);
    e.input.data.motion.y = (float)(short)HIWORD(wp) / WHEEL_DELTA;
    WINDOW_INS(WinPush)(s, e);
    return 0;
  case WM_KEYDOWN:
  case WM_SYSKEYDOWN:
  case WM_KEYUP:
  case WM_SYSKEYUP:
    e.input.type = (message == WM_KEYUP || message == WM_SYSKEYUP)
                       ? INPUT_CONST(EVENT_KEY_RELEASE)
                       : ((lp & ((LPARAM)1 << 30)) ? INPUT_CONST(EVENT_KEY_REPEAT)
                                                   : INPUT_CONST(EVENT_KEY_PRESS));
    e.input.data.key = WINDOW_INS(WinKey)(wp);
    WINDOW_INS(WinPush)(s, e);
    return (message == WM_SYSKEYDOWN || message == WM_SYSKEYUP)
               ? s->DefWindowProcW(hwnd, message, wp, lp)
               : 0;
  case WM_CHAR: {
    uint32_t cp = (uint32_t)wp;
    size_t bytes;
    if (cp >= 0xD800 && cp <= 0xDBFF) {
      s->highSurrogate = (uint16_t)cp;
      return 0;
    }
    if (cp >= 0xDC00 && cp <= 0xDFFF) {
      if (!s->highSurrogate)
        return 0;
      cp = 0x10000 + (((uint32_t)s->highSurrogate - 0xD800) << 10) + (cp - 0xDC00);
    }
    s->highSurrogate = 0;
    if (cp == '\r')
      cp = '\n';
    if (cp < 32 && cp != '\n')
      return 0;
    if ((e.input.modifiers & INPUT_CONST(MODIFIER_CONTROL)) &&
        !(e.input.modifiers & INPUT_CONST(MODIFIER_ALT)))
      return 0;
    e.input.type = INPUT_CONST(EVENT_TEXT_INPUT);
    if (TEXT_UTF8_FUNC(Encode)(cp, (unsigned char *)e.input.data.text.utf8,
                               sizeof(e.input.data.text.utf8) - 1,
                               &bytes) == STATUS_CONST(SUCCESS)) {
      e.input.data.text.length = bytes;
      e.input.data.text.utf8[bytes] = 0;
      WINDOW_INS(WinPush)(s, e);
    }
    return 0;
  }
  default:
    return s->DefWindowProcW(hwnd, message, wp, lp);
  }
}
static inline void WINDOW_INS(WinRelease)(WINDOW_INS(WinState) * s) {
  if (!s)
    return;
  if (s->window)
    s->DestroyWindow(s->window);
  if (s->className[0] && s->UnregisterClassW)
    s->UnregisterClassW(s->className, s->instance);
  if (s->gdi)
    FreeLibrary(s->gdi);
  if (s->user)
    FreeLibrary(s->user);
  Memory_Free(s);
}
static inline OPSTATUS WINDOW_INS(NativeOpen)(WINDOW_TYPE(TWindow) * w, const char *title,
                                              TDUAL_TYPE(uint32) size) {
  WINDOW_INS(WinState) * s;
  void *memory = NULL;
  OPSTATUS status;
  WNDCLASSW cls = {0};
  RECT rect = {0};
  wchar_t *wide;
  int count;
  status = ALLOC_FUNC(AllocBytes)(&memory, sizeof(*s));
  if (status != STATUS_CONST(SUCCESS))
    return status;
  s = (WINDOW_INS(WinState) *)memory;
  memset(s, 0, sizeof(*s));
  s->user = LoadLibraryW(L"user32.dll");
  s->gdi = LoadLibraryW(L"gdi32.dll");
  if (!s->user || !s->gdi) {
    WINDOW_INS(WinRelease)(s);
    return STATUS_CONST(NOT_AVAILABLE);
  }
#define COSMERON_MACRO_INTERNAL_WINDOW_WIN_LOAD(RET, NAME, ARGS)                                   \
  do {                                                                                             \
    FARPROC symbol = GetProcAddress(s->user, PP_OP_STRING(NAME));                                  \
    if (!symbol || sizeof(symbol) != sizeof(s->NAME)) {                                            \
      WINDOW_INS(WinRelease)(s);                                                                   \
      return STATUS_CONST(NOT_AVAILABLE);                                                          \
    }                                                                                              \
    memcpy(&s->NAME, &symbol, sizeof(symbol));                                                     \
  } while (0);
  COSMERON_MACRO_INTERNAL_WINDOW_WIN_FUNCTIONS(COSMERON_MACRO_INTERNAL_WINDOW_WIN_LOAD)
#undef COSMERON_MACRO_INTERNAL_WINDOW_WIN_LOAD
  {
    FARPROC symbol = GetProcAddress(s->gdi, "StretchDIBits");
    memcpy(&s->StretchDIBits, &symbol, sizeof(symbol));
  }
  if (!s->StretchDIBits) {
    WINDOW_INS(WinRelease)(s);
    return STATUS_CONST(NOT_AVAILABLE);
  }
  s->instance = GetModuleHandleW(NULL);
  swprintf(s->className, 64, L"Cosmeron_%llx", (unsigned long long)(uintptr_t)s);
  cls.lpfnWndProc = WINDOW_INS(WinProc);
  cls.hInstance = s->instance;
  cls.lpszClassName = s->className;
  if (!s->RegisterClassW(&cls)) {
    WINDOW_INS(WinRelease)(s);
    return STATUS_CONST(GENERIC_ERROR);
  }
  count = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, title, -1, NULL, 0);
  if (count <= 0) {
    WINDOW_INS(WinRelease)(s);
    return STATUS_CONST(INVALID_SEQUENCE);
  }
  memory = NULL;
  status = ALLOC_FUNC(AllocArray)(&memory, (size_t)count, sizeof(wchar_t));
  if (status != STATUS_CONST(SUCCESS)) {
    WINDOW_INS(WinRelease)(s);
    return status;
  }
  wide = (wchar_t *)memory;
  MultiByteToWideChar(CP_UTF8, 0, title, -1, wide, count);
  rect.right = (LONG)size.x;
  rect.bottom = (LONG)size.y;
  s->AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);
  s->window = s->CreateWindowExW(0, s->className, wide, WS_OVERLAPPEDWINDOW | WS_VISIBLE,
                                 CW_USEDEFAULT, CW_USEDEFAULT, rect.right - rect.left,
                                 rect.bottom - rect.top, NULL, NULL, s->instance, s);
  Memory_Free(wide);
  if (!s->window) {
    WINDOW_INS(WinRelease)(s);
    return STATUS_CONST(GENERIC_ERROR);
  }
  w->native = s;
  return STATUS_CONST(SUCCESS);
}
static inline OPSTATUS WINDOW_INS(NativePoll)(WINDOW_TYPE(TWindow) * w, WINDOW_TYPE(TEvent) * out) {
  WINDOW_INS(WinState) *s = (WINDOW_INS(WinState) *)w->native;
  MSG message;
  while (!s->count && s->PeekMessageW(&message, s->window, 0, 0, PM_REMOVE)) {
    s->TranslateMessage(&message);
    s->DispatchMessageW(&message);
  }
  if (s->overflow) {
    s->overflow = false;
    return STATUS_CONST(INSUFFICIENT_SPACE);
  }
  if (!s->count)
    return STATUS_CONST(WOULD_BLOCK);
  *out = s->events[s->head];
  s->head = (s->head + 1) % WINDOW_EVENT_CAPACITY;
  --s->count;
  return STATUS_CONST(SUCCESS);
}
static inline OPSTATUS WINDOW_INS(NativeUpdate)(WINDOW_TYPE(TWindow) * w,
                                                const GRAPHICS_TYPE(TSurface) * surface) {
  WINDOW_INS(WinState) *s = (WINDOW_INS(WinState) *)w->native;
  BITMAPINFO info = {0};
  HDC dc;
  void *memory = NULL;
  const uint32_t *pixels = surface->pixels;
  uint32_t y;
  OPSTATUS status;
  int result;
  if (surface->stride != surface->size.x) {
    status = ALLOC_FUNC(AllocArray)(&memory, (size_t)surface->size.x * surface->size.y,
                                    sizeof(uint32_t));
    if (status != STATUS_CONST(SUCCESS))
      return status;
    for (y = 0; y < surface->size.y; ++y)
      memcpy((uint32_t *)memory + (size_t)y * surface->size.x, pixels + (size_t)y * surface->stride,
             surface->size.x * sizeof(uint32_t));
    pixels = (const uint32_t *)memory;
  }
  info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
  info.bmiHeader.biWidth = (LONG)surface->size.x;
  info.bmiHeader.biHeight = -(LONG)surface->size.y;
  info.bmiHeader.biPlanes = 1;
  info.bmiHeader.biBitCount = 32;
  info.bmiHeader.biCompression = BI_RGB;
  dc = s->GetDC(s->window);
  if (!dc) {
    Memory_Free(memory);
    return STATUS_CONST(GENERIC_ERROR);
  }
  result = s->StretchDIBits(dc, 0, 0, (int)surface->size.x, (int)surface->size.y, 0, 0,
                            (int)surface->size.x, (int)surface->size.y, pixels, &info,
                            DIB_RGB_COLORS, SRCCOPY);
  s->ReleaseDC(s->window, dc);
  Memory_Free(memory);
  return result == 0 || result == (int)GDI_ERROR ? STATUS_CONST(GENERIC_ERROR)
                                                 : STATUS_CONST(SUCCESS);
}
static inline void WINDOW_INS(NativeClose)(WINDOW_TYPE(TWindow) * w) {
  WINDOW_INS(WinRelease)((WINDOW_INS(WinState) *)w->native);
}
