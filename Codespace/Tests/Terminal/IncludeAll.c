#include "../../Cosmeron/Modules/Graphics/Terminal/Terminal.h"
#include "../../Cosmeron/Modules/Graphics/Canvas/Canvas.h"
#include "../../Cosmeron/Modules/Graphics/UI/UI.h"
int main(void) {
  TERMINAL_TYPE(TCanvas) canvas={0};
  if (TERMINAL_FUNC(Canvas_Create)(&canvas,
      (TDUAL_TYPE(uint16)){.col=2,.row=2})!=STATUS_CONST(SUCCESS)) return 1;
  TERMINAL_FUNC(Canvas_Free)(&canvas);
  return 0;
}
