#define COSMERON_NAMESPACE Lab
#define COSMERON_NAMESPACE_CONST LAB
#include "../../Cosmeron/Modules/Input/Keyboard/Keyboard.h"
#include "../../Cosmeron/Modules/Input/Mouse/Mouse.h"
#include "../../Cosmeron/Modules/Input/Controller/Controller.h"
#include "../../Cosmeron/Modules/Input/Gamepad/Gamepad.h"
#include "../../Cosmeron/Modules/Input/Event/Event.h"
#include <assert.h>
int main(void) {
  Lab_Input_Internal_Key(LAB_INPUT_KEY_A, true, false);
  assert(Lab_Input_Keyboard_IsDown(LAB_INPUT_KEY_A));
  assert(Lab_Input_Keyboard_IsPressed(LAB_INPUT_KEY_A));
  return 0;
}
