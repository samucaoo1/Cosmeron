#include "../../Cosmeron/Modules/Input/Input.h"
#include "../../Cosmeron/Modules/Input/Event/Event.h"
#include <assert.h>
#include <string.h>
#include <stdio.h>
int main(void) {
  INPUT_TYPE(TEvent) event = {0}, out = {0};
  assert(INPUT_FUNC(SetCaptureMode)(INPUT_CONST(CAPTURE_APPLICATION)) == STATUS_CONST(SUCCESS));
  event.type = INPUT_CONST(EVENT_TEXT_INPUT);
  strcpy(event.data.text.utf8, "Olá");
  event.data.text.length = strlen(event.data.text.utf8);
  assert(INPUT_EVENT_FUNC(Submit)(&event) == STATUS_CONST(SUCCESS));
  event.data.text.utf8[0] = 'X';
  assert(INPUT_FUNC(Update)() == STATUS_CONST(SUCCESS));
  assert(INPUT_EVENT_FUNC(Poll)(&out));
  assert(out.type == INPUT_CONST(EVENT_TEXT_INPUT) && strcmp(out.data.text.utf8, "Olá") == 0);
  event.type = INPUT_CONST(EVENT_TEXT_COMPOSITION);
  strcpy(event.data.text.utf8, "é");
  event.data.text.length = 2;
  event.data.text.caret = 1;
  assert(INPUT_EVENT_FUNC(Submit)(&event) == STATUS_CONST(INVALID_ARGUMENT));
  event.data.text.caret = 2;
  assert(INPUT_EVENT_FUNC(Submit)(&event) == STATUS_CONST(SUCCESS));
  assert(INPUT_FUNC(Update)() == STATUS_CONST(SUCCESS));
  assert(INPUT_EVENT_FUNC(Poll)(&out));
  assert(out.type == INPUT_CONST(EVENT_TEXT_COMPOSITION));
  memset(&event, 0, sizeof(event));
  event.type = INPUT_CONST(EVENT_KEY_PRESS);
  event.data.key = LIB_PREFIX_CONST(INPUT_KEY_A);
  event.modifiers = INPUT_CONST(MODIFIER_CONTROL) | INPUT_CONST(MODIFIER_SHIFT);
  assert(INPUT_EVENT_FUNC(Submit)(&event) == STATUS_CONST(SUCCESS));
  assert(INPUT_FUNC(Update)() == STATUS_CONST(SUCCESS));
  assert(INPUT_EVENT_FUNC(Poll)(&out));
  assert(out.modifiers == event.modifiers);
  puts("InputText events passed");
  return 0;
}
