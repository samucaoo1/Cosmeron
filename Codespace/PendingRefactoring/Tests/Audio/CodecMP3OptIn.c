#define AUDIO_CODEC_EXPERIMENTAL_NATIVE_MP3 1
#include "../../Cosmeron/Modules/Audio/CodecMinimal.h"
#include <assert.h>
#include <string.h>

int main(void) {
  unsigned char file[4*417] = {0};
  AUDIO_CODEC_TYPE(TDecoder) decoder = {0};
  AUDIO_CODEC_TYPE(TInfo) info = {0};
  float pcm[64] = {0}, reference[64] = {0};
  size_t i, got = 0;
  for (i = 0; i < sizeof file; i += 417) {
    file[i] = 0xff; file[i+1] = 0xfb;
    file[i+2] = 0x90; file[i+3] = 0xc4;
  }
  assert(AUDIO_CODEC_FUNC(OpenMemory)(
      &decoder, file, sizeof file) == STATUS_CONST(SUCCESS));
  assert(AUDIO_CODEC_FUNC(GetInfo)(
      &decoder, &info) == STATUS_CONST(SUCCESS));
  assert(info.channels == 1 && info.sampleRate == 44100 &&
      info.lengthKnown && info.frameCount == 4608);
  assert(AUDIO_CODEC_FUNC(ReadFrames)(
      &decoder, reference, 64, &got) == STATUS_CONST(SUCCESS));
  assert(got == 64);
  for (i = 0; i < got; ++i) assert(reference[i] == 0.0f);
  assert(AUDIO_CODEC_FUNC(SeekFrame)(
      &decoder, 2305) == STATUS_CONST(SUCCESS));
  assert(AUDIO_CODEC_FUNC(ReadFrames)(
      &decoder, pcm, 64, &got) == STATUS_CONST(SUCCESS) && got == 64);
  assert(!memcmp(reference, pcm, sizeof reference));
  assert(AUDIO_CODEC_FUNC(SeekFrame)(
      &decoder, 0) == STATUS_CONST(SUCCESS));
  assert(AUDIO_CODEC_FUNC(ReadFrames)(
      &decoder, pcm, 64, &got) == STATUS_CONST(SUCCESS) && got == 64);
  assert(!memcmp(reference, pcm, sizeof reference));
  AUDIO_CODEC_FUNC(Close)(&decoder);
  AUDIO_CODEC_FUNC(Close)(&decoder);
  assert(decoder.internal == NULL);
  return 0;
}
