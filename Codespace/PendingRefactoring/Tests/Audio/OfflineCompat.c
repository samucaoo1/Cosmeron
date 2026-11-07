#define AUDIO_NO_DESKTOP
#include "../../Cosmeron/Modules/Audio/Audio.h"
#include "../../Cosmeron/Modules/Audio/Beep/Beep.h"
#include <assert.h>
#ifdef cosmeron_audio_vendor_MINIAUDIO_IMPLEMENTATION
#error "AUDIO_NO_DESKTOP must suppress Miniaudio"
#endif
int main(void) {
  AUDIO_PCM_TYPE(TBuffer) pcm = {0};
  assert(AUDIO_BEEP_FUNC(RenderTone)(440, 10, 0.25f, 48000,
                                   &pcm) == STATUS_CONST(SUCCESS));
  AUDIO_PCM_FUNC(Destroy)(&pcm);
  return 0;
}
