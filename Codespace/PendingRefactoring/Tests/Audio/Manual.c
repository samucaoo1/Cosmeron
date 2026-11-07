#include "../../Cosmeron/Modules/Audio/Device/Manual.h"
#include <assert.h>
static void silent(void *user, float *dst, size_t frames, uint32_t ch) {
  size_t i;
  (void)user;
  for (i = 0; i < frames * ch; ++i) dst[i] = 0.0f;
}
int main(void) {
  AUDIO_DEVICE_TYPE(TManual) device = {0};
  AUDIO_DEVICE_TYPE(TManualConfig) cfg = {48000, 2, silent, NULL, NULL};
  const float src[] = {-1.f, 0.f, 1.f, 0.5f};
  unsigned char pcm[sizeof(src) * 2] = {0};
  assert(AUDIO_DEVICE_FUNC(Manual_EncodeS16LE)(src, 4, pcm) ==
         STATUS_CONST(SUCCESS));
  assert(pcm[0] == 0 && pcm[1] == 0x80);
  assert(pcm[2] == 0 && pcm[3] == 0);
  assert(pcm[4] == 0xff && pcm[5] == 0x7f);
  assert(pcm[6] == 0xff && pcm[7] == 0x3f);
  assert(AUDIO_DEVICE_FUNC(Manual_Start)(&device) ==
         STATUS_CONST(INVALID_ARGUMENT));
  assert(!AUDIO_DEVICE_FUNC(Manual_IsStarted)(&device));
  AUDIO_DEVICE_FUNC(Manual_Close)(&device);
  (void)cfg;
  return 0;
}
