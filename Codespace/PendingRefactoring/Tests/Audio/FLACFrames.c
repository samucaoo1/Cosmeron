#include "../../Cosmeron/Modules/Audio/Codec/Impl/FLACFrame.inc"
#include <assert.h>
#include <stdio.h>

int main(void) {
  const char *path = "Fixtures/Tone.flac";
  AUDIO_INS(TFLACStreamInfo) meta = {0};
  FILE *file = fopen(path, "rb");
  unsigned char *encoded;
  float *pcm;
  size_t size, offset, produced = 0;
  long n;
  double energy = 0;
  assert(file && !fseek(file, 0, SEEK_END));
  n = ftell(file);
  assert(n > 42 && !fseek(file, 0, SEEK_SET));
  size = (size_t)n;
  encoded = (unsigned char *)malloc(size);
  assert(encoded && fread(encoded, 1, size, file) == size);
  assert(fclose(file) == 0);
  assert(AUDIO_INS(FLAC_ParseMetadata)(encoded, size, &meta) == STATUS_CONST(SUCCESS));
  assert(meta.channels == 2 && meta.sampleRate == 44100);
  pcm = (float *)malloc((size_t)meta.maximumBlockSize * meta.channels * sizeof(float));
  assert(pcm);
  offset = meta.frameOffset;
  while (offset < size && (!meta.totalSamples || produced < meta.totalSamples)) {
    size_t frames = 0, next = 0, i;
    assert(AUDIO_INS(FLAC_DecodeFrame)(encoded, size, &meta, offset, pcm,
        meta.maximumBlockSize, &frames, &next) == STATUS_CONST(SUCCESS));
    assert(frames && next > offset && next <= size);
    for (i = 0; i < frames * meta.channels; ++i) {
      float sample = pcm[i];
      assert(sample >= -1.0f && sample < 1.0f);
      energy += sample >= 0 ? sample : -sample;
    }
    produced += frames;
    offset = next;
  }
  assert(produced == meta.totalSamples && produced > 1000 && energy > 10.0);
  assert(offset == size);
  {
    size_t frames = 0, next = 0;
    unsigned char byte = encoded[meta.frameOffset];
    encoded[meta.frameOffset] ^= 1;
    assert(AUDIO_INS(FLAC_DecodeFrame)(encoded, size, &meta, meta.frameOffset,
        pcm, meta.maximumBlockSize, &frames, &next) != STATUS_CONST(SUCCESS));
    encoded[meta.frameOffset] = byte;
    assert(AUDIO_INS(FLAC_DecodeFrame)(encoded, meta.frameOffset + 8, &meta,
        meta.frameOffset, pcm, meta.maximumBlockSize, &frames, &next) != STATUS_CONST(SUCCESS));
  }
  free(pcm);
  free(encoded);
  return 0;
}
