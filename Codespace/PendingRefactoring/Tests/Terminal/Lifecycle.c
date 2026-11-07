#include <assert.h>
#include <string.h>

#include "../../Cosmeron/Modules/Graphics/Terminal/Terminal.h"

int main(void) {
  /* In automated non-TTY pipelines (pipes/CI), terminal state emission safely checks TTY */
  OPSTATUS status = Terminal_EnterAlternateScreen();
  assert(status == STATUS_CONST(SUCCESS) || status == STATUS_CONST(NOT_AVAILABLE));

  status = Terminal_LeaveAlternateScreen();
  assert(status == STATUS_CONST(SUCCESS) || status == STATUS_CONST(NOT_AVAILABLE));

  status = Terminal_EnterRawMode();
  assert(status == STATUS_CONST(SUCCESS) || status == STATUS_CONST(NOT_AVAILABLE) || status == STATUS_CONST(GENERIC_ERROR));

  status = Terminal_LeaveRawMode();
  assert(status == STATUS_CONST(SUCCESS) || status == STATUS_CONST(NOT_AVAILABLE) || status == STATUS_CONST(GENERIC_ERROR));

  status = Terminal_SaveState();
  assert(status == STATUS_CONST(SUCCESS) || status == STATUS_CONST(NOT_AVAILABLE));

  status = Terminal_RestoreState();
  assert(status == STATUS_CONST(SUCCESS) || status == STATUS_CONST(NOT_AVAILABLE));

  return 0;
}
