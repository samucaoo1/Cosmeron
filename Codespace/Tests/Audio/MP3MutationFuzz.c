/* Self-contained C11 MP3 input-mutation regression and optional libFuzzer
 * target. Exercises the independent native decoder.
 *
 * Strict C11: cc -std=c11 -Wall -Wextra -Wpedantic -Werror MP3MutationFuzz.c
 * Fuzzing: clang -DCOSMERON_MP3_LIBFUZZER -fsanitize=fuzzer,address,undefined
 *          -O1 -g MP3MutationFuzz.c -o mp3-fuzz
 */
#include "../../Cosmeron/Modules/Audio/Codec/MP3Memory.h"
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

/* Use short bounded inputs: a single invocation is deterministic, fast
 * and leaves no live allocations, even when a frame is malformed. */
int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
  AUDIO_CODEC_TYPE(TMP3Memory) stream = {0};
  AUDIO_CODEC_TYPE(TMP3FrameInfo) header;
  uint32_t sampleRate = 0, channels = 0;
  uint64_t frames = 0;
  size_t at = 0, got = 0;
  float pcm[127 * 2] = {0};
  OPSTATUS status;
  if (!data || size == 0 || size > 16384) return 0;

  /* The independent scanners are exposed so a malformed tag never has to
   * make it all the way to PCM playback to exercise its parsing bounds. */
  (void)AUDIO_CODEC_FUNC(MP3_SkipID3)(data, size, &at);
  (void)AUDIO_CODEC_FUNC(MP3_ParseHeader)(data, size, &header);
  (void)AUDIO_CODEC_FUNC(MP3_FindFrame)(data, size, 0, &at, &header);

  status = AUDIO_CODEC_FUNC(MP3_MemoryOpen)(&stream, data, size);
  if (status == STATUS_CONST(SUCCESS)) {
    assert(stream.internal != NULL);
    assert(AUDIO_CODEC_FUNC(MP3_MemoryInfo)(
        &stream, &sampleRate, &channels, &frames) == STATUS_CONST(SUCCESS));
    assert((channels == 1 || channels == 2) && sampleRate > 0);
    /* A header cannot yield a more-than-file-sized compressed frame count. */
    assert(frames <= (uint64_t)size * 1152U);
    (void)AUDIO_CODEC_FUNC(MP3_MemoryRead)(
        &stream, pcm, 127, &got);
    assert(got <= 127);
    /* Errors on malformed samples are allowed. Read/seek must remain
     * memory-safe and the descriptor must be reusable after Close. */
    status = AUDIO_CODEC_FUNC(MP3_MemorySeek)(&stream, frames / 2);
    if (status == STATUS_CONST(SUCCESS)) {
      got = 0;
      (void)AUDIO_CODEC_FUNC(MP3_MemoryRead)(
          &stream, pcm, 13, &got);
      assert(got <= 13);
    }
    (void)AUDIO_CODEC_FUNC(MP3_MemorySeek)(&stream, 0);
  } else {
    assert(stream.internal == NULL);
  }
  AUDIO_CODEC_FUNC(MP3_MemoryClose)(&stream);
  assert(stream.internal == NULL);
  return 0;
}

#ifndef COSMERON_MP3_LIBFUZZER
int main(void) {
  unsigned char seed[4U * 417U] = {0};
  unsigned char mutated[4U * 417U + 12U];
  unsigned int state = 0x7A4DB239U;
  size_t i, j, length;
  /* Four valid silent MPEG-1 mono frames. Mutate sync, side info,
   * main data, Xing fields, payload length and optional ID3 prefix. */
  for (i = 0; i < sizeof seed; i += 417) {
    seed[i] = 0xFF; seed[i+1] = 0xFB;
    seed[i+2] = 0x90; seed[i+3] = 0xC4;
  }
  assert(LLVMFuzzerTestOneInput(seed, sizeof seed) == 0);
  for (i = 0; i < 384; ++i) {
    memcpy(mutated, seed, sizeof seed);
    state ^= state << 13;
    state ^= state >> 17;
    state ^= state << 5;
    j = (size_t)state % sizeof seed;
    switch (i % 7U) {
      case 0: mutated[j] ^= (unsigned char)(1U << (i & 7U)); break;
      case 1: mutated[j] = (unsigned char)state; break;
      case 2: mutated[j] = 0xFF; break;
      case 3: mutated[j] = 0; break;
      case 4:
        /* Shift one MPEG header into the payload to corrupt frame sync. */
        mutated[j] = (unsigned char)(state >> 16);
        if (j+1 < sizeof seed) mutated[j+1] = 0xFF;
        break;
      case 5:
        memcpy(mutated, "ID3\x04\0\0\0\0\0\0", 10);
        mutated[9] = (unsigned char)(state & 127U);
        break;
      default:
        mutated[0] = (unsigned char)state;
        mutated[1] = (unsigned char)(state >> 8);
        mutated[2] = (unsigned char)(state >> 16);
        break;
    }
    length = (i % 5U == 0) ? (size_t)(state % sizeof seed) : sizeof seed;
    assert(LLVMFuzzerTestOneInput(mutated, length) == 0);
  }
  /* Exhaust every short input boundary, including all partial ID3/
   * MPEG headers, before running the randomized deterministic corpus. */
  for (i = 0; i < 48; ++i)
    assert(LLVMFuzzerTestOneInput(seed, i) == 0);
  return 0;
}
#endif
