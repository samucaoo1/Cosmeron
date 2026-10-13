#include "../../Cosmeron/Modules/Graphics/Window/Window.h"
#include <assert.h>
#include <stdio.h>
typedef struct Host {
  unsigned polls, updates, closed;
} Host;
static OPSTATUS openHost(void *user, const char *title, TDUAL_TYPE(uint32) size) {
  assert(user && title && size.x == 16);
  return STATUS_CONST(SUCCESS);
}
static OPSTATUS pollHost(void *user, WINDOW_TYPE(TEvent) * out) {
  Host *h = (Host *)user;
  WINDOW_TYPE(TEvent) e = {0};
  if (h->polls++ == 0) {
    e.type = WINDOW_CONST(EVENT_RESIZE);
    e.size.x = 32;
    e.size.y = 24;
    *out = e;
    return STATUS_CONST(SUCCESS);
  }
  return STATUS_CONST(WOULD_BLOCK);
}
static OPSTATUS updateHost(void *user, const GRAPHICS_TYPE(TSurface) * surface) {
  Host *h = (Host *)user;
  assert(surface->size.x == 32);
  ++h->updates;
  return STATUS_CONST(SUCCESS);
}
static void closeHost(void *user) { ++((Host *)user)->closed; }
int main(void) {
  Host h = {0};
  WINDOW_TYPE(TWindow) window = {0};
  WINDOW_TYPE(TAdapter) adapter = {0};
  WINDOW_TYPE(TEvent) event = {0};
  GRAPHICS_TYPE(TSurface) s = {0};
  adapter.user = &h;
  adapter.open = openHost;
  adapter.poll = pollHost;
  adapter.update = updateHost;
  adapter.close = closeHost;
  assert(WINDOW_FUNC(OpenWith)(&window, "Host", (TDUAL_TYPE(uint32)){.x = 16, .y = 16}, &adapter) ==
         STATUS_CONST(SUCCESS));
  assert(WINDOW_FUNC(IsOpen)(&window));
  assert(WINDOW_FUNC(Poll)(&window, &event) == STATUS_CONST(SUCCESS));
  assert(event.type == WINDOW_CONST(EVENT_RESIZE) && window.size.x == 32);
  event.type = WINDOW_CONST(EVENT_CLOSE);
  assert(WINDOW_FUNC(Poll)(&window, &event) == STATUS_CONST(WOULD_BLOCK));
  assert(event.type == WINDOW_CONST(EVENT_CLOSE));
  assert(GRAPHICS_FUNC(Surface_Create)(&s, (TDUAL_TYPE(uint32)){.x = 32, .y = 24}) ==
         STATUS_CONST(SUCCESS));
  assert(WINDOW_FUNC(Update)(&window, &s) == STATUS_CONST(SUCCESS));
  assert(h.updates == 1);
  WINDOW_FUNC(Close)(&window);
  WINDOW_FUNC(Close)(&window);
  assert(h.closed == 1 && !WINDOW_FUNC(IsOpen)(&window));
  GRAPHICS_FUNC(Surface_Free)(&s);
  puts("Window adapter passed");
  return 0;
}
