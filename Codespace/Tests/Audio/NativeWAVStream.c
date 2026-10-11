#include "../../Cosmeron/Modules/Audio/CodecMinimal.h"
#include <assert.h>
#include <string.h>

static void store16(unsigned char *p, unsigned int n) {
  p[0] = (unsigned char)n;
  p[1] = (unsigned char)(n >> 8);
}
static void store32(unsigned char *p, unsigned int n) {
  p[0] = (unsigned char)n;
  p[1] = (unsigned char)(n >> 8);
  p[2] = (unsigned char)(n >> 16);
  p[3] = (unsigned char)(n >> 24);
}
static void pcm16(unsigned char *out, bool extensible) {
  static const unsigned char subtype[16] = {
    1,0,0,0,0,0,0x10,0,0x80,0,0,0xaa,0,0x38,0x9b,0x71
  };
  unsigned int fmtSize = extensible ? 40 : 16;
  size_t dataAt = 20 + fmtSize;
  memset(out, 0, 100);
  memcpy(out, "RIFF", 4);
  store32(out + 4, (unsigned int)(fmtSize + 24));
  memcpy(out + 8, "WAVEfmt ", 8);
  store32(out + 16, fmtSize);
  store16(out + 20, extensible ? 0xfffe : 1);
  store16(out + 22, 1);
  store32(out + 24, 8000);
  store32(out + 28, 16000);
  store16(out + 32, 2);
  store16(out + 34, 16);
  if (extensible) {
    store16(out + 36, 22);
    store16(out + 38, 16);
    store32(out + 40, 4);
    memcpy(out + 44, subtype, 16);
  }
  memcpy(out + dataAt, "data", 4);
  store32(out + dataAt + 4, 4);
  store16(out + dataAt + 8, 0x8000);
  store16(out + dataAt + 10, 0x7fff);
}
static void exercise(bool extensible) {
  unsigned char wav[100];
  size_t size = extensible ? 72 : 48, count = 99;
  AUDIO_CODEC_TYPE(TDecoder) decoder = {0};
  AUDIO_CODEC_TYPE(TInfo) info = {0};
  AUDIO_PCM_TYPE(TBuffer) pcm = {0};
  float samples[4] = {0};
  pcm16(wav, extensible);
  assert(AUDIO_CODEC_FUNC(OpenMemory)(&decoder, wav, size) == STATUS_CONST(SUCCESS));
  assert(((AUDIO_INS(Decoder) *)decoder.internal)->nativeWav);
  assert(AUDIO_CODEC_FUNC(GetInfo)(&decoder, &info) == STATUS_CONST(SUCCESS));
  assert(info.sampleRate == 8000 && info.channels == 1 &&
      info.frameCount == 2 && info.lengthKnown);
  assert(AUDIO_CODEC_FUNC(ReadFrames)(&decoder, samples, 1, &count) ==
      STATUS_CONST(SUCCESS) && count == 1 && samples[0] == -1.f);
  assert(AUDIO_CODEC_FUNC(ReadFrames)(&decoder, samples, 1, &count) ==
      STATUS_CONST(SUCCESS) && count == 1 && samples[0] > .999f);
  assert(AUDIO_CODEC_FUNC(ReadFrames)(&decoder, samples, 1, &count) ==
      STATUS_CONST(SUCCESS) && count == 0);
  assert(AUDIO_CODEC_FUNC(SeekFrame)(&decoder, 0) == STATUS_CONST(SUCCESS));
  assert(AUDIO_CODEC_FUNC(ReadFrames)(&decoder, samples, 2, &count) ==
      STATUS_CONST(SUCCESS) && count == 2 && samples[0] == -1.f);
  assert(AUDIO_CODEC_FUNC(SeekFrame)(&decoder, 3) == STATUS_CONST(OUT_OF_RANGE));
  AUDIO_CODEC_FUNC(Close)(&decoder);
  AUDIO_CODEC_FUNC(Close)(&decoder);
  assert(AUDIO_CODEC_FUNC(WAV_DecodeMemory)(wav, size, &pcm) ==
      STATUS_CONST(SUCCESS) && pcm.frameCount == 2);
  assert(pcm.samples[0] == -1.f && pcm.samples[1] > .999f);
  AUDIO_PCM_FUNC(Destroy)(&pcm);
  store32(wav + 28, 7);
  assert(AUDIO_CODEC_FUNC(OpenMemory)(&decoder, wav, size) ==
      STATUS_CONST(INVALID_ARGUMENT) && !decoder.internal);
}
int main(void) {
  exercise(false);
  exercise(true);
  return 0;
}
