#include <assert.h>
#include <string.h>

#include "../../Cosmeron/Modules/Graphics/UI/UI.h"

int main(void) {
  const struct UI_FunctionTable *ft = UI_GetFunctionTable();
  assert(ft != NULL);
  assert(ft->Init != NULL);
  assert(ft->Begin != NULL);
  assert(ft->End != NULL);
  assert(ft->Render != NULL);
  assert(ft->Present != NULL);
  assert(ft->Dispatch != NULL);
  assert(ft->DispatchInput != NULL);
  assert(ft->Button != NULL);
  assert(ft->ButtonEx != NULL);
  assert(ft->Scrollbar != NULL);
  assert(ft->ScrollbarEx != NULL);
  assert(ft->Focus_Set != NULL);
  assert(ft->Focus_Clear != NULL);
  assert(ft->Layout != NULL);
  assert(ft->Layout_Grid != NULL);
  assert(ft->Layout_Center != NULL);

  UI_TYPE(TContext) ui = {0};
  assert(ft->Init(&ui) == STATUS_CONST(SUCCESS));
  assert(ui.api == ft);

  TERMINAL_TYPE(TCanvas) canvas = {0};
  assert(TERMINAL_FUNC(Canvas_Create)(&canvas, (TDUAL_TYPE(uint16)){.col = 20, .row = 10}) == STATUS_CONST(SUCCESS));

  assert(ui.api->Begin(&ui, &canvas) == STATUS_CONST(SUCCESS));
  bool pressed = false;
  assert(ui.api->Button(&ui, (TQUAD_TYPE(uint16)){.left = 1, .top = 1, .right = 10, .bottom = 3}, "Test", &pressed) == STATUS_CONST(SUCCESS));
  assert(ui.api->End(&ui) == STATUS_CONST(SUCCESS));

  TERMINAL_FUNC(Canvas_Free)(&canvas);
  return 0;
}
