#include "../../Cosmeron/Modules/Audio/Offline.h"
OPSTATUS OfflineTU_Render(float *, size_t);
OPSTATUS OfflineTU_Beep(AUDIO_PCM_TYPE(TBuffer) *);
int main(void) {
  float samples[64] = {0};
  AUDIO_PCM_TYPE(TBuffer) pcm = {0};
  if (OfflineTU_Render(samples, 64) != STATUS_CONST(SUCCESS)) return 1;
  if (OfflineTU_Beep(&pcm) != STATUS_CONST(SUCCESS)) return 2;
  if (pcm.frameCount != 480 || pcm.samples == NULL) return 3;
  AUDIO_PCM_FUNC(Destroy)(&pcm);
  return 0;
}
