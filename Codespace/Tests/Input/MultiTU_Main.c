#define INPUT_DEFINE_STORAGE
#include "../../Cosmeron/Modules/Input/Input.h"
#include <assert.h>
void inputPress(void);
bool inputRead(void);
int main(void) {
  assert(!inputRead()); inputPress(); assert(inputRead());
  assert(INPUT_KEYBOARD_FUNC(IsPressed)(LIB_PREFIX_CONST(INPUT_KEY_SPACE)));
  return 0;
}
