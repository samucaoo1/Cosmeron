#include "../../Cosmeron/Modules/Audio/CodecOnly.h"
#include <assert.h>
int main(void) {
  AUDIO_CODEC_TYPE(TDecoder) decoder = {0};
  AUDIO_PCM_TYPE(TBuffer) tone = {0};
  AUDIO_CODEC_FUNC(Close)(&decoder);
  assert(AUDIO_BEEP_FUNC(RenderTone)(440, 5, 0.2f, 48000,
                                    &tone) == STATUS_CONST(SUCCESS));
  assert(tone.frameCount > 0);
  AUDIO_PCM_FUNC(Destroy)(&tone);
  return 0;
}
