#include "../../Cosmeron/Modules/Audio/Desktop.h"
#include <assert.h>
int main(void) {
  AUDIO_DEVICE_TYPE(TDevice) device = {0};
  AUDIO_PLAYER_TYPE(TPlayer) player = {0};
  AUDIO_PLAYER_TYPE(TConfig) config = AUDIO_PLAYER_FUNC(Config)();
  assert(config.sampleRate > 0);
  assert(!AUDIO_DEVICE_FUNC(IsStarted)(&device));
  AUDIO_DEVICE_FUNC(Close)(&device);
  AUDIO_PLAYER_FUNC(Close)(&player);
  return 0;
}
