#define INPUT_DISABLE_FUNCTION_TABLE
#include "../../Cosmeron/Modules/Input/Input.h"
#include <assert.h>

int main(void) {
  assert(!Input_Keyboard_IsDown(INPUT_KEY_A));
  assert(!Input_Mouse_IsDown(INPUT_MOUSE_BUTTON_LEFT));
  assert(Input_Event_GetDroppedCount() == 0);
  assert(!Input_Gamepad_IsMapped(0));
  assert(!Input_Controller_IsConnected(0));
  return 0;
}
