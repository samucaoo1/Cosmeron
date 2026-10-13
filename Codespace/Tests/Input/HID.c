#define INPUT_NO_NATIVE
#include "../../Cosmeron/Modules/Input/Input.h"
#include "../../Cosmeron/Modules/Input/Internal/HID.inc"
#include <assert.h>
int main(void) {
  const unsigned char report[] = {0x80, 0xff, 0x0f, 0x80};
  int64_t value = 123;
  assert(Input_Internal_HIDController(1, 5));
  assert(!Input_Internal_HIDController(1, 6));
  assert(Input_Internal_HIDNormalize(-32768, -32768, 32767) == -1);
  assert(Input_Internal_HIDNormalize(65535, 0, 65535) == 1);
  assert(Input_Internal_HIDNormalize(0, 0, 0) == 0);
  assert(Input_Internal_HIDHat(0, 0, 7) == INPUT_HAT_UP);
  assert(Input_Internal_HIDHat(1, 0, 7) == (INPUT_HAT_UP | INPUT_HAT_RIGHT));
  assert(Input_Internal_HIDHat(2, 0, 3) == INPUT_HAT_DOWN);
  assert(Input_Internal_HIDHat(8, 0, 7) == INPUT_HAT_CENTERED);
  assert(Input_Internal_HIDBits(report, sizeof(report), 8, 12, true, &value) && value == -1);
  assert(Input_Internal_HIDBits(report, sizeof(report), 7, 9, false, &value) && value == 511);
  assert(Input_Internal_HIDBits(report, sizeof(report), 0, 32, true, &value) && value == INT64_C(-2146435200));
  value = 123;
  assert(!Input_Internal_HIDBits(report, sizeof(report), 31, 2, false, &value) && value == 123);
  assert(!Input_Internal_HIDBits(report, sizeof(report), UINT32_MAX, 32, false, &value));
  return 0;
}
