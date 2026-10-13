#define GUI_MAX_WIDGETS 1
#define GUI_MAX_COMMANDS 8
#define GUI_TEXT_CAPACITY 64
#define GUI_MAX_EVENTS 4
#include "Test.h"
static OPSTATUS custom(void *user, GRAPHICS_TYPE(TSurface) * s, const GUI_TYPE(TCommand) * commands,
                       size_t count, float scale) {
  size_t *seen = (size_t *)user;
  assert(s && commands && scale == 1);
  *seen = count;
  return STATUS_CONST(SUCCESS);
}
int main(void) {
  GRAPHICS_TYPE(TSurface) s = {0};
  GUI_TYPE(TContext) c = {0};
  bool output = true;
  size_t seen = 0, i;
  uint32_t original;
  OK(GRAPHICS_FUNC(Surface_Create)(&s, (TDUAL_TYPE(uint32)){.x = 80, .y = 60}));
  OK(GRAPHICS_FUNC(Surface_Clear)(&s, UINT32_C(0xFF123456)));
  original = s.pixels[0];
  OK(GUI_FUNC(Begin)(&c, &s));
  for (i = 0; i < 8; ++i)
    OK(GUI_FUNC(DrawRectangle)(&c, rect(0, 0, 4, 4), UINT32_C(0xFFFFFFFF), 0));
  assert(GUI_FUNC(Button)(&c, rect(5, 5, 30, 20), "Fail", &output) ==
         STATUS_CONST(INSUFFICIENT_SPACE));
  assert(output && c.idCount == 0);
  OK(GUI_FUNC(End)(&c));
  OK(GRAPHICS_FUNC(Surface_Clear)(&s, original));
  OK(GUI_FUNC(Begin)(&c, &s));
  OK(GUI_FUNC(Clip_Push)(&c, rect(0, 0, 10, 10)));
  OK(GUI_FUNC(DrawRectangle)(&c, rect(0, 0, 8, 8), UINT32_C(0xFFFFFFFF), 0));
  assert(GUI_FUNC(End)(&c) == STATUS_CONST(INVALID_ARGUMENT));
  assert(!c.frame && s.pixels[0] == original);
  OK(GUI_FUNC(Begin)(&c, &s));
  for (i = 0; i < 4; ++i)
    position(&c, (float)i, 1);
  {
    INPUT_TYPE(TEvent) e = {0};
    e.type = INPUT_CONST(EVENT_MOUSE_POSITION);
    e.data.motion.x = 20;
    assert(GUI_FUNC(Event)(&c, &e) == STATUS_CONST(INSUFFICIENT_SPACE));
    assert(c.pointer.x == 3);
  }
  OK(GUI_FUNC(End)(&c));
  c.renderer = custom;
  c.rendererUser = &seen;
  OK(GUI_FUNC(Begin)(&c, &s));
  OK(GUI_FUNC(DrawRectangle)(&c, rect(0, 0, 4, 4), UINT32_C(0xFFFFFFFF), 0));
  OK(GUI_FUNC(End)(&c));
  assert(seen == 1 && s.pixels[0] == original);
  c.renderer = NULL;
  OK(GUI_FUNC(Begin)(&c, &s));
  assert(GUI_FUNC(DrawText)(&c, rect(0, 0, 20, 20), "\xC0\xAF", 0, NULL, 1) ==
         STATUS_CONST(INVALID_SEQUENCE));
  assert(c.commandCount == 0);
  OK(GUI_FUNC(End)(&c));
  GRAPHICS_FUNC(Surface_Free)(&s);
  puts("Failure passed");
  return 0;
}
