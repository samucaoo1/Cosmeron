#define COSMERON_NAMESPACE Lab
#define COSMERON_NAMESPACE_CONST LAB
#include "Test.h"
int main(void) {
  GUI_TYPE(TContext) c = {0};
  GRAPHICS_TYPE(TSurface) s = {0};
  OK(GRAPHICS_FUNC(Surface_Create)(&s, (TDUAL_TYPE(uint32)){.x = 16, .y = 16}));
  OK(GUI_FUNC(Begin)(&c, &s));
  OK(GUI_FUNC(End)(&c));
  GRAPHICS_FUNC(Surface_Free)(&s);
  return 0;
}
