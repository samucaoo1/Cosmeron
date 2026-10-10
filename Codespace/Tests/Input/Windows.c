#include "../../Cosmeron/Modules/Input/Input.h"
#include <assert.h>
#include <stdio.h>
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
  puts("Windows: source aggregation"); fflush(stdout);
  Input_Internal_Device *d = Input_Internal_Connect(0, "Timer test");
  puts("Windows: connected"); fflush(stdout);
  LONG i;
  uint32_t a = Input_Internal_WindowsSource((HANDLE)(uintptr_t)1);
  puts("Windows: first source"); fflush(stdout);
  uint32_t b = Input_Internal_WindowsSource((HANDLE)(uintptr_t)2);
  puts("Windows: second source"); fflush(stdout);
  assert(d && a != b);
  assert(Input_Internal_WindowsKey(VK_RETURN | 0x100) == INPUT_KEY_KEYPAD_ENTER);
  assert(Input_Internal_WindowsPhysical(0x11c) == INPUT_KEY_KEYPAD_ENTER);
  Input_Internal_SourceKey(a, INPUT_KEY_A, INPUT_KEY_A, true, false);
  Input_Internal_SourceKey(b, INPUT_KEY_A, INPUT_KEY_A, true, false);
  Input_Internal_Begin();
  Input_Internal_RemoveSource(a);
  assert(Input_Keyboard_IsDown(INPUT_KEY_A) && !Input_Keyboard_IsReleased(INPUT_KEY_A));
  Input_Internal_RemoveSource(b); assert(!Input_Keyboard_IsDown(INPUT_KEY_A));
  puts("Windows: timer start"); fflush(stdout);
  d->rumble = true; Input_Internal_windowsState.set = mockRumble;
  assert(Input_Controller_Rumble(d->id, 1, 1, 10) == STATUS_SUCCESS);
  puts("Windows: waiting for timer"); fflush(stdout);
  /* No Input_Update: the OS callback itself must stop the motors. */
  for (i = 0; i < 100 && !InterlockedCompareExchange(&inputStops, 0, 0); ++i) Sleep(10);
  assert(InterlockedCompareExchange(&inputStops, 0, 0) > 0);
  puts("Windows: cancel timer"); fflush(stdout);
  Input_Internal_WindowsCancelTimer(0);
  assert(Input_Controller_Rumble(d->id, 1, 1, 10000) == STATUS_SUCCESS);
  assert(Input_Controller_StopRumble(d->id) == STATUS_SUCCESS);
  assert(Input_Internal_windowsState.timers[0] == NULL);
#endif
  return 0;
}
