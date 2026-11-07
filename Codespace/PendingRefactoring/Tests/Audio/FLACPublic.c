#include "../../Cosmeron/Modules/Audio/CodecMinimal.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

int main(void) {
  FILE *file = fopen("Fixtures/Tone.flac", "rb");
  AUDIO_CODEC_TYPE(TDecoder) native = {0}, reference = {0};
  AUDIO_CODEC_TYPE(TInfo) info = {0};
  unsigned char *bytes;
  float samples[256], referenceSamples[256];
  long length;
  size_t frames, other, total = 0, i;
  double energy = 0;
  assert(file && !fseek(file, 0, SEEK_END));
  length = ftell(file);
  assert(length > 42 && !fseek(file, 0, SEEK_SET));
  bytes = (unsigned char *)malloc((size_t)length);
  assert(bytes && fread(bytes, 1, (size_t)length, file) == (size_t)length);
  assert(fclose(file) == 0);
  assert(AUDIO_CODEC_FUNC(OpenMemory)(&native, bytes, (size_t)length) ==
      STATUS_CONST(SUCCESS));
  assert(((AUDIO_INS(Decoder) *)native.internal)->nativeFlac);
  assert(AUDIO_CODEC_FUNC(OpenFile)(&reference, "Fixtures/Tone.flac") ==
      STATUS_CONST(SUCCESS));
  assert(AUDIO_CODEC_FUNC(GetInfo)(&native, &info) == STATUS_CONST(SUCCESS));
  assert(info.channels == 2 && info.sampleRate == 44100 &&
      info.lengthKnown && info.frameCount > 1000);
  for (;;) {
    assert(AUDIO_CODEC_FUNC(ReadFrames)(&native, samples, 128, &frames) ==
        STATUS_CONST(SUCCESS));
    assert(AUDIO_CODEC_FUNC(ReadFrames)(&reference, referenceSamples, 128, &other) ==
        STATUS_CONST(SUCCESS));
    assert(frames == other);
    for (i = 0; i < frames * info.channels; ++i) {
      float difference = samples[i] - referenceSamples[i];
      assert(difference < 0.00003f && difference > -0.00003f);
      energy += samples[i] >= 0 ? samples[i] : -samples[i];
    }
    total += frames;
    if (!frames) break;
  }
  assert(total == info.frameCount && energy > 10);
  assert(AUDIO_CODEC_FUNC(SeekFrame)(&native, 17) == STATUS_CONST(SUCCESS));
  assert(AUDIO_CODEC_FUNC(SeekFrame)(&reference, 17) == STATUS_CONST(SUCCESS));
  assert(AUDIO_CODEC_FUNC(ReadFrames)(&native, samples, 128, &frames) ==
      STATUS_CONST(SUCCESS) && frames == 128);
  assert(AUDIO_CODEC_FUNC(ReadFrames)(&reference, referenceSamples, 128, &other) ==
      STATUS_CONST(SUCCESS) && frames == other);
  for (i = 0; i < frames * info.channels; ++i)
    assert(samples[i] - referenceSamples[i] < 0.00003f &&
        samples[i] - referenceSamples[i] > -0.00003f);
  assert(AUDIO_CODEC_FUNC(SeekFrame)(&native, info.frameCount + 1) ==
      STATUS_CONST(OUT_OF_RANGE));
  assert(AUDIO_CODEC_FUNC(SeekFrame)(&native, 0) == STATUS_CONST(SUCCESS));
  assert(AUDIO_CODEC_FUNC(ReadFrames)(&native, samples, 128, &frames) ==
      STATUS_CONST(SUCCESS) && frames == 128);
  AUDIO_CODEC_FUNC(Close)(&native);
  AUDIO_CODEC_FUNC(Close)(&reference);
  free(bytes);
  return 0;
}
