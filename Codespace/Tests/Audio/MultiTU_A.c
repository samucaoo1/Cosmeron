#include "../../Cosmeron/Modules/Audio/Audio.h"
OPSTATUS audioOpen(AUDIO_CODEC_TYPE(TDecoder) * decoder) {
  return AUDIO_CODEC_FUNC(OpenFile)(decoder, "Fixtures/Tone.flac");
}
