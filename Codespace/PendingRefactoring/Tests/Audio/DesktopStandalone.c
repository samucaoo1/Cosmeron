#include "../../Cosmeron/Modules/Audio/Desktop.h"
#include <assert.h>
/* Force the real device-open backend into the executable without calling it:
 * a VM must not require real sound hardware to run this link test. */
typedef OPSTATUS (*DeviceOpenProc)(AUDIO_DEVICE_TYPE(TDevice) *,
                                   const AUDIO_DEVICE_TYPE(TConfig) *);
static DeviceOpenProc volatile deviceOpen = AUDIO_DEVICE_FUNC(Open);
int main(void) {
  AUDIO_DEVICE_TYPE(TDevice) device = {0};
  AUDIO_PLAYER_TYPE(TPlayer) player = {0};
  AUDIO_PLAYER_TYPE(TConfig) config = AUDIO_PLAYER_FUNC(Config)();
  assert(deviceOpen != NULL && config.sampleRate > 0);
  assert(!AUDIO_DEVICE_FUNC(IsStarted)(&device));
  AUDIO_DEVICE_FUNC(Close)(&device);
  AUDIO_PLAYER_FUNC(Close)(&player);
  return 0;
}
