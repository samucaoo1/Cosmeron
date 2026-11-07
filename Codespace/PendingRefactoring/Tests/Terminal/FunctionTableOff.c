#define TERMINAL_DISABLE_FUNCTION_TABLE
#include <assert.h>
#include <string.h>

#include "../../Cosmeron/Modules/Graphics/Terminal/Terminal.h"

int main(void) {
  assert(Terminal_Standard() != NULL);
  assert(Terminal_Cursor_Show() != STATUS_CONST(INVALID_ARGUMENT));
  return 0;
}
