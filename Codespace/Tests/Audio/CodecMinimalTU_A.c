#include "../../Cosmeron/Modules/Audio/CodecMinimal.h"
/* Minimal Miniaudio decoder in the first translation unit. */
int CodecMinimalTU_Decode(void) {
  static const unsigned char pcm_wav[] = {
    'R','I','F','F',38,0,0,0, 'W','A','V','E',
    'f','m','t',' ',16,0,0,0, 1,0,1,0,
    0x40,0x1f,0,0,0x80,0x3e,0,0,2,0,16,0,
    'd','a','t','a',2,0,0,0, 0,0
  };
  AUDIO_CODEC_TYPE(TDecoder) decoder = {0};
  float pcm = -1.0f;
  size_t frames = 0;
  OPSTATUS status = AUDIO_CODEC_FUNC(OpenMemory)(
    &decoder, pcm_wav, sizeof pcm_wav);
  if (status != STATUS_CONST(SUCCESS)) return 1;
  status = AUDIO_CODEC_FUNC(ReadFrames)(&decoder, &pcm, 1, &frames);
  AUDIO_CODEC_FUNC(Close)(&decoder);
  if (status != STATUS_CONST(SUCCESS) || frames != 1 || pcm != 0.f)
    return 2;
  return 0;
}
