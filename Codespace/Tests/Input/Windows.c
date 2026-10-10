#include "../../Cosmeron/Modules/Input/Input.h"
#include <assert.h>
#if defined(_WIN32) && !defined(INPUT_NO_NATIVE)
static volatile LONG inputStops;
static DWORD WINAPI mockRumble(DWORD slot, XINPUT_VIBRATION *v) {
  assert(slot == 0);
  if (!v->wLeftMotorSpeed && !v->wRightMotorSpeed) InterlockedIncrement(&inputStops);
  return ERROR_SUCCESS;
}
#endif
int main(void) {
#if defined(_WIN32) && !defined(INPUT_NO_NATIVE)
  Input_Internal_Device *d = Input_Internal_Connect(0, "Timer test");
  LONG i;
  uint32_t a = Input_Internal_WindowsSource((HANDLE)(uintptr_t)1);
  uint32_t b = Input_Internal_WindowsSource((HANDLE)(uintptr_t)2);
  assert(d && a != b);
  assert(Input_Internal_WindowsKey(VK_RETURN | 0x100) == INPUT_KEY_KEYPAD_ENTER);
  assert(Input_Internal_WindowsPhysical(0x11c) == INPUT_KEY_KEYPAD_ENTER);
  Input_Internal_SourceKey(a, INPUT_KEY_A, INPUT_KEY_A, true, false);
  Input_Internal_SourceKey(b, INPUT_KEY_A, INPUT_KEY_A, true, false);
  Input_Internal_Begin();
  Input_Internal_RemoveSource(a);
  assert(Input_Keyboard_IsDown(INPUT_KEY_A) && !Input_Keyboard_IsReleased(INPUT_KEY_A));
  Input_Internal_RemoveSource(b); assert(!Input_Keyboard_IsDown(INPUT_KEY_A));
  d->rumble = true; Input_Internal_windowsState.set = mockRumble;
  assert(Input_Controller_Rumble(d->id, 1, 1, 10) == STATUS_SUCCESS);
  /* No Input_Update: the OS callback itself must stop the motors. */
  for (i = 0; i < 100 && !InterlockedCompareExchange(&inputStops, 0, 0); ++i) Sleep(10);
  assert(InterlockedCompareExchange(&inputStops, 0, 0) > 0);
  Input_Internal_WindowsCancelTimer(0);
  assert(Input_Controller_Rumble(d->id, 1, 1, 10000) == STATUS_SUCCESS);
  assert(Input_Controller_StopRumble(d->id) == STATUS_SUCCESS);
  assert(Input_Internal_windowsState.timers[0] == NULL);
#endif
  return 0;
}
