#include "../../Cosmeron/Modules/Audio/Codec/Impl/FLACFrame.inc"
#include <assert.h>
#include <stdio.h>
#include <string.h>

int main(void) {
  unsigned char bytes[20480];
  AUDIO_INS(TFLACStreamInfo) meta;
  FILE *file = fopen("Fixtures/Tone.flac", "rb");
  float *pcm;
  size_t total, first, at;
  uint32_t seed = UINT32_C(0x5a17c0de);
  unsigned int i;
  assert(file != NULL);
  total = fread(bytes, 1, sizeof bytes, file);
  assert(fclose(file) == 0);
  assert(total > 128 && total < sizeof bytes);
  assert(AUDIO_INS(FLAC_ParseMetadata)(bytes, total, &meta) == STATUS_CONST(SUCCESS));
  assert(meta.frameOffset < total);
  pcm = (float *)malloc((size_t)meta.maximumBlockSize * meta.channels * sizeof(float));
  assert(pcm != NULL);
  first = meta.frameOffset;
  for (at = first; at < total; at += 17) {
    size_t frames = 0, next = 0;
    OPSTATUS status;
    unsigned char original = bytes[at];
    bytes[at] ^= 0x80;
    status = AUDIO_INS(FLAC_DecodeFrame)(bytes, total, &meta,
        first, pcm, meta.maximumBlockSize, &frames, &next);
    assert(status != STATUS_CONST(OUT_OF_MEMORY));
    bytes[at] = original;
  }
  for (i = 0; i < 512; ++i) {
    size_t frames = 0, next = 0, where;
    unsigned char original;
    OPSTATUS status;
    seed ^= seed << 13; seed ^= seed >> 17; seed ^= seed << 5;
    where = first + seed % (total - first);
    original = bytes[where];
    bytes[where] ^= (unsigned char)(seed >> 24);
    status = AUDIO_INS(FLAC_DecodeFrame)(bytes, total, &meta,
        first, pcm, meta.maximumBlockSize, &frames, &next);
    assert(status == STATUS_CONST(SUCCESS) ||
           status == STATUS_CONST(INVALID_SEQUENCE) ||
           status == STATUS_CONST(INCOMPLETE_SEQUENCE));
    bytes[where] = original;
  }
  {
    AUDIO_INS(TFLACBits) bits = {bytes, total, total * 8 - 1};
    uint64_t v;
    assert(!AUDIO_INS(FLAC_ReadBits)(&bits, 9, &v));
  }
  free(pcm);
  return 0;
}
