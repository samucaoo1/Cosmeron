#define AUDIO_CODEC_EXPERIMENTAL_NATIVE_MP3 1
#include "../../Cosmeron/Modules/Audio/CodecMinimal.h"
#include <assert.h>
#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Inject callback failures without modifying the compressed MP3 file.
 * A failed seek must not change logical playback position; partial
 * reads expose the precise number of already-delivered PCM frames. */
typedef struct TFaultReader {
  const unsigned char *data;
  size_t size, position, chunk;
  unsigned int failRead, failSeek, returnEOF, overreport;
  size_t readCalls, seekCalls;
} TFaultReader;

static OPSTATUS faultRead(
    void *user, void *out, size_t capacity, size_t *outBytes) {
  TFaultReader *r = (TFaultReader *)user;
  size_t amount;
  ++r->readCalls;
  if (r->failRead) {
    --r->failRead;
    *outBytes = 0;
    return STATUS_CONST(GENERIC_ERROR);
  }
  if (r->overreport) {
    --r->overreport;
    *outBytes = capacity + 1;
    return STATUS_CONST(SUCCESS);
  }
  if (r->returnEOF) {
    --r->returnEOF;
    *outBytes = 0;
    return STATUS_CONST(SUCCESS);
  }
  if (r->position > r->size) {
    *outBytes = 0;
    return STATUS_CONST(SUCCESS);
  }
  amount = r->size - r->position;
  if (amount > capacity) amount = capacity;
  if (amount > r->chunk) amount = r->chunk;
  if (amount) memcpy(out, r->data + r->position, amount);
  r->position += amount;
  *outBytes = amount;
  return STATUS_CONST(SUCCESS);
}

static OPSTATUS faultSeek(void *user, int64_t offset,
                          AUDIO_CODEC_TYPE(TSeekOrigin) origin) {
  TFaultReader *r = (TFaultReader *)user;
  size_t base, pos;
  ++r->seekCalls;
  if (r->failSeek) {
    --r->failSeek;
    return STATUS_CONST(GENERIC_ERROR);
  }
  base = origin == AUDIO_CODEC_CONST(SEEK_START) ? 0 :
         origin == AUDIO_CODEC_CONST(SEEK_CURRENT) ? r->position : r->size;
  if (offset < 0) {
    uint64_t magnitude = (uint64_t)(-(offset + 1)) + 1;
    if (magnitude > base) return STATUS_CONST(OUT_OF_RANGE);
    pos = base - (size_t)magnitude;
  } else {
    if ((uint64_t)offset > SIZE_MAX - base)
      return STATUS_CONST(OUT_OF_RANGE);
    pos = base + (size_t)offset;
  }
  r->position = pos;
  return STATUS_CONST(SUCCESS);
}

static AUDIO_CODEC_TYPE(TReader) makeReader(TFaultReader *context) {
  AUDIO_CODEC_TYPE(TReader) reader;
  reader.userData = context;
  reader.read = faultRead;
  reader.seek = faultSeek;
  return reader;
}

static void checkNext(AUDIO_CODEC_TYPE(TDecoder) *stream,
                      AUDIO_CODEC_TYPE(TMP3Memory) *memory,
                      uint64_t at, uint32_t channels) {
  float actual[37*2], expected[37*2];
  size_t n = 0, m = 0;
  assert(AUDIO_CODEC_FUNC(MP3_MemorySeek)(memory, at) ==
      STATUS_CONST(SUCCESS));
  assert(AUDIO_CODEC_FUNC(ReadFrames)(stream, actual, 37, &n) ==
      STATUS_CONST(SUCCESS));
  assert(AUDIO_CODEC_FUNC(MP3_MemoryRead)(memory, expected, 37, &m) ==
      STATUS_CONST(SUCCESS));
  assert(n == m && n == 37);
  assert(!memcmp(actual, expected, n*channels*sizeof(float)));
}

static void runFaults(const unsigned char *bytes, size_t size) {
  TFaultReader context = {0};
  AUDIO_CODEC_TYPE(TReader) reader;
  AUDIO_CODEC_TYPE(TDecoder) stream = {0};
  AUDIO_CODEC_TYPE(TMP3Memory) reference = {0};
  AUDIO_CODEC_TYPE(TInfo) info = {0};
  uint32_t rate, channels;
  uint64_t frames, current;
  float *buffer;
  size_t got = 999;
  OPSTATUS status;
  context.data = bytes;
  context.size = size;
  context.chunk = 3; /* force short headers and compressed frames */
  reader = makeReader(&context);
  assert(AUDIO_CODEC_FUNC(MP3_MemoryOpen)(
      &reference, bytes, size) == STATUS_CONST(SUCCESS));
  assert(AUDIO_CODEC_FUNC(MP3_MemoryInfo)(
      &reference, &rate, &channels, &frames) == STATUS_CONST(SUCCESS));
  assert(frames > 2500);
  assert(AUDIO_CODEC_FUNC(OpenReader)(
      &stream, &reader) == STATUS_CONST(SUCCESS));
  assert(AUDIO_CODEC_FUNC(GetInfo)(
      &stream, &info) == STATUS_CONST(SUCCESS));
  assert(info.frameCount == frames && info.sampleRate == rate &&
      info.channels == channels);
  buffer = (float *)malloc(1500 * channels * sizeof(float));
  assert(buffer);

  /* Seek errors do not publish scratch decoder state. */
  assert(AUDIO_CODEC_FUNC(ReadFrames)(
      &stream, buffer, 127, &got) == STATUS_CONST(SUCCESS) && got == 127);
  current = 127;
  context.failRead = 1;
  status = AUDIO_CODEC_FUNC(SeekFrame)(&stream, frames/2);
  assert(status == STATUS_CONST(GENERIC_ERROR) && context.failRead == 0);
  checkNext(&stream, &reference, current, channels);
  current += 37;
  context.failSeek = 1;
  status = AUDIO_CODEC_FUNC(SeekFrame)(&stream, frames/2);
  assert(status == STATUS_CONST(GENERIC_ERROR) && context.failSeek == 0);
  checkNext(&stream, &reference, current, channels);
  current += 37;
  assert(AUDIO_CODEC_FUNC(SeekFrame)(
      &stream, frames+1) == STATUS_CONST(OUT_OF_RANGE));
  checkNext(&stream, &reference, current, channels);
  current += 37;

  /* Force a failure in the next encoded frame after some PCM is
   * delivered; the partial-frame count must remain meaningful. */
  context.failRead = 1;
  got = SIZE_MAX;
  status = AUDIO_CODEC_FUNC(ReadFrames)(
      &stream, buffer, 1500, &got);
  assert(status == STATUS_CONST(GENERIC_ERROR));
  assert(got > 0 && got < 1500);
  current += got;
  checkNext(&stream, &reference, current, channels);

  /* Subsequent seeks use absolute offsets and recover from callback
   * cursor changes caused by the failed operation. */
  assert(AUDIO_CODEC_FUNC(SeekFrame)(
      &stream, frames-100) == STATUS_CONST(SUCCESS));
  assert(AUDIO_CODEC_FUNC(MP3_MemorySeek)(
      &reference, frames-100) == STATUS_CONST(SUCCESS));
  {
    float actual[100*2], expected[100*2];
    size_t a = 0, b = 0;
    assert(AUDIO_CODEC_FUNC(ReadFrames)(
        &stream, actual, 100, &a) == STATUS_CONST(SUCCESS));
    assert(AUDIO_CODEC_FUNC(MP3_MemoryRead)(
        &reference, expected, 100, &b) == STATUS_CONST(SUCCESS));
    assert(a == 100 && b == 100);
    assert(!memcmp(actual, expected, a*channels*sizeof(float)));
  }
  free(buffer);
  AUDIO_CODEC_FUNC(Close)(&stream);
  AUDIO_CODEC_FUNC(MP3_MemoryClose)(&reference);
}

static void runMalformed(const unsigned char *bytes, size_t size) {
  TFaultReader context = {0};
  AUDIO_CODEC_TYPE(TDecoder) stream = {0};
  AUDIO_CODEC_TYPE(TReader) reader;
  unsigned char invalidTag[10] = {'I','D','3',4,0,0,0,0,0,0};
  unsigned char shortMp3[11] = {0xff,0xfb,0x90,0xc4};
  context.data = bytes;
  context.size = size;
  context.chunk = 2;
  reader = makeReader(&context);
  context.failSeek = 1;
  assert(AUDIO_CODEC_FUNC(OpenReader)(
      &stream, &reader) == STATUS_CONST(GENERIC_ERROR));
  assert(stream.internal == NULL);
  context.failRead = 1;
  assert(AUDIO_CODEC_FUNC(OpenReader)(
      &stream, &reader) == STATUS_CONST(GENERIC_ERROR));
  assert(stream.internal == NULL);
  context.overreport = 1;
  assert(AUDIO_CODEC_FUNC(OpenReader)(
      &stream, &reader) == STATUS_CONST(INVALID_SEQUENCE));
  assert(stream.internal == NULL);
  context.returnEOF = 1;
  assert(AUDIO_CODEC_FUNC(OpenReader)(
      &stream, &reader) == STATUS_CONST(INCOMPLETE_SEQUENCE));
  assert(stream.internal == NULL);
  context.data = bytes;
  context.size = size-1;
  assert(AUDIO_CODEC_FUNC(OpenReader)(
      &stream, &reader) != STATUS_CONST(SUCCESS));
  assert(stream.internal == NULL);
  /* Seekable, but invalid/truncated ID3v2 tags cannot create a decoder. */
  invalidTag[6] = 0x80; /* invalid synchsafe 7-bit octet */
  context.data = invalidTag;
  context.size = sizeof invalidTag;
  assert(AUDIO_CODEC_FUNC(OpenReader)(
      &stream, &reader) != STATUS_CONST(SUCCESS));
  assert(stream.internal == NULL);
  context.data = shortMp3;
  context.size = sizeof shortMp3;
  assert(AUDIO_CODEC_FUNC(OpenReader)(
      &stream, &reader) != STATUS_CONST(SUCCESS));
  assert(stream.internal == NULL);
  AUDIO_CODEC_FUNC(Close)(&stream);
}

int main(void) {
  const char *path = "Fixtures/Tone.mp3";
  unsigned char *bytes;
  FILE *f = fopen(path,"rb");
  long length;
  assert(f && fseek(f,0,SEEK_END) == 0);
  length = ftell(f);
  assert(length > 200 && length < 2000000 &&
      fseek(f,0,SEEK_SET) == 0);
  bytes = (unsigned char *)malloc((size_t)length);
  assert(bytes && fread(bytes,1,(size_t)length,f) == (size_t)length);
  assert(fclose(f) == 0);
  runFaults(bytes,(size_t)length);
  runMalformed(bytes,(size_t)length);
  free(bytes);
  return 0;
}
