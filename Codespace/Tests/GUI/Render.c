#include "Test.h"
int main(void) {
  uint32_t memory[16 * 12];
  GRAPHICS_TYPE(TSurface) s = {0};
  GUI_TYPE(TContext) c = {0};
  size_t i;
  for (i = 0; i < 16 * 12; ++i)
    memory[i] = UINT32_C(0xFF000000);
  OK(GRAPHICS_FUNC(Surface_Borrow)(&s, memory, (TDUAL_TYPE(uint32)){.x = 12, .y = 12}, 16,
                                   16 * 12));
  OK(GUI_FUNC(Begin)(&c, &s));
  OK(GUI_FUNC(Clip_Push)(&c, rect(2, 2, 5, 5)));
  OK(GUI_FUNC(DrawRectangle)(&c, rect(-10, -10, 30, 30), UINT32_C(0xFFFF0000), 0));
  OK(GUI_FUNC(Clip_Pop)(&c));
  assert(memory[2 * 16 + 2] == UINT32_C(0xFF000000));
  OK(GUI_FUNC(End)(&c));
  assert(memory[2 * 16 + 2] == UINT32_C(0xFFFF0000));
  assert(memory[2 * 16 + 7] == UINT32_C(0xFF000000));
  assert(memory[12] == UINT32_C(0xFF000000));
  OK(GUI_FUNC(Scale_Set)(&c, 2));
  OK(GUI_FUNC(Begin)(&c, &s));
  OK(GUI_FUNC(DrawRectangle)(&c, rect(0, 0, 1, 1), UINT32_C(0xFFFFFFFF), 0));
  OK(GUI_FUNC(End)(&c));
  assert(memory[0] == UINT32_C(0xFFFFFFFF) && memory[17] == UINT32_C(0xFFFFFFFF) &&
         memory[2] == UINT32_C(0xFF000000));
  OK(GUI_FUNC(Begin)(&c, &s));
  OK(GUI_FUNC(DrawRectangle)(&c, rect(0, 0, 5, 5), UINT32_C(0xFF00FF00), 2));
  OK(GUI_FUNC(End)(&c));
  assert(memory[0] == UINT32_C(0xFFFFFFFF) && memory[4 * 16 + 4] == UINT32_C(0xFF00FF00));
  {
    char label[] = "A";
    OK(GUI_FUNC(Begin)(&c, &s));
    OK(GUI_FUNC(DrawText)(&c, rect(0, 0, 10, 10), label, UINT32_C(0xFFFFFFFF), NULL, 1));
    label[0] = 'B';
    assert(c.commands[0].text[0] == 'A');
    OK(GUI_FUNC(End)(&c));
  }
  puts("Render passed");
  return 0;
}
