#include <assert.h>
#include <math.h>
#include "../../Cosmeron/Modules/Graphics/Compositor/Compositor.h"
#include "../../Cosmeron/Modules/Graphics/Animation/Animation.h"
#include "../../Cosmeron/Modules/Graphics/PixelCanvas/PixelCanvas.h"
#include "../../Cosmeron/Modules/Graphics/Rasterization/Rasterization.h"

#define OK STATUS_CONST(SUCCESS)

static void test_compositor(void) {
  TERMINAL_TYPE(TCanvas) c1 = {0};
  TERMINAL_TYPE(TCanvas) c2 = {0};
  TERMINAL_TYPE(TCanvas) dest = {0};

  assert(TERMINAL_FUNC(Canvas_Create)(&c1, (TDUAL_TYPE(uint16)){.col = 10, .row = 10}) == OK);
  assert(TERMINAL_FUNC(Canvas_Create)(&c2, (TDUAL_TYPE(uint16)){.col = 10, .row = 10}) == OK);
  assert(TERMINAL_FUNC(Canvas_Create)(&dest, (TDUAL_TYPE(uint16)){.col = 10, .row = 10}) == OK);

  TEXT_GRID_ATTRIBUTE_TYPE(Cell) s1 = TEXT_GRID_ATTRIBUTE_FUNC(Default)();
  s1.foreground = 0xFF0000;
  assert(TERMINAL_FUNC(Canvas_Set)(&c1, (TDUAL_TYPE(uint16)){.x = 2, .y = 2}, 'A', s1) == OK);

  TEXT_GRID_ATTRIBUTE_TYPE(Cell) s2 = TEXT_GRID_ATTRIBUTE_FUNC(Default)();
  s2.foreground = 0x00FF00;
  assert(TERMINAL_FUNC(Canvas_Set)(&c2, (TDUAL_TYPE(uint16)){.x = 2, .y = 2}, 'B', s2) == OK);

  TCompositor comp = {0};
  assert(TCompositor_Init(&comp) == OK);
  assert(comp.api->Create(&comp, 8) == OK);

  size_t idx1, idx2;
  assert(comp.api->AddLayer(&comp, &c1, 0, (TDUAL_TYPE(int32)){.x = 0, .y = 0}, 1.0f, &idx1) == OK);
  assert(comp.api->AddLayer(&comp, &c2, 1, (TDUAL_TYPE(int32)){.x = 0, .y = 0}, 1.0f, &idx2) == OK);
  assert(comp.api->LayerCount(&comp) == 2);

  /* Compose: c2 (z=1) should overwrite c1 (z=0) at (2, 2) */
  assert(comp.api->Compose(&comp, &dest) == OK);

  uint32_t glyph = 0;
  TEXT_GRID_ATTRIBUTE_TYPE(Cell) style;
  assert(TERMINAL_FUNC(Canvas_Get)(&dest, (TDUAL_TYPE(uint16)){.x = 2, .y = 2}, &glyph, &style) == OK);
  assert(glyph == 'B');

  /* Invert z-order: c1 gets z=2 */
  assert(comp.api->SetLayerZ(&comp, idx1, 2) == OK);
  assert(comp.api->Compose(&comp, &dest) == OK);
  assert(TERMINAL_FUNC(Canvas_Get)(&dest, (TDUAL_TYPE(uint16)){.x = 2, .y = 2}, &glyph, &style) == OK);
  assert(glyph == 'A');

  comp.api->Destroy(&comp);
  TERMINAL_FUNC(Canvas_Free)(&dest);
  TERMINAL_FUNC(Canvas_Free)(&c2);
  TERMINAL_FUNC(Canvas_Free)(&c1);
}

static void test_animation(void) {
  TAnimationController anim = {0};
  assert(TAnimationController_Init(&anim) == OK);
  assert(anim.api->Configure(&anim, 0.0f, 100.0f, 2.0f, ANIMATION_CONST(EASING_LINEAR)) == OK);

  assert(fabsf(anim.api->Value(&anim) - 0.0f) < 1e-4f);
  assert(fabsf(anim.api->Progress(&anim) - 0.0f) < 1e-4f);

  /* Advance 1 second (halfway) */
  assert(anim.api->Update(&anim, 1.0f) == OK);
  assert(fabsf(anim.api->Progress(&anim) - 0.5f) < 1e-4f);
  assert(fabsf(anim.api->Value(&anim) - 50.0f) < 1e-4f);

  /* Finish */
  assert(anim.api->Update(&anim, 1.0f) == OK);
  assert(anim.api->IsFinished(&anim));
  assert(fabsf(anim.api->Value(&anim) - 100.0f) < 1e-4f);

  /* Test Easing Functions */
  assert(fabsf(ANIMATION_FUNC(Ease)(0.5f, ANIMATION_CONST(EASING_IN_QUAD)) - 0.25f) < 1e-4f);
  assert(fabsf(ANIMATION_FUNC(Ease)(0.5f, ANIMATION_CONST(EASING_OUT_QUAD)) - 0.75f) < 1e-4f);
}

static void test_pixel_canvas(void) {
  TPixelCanvas pc = {0};
  assert(TPixelCanvas_Init(&pc) == OK);
  /* 20 pixels wide, 20 pixels high -> fits in 20 col x 10 row terminal canvas */
  assert(pc.api->Create(&pc, 20, 20) == OK);
  assert(pc.api->Width(&pc) == 20);
  assert(pc.api->Height(&pc) == 20);

  /* Clear with blue */
  assert(pc.api->Clear(&pc, 0xFF0000FF) == OK);

  /* Draw red line */
  assert(pc.api->DrawLine(&pc, 0, 0, 19, 0, 0xFFFF0000) == OK);

  uint32_t c = 0;
  assert(pc.api->GetPixel(&pc, 5, 0, &c) == OK);
  assert(c == 0xFFFF0000);

  /* Render to Terminal Canvas */
  TERMINAL_TYPE(TCanvas) termCanvas = {0};
  assert(TERMINAL_FUNC(Canvas_Create)(&termCanvas, (TDUAL_TYPE(uint16)){.col = 20, .row = 10}) == OK);
  assert(pc.api->RenderToCanvas(&pc, &termCanvas, (TDUAL_TYPE(uint16)){.col = 0, .row = 0}) == OK);

  /* Top row of termCanvas (row 0) should have:
   * top pixel (y=0) = Red (0xFF0000) -> foreground
   * bot pixel (y=1) = Blue (0x0000FF) -> background
   * glyph = 0x2580 (upper half block) */
  uint32_t glyph = 0;
  TEXT_GRID_ATTRIBUTE_TYPE(Cell) cell;
  assert(TERMINAL_FUNC(Canvas_Get)(&termCanvas, (TDUAL_TYPE(uint16)){.col = 5, .row = 0}, &glyph, &cell) == OK);
  assert(glyph == 0x2580);
  assert((cell.foreground & 0xFFFFFF) == 0xFF0000);
  assert((cell.background & 0xFFFFFF) == 0x0000FF);

  TERMINAL_FUNC(Canvas_Free)(&termCanvas);
  pc.api->Destroy(&pc);
}

static void test_raster_extensions(void) {
  TERMINAL_TYPE(TCanvas) canvas = {0};
  assert(TERMINAL_FUNC(Canvas_Create)(&canvas, (TDUAL_TYPE(uint16)){.col = 20, .row = 10}) == OK);
  TEXT_GRID_ATTRIBUTE_TYPE(Cell) style = TEXT_GRID_ATTRIBUTE_FUNC(Default)();

  /* LineStyled (Dashed) */
  assert(RASTER_FUNC(LineStyled)(&canvas,
                                 (TDUAL_TYPE(int32)){.x = 0, .y = 2},
                                 (TDUAL_TYPE(int32)){.x = 10, .y = 2},
                                 RASTER_CONST(LINE_DASHED), '-', style) == OK);

  /* FloodFill an enclosed 5x5 box */
  assert(RASTER_FUNC(Rectangle)(&canvas,
                                (TQUAD_TYPE(uint16)){.left = 4, .top = 3, .right = 8, .bottom = 7},
                                '#', style) == OK);
  assert(RASTER_FUNC(FloodFill)(&canvas, (TDUAL_TYPE(int32)){.x = 6, .y = 5}, '.', style) == OK);

  /* Center should be '.' */
  uint32_t cp = 0;
  TEXT_GRID_ATTRIBUTE_TYPE(Cell) outStyle;
  assert(TERMINAL_FUNC(Canvas_Get)(&canvas, (TDUAL_TYPE(uint16)){.col = 6, .row = 5}, &cp, &outStyle) == OK);
  assert(cp == '.');

  /* Border should still be '#' */
  assert(TERMINAL_FUNC(Canvas_Get)(&canvas, (TDUAL_TYPE(uint16)){.col = 4, .row = 3}, &cp, &outStyle) == OK);
  assert(cp == '#');

  TERMINAL_FUNC(Canvas_Free)(&canvas);
}

int main(void) {
  test_compositor();
  test_animation();
  test_pixel_canvas();
  test_raster_extensions();
  return 0;
}
