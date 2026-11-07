#include <assert.h>
#include <string.h>

#include "../../Cosmeron/Modules/Input/Input.h"

#define OK(x) assert((x) == STATUS_CONST(SUCCESS))

int main(void) {
  TTerminalInput term = {0};
  OK(INPUT_TERMINAL_FUNC(Init)(&term));
  assert(term.api != NULL);

  OK(term.api->Create(&term, false));
  assert(term.api->PendingEvents(&term) == 0U);
  assert(!term.api->InEscapeSequence(&term));

  size_t consumed = 0;
  Input_TEvent event;
  bool found = false;

  /* ------------------------------------------------------------
   * 1. Standard Characters & Controls
   * ------------------------------------------------------------ */
  /* 'a' */
  OK(term.api->Feed(&term, "a", 1, &consumed));
  assert(consumed == 1);
  OK(term.api->PollEvent(&term, &event, &found));
  assert(found && event.type == INPUT_EVENT_KEY_PRESS && event.data.key == INPUT_KEY_A);
  OK(term.api->PollEvent(&term, &event, &found));
  assert(found && event.type == INPUT_EVENT_KEY_RELEASE && event.data.key == INPUT_KEY_A);

  /* Enter '\r' */
  OK(term.api->Feed(&term, "\r", 1, &consumed));
  OK(term.api->PollEvent(&term, &event, &found));
  assert(found && event.type == INPUT_EVENT_KEY_PRESS && event.data.key == INPUT_KEY_ENTER);
  OK(term.api->PollEvent(&term, &event, &found));
  assert(found && event.type == INPUT_EVENT_KEY_RELEASE && event.data.key == INPUT_KEY_ENTER);

  /* Tab '\t' */
  OK(term.api->Feed(&term, "\t", 1, &consumed));
  OK(term.api->PollEvent(&term, &event, &found));
  assert(found && event.type == INPUT_EVENT_KEY_PRESS && event.data.key == INPUT_KEY_TAB);
  OK(term.api->PollEvent(&term, &event, &found));
  assert(found && event.type == INPUT_EVENT_KEY_RELEASE && event.data.key == INPUT_KEY_TAB);

  /* Backspace (DEL 127) */
  OK(term.api->Feed(&term, "\x7f", 1, &consumed));
  OK(term.api->PollEvent(&term, &event, &found));
  assert(found && event.type == INPUT_EVENT_KEY_PRESS && event.data.key == INPUT_KEY_BACKSPACE);
  OK(term.api->PollEvent(&term, &event, &found));
  assert(found && event.type == INPUT_EVENT_KEY_RELEASE && event.data.key == INPUT_KEY_BACKSPACE);

  /* Ctrl+C (ASCII 3) */
  OK(term.api->Feed(&term, "\x03", 1, &consumed));
  OK(term.api->PollEvent(&term, &event, &found));
  assert(found && event.type == INPUT_EVENT_KEY_PRESS && event.data.key == INPUT_KEY_C);
  assert((event.modifiers & INPUT_MODIFIER_CONTROL) != 0);
  OK(term.api->PollEvent(&term, &event, &found));
  assert(found && event.type == INPUT_EVENT_KEY_RELEASE && event.data.key == INPUT_KEY_C);

  /* ------------------------------------------------------------
   * 2. ANSI Arrows and Modifiers
   * ------------------------------------------------------------ */
  /* Up arrow: "\x1b[A" */
  OK(term.api->Feed(&term, "\x1b[A", 3, &consumed));
  OK(term.api->PollEvent(&term, &event, &found));
  assert(found && event.type == INPUT_EVENT_KEY_PRESS && event.data.key == INPUT_KEY_UP);
  OK(term.api->PollEvent(&term, &event, &found));
  assert(found && event.type == INPUT_EVENT_KEY_RELEASE && event.data.key == INPUT_KEY_UP);

  /* Down arrow: "\x1b[B" */
  OK(term.api->Feed(&term, "\x1b[B", 3, &consumed));
  OK(term.api->PollEvent(&term, &event, &found));
  assert(found && event.type == INPUT_EVENT_KEY_PRESS && event.data.key == INPUT_KEY_DOWN);
  OK(term.api->PollEvent(&term, &event, &found));
  assert(found && event.type == INPUT_EVENT_KEY_RELEASE && event.data.key == INPUT_KEY_DOWN);

  /* Ctrl+Up arrow: "\x1b[1;5A" */
  OK(term.api->Feed(&term, "\x1b[1;5A", 6, &consumed));
  OK(term.api->PollEvent(&term, &event, &found));
  assert(found && event.type == INPUT_EVENT_KEY_PRESS && event.data.key == INPUT_KEY_UP);
  assert((event.modifiers & INPUT_MODIFIER_CONTROL) != 0);
  OK(term.api->PollEvent(&term, &event, &found));
  assert(found && event.type == INPUT_EVENT_KEY_RELEASE);

  /* Shift+Right arrow: "\x1b[1;2C" */
  OK(term.api->Feed(&term, "\x1b[1;2C", 6, &consumed));
  OK(term.api->PollEvent(&term, &event, &found));
  assert(found && event.type == INPUT_EVENT_KEY_PRESS && event.data.key == INPUT_KEY_RIGHT);
  assert((event.modifiers & INPUT_MODIFIER_SHIFT) != 0);
  OK(term.api->PollEvent(&term, &event, &found));
  assert(found && event.type == INPUT_EVENT_KEY_RELEASE);

  /* Alt+Down arrow: "\x1b[1;3B" */
  OK(term.api->Feed(&term, "\x1b[1;3B", 6, &consumed));
  OK(term.api->PollEvent(&term, &event, &found));
  assert(found && event.type == INPUT_EVENT_KEY_PRESS && event.data.key == INPUT_KEY_DOWN);
  assert((event.modifiers & INPUT_MODIFIER_ALT) != 0);
  OK(term.api->PollEvent(&term, &event, &found));
  assert(found && event.type == INPUT_EVENT_KEY_RELEASE);

  /* ------------------------------------------------------------
   * 3. Function Keys and Navigation Keys
   * ------------------------------------------------------------ */
  /* F1 via SS3: "\x1bOP" */
  OK(term.api->Feed(&term, "\x1bOP", 3, &consumed));
  OK(term.api->PollEvent(&term, &event, &found));
  assert(found && event.data.key == INPUT_KEY_F1);
  OK(term.api->PollEvent(&term, &event, &found));

  /* F5 via CSI: "\x1b[15~" */
  OK(term.api->Feed(&term, "\x1b[15~", 5, &consumed));
  OK(term.api->PollEvent(&term, &event, &found));
  assert(found && event.data.key == INPUT_KEY_F5);
  OK(term.api->PollEvent(&term, &event, &found));

  /* F12: "\x1b[24~" */
  OK(term.api->Feed(&term, "\x1b[24~", 5, &consumed));
  OK(term.api->PollEvent(&term, &event, &found));
  assert(found && event.data.key == INPUT_KEY_F12);
  OK(term.api->PollEvent(&term, &event, &found));

  /* Insert: "\x1b[2~" */
  OK(term.api->Feed(&term, "\x1b[2~", 4, &consumed));
  OK(term.api->PollEvent(&term, &event, &found));
  assert(found && event.data.key == INPUT_KEY_INSERT);
  OK(term.api->PollEvent(&term, &event, &found));

  /* Delete: "\x1b[3~" */
  OK(term.api->Feed(&term, "\x1b[3~", 4, &consumed));
  OK(term.api->PollEvent(&term, &event, &found));
  assert(found && event.data.key == INPUT_KEY_DELETE);
  OK(term.api->PollEvent(&term, &event, &found));

  /* Home: "\x1b[H" */
  OK(term.api->Feed(&term, "\x1b[H", 3, &consumed));
  OK(term.api->PollEvent(&term, &event, &found));
  assert(found && event.data.key == INPUT_KEY_HOME);
  OK(term.api->PollEvent(&term, &event, &found));

  /* End: "\x1b[F" */
  OK(term.api->Feed(&term, "\x1b[F", 3, &consumed));
  OK(term.api->PollEvent(&term, &event, &found));
  assert(found && event.data.key == INPUT_KEY_END);
  OK(term.api->PollEvent(&term, &event, &found));

  /* Page Up: "\x1b[5~" */
  OK(term.api->Feed(&term, "\x1b[5~", 4, &consumed));
  OK(term.api->PollEvent(&term, &event, &found));
  assert(found && event.data.key == INPUT_KEY_PAGE_UP);
  OK(term.api->PollEvent(&term, &event, &found));

  /* Page Down: "\x1b[6~" */
  OK(term.api->Feed(&term, "\x1b[6~", 4, &consumed));
  OK(term.api->PollEvent(&term, &event, &found));
  assert(found && event.data.key == INPUT_KEY_PAGE_DOWN);
  OK(term.api->PollEvent(&term, &event, &found));

  /* ------------------------------------------------------------
   * 4. SGR Mouse Reporting
   * ------------------------------------------------------------ */
  /* Left button press at col 10, row 20: "\x1b[<0;10;20M" */
  OK(term.api->Feed(&term, "\x1b[<0;10;20M", 11, &consumed));
  OK(term.api->PollEvent(&term, &event, &found));
  assert(found && event.type == INPUT_EVENT_MOUSE_POSITION);
  assert(event.data.motion.x == 10.0f && event.data.motion.y == 20.0f);
  OK(term.api->PollEvent(&term, &event, &found));
  assert(found && event.type == INPUT_EVENT_MOUSE_BUTTON_PRESS);
  assert(event.data.mouseButton == INPUT_MOUSE_BUTTON_LEFT);

  /* Left button release at col 10, row 20: "\x1b[<0;10;20m" */
  OK(term.api->Feed(&term, "\x1b[<0;10;20m", 11, &consumed));
  OK(term.api->PollEvent(&term, &event, &found));
  assert(found && event.type == INPUT_EVENT_MOUSE_POSITION);
  OK(term.api->PollEvent(&term, &event, &found));
  assert(found && event.type == INPUT_EVENT_MOUSE_BUTTON_RELEASE);
  assert(event.data.mouseButton == INPUT_MOUSE_BUTTON_LEFT);

  /* Motion with button pressed: "\x1b[<32;15;25M" */
  OK(term.api->Feed(&term, "\x1b[<32;15;25M", 12, &consumed));
  OK(term.api->PollEvent(&term, &event, &found));
  assert(found && event.type == INPUT_EVENT_MOUSE_POSITION);
  assert(event.data.motion.x == 15.0f && event.data.motion.y == 25.0f);

  /* Wheel Scroll Up: "\x1b[<64;10;20M" */
  OK(term.api->Feed(&term, "\x1b[<64;10;20M", 12, &consumed));
  OK(term.api->PollEvent(&term, &event, &found));
  assert(found && event.type == INPUT_EVENT_MOUSE_SCROLL);
  assert(event.data.motion.y == 1.0f);

  /* Wheel Scroll Down: "\x1b[<65;10;20M" */
  OK(term.api->Feed(&term, "\x1b[<65;10;20M", 12, &consumed));
  OK(term.api->PollEvent(&term, &event, &found));
  assert(found && event.type == INPUT_EVENT_MOUSE_SCROLL);
  assert(event.data.motion.y == -1.0f);

  /* ------------------------------------------------------------
   * 5. Cleanup
   * ------------------------------------------------------------ */
  OK(term.api->Destroy(&term));
  assert(term.api->PendingEvents(&term) == 0U);

  return 0;
}
