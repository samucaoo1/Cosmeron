#include "../../Cosmeron/Modules/Input/Keyboard/Keyboard.h"
bool inputRead(void) { return INPUT_KEYBOARD_FUNC(IsDown)(LIB_PREFIX_CONST(INPUT_KEY_SPACE)); }
