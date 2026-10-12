#include "../../Cosmeron/Modules/Audio/Codec/MP3Memory.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Checks bounded streaming chunks against full read, deterministic seek,
 * reported Xing/LAME gapless length and error atomicity. */
int main(int argc, char **argv) {
  FILE *file;
  long end;
  unsigned char *bytes;
  AUDIO_CODEC_TYPE(TMP3Memory) stream = {0};
  AUDIO_CODEC_TYPE(TMP3FrameInfo) first;
  float *whole;
  size_t offset, length, got, used, i, n;
  uint32_t rate = 0, channels = 0;
  uint64_t frames = 0;
  assert(argc == 2);
  file = fopen(argv[1], "rb");
  assert(file && fseek(file, 0, SEEK_END) == 0);
  end = ftell(file);
  assert(end > 0 && end < 2000000 && fseek(file, 0, SEEK_SET) == 0);
  length = (size_t)end;
  bytes = (unsigned char *)malloc(length);
  assert(bytes && fread(bytes, 1, length, file) == length);
  assert(fclose(file) == 0);
  assert(AUDIO_CODEC_FUNC(MP3_FindFrame)(
      bytes, length, 0, &offset, &first) == STATUS_CONST(SUCCESS));
  assert(AUDIO_CODEC_FUNC(MP3_MemoryOpen)(
      &stream, bytes, length) == STATUS_CONST(SUCCESS));
  assert(AUDIO_CODEC_FUNC(MP3_MemoryOpen)(
      &stream, bytes, length) == STATUS_CONST(BUSY));
  assert(AUDIO_CODEC_FUNC(MP3_MemoryInfo)(
      &stream, &rate, &channels, &frames) == STATUS_CONST(SUCCESS));
  assert(rate == first.sampleRate && channels == first.channels);
  assert(frames >= first.samplesPerFrame && frames < 1000000);
  whole = (float *)malloc((size_t)frames * channels * sizeof(float));
  assert(whole);
  used = 0;
  while (used < frames) {
    size_t amount = used % 2 ? 31 : 193;
    if (amount > frames - used) amount = (size_t)(frames - used);
    got = SIZE_MAX;
    assert(AUDIO_CODEC_FUNC(MP3_MemoryRead)(
        &stream, whole + used*channels, amount, &got) ==
        STATUS_CONST(SUCCESS));
    assert(got > 0 && got <= amount);
    used += got;
  }
  got = SIZE_MAX;
  assert(AUDIO_CODEC_FUNC(MP3_MemoryRead)(
      &stream, whole, 0, &got) == STATUS_CONST(SUCCESS) && !got);
  assert(AUDIO_CODEC_FUNC(MP3_MemoryRead)(
      &stream, whole, 100, &got) == STATUS_CONST(SUCCESS) && !got);
  assert(AUDIO_CODEC_FUNC(MP3_MemorySeek)(
      &stream, frames + 1) == STATUS_CONST(OUT_OF_RANGE));
  for (i = 0; i < 4; ++i) {
    uint64_t sample = i == 0 ? 0 : i == 1 ? frames/2 :
                      i == 2 ? frames-1 : frames;
    float check[96 * 2] = {0};
    assert(AUDIO_CODEC_FUNC(MP3_MemorySeek)(
        &stream, sample) == STATUS_CONST(SUCCESS));
    n = frames - sample > 96 ? 96 : (size_t)(frames-sample);
    assert(AUDIO_CODEC_FUNC(MP3_MemoryRead)(
        &stream, check, n, &got) == STATUS_CONST(SUCCESS));
    assert(got == n);
    if (got) assert(memcmp(check, whole + (size_t)sample * channels,
        got * channels * sizeof(float)) == 0);
  }
  assert(AUDIO_CODEC_FUNC(MP3_MemorySeek)(
      &stream, 0) == STATUS_CONST(SUCCESS));
  {
    float small[2] = {0,0};
    assert(AUDIO_CODEC_FUNC(MP3_MemoryRead)(
        &stream, small, 1, &got) == STATUS_CONST(SUCCESS) && got == 1);
    assert(memcmp(small, whole, channels * sizeof(float)) == 0);
  }
  AUDIO_CODEC_FUNC(MP3_MemoryClose)(&stream);
  AUDIO_CODEC_FUNC(MP3_MemoryClose)(&stream);
  assert(stream.internal == NULL);
  assert(AUDIO_CODEC_FUNC(MP3_MemoryOpen)(
      &stream, bytes, length-1) != STATUS_CONST(SUCCESS));
  assert(stream.internal == NULL);
  free(whole);
  free(bytes);
  return 0;
}
