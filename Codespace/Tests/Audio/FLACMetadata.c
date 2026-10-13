#include "../../Cosmeron/Modules/Audio/Codec/Impl/FLACStream.inc"
#include <assert.h>
#include <stdio.h>

static void write64(unsigned char *data, uint64_t value) {
  unsigned int i;
  for (i = 0; i < 8; ++i)
    data[7 - i] = (unsigned char)(value >> (i * 8));
}
int main(void) {
  AUDIO_INS(TFLACStreamInfo) info = {0};
  unsigned char metadata[60] = {0};
  unsigned char original[60];
  FILE *file = fopen("Fixtures/Tone.flac", "rb");
  unsigned char *content;
  long length;
  assert(file != NULL && !fseek(file, 0, SEEK_END));
  length = ftell(file);
  assert(length >= 42 && !fseek(file, 0, SEEK_SET));
  content = (unsigned char *)malloc((size_t)length);
  assert(content != NULL);
  assert(fread(content, 1, (size_t)length, file) == (size_t)length);
  assert(fclose(file) == 0);
  assert(AUDIO_INS(FLAC_ParseMetadata)(content, (size_t)length, &info) ==
      STATUS_CONST(SUCCESS));
  assert(info.sampleRate == 44100 && info.channels == 2 &&
         info.bitsPerSample >= 4 && info.bitsPerSample <= 32);
  assert(info.frameOffset < (size_t)length);
  free(content);

  memcpy(metadata, "fLaC", 4);
  metadata[4] = 0x80;
  metadata[7] = 34;
  metadata[8] = 0;
  metadata[9] = 16;
  metadata[10] = 4;
  metadata[11] = 0;
  write64(metadata + 18, ((uint64_t)8000 << 44) |
      ((uint64_t)15 << 36) | UINT64_C(1440));
  assert(AUDIO_INS(FLAC_ParseMetadata)(metadata, 42, &info) ==
      STATUS_CONST(SUCCESS));
  assert(info.sampleRate == 8000 && info.channels == 1 &&
         info.bitsPerSample == 16 && info.totalSamples == 1440 &&
         info.frameOffset == 42 && info.minimumBlockSize == 16);
  memcpy(original, metadata, sizeof metadata);
  metadata[7] = 35;
  assert(AUDIO_INS(FLAC_ParseMetadata)(metadata, 42, &info) ==
      STATUS_CONST(INCOMPLETE_SEQUENCE));
  memcpy(metadata, original, sizeof metadata);
  metadata[4] = 0;
  metadata[42] = 0x84;
  metadata[45] = 3;
  metadata[46] = 0x11;
  metadata[47] = 0x22;
  metadata[48] = 0x33;
  assert(AUDIO_INS(FLAC_ParseMetadata)(metadata, 49, &info) ==
      STATUS_CONST(SUCCESS) && info.frameOffset == 49);
  metadata[46] = 0x77;
  assert(AUDIO_INS(FLAC_ParseMetadata)(metadata, 48, &info) ==
      STATUS_CONST(INCOMPLETE_SEQUENCE));
  memcpy(metadata, original, sizeof metadata);
  metadata[0] = 'X';
  assert(AUDIO_INS(FLAC_ParseMetadata)(metadata, 42, &info) ==
      STATUS_CONST(NOT_SUPPORTED));
  return 0;
}
