#include "../../Cosmeron/Modules/Audio/Audio.h"
#include <assert.h>
#include <stdio.h>

static OPSTATUS readBytes(void *user, void *out, size_t capacity,
                          size_t *outCount) {
  FILE *file = (FILE *)user;
  *outCount = fread(out, 1, capacity, file);
  return ferror(file) ? STATUS_CONST(GENERIC_ERROR) : STATUS_CONST(SUCCESS);
}
static OPSTATUS seekBytes(void *user, int64_t offset,
                          AUDIO_CODEC_TYPE(TSeekOrigin) origin) {
  int native = origin == AUDIO_CODEC_CONST(SEEK_START)     ? SEEK_SET
               : origin == AUDIO_CODEC_CONST(SEEK_CURRENT) ? SEEK_CUR
                                                           : SEEK_END;
  if (offset > LONG_MAX || offset < LONG_MIN)
    return STATUS_CONST(OUT_OF_RANGE);
  return fseek((FILE *)user, (long)offset, native) == 0
             ? STATUS_CONST(SUCCESS)
             : STATUS_CONST(GENERIC_ERROR);
}
static void testFile(const char *path) {
  AUDIO_CODEC_TYPE(TDecoder) decoder = {0};
  AUDIO_CODEC_TYPE(TInfo) info;
  float samples[514];
  size_t count, total = 0, i;
  double energy = 0;
  assert(AUDIO_CODEC_FUNC(OpenFile)(&decoder, path) == STATUS_CONST(SUCCESS));
  assert(AUDIO_CODEC_FUNC(GetInfo)(&decoder, &info) == STATUS_CONST(SUCCESS));
  assert(info.channels == 2 && info.sampleRate == 44100);
  do {
    assert(AUDIO_CODEC_FUNC(ReadFrames)(&decoder, samples, 257, &count) ==
           STATUS_CONST(SUCCESS));
    total += count;
    for (i = 0; i < count * 2; ++i)
      energy += samples[i] * samples[i];
  } while (count != 0);
  assert(total >= 4410 && total < 7000 && energy > 1);
  assert(AUDIO_CODEC_FUNC(SeekFrame)(&decoder, 100) == STATUS_CONST(SUCCESS));
  assert(AUDIO_CODEC_FUNC(ReadFrames)(&decoder, samples, 257, &count) ==
             STATUS_CONST(SUCCESS) &&
         count == 257);
  AUDIO_CODEC_FUNC(Close)(&decoder);
  AUDIO_CODEC_FUNC(Close)(&decoder);
}
int main(void) {
  AUDIO_CODEC_TYPE(TDecoder) decoder = {0};
  AUDIO_CODEC_TYPE(TReader) reader;
  AUDIO_PCM_TYPE(TBuffer) tone = {0};
  FILE *file;
  unsigned char bytes[20000];
  size_t size, count, i;
  float samples[128];
  testFile("Fixtures/Tone.wav");
  testFile("Fixtures/Tone.mp3");
  testFile("Fixtures/Tone.flac");
  file = fopen("Fixtures/Tone.flac", "rb");
  assert(file != NULL);
  reader.userData = file;
  reader.read = readBytes;
  reader.seek = seekBytes;
  assert(AUDIO_CODEC_FUNC(OpenReader)(&decoder, &reader) ==
         STATUS_CONST(SUCCESS));
  assert(AUDIO_CODEC_FUNC(ReadFrames)(&decoder, samples, 64, &count) ==
             STATUS_CONST(SUCCESS) &&
         count == 64);
  AUDIO_CODEC_FUNC(Close)(&decoder);
  fclose(file);
  file = fopen("Fixtures/Tone.mp3", "rb");
  assert(file != NULL);
  size = fread(bytes, 1, sizeof(bytes), file);
  fclose(file);
  assert(AUDIO_CODEC_FUNC(OpenMemory)(&decoder, bytes, size) ==
         STATUS_CONST(SUCCESS));
  assert(AUDIO_CODEC_FUNC(ReadFrames)(&decoder, samples, 64, &count) ==
             STATUS_CONST(SUCCESS) &&
         count == 64);
  AUDIO_CODEC_FUNC(Close)(&decoder);
  for (i = 1; i < 48; ++i) {
    OPSTATUS status = AUDIO_CODEC_FUNC(OpenMemory)(&decoder, bytes, i);
    if (status == STATUS_CONST(SUCCESS))
      AUDIO_CODEC_FUNC(Close)(&decoder);
    else
      assert(decoder.internal == NULL);
  }
  assert(AUDIO_CODEC_FUNC(OpenFile)(&decoder, "Fixtures/does-not-exist.wav") !=
         STATUS_CONST(SUCCESS));
  assert(decoder.internal == NULL);
  assert(AUDIO_BEEP_FUNC(RenderTone)(440, 100, 0.25f, 44100, &tone) ==
         STATUS_CONST(SUCCESS));
  assert(AUDIO_CODEC_FUNC(WriteWAV)("roundtrip.wav", &tone) ==
         STATUS_CONST(SUCCESS));
  assert(AUDIO_CODEC_FUNC(OpenFile)(&decoder, "roundtrip.wav") ==
         STATUS_CONST(SUCCESS));
  assert(AUDIO_CODEC_FUNC(ReadFrames)(&decoder, samples, 128, &count) ==
             STATUS_CONST(SUCCESS) &&
         count == 128);
  for (i = 0; i < count; ++i) {
    float delta = samples[i] - tone.samples[i];
    assert(delta > -0.0001f && delta < 0.0001f);
  }
  AUDIO_CODEC_FUNC(Close)(&decoder);
  AUDIO_PCM_FUNC(Destroy)(&tone);
  remove("roundtrip.wav");
  puts("Audio codecs WAV/MP3/FLAC: OK");
  return 0;
}
