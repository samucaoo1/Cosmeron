#define UI_DISABLE_FUNCTION_TABLE
#include <assert.h>
#include <string.h>

#include "../../Cosmeron/Modules/Graphics/UI/UI.h"

int main(void) {
  UI_TYPE(TContext) ui = {0};
  assert(UI_FUNC(Init)(&ui) == STATUS_CONST(SUCCESS));

  TERMINAL_TYPE(TCanvas) canvas = {0};
  assert(TERMINAL_FUNC(Canvas_Create)(&canvas, (TDUAL_TYPE(uint16)){.col = 20, .row = 10}) == STATUS_CONST(SUCCESS));

  assert(UI_FUNC(Begin)(&ui, &canvas) == STATUS_CONST(SUCCESS));
  bool pressed = false;
  assert(UI_FUNC(Button)(&ui, (TQUAD_TYPE(uint16)){.left = 1, .top = 1, .right = 10, .bottom = 3}, "Test", &pressed) == STATUS_CONST(SUCCESS));
  assert(UI_FUNC(End)(&ui) == STATUS_CONST(SUCCESS));

  TERMINAL_FUNC(Canvas_Free)(&canvas);
  return 0;
}
