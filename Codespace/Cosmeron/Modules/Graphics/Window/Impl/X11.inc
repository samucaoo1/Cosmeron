#pragma once
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include <dlfcn.h>
#include <locale.h>
/* Xlib defines Status as a macro; do not leak it into Cosmeron namespaces. */
#undef Status

#define COSMERON_MACRO_INTERNAL_WINDOW_X_FUNCTIONS(X)                                              \
  X(Display *, OpenDisplay, (const char *))                                                        \
  X(int, CloseDisplay, (Display *))                                                                \
  X(int, DefaultScreen, (Display *))                                                               \
  X(Window, RootWindow, (Display *, int))                                                          \
  X(Visual *, DefaultVisual, (Display *, int))                                                     \
  X(int, DefaultDepth, (Display *, int))                                                           \
  X(unsigned long, BlackPixel, (Display *, int))                                                   \
  X(Window, CreateSimpleWindow,                                                                    \
    (Display *, Window, int, int, unsigned, unsigned, unsigned, unsigned long, unsigned long))     \
  X(int, DestroyWindow, (Display *, Window))                                                       \
  X(int, StoreName, (Display *, Window, const char *))                                             \
  X(int, SelectInput, (Display *, Window, long))                                                   \
  X(int, MapWindow, (Display *, Window))                                                           \
  X(Atom, InternAtom, (Display *, const char *, Bool))                                             \
  X(int, SetWMProtocols, (Display *, Window, Atom *, int))                                         \
  X(GC, CreateGC, (Display *, Drawable, unsigned long, XGCValues *))                               \
  X(int, FreeGC, (Display *, GC))                                                                  \
  X(int, Pending, (Display *))                                                                     \
  X(int, NextEvent, (Display *, XEvent *))                                                         \
  X(XImage *, CreateImage,                                                                         \
    (Display *, Visual *, unsigned, int, int, char *, unsigned, unsigned, int, int))               \
  X(int, PutImage, (Display *, Drawable, GC, XImage *, int, int, int, int, unsigned, unsigned))    \
  X(int, Flush, (Display *))                                                                       \
  X(int, LookupString, (XKeyEvent *, char *, int, KeySym *, XComposeStatus *))                     \
  X(Bool, FilterEvent, (XEvent *, Window))                                                         \
  X(XIM, OpenIM, (Display *, struct _XrmHashBucketRec *, char *, char *))                          \
  X(int, CloseIM, (XIM))                                                                           \
  X(XIC, CreateIC, (XIM, ...))                                                                     \
  X(void, DestroyIC, (XIC))                                                                        \
  X(void, SetICFocus, (XIC))                                                                       \
  X(void, UnsetICFocus, (XIC))                                                                     \
  X(int, utf8LookupString, (XIC, XKeyPressedEvent *, char *, int, KeySym *, int *))

typedef struct WINDOW_INS(XState) {
  void *library;
  Display *display;
  Window window;
  GC gc;
  XImage *image;
  Visual *visual;
  int depth;
  Atom closeAtom;
  XIM im;
  XIC ic;
  WINDOW_TYPE(TEvent) events[WINDOW_EVENT_CAPACITY];
  size_t head, count;
#define COSMERON_MACRO_INTERNAL_WINDOW_X_FIELD(RET, NAME, ARGS) RET(*PP_OP_CAT2(x, NAME)) ARGS;
  COSMERON_MACRO_INTERNAL_WINDOW_X_FUNCTIONS(COSMERON_MACRO_INTERNAL_WINDOW_X_FIELD)
#undef COSMERON_MACRO_INTERNAL_WINDOW_X_FIELD
} WINDOW_INS(XState);
static inline void WINDOW_INS(XRelease)(WINDOW_INS(XState) * s) {
  if (!s)
    return;
  if (s->image)
    XDestroyImage(s->image);
  if (s->ic)
    s->xDestroyIC(s->ic);
  if (s->im)
    s->xCloseIM(s->im);
  if (s->gc)
    s->xFreeGC(s->display, s->gc);
  if (s->window)
    s->xDestroyWindow(s->display, s->window);
  if (s->display)
    s->xCloseDisplay(s->display);
  if (s->library)
    dlclose(s->library);
  Memory_Free(s);
}
static inline OPSTATUS WINDOW_INS(NativeOpen)(WINDOW_TYPE(TWindow) * w, const char *title,
                                              TDUAL_TYPE(uint32) size) {
  WINDOW_INS(XState) * s;
  void *memory = NULL;
  int screen;
  OPSTATUS status;
  status = ALLOC_FUNC(AllocBytes)(&memory, sizeof(*s));
  if (status != STATUS_CONST(SUCCESS))
    return status;
  s = (WINDOW_INS(XState) *)memory;
  memset(s, 0, sizeof(*s));
  s->library = dlopen("libX11.so.6", RTLD_LOCAL | RTLD_LAZY);
  if (!s->library)
    s->library = dlopen("libX11.so", RTLD_LOCAL | RTLD_LAZY);
  if (!s->library) {
    WINDOW_INS(XRelease)(s);
    return STATUS_CONST(NOT_AVAILABLE);
  }
#define COSMERON_MACRO_INTERNAL_WINDOW_X_LOAD(RET, NAME, ARGS)                                     \
  do {                                                                                             \
    void *symbol = dlsym(s->library, "X" PP_OP_STRING(NAME));                                      \
    if (sizeof(symbol) != sizeof(s->PP_OP_CAT2(x, NAME)) || !symbol) {                             \
      WINDOW_INS(XRelease)(s);                                                                     \
      return STATUS_CONST(NOT_AVAILABLE);                                                          \
    }                                                                                              \
    memcpy(&s->PP_OP_CAT2(x, NAME), &symbol, sizeof(symbol));                                      \
  } while (0);
  COSMERON_MACRO_INTERNAL_WINDOW_X_FUNCTIONS(COSMERON_MACRO_INTERNAL_WINDOW_X_LOAD)
#undef COSMERON_MACRO_INTERNAL_WINDOW_X_LOAD
  s->display = s->xOpenDisplay(NULL);
  if (!s->display) {
    WINDOW_INS(XRelease)(s);
    return STATUS_CONST(NOT_AVAILABLE);
  }
  screen = s->xDefaultScreen(s->display);
  s->visual = s->xDefaultVisual(s->display, screen);
  s->depth = s->xDefaultDepth(s->display, screen);
  if (s->visual->class != TrueColor) {
    WINDOW_INS(XRelease)(s);
    return STATUS_CONST(NOT_SUPPORTED);
  }
  s->window = s->xCreateSimpleWindow(s->display, s->xRootWindow(s->display, screen), 0, 0, size.x,
                                     size.y, 0, 0, s->xBlackPixel(s->display, screen));
  if (!s->window) {
    WINDOW_INS(XRelease)(s);
    return STATUS_CONST(GENERIC_ERROR);
  }
  s->gc = s->xCreateGC(s->display, s->window, 0, NULL);
  if (!s->gc) {
    WINDOW_INS(XRelease)(s);
    return STATUS_CONST(GENERIC_ERROR);
  }
  s->xStoreName(s->display, s->window, title);
  s->xSelectInput(s->display, s->window,
                  KeyPressMask | KeyReleaseMask | ButtonPressMask | ButtonReleaseMask |
                      PointerMotionMask | StructureNotifyMask | FocusChangeMask | ExposureMask);
  s->closeAtom = s->xInternAtom(s->display, "WM_DELETE_WINDOW", False);
  s->xSetWMProtocols(s->display, s->window, &s->closeAtom, 1);
  /* The application owns its process locale. OpenIM works when it has selected a UTF-8 locale. */
  s->im = s->xOpenIM(s->display, NULL, NULL, NULL);
  if (s->im)
    s->ic = s->xCreateIC(s->im, XNInputStyle, XIMPreeditNothing | XIMStatusNothing, XNClientWindow,
                         s->window, XNFocusWindow, s->window, NULL);
  s->xMapWindow(s->display, s->window);
  s->xFlush(s->display);
  w->native = s;
  return STATUS_CONST(SUCCESS);
}
static inline INPUT_TYPE(Modifiers) WINDOW_INS(XModifiers)(unsigned state) {
  INPUT_TYPE(Modifiers) m = 0;
  if (state & ShiftMask)
    m |= INPUT_CONST(MODIFIER_SHIFT);
  if (state & ControlMask)
    m |= INPUT_CONST(MODIFIER_CONTROL);
  if (state & Mod1Mask)
    m |= INPUT_CONST(MODIFIER_ALT);
  if (state & Mod4Mask)
    m |= INPUT_CONST(MODIFIER_SUPER);
  return m;
}
static inline INPUT_TYPE(Key) WINDOW_INS(XKey)(KeySym key) {
  if (key >= XK_a && key <= XK_z)
    return (INPUT_TYPE(Key))(LIB_PREFIX_CONST(INPUT_KEY_A) + (key - XK_a));
  if (key >= XK_A && key <= XK_Z)
    return (INPUT_TYPE(Key))(LIB_PREFIX_CONST(INPUT_KEY_A) + (key - XK_A));
  if (key >= XK_0 && key <= XK_9)
    return (INPUT_TYPE(Key))(LIB_PREFIX_CONST(INPUT_KEY_DIGIT_0) + (key - XK_0));
  if (key >= XK_F1 && key <= XK_F12)
    return (INPUT_TYPE(Key))(LIB_PREFIX_CONST(INPUT_KEY_F1) + (key - XK_F1));
  switch (key) {
  case XK_Return:
    return LIB_PREFIX_CONST(INPUT_KEY_ENTER);
  case XK_Tab:
  case XK_ISO_Left_Tab:
    return LIB_PREFIX_CONST(INPUT_KEY_TAB);
  case XK_Escape:
    return LIB_PREFIX_CONST(INPUT_KEY_ESCAPE);
  case XK_space:
    return LIB_PREFIX_CONST(INPUT_KEY_SPACE);
  case XK_BackSpace:
    return LIB_PREFIX_CONST(INPUT_KEY_BACKSPACE);
  case XK_Delete:
    return LIB_PREFIX_CONST(INPUT_KEY_DELETE);
  case XK_Left:
    return LIB_PREFIX_CONST(INPUT_KEY_LEFT);
  case XK_Right:
    return LIB_PREFIX_CONST(INPUT_KEY_RIGHT);
  case XK_Up:
    return LIB_PREFIX_CONST(INPUT_KEY_UP);
  case XK_Down:
    return LIB_PREFIX_CONST(INPUT_KEY_DOWN);
  case XK_Home:
    return LIB_PREFIX_CONST(INPUT_KEY_HOME);
  case XK_End:
    return LIB_PREFIX_CONST(INPUT_KEY_END);
  case XK_Page_Up:
    return LIB_PREFIX_CONST(INPUT_KEY_PAGE_UP);
  case XK_Page_Down:
    return LIB_PREFIX_CONST(INPUT_KEY_PAGE_DOWN);
  default:
    return LIB_PREFIX_CONST(INPUT_KEY_UNKNOWN);
  }
}
static inline OPSTATUS WINDOW_INS(XPush)(WINDOW_INS(XState) * s, WINDOW_TYPE(TEvent) e) {
  if (s->count == WINDOW_EVENT_CAPACITY)
    return STATUS_CONST(INSUFFICIENT_SPACE);
  s->events[(s->head + s->count++) % WINDOW_EVENT_CAPACITY] = e;
  return STATUS_CONST(SUCCESS);
}
static inline OPSTATUS WINDOW_INS(NativePoll)(WINDOW_TYPE(TWindow) * w, WINDOW_TYPE(TEvent) * out) {
  WINDOW_INS(XState) *s = (WINDOW_INS(XState) *)w->native;
  while (!s->count && s->xPending(s->display)) {
    XEvent x;
    WINDOW_TYPE(TEvent) e = {0};
    OPSTATUS status = STATUS_CONST(SUCCESS);
    s->xNextEvent(s->display, &x);
    if (s->xFilterEvent(&x, s->window))
      continue;
    if (x.type == ClientMessage && (Atom)x.xclient.data.l[0] == s->closeAtom) {
      e.type = WINDOW_CONST(EVENT_CLOSE);
      status = WINDOW_INS(XPush)(s, e);
    } else if (x.type == ConfigureNotify && x.xconfigure.width > 0 && x.xconfigure.height > 0) {
      e.type = WINDOW_CONST(EVENT_RESIZE);
      e.size.x = (uint32_t)x.xconfigure.width;
      e.size.y = (uint32_t)x.xconfigure.height;
      if (e.size.x != w->size.x || e.size.y != w->size.y)
        status = WINDOW_INS(XPush)(s, e);
    } else if (x.type == FocusIn || x.type == FocusOut) {
      e.input.type =
          x.type == FocusIn ? INPUT_CONST(EVENT_FOCUS_GAINED) : INPUT_CONST(EVENT_FOCUS_LOST);
      if (s->ic) {
        if (x.type == FocusIn)
          s->xSetICFocus(s->ic);
        else
          s->xUnsetICFocus(s->ic);
      }
      status = WINDOW_INS(XPush)(s, e);
    } else if (x.type == MotionNotify) {
      e.input.type = INPUT_CONST(EVENT_MOUSE_POSITION);
      e.input.data.motion.x = (float)x.xmotion.x;
      e.input.data.motion.y = (float)x.xmotion.y;
      status = WINDOW_INS(XPush)(s, e);
    } else if (x.type == ButtonPress || x.type == ButtonRelease) {
      e.input.type = INPUT_CONST(EVENT_MOUSE_POSITION);
      e.input.data.motion.x = (float)x.xbutton.x;
      e.input.data.motion.y = (float)x.xbutton.y;
      status = WINDOW_INS(XPush)(s, e);
      memset(&e, 0, sizeof(e));
      e.input.modifiers = WINDOW_INS(XModifiers)(x.xbutton.state);
      if (x.xbutton.button == 4 || x.xbutton.button == 5) {
        if (x.type == ButtonRelease)
          continue;
        e.input.type = INPUT_CONST(EVENT_MOUSE_SCROLL);
        e.input.data.motion.y = x.xbutton.button == 4 ? 1 : -1;
      } else {
        e.input.type = x.type == ButtonPress ? INPUT_CONST(EVENT_MOUSE_BUTTON_PRESS)
                                             : INPUT_CONST(EVENT_MOUSE_BUTTON_RELEASE);
        if (x.xbutton.button == 1)
          e.input.data.mouseButton = INPUT_CONST(MOUSE_BUTTON_LEFT);
        else if (x.xbutton.button == 2)
          e.input.data.mouseButton = INPUT_CONST(MOUSE_BUTTON_MIDDLE);
        else if (x.xbutton.button == 3)
          e.input.data.mouseButton = INPUT_CONST(MOUSE_BUTTON_RIGHT);
        else
          continue;
      }
      if (status == STATUS_CONST(SUCCESS))
        status = WINDOW_INS(XPush)(s, e);
    } else if (x.type == KeyPress || x.type == KeyRelease) {
      char text[256];
      KeySym key = 0;
      int length;
      int lookup = 0;
      if (x.type == KeyPress && s->ic)
        length = s->xutf8LookupString(s->ic, &x.xkey, text, (int)sizeof(text), &key, &lookup);
      else
        length = s->xLookupString(&x.xkey, text, (int)sizeof(text), &key, NULL);
      if (lookup == XBufferOverflow || length >= (int)sizeof(text))
        return STATUS_CONST(INSUFFICIENT_SPACE);
      e.input.type =
          x.type == KeyPress ? INPUT_CONST(EVENT_KEY_PRESS) : INPUT_CONST(EVENT_KEY_RELEASE);
      e.input.data.key = WINDOW_INS(XKey)(key);
      e.input.modifiers = WINDOW_INS(XModifiers)(x.xkey.state);
      status = WINDOW_INS(XPush)(s, e);
      if (x.type == KeyPress && length > 0 && !(x.xkey.state & (ControlMask | Mod1Mask))) {
        size_t offset = 0;
        while (offset < (size_t)length) {
          size_t used = 0;
          memset(&e, 0, sizeof(e));
          e.input.type = INPUT_CONST(EVENT_TEXT_INPUT);
          while (offset < (size_t)length) {
            TEXT_TYPE(TChar32) cp;
            size_t n;
            OPSTATUS decoded;
            if (s->ic) {
              decoded = TEXT_UTF8_FUNC(Decode)((const unsigned char *)text + offset,
                                               (size_t)length - offset, &cp, &n);
              if (decoded != STATUS_CONST(SUCCESS))
                return decoded;
            } else {
              cp = (unsigned char)text[offset];
              n = 1;
            }
            if (cp < 32 && cp != '\n' && cp != '\r') {
              offset += n;
              continue;
            }
            if (cp == '\r')
              cp = '\n';
            {
              unsigned char encoded[4];
              size_t bytes;
              decoded = TEXT_UTF8_FUNC(Encode)(cp, encoded, sizeof(encoded), &bytes);
              if (decoded != STATUS_CONST(SUCCESS))
                return decoded;
              if (used + bytes >= sizeof(e.input.data.text.utf8))
                break;
              memcpy(e.input.data.text.utf8 + used, encoded, bytes);
              used += bytes;
              offset += n;
            }
          }
          e.input.data.text.length = used;
          e.input.data.text.utf8[used] = 0;
          if (used) {
            status = WINDOW_INS(XPush)(s, e);
            if (status != STATUS_CONST(SUCCESS))
              return status;
          }
        }
      }
    }
    if (status != STATUS_CONST(SUCCESS))
      return status;
  }
  if (!s->count)
    return STATUS_CONST(WOULD_BLOCK);
  *out = s->events[s->head];
  s->head = (s->head + 1) % WINDOW_EVENT_CAPACITY;
  --s->count;
  return STATUS_CONST(SUCCESS);
}
static inline unsigned long WINDOW_INS(XChannel)(uint32_t value, unsigned long mask) {
  unsigned shift = 0;
  unsigned long max;
  if (!mask)
    return 0;
  while (!(mask & (1UL << shift)))
    ++shift;
  max = mask >> shift;
  return (((unsigned long)value * max + 127) / 255) << shift;
}
static inline OPSTATUS WINDOW_INS(NativeUpdate)(WINDOW_TYPE(TWindow) * w,
                                                const GRAPHICS_TYPE(TSurface) * surface) {
  WINDOW_INS(XState) *s = (WINDOW_INS(XState) *)w->native;
  uint32_t x, y;
  if (s->image && ((uint32_t)s->image->width != surface->size.x ||
                   (uint32_t)s->image->height != surface->size.y)) {
    XDestroyImage(s->image);
    s->image = NULL;
  }
  if (!s->image) {
    void *memory = NULL;
    OPSTATUS status;
    s->image = s->xCreateImage(s->display, s->visual, (unsigned)s->depth, ZPixmap, 0, NULL,
                               surface->size.x, surface->size.y, 32, 0);
    if (!s->image)
      return STATUS_CONST(OUT_OF_MEMORY);
    if (s->image->bytes_per_line <= 0 ||
        (size_t)s->image->height > SIZE_MAX / (size_t)s->image->bytes_per_line) {
      XDestroyImage(s->image);
      s->image = NULL;
      return STATUS_CONST(ARITHMETIC_OVERFLOW);
    }
    status =
        ALLOC_FUNC(AllocArray)(&memory, (size_t)s->image->height, (size_t)s->image->bytes_per_line);
    if (status != STATUS_CONST(SUCCESS)) {
      XDestroyImage(s->image);
      s->image = NULL;
      return status;
    }
    s->image->data = (char *)memory;
  }
  for (y = 0; y < surface->size.y; ++y)
    for (x = 0; x < surface->size.x; ++x) {
      uint32_t p = surface->pixels[(size_t)y * surface->stride + x];
      unsigned long native = WINDOW_INS(XChannel)((p >> 16) & 255U, s->visual->red_mask) |
                             WINDOW_INS(XChannel)((p >> 8) & 255U, s->visual->green_mask) |
                             WINDOW_INS(XChannel)(p & 255U, s->visual->blue_mask);
      XPutPixel(s->image, (int)x, (int)y, native);
    }
  s->xPutImage(s->display, s->window, s->gc, s->image, 0, 0, 0, 0, surface->size.x,
               surface->size.y);
  s->xFlush(s->display);
  return STATUS_CONST(SUCCESS);
}
static inline void WINDOW_INS(NativeClose)(WINDOW_TYPE(TWindow) * w) {
  WINDOW_INS(XRelease)((WINDOW_INS(XState) *)w->native);
}
