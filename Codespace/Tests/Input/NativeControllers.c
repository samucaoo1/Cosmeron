#define INPUT_NO_SDL_CONTROLLERS
#include "../../Cosmeron/Modules/Input/Input.h"
#include <assert.h>
int main(void) {
#if defined(__APPLE__) || defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__NetBSD__) || defined(__DragonFly__)
  OPSTATUS status;
  /* Native runtime smoke test: availability depends on runner permissions and
   * attached devices. Crucially this executes with the SDL path compiled out. */
  assert(Input_SetCaptureMode(INPUT_CAPTURE_APPLICATION) == STATUS_SUCCESS);
  status = Input_Update();
  assert(status == STATUS_SUCCESS);
  status = Input_Update();
  assert(status == STATUS_SUCCESS);
#if defined(__OpenBSD__) || defined(__NetBSD__) || defined(__DragonFly__)
  assert(Input_Internal_BSDHIDStart()); /* base-system library ABI */
#endif
#endif
  return 0;
}
