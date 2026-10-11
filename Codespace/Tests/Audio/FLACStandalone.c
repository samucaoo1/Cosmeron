#include "../../Cosmeron/Modules/Audio/Codec/FLAC.h"
#include <assert.h>
#include <stdio.h>

int main(void) {
  FILE *file = fopen("Fixtures/Tone.flac", "rb");
  AUDIO_PCM_TYPE(TBuffer) result = {0};
  unsigned char *input;
  long bytes;
  size_t i;
  double energy = 0;
  assert(file && !fseek(file, 0, SEEK_END));
  bytes = ftell(file);
  assert(bytes > 42 && !fseek(file, 0, SEEK_SET));
  input = (unsigned char *)malloc((size_t)bytes);
  assert(input && fread(input, 1, (size_t)bytes, file) == (size_t)bytes);
  assert(fclose(file) == 0);
  assert(AUDIO_CODEC_FUNC(FLAC_DecodeMemory)(input, (size_t)bytes, &result) ==
      STATUS_CONST(SUCCESS));
  assert(result.sampleRate == 44100 && result.channels == 2 &&
      result.frameCount > 1000);
  for (i = 0; i < result.frameCount * result.channels; ++i) {
    float value = result.samples[i];
    assert(value >= -1.0f && value < 1.0f);
    energy += value < 0 ? -value : value;
  }
  assert(energy > 10);
  assert(AUDIO_CODEC_FUNC(FLAC_DecodeMemory)(input, (size_t)bytes, &result) ==
      STATUS_CONST(BUSY));
  AUDIO_PCM_FUNC(Destroy)(&result);
  input[0] = 0;
  assert(AUDIO_CODEC_FUNC(FLAC_DecodeMemory)(input, (size_t)bytes, &result) ==
      STATUS_CONST(NOT_SUPPORTED));
  assert(result.samples == NULL);
  free(input);
  return 0;
}
