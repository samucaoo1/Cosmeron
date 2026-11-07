#include "../../Cosmeron/Modules/Audio/Audio.h"
OPSTATUS audioRead(AUDIO_CODEC_TYPE(TDecoder) * decoder) {
  float samples[128];
  size_t count;
  OPSTATUS result = AUDIO_CODEC_FUNC(ReadFrames)(decoder, samples, 64, &count);
  AUDIO_CODEC_FUNC(Close)(decoder);
  return result == STATUS_CONST(SUCCESS) && count == 64
             ? STATUS_CONST(SUCCESS)
             : STATUS_CONST(GENERIC_ERROR);
}
