#include "Test.h"
int main(void) {
  uint32_t pixels[32];
  GRAPHICS_TYPE(TSurface) s = {0}, view = {0}, owner = {0};
  size_t i;
  for (i = 0; i < 32; ++i)
    pixels[i] = UINT32_C(0xDEADBEEF);
  OK(GRAPHICS_FUNC(Surface_Borrow)(&s, pixels, (TDUAL_TYPE(uint32)){.x = 4, .y = 3}, 8, 32));
  OK(GRAPHICS_FUNC(Surface_Clear)(&s, UINT32_C(0xFF123456)));
  assert(pixels[4] == UINT32_C(0xDEADBEEF) && pixels[12] == UINT32_C(0xDEADBEEF));
  OK(GRAPHICS_FUNC(Surface_View)(
      &s, (TQUAD_TYPE(uint32)){.value1 = 1, .value2 = 1, .value3 = 2, .value4 = 2}, &view));
  OK(GRAPHICS_FUNC(Surface_Clear)(&view, UINT32_C(0xFFABCDEF)));
  assert(pixels[9] == UINT32_C(0xFFABCDEF));
  GRAPHICS_FUNC(Surface_Free)(&view);
  assert(pixels[9] == UINT32_C(0xFFABCDEF));
  assert(GRAPHICS_FUNC(Surface_Resize)(&s, (TDUAL_TYPE(uint32)){.x = 2, .y = 2}) ==
         STATUS_CONST(NOT_SUPPORTED));
  OK(GRAPHICS_FUNC(Surface_Create)(&owner, (TDUAL_TYPE(uint32)){.x = 2, .y = 2}));
  owner.pixels[3] = UINT32_C(0xFF778899);
  OK(GRAPHICS_FUNC(Surface_Resize)(&owner, (TDUAL_TYPE(uint32)){.x = 3, .y = 3}));
  assert(owner.pixels[4] == UINT32_C(0xFF778899) && owner.pixels[8] == 0);
  assert(GRAPHICS_FUNC(Surface_Blend)(UINT32_C(0xFF000000), UINT32_C(0x80FFFFFF)) ==
         UINT32_C(0xFF808080));
  assert(GRAPHICS_FUNC(Surface_Blend)(0, UINT32_C(0x80FF0000)) == UINT32_C(0x80FF0000));
  OK(GRAPHICS_FUNC(Surface_Line)(&owner, (TDUAL_TYPE(int32)){.x = INT32_MIN, .y = INT32_MIN},
                                 (TDUAL_TYPE(int32)){.x = INT32_MAX, .y = INT32_MAX},
                                 UINT32_C(0xFFFFFFFF)));
  assert(owner.pixels[0] == UINT32_C(0xFFFFFFFF) && owner.pixels[8] == UINT32_C(0xFFFFFFFF));
  GRAPHICS_FUNC(Surface_Free)(&owner);
  assert(!owner.pixels && !owner.owns);
  s.stride = SIZE_MAX;
  s.size.y = 3;
  assert(!GRAPHICS_FUNC(Surface_IsValid)(&s));
  puts("Surface passed");
  return 0;
}
