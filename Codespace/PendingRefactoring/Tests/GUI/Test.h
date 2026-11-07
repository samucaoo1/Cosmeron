#pragma once
#include "../../Cosmeron/Modules/Graphics/GUI/GUI.h"
#include <assert.h>
#include <stdio.h>
#define OK(EXPR) assert((EXPR) == STATUS_CONST(SUCCESS))
static inline TQUAD_TYPE(float) rect(float x, float y, float width, float height) {
  TQUAD_TYPE(float) r;
  r.value1 = x;
  r.value2 = y;
  r.value3 = width;
  r.value4 = height;
  return r;
}
static inline void position(GUI_TYPE(TContext) * c, float x, float y) {
  INPUT_TYPE(TEvent) e = {0};
  e.type = INPUT_CONST(EVENT_MOUSE_POSITION);
  e.data.motion.x = x;
  e.data.motion.y = y;
  OK(GUI_FUNC(Event)(c, &e));
}
static inline void mouse(GUI_TYPE(TContext) * c, bool down) {
  INPUT_TYPE(TEvent) e = {0};
  e.type = down ? INPUT_CONST(EVENT_MOUSE_BUTTON_PRESS) : INPUT_CONST(EVENT_MOUSE_BUTTON_RELEASE);
  e.data.mouseButton = INPUT_CONST(MOUSE_BUTTON_LEFT);
  OK(GUI_FUNC(Event)(c, &e));
}
static inline void key(GUI_TYPE(TContext) * c, INPUT_TYPE(Key) k, INPUT_TYPE(Modifiers) m) {
  INPUT_TYPE(TEvent) e = {0};
  e.type = INPUT_CONST(EVENT_KEY_PRESS);
  e.data.key = k;
  e.modifiers = m;
  OK(GUI_FUNC(Event)(c, &e));
}
static inline void text(GUI_TYPE(TContext) * c, const char *s) {
  INPUT_TYPE(TEvent) e = {0};
  e.type = INPUT_CONST(EVENT_TEXT_INPUT);
  e.data.text.length = strlen(s);
  memcpy(e.data.text.utf8, s, e.data.text.length + 1);
  OK(GUI_FUNC(Event)(c, &e));
}
