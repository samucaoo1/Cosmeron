#include "../../Cosmeron/Modules/Audio/Codec/MP3Memory.h"
#include <assert.h>
#include <string.h>
int main(void) {
  AUDIO_CODEC_TYPE(TMP3Memory) stream = {0};
  unsigned char frames[3*417] = {0};
  float output[64], first[64];
  uint32_t rate = 0, channels = 0;
  uint64_t count = 0;
  size_t got = 0, i;
  for (i = 0; i < sizeof frames; i += 417) {
    frames[i] = 0xff; frames[i+1] = 0xfb;
    frames[i+2] = 0x90; frames[i+3] = 0xc4;
  }
  assert(AUDIO_CODEC_FUNC(MP3_MemoryOpen)(
      &stream, frames, sizeof frames) == STATUS_CONST(SUCCESS));
  assert(AUDIO_CODEC_FUNC(MP3_MemoryInfo)(
      &stream,&rate,&channels,&count) == STATUS_CONST(SUCCESS));
  assert(rate == 44100 && channels == 1 && count == 3456);
  assert(AUDIO_CODEC_FUNC(MP3_MemoryRead)(
      &stream, first, 64, &got) == STATUS_CONST(SUCCESS) && got == 64);
  for (i = 0; i < got; ++i) assert(first[i] == 0.f);
  assert(AUDIO_CODEC_FUNC(MP3_MemorySeek)(
      &stream, 1900) == STATUS_CONST(SUCCESS));
  assert(AUDIO_CODEC_FUNC(MP3_MemoryRead)(
      &stream, output, 64, &got) == STATUS_CONST(SUCCESS) && got == 64);
  for (i = 0; i < got; ++i) assert(output[i] == 0.f);
  assert(AUDIO_CODEC_FUNC(MP3_MemorySeek)(
      &stream, 0) == STATUS_CONST(SUCCESS));
  assert(AUDIO_CODEC_FUNC(MP3_MemoryRead)(
      &stream, output, 64, &got) == STATUS_CONST(SUCCESS) && got == 64);
  assert(!memcmp(first, output, sizeof first));
  assert(AUDIO_CODEC_FUNC(MP3_MemorySeek)(
      &stream, count) == STATUS_CONST(SUCCESS));
  assert(AUDIO_CODEC_FUNC(MP3_MemoryRead)(
      &stream, output, 64, &got) == STATUS_CONST(SUCCESS) && got == 0);
  AUDIO_CODEC_FUNC(MP3_MemoryClose)(&stream);
  AUDIO_CODEC_FUNC(MP3_MemoryClose)(&stream);
  return 0;
}
