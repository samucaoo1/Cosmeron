#include "../../Cosmeron/Modules/Audio/CodecMinimal.h"
#include <assert.h>
#include <stdio.h>
/* An all-zero encoded right channel must remain silent without MS/intensity.
 * Some reference decoders mishandle the nonstandard MPEG-2.5 8-kHz mixed
 * layout; verify native left PCM has signal and right channel is silent. */
int main(int argc, char **argv) {
  unsigned char bytes[65536];
  float native[128 * 2];
  AUDIO_CODEC_TYPE(TDecoder) decoder = {0};
  AUDIO_CODEC_TYPE(TInfo) info;
  FILE *file;
  size_t size, got = 0, i;
  double xx = 0;
  assert(argc == 2);
  file = fopen(argv[1], "rb");
  assert(file);
  size = fread(bytes, 1, sizeof bytes, file);
  fclose(file);
  assert(AUDIO_CODEC_FUNC(OpenMemory)(&decoder, bytes, size) == STATUS_CONST(SUCCESS));
  assert(AUDIO_CODEC_FUNC(GetInfo)(&decoder, &info) == STATUS_CONST(SUCCESS));
  assert(info.channels == 2 && info.sampleRate == 8000);
  for (;;) {
    assert(AUDIO_CODEC_FUNC(ReadFrames)(&decoder, native, 128, &got) == STATUS_CONST(SUCCESS));
    if (!got) break;
    for (i = 0; i < got; ++i) {
      double a = native[2 * i];
      assert(native[2 * i + 1] == 0);
      xx += a * a;
    }
  }
  assert(xx > 0);
  printf("[8-kHz mixed baseline] native right energy=0; left energy=%.9g\n", xx);
  AUDIO_CODEC_FUNC(Close)(&decoder);
  return 0;
}
