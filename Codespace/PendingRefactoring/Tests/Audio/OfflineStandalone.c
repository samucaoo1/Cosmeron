#include "../../Cosmeron/Modules/Audio/Offline.h"
#include <assert.h>
#ifdef cosmeron_audio_vendor_MINIAUDIO_IMPLEMENTATION
#error "Offline.h imported a desktop vendor"
#endif
int main(void) {
  AUDIO_PCM_TYPE(TBuffer) pcm = {0};
  size_t error = 0;
  assert(AUDIO_BEEP_FUNC(RenderMML)("T120 O4 C P D", 48000, &pcm,
                                   &error) == STATUS_CONST(SUCCESS));
  assert(pcm.frameCount > 0 && pcm.samples != NULL);
  AUDIO_PCM_FUNC(Destroy)(&pcm);
  return 0;
}
