#include "../../Cosmeron/Modules/Audio/Audio.h"
#include <assert.h>
OPSTATUS audioOpen(AUDIO_CODEC_TYPE(TDecoder) * decoder);
OPSTATUS audioRead(AUDIO_CODEC_TYPE(TDecoder) * decoder);
int main(void) {
  AUDIO_CODEC_TYPE(TDecoder) decoder = {0};
  assert(audioOpen(&decoder) == STATUS_CONST(SUCCESS));
  assert(audioRead(&decoder) == STATUS_CONST(SUCCESS));
  return 0;
}
