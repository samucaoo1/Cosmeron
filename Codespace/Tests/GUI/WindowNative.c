#include "../../Cosmeron/Modules/Graphics/Window/Window.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
int main(void) {
#if defined(COSMERON_MACRO_INTERNAL_WINDOW_X11)
  WINDOW_TYPE(TWindow) w = {0};
  WINDOW_TYPE(TEvent) e = {0};
  GRAPHICS_TYPE(TSurface) surface = {0};
  WINDOW_INS(XState) * s;
  int (*sync)(Display *, Bool) = NULL;
  int (*resize)(Display *, Window, unsigned, unsigned) = NULL;
  int (*send)(Display *, Window, Bool, long, XEvent *) = NULL;
  KeyCode (*code)(Display *, KeySym) = NULL;
  XImage *(*getImage)(Display *, Drawable, int, int, unsigned, unsigned, unsigned long, int) = NULL;
  bool resized = false, pressed = false, committed = false, released = false;
  unsigned i;
  void *symbol;
  XEvent x = {0};
  XImage *image;
  if (!getenv("COSMERON_WINDOW_NATIVE_TEST")) {
    puts("Window native: opt-in display test");
    return 0;
  }
  {
    OPSTATUS status =
        WINDOW_FUNC(Open)(&w, "Cosmeron native test", (TDUAL_TYPE(uint32)){.x = 64, .y = 48});
    if (status != STATUS_CONST(SUCCESS)) {
      fprintf(stderr, "Native open status: %d\n", (int)status);
      return 1;
    }
  }
  s = (WINDOW_INS(XState) *)w.native;
#define LOAD(F, N)                                                                                 \
  do {                                                                                             \
    symbol = dlsym(s->library, N);                                                                 \
    memcpy(&F, &symbol, sizeof(F));                                                                \
    assert(F);                                                                                     \
  } while (0)
  LOAD(sync, "XSync");
  LOAD(resize, "XResizeWindow");
  LOAD(send, "XSendEvent");
  LOAD(code, "XKeysymToKeycode");
  LOAD(getImage, "XGetImage");
#undef LOAD
  sync(s->display, False);
  resize(s->display, s->window, 80, 60);
  sync(s->display, False);
  for (i = 0; i < 100; ++i) {
    OPSTATUS status = WINDOW_FUNC(Poll)(&w, &e);
    if (status == STATUS_CONST(WOULD_BLOCK))
      break;
    assert(status == STATUS_CONST(SUCCESS));
    if (e.type == WINDOW_CONST(EVENT_RESIZE))
      resized = e.size.x == 80 && e.size.y == 60;
  }
  assert(resized);
  assert(GRAPHICS_FUNC(Surface_Create)(&surface, w.size) == STATUS_CONST(SUCCESS));
  assert(GRAPHICS_FUNC(Surface_Clear)(&surface, UINT32_C(0xFF2288DD)) == STATUS_CONST(SUCCESS));
  assert(WINDOW_FUNC(Update)(&w, &surface) == STATUS_CONST(SUCCESS));
  sync(s->display, False);
  image = getImage(s->display, s->window, 0, 0, 80, 60, AllPlanes, ZPixmap);
  assert(image);
  assert((XGetPixel(image, 30, 30) & UINT32_C(0xFFFFFF)) == UINT32_C(0x2288DD));
  XDestroyImage(image);
  x.xkey.type = KeyPress;
  x.xkey.display = s->display;
  x.xkey.window = s->window;
  x.xkey.root = s->xRootWindow(s->display, s->xDefaultScreen(s->display));
  x.xkey.keycode = code(s->display, XK_a);
  x.xkey.same_screen = True;
  assert(send(s->display, s->window, False, KeyPressMask, &x));
  x.xkey.type = KeyRelease;
  assert(send(s->display, s->window, False, KeyReleaseMask, &x));
  sync(s->display, False);
  for (i = 0; i < 100; ++i) {
    OPSTATUS status = WINDOW_FUNC(Poll)(&w, &e);
    if (status == STATUS_CONST(WOULD_BLOCK))
      break;
    assert(status == STATUS_CONST(SUCCESS));
    if (e.type != WINDOW_CONST(EVENT_INPUT))
      continue;
    if (e.input.type == INPUT_CONST(EVENT_KEY_PRESS))
      pressed = e.input.data.key == LIB_PREFIX_CONST(INPUT_KEY_A);
    if (e.input.type == INPUT_CONST(EVENT_KEY_RELEASE))
      released = e.input.data.key == LIB_PREFIX_CONST(INPUT_KEY_A);
    if (e.input.type == INPUT_CONST(EVENT_TEXT_INPUT))
      committed = e.input.data.text.length == 1 && e.input.data.text.utf8[0] == 'a';
  }
  assert(pressed && released && committed);
  WINDOW_FUNC(Close)(&w);
  GRAPHICS_FUNC(Surface_Free)(&surface);
  puts("Window native X11 pixels, resize and text passed");
#else
  if (getenv("COSMERON_WINDOW_NATIVE_TEST"))
    return 1;
  puts("Window native: X11 development headers not enabled");
#endif
  return 0;
}
