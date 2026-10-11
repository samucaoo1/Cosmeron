#include "../../Cosmeron/Modules/Audio/CodecMinimal.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

typedef struct AUDIO_INS(TTestFLACReader) {
  const unsigned char *bytes;
  size_t size, offset, calls;
} AUDIO_INS(TTestFLACReader);
static OPSTATUS testRead(void *context, void *dst, size_t requested, size_t *received) {
  AUDIO_INS(TTestFLACReader) *reader = (AUDIO_INS(TTestFLACReader) *)context;
  size_t amount = reader->size - reader->offset;
  if (amount > requested) amount = requested;
  if (amount > 7) amount = 7;
  if (amount) memcpy(dst, reader->bytes + reader->offset, amount);
  reader->offset += amount;
  ++reader->calls;
  *received = amount;
  return STATUS_CONST(SUCCESS);
}
static OPSTATUS testSeek(void *context, int64_t shift,
                         AUDIO_CODEC_TYPE(TSeekOrigin) origin) {
  AUDIO_INS(TTestFLACReader) *reader = (AUDIO_INS(TTestFLACReader) *)context;
  int64_t base = origin == AUDIO_CODEC_CONST(SEEK_START) ? 0 :
      origin == AUDIO_CODEC_CONST(SEEK_CURRENT) ?
          (int64_t)reader->offset : (int64_t)reader->size;
  if (shift < -base || shift > (int64_t)reader->size - base)
    return STATUS_CONST(OUT_OF_RANGE);
  reader->offset = (size_t)(base + shift);
  return STATUS_CONST(SUCCESS);
}
int main(void) {
  AUDIO_CODEC_TYPE(TDecoder) memory = {0}, fileDecoder = {0}, callback = {0};
  AUDIO_CODEC_TYPE(TInfo) info = {0};
  AUDIO_INS(TTestFLACReader) input;
  AUDIO_CODEC_TYPE(TReader) reader;
  cosmeron_audio_vendor_ma_decoder reference;
  cosmeron_audio_vendor_ma_decoder_config config;
  unsigned char *bytes;
  float a[256], b[256], c[256], d[256];
  FILE *f = fopen("Fixtures/Tone.flac", "rb");
  long n;
  size_t produced = 0, na, nb, nc, i, nref = 0;
  assert(f != NULL && !fseek(f, 0, SEEK_END));
  n = ftell(f);
  assert(n > 42 && !fseek(f, 0, SEEK_SET));
  bytes = (unsigned char *)malloc((size_t)n);
  assert(bytes && fread(bytes, 1, (size_t)n, f) == (size_t)n && !fclose(f));
  input.bytes = bytes;
  input.size = (size_t)n;
  input.offset = input.calls = 0;
  reader.userData = &input;
  reader.read = testRead;
  reader.seek = testSeek;
  assert(AUDIO_CODEC_FUNC(OpenMemory)(&memory, bytes, (size_t)n) ==
      STATUS_CONST(SUCCESS));
  assert(AUDIO_CODEC_FUNC(OpenFile)(&fileDecoder, "Fixtures/Tone.flac") ==
      STATUS_CONST(SUCCESS));
  assert(AUDIO_CODEC_FUNC(OpenReader)(&callback, &reader) ==
      STATUS_CONST(SUCCESS));
  assert(((AUDIO_INS(Decoder) *)fileDecoder.internal)->nativeFlac);
  assert(((AUDIO_INS(Decoder) *)fileDecoder.internal)->flacStreaming);
  assert(((AUDIO_INS(Decoder) *)fileDecoder.internal)->flacBytes == NULL);
  assert(((AUDIO_INS(Decoder) *)callback.internal)->flacStreaming);
  assert(AUDIO_CODEC_FUNC(GetInfo)(&callback, &info) == STATUS_CONST(SUCCESS));
  assert(info.sampleRate == 44100 && info.channels == 2 &&
      info.lengthKnown && info.frameCount > 1000);
  config = cosmeron_audio_vendor_ma_decoder_config_init(
      cosmeron_audio_vendor_ma_format_f32, 0, 0);
  assert(cosmeron_audio_vendor_ma_decoder_init_memory(bytes, (size_t)n,
      &config, &reference) == cosmeron_audio_vendor_MA_SUCCESS);
  for (;;) {
    cosmeron_audio_vendor_ma_uint64 nv = 0;
    assert(AUDIO_CODEC_FUNC(ReadFrames)(&memory, a, 128, &na) == STATUS_CONST(SUCCESS));
    assert(AUDIO_CODEC_FUNC(ReadFrames)(&fileDecoder, b, 128, &nb) == STATUS_CONST(SUCCESS));
    assert(AUDIO_CODEC_FUNC(ReadFrames)(&callback, c, 128, &nc) == STATUS_CONST(SUCCESS));
    assert(cosmeron_audio_vendor_ma_decoder_read_pcm_frames(&reference,
        d, 128, &nv) >= cosmeron_audio_vendor_MA_SUCCESS ||
        nv == 0);
    nref = (size_t)nv;
    assert(na == nb && na == nc && na == nref);
    for (i = 0; i < na * info.channels; ++i) {
      float delta = a[i] - d[i];
      assert(delta > -0.00004f && delta < 0.00004f);
      assert(b[i] == a[i] && c[i] == a[i]);
    }
    produced += na;
    if (!na) break;
  }
  assert(produced == info.frameCount && input.calls > 100);
  assert(((AUDIO_INS(Decoder) *)callback.internal)->flacEncodedCapacity <=
      AUDIO_FLAC_MAX_COMPRESSED_FRAME);
  assert(AUDIO_CODEC_FUNC(SeekFrame)(&callback, 17) == STATUS_CONST(SUCCESS));
  assert(AUDIO_CODEC_FUNC(SeekFrame)(&memory, 17) == STATUS_CONST(SUCCESS));
  assert(AUDIO_CODEC_FUNC(SeekFrame)(&fileDecoder, 17) == STATUS_CONST(SUCCESS));
  assert(AUDIO_CODEC_FUNC(ReadFrames)(&callback, c, 128, &nc) == STATUS_CONST(SUCCESS));
  assert(AUDIO_CODEC_FUNC(ReadFrames)(&memory, a, 128, &na) == STATUS_CONST(SUCCESS));
  assert(AUDIO_CODEC_FUNC(ReadFrames)(&fileDecoder, b, 128, &nb) == STATUS_CONST(SUCCESS));
  assert(na == nc && nc == nb);
  assert(!memcmp(a, b, na * info.channels * sizeof(float)));
  assert(!memcmp(a, c, na * info.channels * sizeof(float)));
  assert(AUDIO_CODEC_FUNC(SeekFrame)(&callback, info.frameCount + 1) ==
      STATUS_CONST(OUT_OF_RANGE));
  AUDIO_CODEC_FUNC(Close)(&memory);
  AUDIO_CODEC_FUNC(Close)(&fileDecoder);
  AUDIO_CODEC_FUNC(Close)(&callback);
  cosmeron_audio_vendor_ma_decoder_uninit(&reference);
  free(bytes);
  return 0;
}
