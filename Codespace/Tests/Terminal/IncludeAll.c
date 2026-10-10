#include "../../Cosmeron/Modules/Graphics/Terminal/Terminal.h"
int main(void) {
  TERMINAL_TYPE(TTerminal) *terminal = TERMINAL_FUNC(Standard)();
  return terminal && TERMINAL_FUNC(StandardHandle)(TERMINAL_CONST(STDOUT)) ? 0 : 1;
}
