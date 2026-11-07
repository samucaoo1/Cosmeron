#include "../../Cosmeron/Modules/Audio/CodecMinimal.h"
#include "../../Cosmeron/Modules/Audio/Codec/MP3Memory.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

/* Compare final presentation-aligned output between native memory
 * and native streaming MP3 decoders. */
int main(int argc, char **argv) {
  unsigned char *bytes;
  float *nativePCM, *referencePCM;
  AUDIO_CODEC_TYPE(TMP3Memory) native = {0};
  AUDIO_CODEC_TYPE(TDecoder) reference = {0};
  AUDIO_CODEC_TYPE(TInfo) refInfo = {0};
  uint32_t rate = 0, channels = 0;
  uint64_t frames = 0;
  size_t n, count, got = 0, completed = 0, i, len;
  double xx = 0, xy = 0, yy = 0, residual, gain;
  FILE *file;
  long length;
  assert(argc == 2);
  file = fopen(argv[1], "rb");
  assert(file && fseek(file, 0, SEEK_END) == 0);
  length = ftell(file);
  assert(length > 0 && length < 2000000 && fseek(file, 0, SEEK_SET) == 0);
  len = (size_t)length;
  bytes = (unsigned char *)malloc(len);
  assert(bytes && fread(bytes, 1, len, file) == len && fclose(file) == 0);
  assert(AUDIO_CODEC_FUNC(MP3_MemoryOpen)(&native, bytes, len) ==
         STATUS_CONST(SUCCESS));
  assert(AUDIO_CODEC_FUNC(MP3_MemoryInfo)(&native, &rate, &channels, &frames) ==
         STATUS_CONST(SUCCESS));
  assert(channels == 1 || channels == 2);
  assert(frames > 0 && frames < 1000000);
  assert(frames <= SIZE_MAX / channels / sizeof(float));
  count = (size_t)frames * channels;
  nativePCM = (float *)malloc(count * sizeof(float));
  referencePCM = (float *)malloc(count * sizeof(float));
  assert(nativePCM && referencePCM);
  while (completed < frames) {
    n = frames - completed > 127 ? 127 : (size_t)(frames - completed);
    assert(AUDIO_CODEC_FUNC(MP3_MemoryRead)(
               &native, nativePCM + completed * channels, n,
               &got) == STATUS_CONST(SUCCESS));
    assert(got == n);
    completed += n;
  }
  assert(AUDIO_CODEC_FUNC(OpenMemory)(&reference, bytes, len) ==
         STATUS_CONST(SUCCESS));
  assert(AUDIO_CODEC_FUNC(GetInfo)(&reference, &refInfo) ==
         STATUS_CONST(SUCCESS));
  assert(refInfo.channels == channels && refInfo.sampleRate == rate);
  completed = 0;
  while (completed < frames) {
    n = frames - completed > 127 ? 127 : (size_t)(frames - completed);
    assert(AUDIO_CODEC_FUNC(ReadFrames)(
               &reference, referencePCM + completed * channels, n,
               &got) == STATUS_CONST(SUCCESS));
    assert(got == n);
    completed += n;
  }
  for (i = 0; i < count; ++i) {
    const double a = nativePCM[i], b = referencePCM[i];
    xx += a * a;
    xy += a * b;
    yy += b * b;
  }
  assert(xx > 1e-8 && yy > 1e-8);
  gain = xy / xx;
  residual = 1.0 - xy * xy / (xx * yy);
  printf("[native mp3 memory parity] %s rate=%u channels=%u "
         "frames=%llu gain=%.9g residual=%.9g\n",
         argv[1], rate, channels, (unsigned long long)frames, gain, residual);
  fflush(stdout);
  assert(gain > 0.999 && gain < 1.001);
  assert(residual >= -0.0000001 && residual < 0.00001);
  AUDIO_CODEC_FUNC(Close)(&reference);
  AUDIO_CODEC_FUNC(MP3_MemoryClose)(&native);
  free(nativePCM);
  free(referencePCM);
  free(bytes);
  return 0;
}
