#include "../../Cosmeron/Modules/Audio/Codec/WAV.h"
#include <assert.h>
#include <stdio.h>

static const unsigned char mono16[] = {
  'R','I','F','F', 40,0,0,0, 'W','A','V','E',
  'f','m','t',' ', 16,0,0,0, 1,0, 1,0,
  0x40,0x1f,0,0, 0x80,0x3e,0,0, 2,0, 16,0,
  'd','a','t','a', 4,0,0,0, 0,0x80, 0xff,0x7f
};
static const unsigned char mono8[] = {
  'R','I','F','F', 40,0,0,0, 'W','A','V','E',
  'f','m','t',' ', 16,0,0,0, 1,0, 1,0,
  0x40,0x1f,0,0, 0x40,0x1f,0,0, 1,0, 8,0,
  'd','a','t','a', 4,0,0,0, 0,128,255,128
};
int main(void) {
  AUDIO_PCM_TYPE(TBuffer) pcm = {0};
  unsigned char bytes[44];
  FILE *file;
  size_t n;
  assert(AUDIO_CODEC_FUNC(WAV_DecodeMemory)(mono16, sizeof mono16, &pcm) ==
         STATUS_CONST(SUCCESS));
  assert(pcm.sampleRate == 8000 && pcm.channels == 1 && pcm.frameCount == 2);
  assert(pcm.samples[0] == -1.f && pcm.samples[1] > 0.999f);
  assert(AUDIO_CODEC_FUNC(WAV_WriteFile)("native-roundtrip.wav", &pcm) ==
         STATUS_CONST(SUCCESS));
  file = fopen("native-roundtrip.wav", "rb");
  assert(file != NULL);
  n = fread(bytes, 1, sizeof bytes, file);
  assert(fclose(file) == 0);
  assert(n == sizeof bytes && memcmp(bytes, "RIFF", 4) == 0);
  assert(remove("native-roundtrip.wav") == 0);
  AUDIO_PCM_FUNC(Destroy)(&pcm);
  assert(AUDIO_CODEC_FUNC(WAV_DecodeMemory)(mono8, sizeof mono8, &pcm) ==
         STATUS_CONST(SUCCESS));
  assert(pcm.frameCount == 4 && pcm.samples[0] == -1.f &&
         pcm.samples[1] == 0.f && pcm.samples[2] > 0.99f);
  AUDIO_PCM_FUNC(Destroy)(&pcm);
  assert(AUDIO_CODEC_FUNC(WAV_DecodeMemory)(mono8, 20, &pcm) ==
         STATUS_CONST(INVALID_ARGUMENT));
  assert(AUDIO_CODEC_FUNC(WAV_DecodeMemory)(NULL, 0, &pcm) ==
         STATUS_CONST(INVALID_ARGUMENT));
  return 0;
}
