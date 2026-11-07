#include "../../Cosmeron/Modules/Graphics/Terminal/Terminal.h"
#include "../../Cosmeron/Modules/Graphics/Canvas/Canvas.h"
#include "../../Cosmeron/Modules/Graphics/UI/UI.h"
#include "../../Cosmeron/Modules/Graphics/Font/Font.h"
#include "../../Cosmeron/Modules/Graphics/FIGlet/FIGlet.h"
#include "../../Cosmeron/Modules/Graphics/Rasterization/Rasterization.h"
#include "../../Cosmeron/Modules/Graphics/Raycasting/Render.h"
#include "../../Cosmeron/Modules/Graphics/Pseudo3DRoad/Render.h"
#include "../../Cosmeron/Modules/Graphics/Geometry/Geometry.h"
#include "../../Cosmeron/Modules/Graphics/Transform/Transform.h"
#include "../../Cosmeron/Modules/Graphics/Palette/Palette.h"
#include "../../Cosmeron/Modules/Graphics/Gradient/Gradient.h"
#include "../../Cosmeron/Modules/Graphics/Compositor/Compositor.h"
#include "../../Cosmeron/Modules/Graphics/Animation/Animation.h"
#include "../../Cosmeron/Modules/Graphics/PixelCanvas/PixelCanvas.h"
#include "../../Cosmeron/Modules/Graphics/Graphics.h"

int main(void) {
  TERMINAL_TYPE(TCanvas) canvas={0};
  if (TERMINAL_FUNC(Canvas_Create)(&canvas,
      (TDUAL_TYPE(uint16)){.col=2,.row=2})!=STATUS_CONST(SUCCESS)) return 1;
  TERMINAL_FUNC(Canvas_Free)(&canvas);
  return 0;
}
