#include "../../Cosmeron/Modules/Input/Input.h"
#include <assert.h>
#if defined(__linux__) && !defined(INPUT_NO_NATIVE)
static void sendEvent(int fd, unsigned short type, unsigned short code, int value) {
  struct input_event e = {0}; e.type = type; e.code = code; e.value = value;
  assert(write(fd, &e, sizeof(e)) == sizeof(e));
}
#endif
int main(void) {
#if defined(__linux__) && !defined(INPUT_NO_NATIVE)
  int fds[2];
  float x, y;
  Input_Internal_LinuxDevice *n = &Input_Internal_linuxState.devices[0];
  struct input_absinfo range = {0};
  Input_TEvent e;
  bool reset = false;
  assert(pipe(fds) == 0);
  assert(fcntl(fds[0], F_SETFL, O_NONBLOCK) == 0);
  n->used = true; n->fd = fds[0]; n->effect = -1;
  /* Feed the real evdev decoder from a pipe; no device access or permissions. */
  Input_Internal_linuxState.started = true;
  Input_Internal_linuxState.scanTime = time(NULL);
  sendEvent(fds[1], EV_KEY, KEY_W, 1);
  sendEvent(fds[1], EV_KEY, KEY_W, 2);
  sendEvent(fds[1], EV_KEY, KEY_W, 0);
  sendEvent(fds[1], EV_REL, REL_X, 3);
  sendEvent(fds[1], EV_REL, REL_Y, -2);
  sendEvent(fds[1], EV_REL, REL_WHEEL, 1);
  sendEvent(fds[1], EV_KEY, BTN_LEFT, 1);
  assert(Input_Update() == STATUS_SUCCESS);
  assert(!Input_Keyboard_IsDown(INPUT_KEY_W));
  assert(Input_Keyboard_IsPressed(INPUT_KEY_W));
  assert(Input_Keyboard_IsReleased(INPUT_KEY_W));
  assert(Input_Keyboard_IsRepeated(INPUT_KEY_W));
  assert(Input_Mouse_IsDown(INPUT_MOUSE_BUTTON_LEFT));
  assert(!Input_Keyboard_IsPhysicalDown(INPUT_KEY_W));
  assert(Input_Keyboard_IsPhysicalPressed(INPUT_KEY_W));
  assert(Input_Keyboard_IsPhysicalReleased(INPUT_KEY_W));
  Input_Mouse_GetDelta(&x, &y); assert(x == 3 && y == -2);
  Input_Mouse_GetPosition(&x, &y); assert(x == 3 && y == -2);
  Input_Mouse_GetScroll(&x, &y); assert(x == 0 && y == 1);
#ifdef REL_WHEEL_HI_RES
  n->preciseY = true; n->preciseX = true;
  sendEvent(fds[1], EV_REL, REL_WHEEL, 1); /* coarse duplicate must be ignored */
  sendEvent(fds[1], EV_REL, REL_WHEEL_HI_RES, 30);
  sendEvent(fds[1], EV_REL, REL_HWHEEL_HI_RES, -60);
  assert(Input_Update() == STATUS_SUCCESS);
  Input_Mouse_GetScroll(&x, &y); assert(x == -0.5f && y == 0.25f);
#endif
  /* A second keyboard keeps the aggregate held when the first releases. */
  Input_Internal_linuxState.devices[1].used = true;
  Input_Internal_SetBit(Input_Internal_linuxState.devices[1].keys, KEY_A, true);
  Input_Internal_SetBit(n->keys, KEY_A, true);
  Input_Internal_Aggregate(KEY_A, false);
  Input_Internal_SetBit(n->keys, KEY_A, false);
  Input_Internal_Aggregate(KEY_A, false);
  assert(Input_Keyboard_IsDown(INPUT_KEY_A));
  Input_Internal_linuxState.devices[1].used = false;
  Input_Internal_Aggregate(KEY_A, false);
  assert(!Input_Keyboard_IsDown(INPUT_KEY_A));
  range.minimum = -32768; range.maximum = 32767;
  assert(Input_Internal_Normalize(-32768, &range) == -1);
  assert(Input_Internal_Normalize(32767, &range) == 1);
  range.minimum = INT_MIN; range.maximum = INT_MAX;
  assert(Input_Internal_Normalize(INT_MIN, &range) == -1);
  assert(Input_Internal_Normalize(INT_MAX, &range) == 1);
  range.minimum = range.maximum = 0; assert(Input_Internal_Normalize(0, &range) == 0);
  /* Drop recovery must ignore intervening key packets. A pipe cannot ioctl;
   * recovery failure closes the source and releases held mouse buttons. */
  sendEvent(fds[1], EV_SYN, SYN_DROPPED, 0);
  sendEvent(fds[1], EV_KEY, KEY_B, 1);
  sendEvent(fds[1], EV_SYN, SYN_REPORT, 0);
  assert(Input_Update() == STATUS_NOT_AVAILABLE);
  assert(!Input_Keyboard_IsDown(INPUT_KEY_B));
  assert(!Input_Mouse_IsDown(INPUT_MOUSE_BUTTON_LEFT));
  assert(Input_Mouse_IsReleased(INPUT_MOUSE_BUTTON_LEFT));
  assert(Input_Event_GetDroppedCount() > 0);
  while (Input_Event_Poll(&e)) if (e.type == INPUT_EVENT_KEY_PRESS && e.data.key == INPUT_KEY_B) reset = true;
  assert(!reset);
  close(fds[1]);
#endif
  return 0;
}
