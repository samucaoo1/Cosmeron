#include "../../Cosmeron/Modules/Input/Input.h"
#include <assert.h>
#if defined(__APPLE__) && defined(__MACH__) && !defined(INPUT_NO_NATIVE)
static int64_t macInteger(CGEventRef e, CGEventField field) {
  (void)e;
  if (field == kCGKeyboardEventKeycode) return 17; /* physical T */
  return 0;
}
static double macNumber(CGEventRef e, CGEventField field) {
  (void)e;
  return field == kCGScrollWheelEventFixedPtDeltaAxis1 ? 0.25 : -0.5;
}
#endif
int main(void) {
#if defined(__APPLE__) && defined(__MACH__) && !defined(INPUT_NO_NATIVE)
  float x, y;
  assert(Input_Internal_MacKey(0) == INPUT_KEY_A);
  assert(Input_Internal_MacKey(17) == INPUT_KEY_T);
  assert(Input_Internal_MacKey(18) == INPUT_KEY_DIGIT_1);
  assert(Input_Internal_MacKey(76) == INPUT_KEY_KEYPAD_ENTER);
  Input_Internal_macState.integer = macInteger;
  Input_Internal_macState.number = macNumber;
  (void)Input_Internal_MacEvent(NULL, kCGEventKeyDown, NULL, NULL);
  assert(Input_Keyboard_IsPhysicalDown(INPUT_KEY_T));
  (void)Input_Internal_MacEvent(NULL, kCGEventKeyUp, NULL, NULL);
  assert(Input_Keyboard_IsPhysicalReleased(INPUT_KEY_T));
  (void)Input_Internal_MacEvent(NULL, kCGEventScrollWheel, NULL, NULL);
  Input_Mouse_GetScroll(&x, &y); assert(x == -0.5f && y == 0.25f);
#endif
  return 0;
}
