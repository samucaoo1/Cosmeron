#include "../../Cosmeron/Modules/Audio/Codec/WAV.h"
#include <string.h>
#include <float.h>

/* Separate handcrafted RIFF samples for PCM24, PCM32 and IEEE float32.
 * No codec vendor, disk I/O, Python, libm or threading is needed. */
static void u16(unsigned char *p, unsigned value) {
  p[0] = (unsigned char)value;
  p[1] = (unsigned char)(value >> 8);
}
static void u32(unsigned char *p, uint32_t value) {
  p[0] = (unsigned char)value;
  p[1] = (unsigned char)(value >> 8);
  p[2] = (unsigned char)(value >> 16);
  p[3] = (unsigned char)(value >> 24);
}
static void makeWave(unsigned char *w, uint16_t format, unsigned bits,
                     const unsigned char *samples, size_t bytes) {
  memset(w, 0, 64);
  memcpy(w, "RIFF", 4);
  u32(w+4, (uint32_t)(36u+bytes));
  memcpy(w+8, "WAVEfmt ", 8);
  u32(w+16, 16);
  u16(w+20, format);
  u16(w+22, 1);
  u32(w+24, 8000);
  u32(w+28, 8000u*(bits/8u));
  u16(w+32, bits/8u);
  u16(w+34, bits);
  memcpy(w+36, "data", 4);
  u32(w+40, (uint32_t)bytes);
  memcpy(w+44, samples, bytes);
}
static int decodeAndCheck(const unsigned char *w, size_t n, float middle) {
  AUDIO_PCM_TYPE(TBuffer) pcm = {0};
  OPSTATUS status = AUDIO_CODEC_FUNC(WAV_DecodeMemory)(w,n,&pcm);
  if (status != STATUS_CONST(SUCCESS)) return 1;
  if (pcm.sampleRate != 8000 || pcm.channels != 1 || pcm.frameCount != 3 ||
      pcm.samples[0] != -1.0f || pcm.samples[1] != middle ||
      pcm.samples[2] < 0.999f) {
    AUDIO_PCM_FUNC(Destroy)(&pcm);
    return 2;
  }
  AUDIO_PCM_FUNC(Destroy)(&pcm);
  return 0;
}
int main(void) {
  unsigned char wav[64], bad[64];
  const unsigned char pcm24[] = {0,0,128, 0,0,0, 255,255,127};
  const unsigned char pcm32[] = {0,0,0,128, 0,0,0,0, 255,255,255,127};
  const unsigned char f32[] = {0,0,128,191, 0,0,128,62, 0,0,128,63};
  const unsigned char nan32[] = {0,0,192,127};
  AUDIO_PCM_TYPE(TBuffer) buffer = {0};
  if (FLT_RADIX != 2 || FLT_MANT_DIG != 24 || FLT_MAX_EXP != 128 ||
      sizeof(float) != 4) return 0; /* Unsupported float target. */

  makeWave(wav,1,24,pcm24,sizeof pcm24);
  if (decodeAndCheck(wav,44u+sizeof pcm24,0.0f)) return 1;
  makeWave(wav,1,32,pcm32,sizeof pcm32);
  if (decodeAndCheck(wav,44u+sizeof pcm32,0.0f)) return 2;
  makeWave(wav,3,32,f32,sizeof f32);
  if (decodeAndCheck(wav,44u+sizeof f32,0.25f)) return 3;

  memcpy(bad,wav,sizeof wav);
  u16(bad+34,24); /* Float WAV needs exactly 32 bits. */
  if (AUDIO_CODEC_FUNC(WAV_DecodeMemory)(bad,44u+sizeof f32,&buffer) !=
      STATUS_CONST(NOT_SUPPORTED) || buffer.samples != NULL) return 4;

  makeWave(wav,3,32,nan32,sizeof nan32);
  if (AUDIO_CODEC_FUNC(WAV_DecodeMemory)(wav,44u+sizeof nan32,&buffer) !=
      STATUS_CONST(INVALID_ARGUMENT) || buffer.samples != NULL) return 5;

  /* Invalid block alignment must be rejected even for otherwise valid PCM. */
  makeWave(wav,1,24,pcm24,sizeof pcm24);
  u16(wav+32,2);
  if (AUDIO_CODEC_FUNC(WAV_DecodeMemory)(wav,44u+sizeof pcm24,&buffer) !=
      STATUS_CONST(INVALID_ARGUMENT) || buffer.samples != NULL) return 6;
  return 0;
}
