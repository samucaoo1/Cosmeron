#include "../../Cosmeron/Modules/Audio/Codec/WAV.h"
#include <stdio.h>
#include <string.h>

/* Deterministic malformed-input, reordered chunk and independent roundtrip
 * coverage for the pure-C11 RIFF decoder. No vendor, libm or audio hardware. */
static const unsigned char good[] = {
  'R','I','F','F', 40,0,0,0, 'W','A','V','E',
  'f','m','t',' ', 16,0,0,0, 1,0, 1,0,
  0x40,0x1f,0,0, 0x80,0x3e,0,0, 2,0, 16,0,
  'd','a','t','a', 4,0,0,0, 0,0x80, 0xff,0x7f
};
static const unsigned char reordered[] = {
  'R','I','F','F', 50,0,0,0, 'W','A','V','E',
  'J','U','N','K', 3,0,0,0, 0x42,0x43,0x44,0,
  'd','a','t','a', 2,0,0,0, 0,0,
  'f','m','t',' ', 16,0,0,0, 1,0, 1,0,
  0x40,0x1f,0,0, 0x80,0x3e,0,0, 2,0, 16,0
};
static int ExpectStatus(const unsigned char *data, size_t count,
                        OPSTATUS wanted) {
  AUDIO_PCM_TYPE(TBuffer) pcm = {0};
  OPSTATUS actual = AUDIO_CODEC_FUNC(WAV_DecodeMemory)(data,count,&pcm);
  if (actual == STATUS_CONST(SUCCESS))
    AUDIO_PCM_FUNC(Destroy)(&pcm);
  else if (pcm.samples != NULL)
    return 1;
  return actual == wanted ? 0 : 1;
}
int main(void) {
  unsigned char broken[sizeof good];
  AUDIO_PCM_TYPE(TBuffer) pcm = {0};
  size_t len;
  FILE *file;
  unsigned char encoded[128];

  /* Truncated RIFF length: never trust the declared data length. */
  for (len = 0; len < sizeof good; ++len)
    if (!ExpectStatus(good,len,STATUS_CONST(INVALID_ARGUMENT))) return 1;
  memcpy(broken,good,sizeof good);
  broken[4] = 0xff;
  if (ExpectStatus(broken,sizeof broken,
                   STATUS_CONST(INVALID_ARGUMENT))) return 2;
  memcpy(broken,good,sizeof good);
  broken[16] = 0xff; /* fmt chunk claims too many bytes */
  if (ExpectStatus(broken,sizeof broken,
                   STATUS_CONST(INVALID_ARGUMENT))) return 3;
  memcpy(broken,good,sizeof good);
  broken[20] = 3; /* IEEE float format not supported by pure-PCM codec */
  if (ExpectStatus(broken,sizeof broken,
                   STATUS_CONST(NOT_SUPPORTED))) return 4;
  memcpy(broken,good,sizeof good);
  broken[22] = 0; /* channels zero */
  if (ExpectStatus(broken,sizeof broken,
                   STATUS_CONST(INVALID_ARGUMENT))) return 5;
  memcpy(broken,good,sizeof good);
  broken[32] = 1; /* incorrect PCM16 block alignment */
  if (ExpectStatus(broken,sizeof broken,
                   STATUS_CONST(INVALID_ARGUMENT))) return 6;
  memcpy(broken,good,sizeof good);
  broken[40] = 3; /* data size not divisible by 16-bit block alignment */
  if (ExpectStatus(broken,sizeof broken,
                   STATUS_CONST(INVALID_ARGUMENT))) return 7;
  memcpy(broken,good,sizeof good);
  broken[12] = 'X'; /* required fmt chunk missing */
  if (ExpectStatus(broken,sizeof broken,
                   STATUS_CONST(INVALID_ARGUMENT))) return 8;

  /* Odd-sized unknown chunk, followed by data and then the fmt chunk. */
  if (AUDIO_CODEC_FUNC(WAV_DecodeMemory)(reordered,sizeof reordered,&pcm) !=
      STATUS_CONST(SUCCESS))
    return 9;
  if (pcm.channels != 1 || pcm.sampleRate != 8000 ||
      pcm.frameCount != 1 || pcm.samples[0] != 0.0f)
    return 10;
  AUDIO_PCM_FUNC(Destroy)(&pcm);

  /* Write PCM16 and independently parse its bytes back through the codec. */
  if (AUDIO_PCM_FUNC(Init)(&pcm,2,2,8000) != STATUS_CONST(SUCCESS))
    return 11;
  pcm.samples[0] = -1.f; pcm.samples[1] = 0.f;
  pcm.samples[2] = 1.f; pcm.samples[3] = 0.5f;
  if (AUDIO_CODEC_FUNC(WAV_WriteFile)("wav-robust-roundtrip.wav",&pcm) !=
      STATUS_CONST(SUCCESS)) return 12;
  AUDIO_PCM_FUNC(Destroy)(&pcm);
  file = fopen("wav-robust-roundtrip.wav","rb");
  if (file == NULL) return 13;
  len = fread(encoded,1,sizeof encoded,file);
  if (fclose(file) != 0 || remove("wav-robust-roundtrip.wav") != 0)
    return 14;
  if (len != 52u) return 15;
  if (AUDIO_CODEC_FUNC(WAV_DecodeMemory)(encoded,len,&pcm) !=
      STATUS_CONST(SUCCESS)) return 16;
  if (pcm.channels != 2 || pcm.sampleRate != 8000 || pcm.frameCount != 2 ||
      pcm.samples[0] != -1.f || pcm.samples[1] != 0.f ||
      pcm.samples[2] < 0.999f ||
      pcm.samples[3] < 0.49f || pcm.samples[3] > 0.51f)
    return 17;
  AUDIO_PCM_FUNC(Destroy)(&pcm);
  return 0;
}
