#define INPUT_NO_SDL_CONTROLLERS
#include "../../Cosmeron/Modules/Input/Input.h"
#include <assert.h>
int main(void) {
#if defined(__OpenBSD__) || defined(__NetBSD__) || defined(__DragonFly__)
  unsigned char report[4] = {1, 0xff, 0x7f, 1};
  Input_Internal_BSDHIDDevice *n = &Input_Internal_bsdHID.slots[0];
  Input_Internal_Device *d = Input_Internal_Connect(0, "Descriptor test");
  assert(d); d->buttonCount = d->axisCount = d->hatCount = 1;
  n->count = 3;
  n->fields[0].pos = 0; n->fields[0].report_size = 1;
  n->fields[1].pos = 8; n->fields[1].report_size = 16; n->fields[1].logical_minimum = -32768; n->fields[1].logical_maximum = 32767;
  n->fields[2].pos = 24; n->fields[2].report_size = 4; n->fields[2].logical_maximum = 7;
  n->kinds[0] = 0; n->kinds[1] = 1; n->kinds[2] = 2;
  Input_Internal_BSDHIDReport(0, report, sizeof(report));
  assert(Input_Controller_IsDown(d->id, 0));
  assert(Input_Controller_GetAxis(d->id, 0) == 1);
  assert(Input_Controller_GetHat(d->id, 0) == (INPUT_HAT_UP | INPUT_HAT_RIGHT));
  report[1] = 0; report[2] = 0x80; report[3] = 15;
  Input_Internal_BSDHIDReport(0, report, sizeof(report));
  assert(Input_Controller_GetAxis(d->id, 0) == -1);
  assert(Input_Controller_GetHat(d->id, 0) == INPUT_HAT_CENTERED);
  n->count = 1; n->kinds[0] = 1;
  n->fields[0].pos = 0; n->fields[0].report_size = 32;
  n->fields[0].logical_minimum = 0; n->fields[0].logical_maximum = -1; /* unsigned 32-bit maximum */
  memset(report, 0xff, sizeof(report));
  Input_Internal_BSDHIDReport(0, report, sizeof(report));
  assert(Input_Controller_GetAxis(d->id, 0) == 1);
#endif
  return 0;
}
