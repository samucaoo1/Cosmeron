#define AUDIO_DEVICE_MINIAUDIO_FALLBACK 0
#include "../../Cosmeron/Modules/Audio/Device/Device.h"
#include <assert.h>
static size_t callbacks;
size_t CrossTU_Count(void) { return callbacks; }
void CrossTU_Render(void *user,float *out,size_t frames,uint32_t channels) {
  AUDIO_DEVICE_TYPE(TDevice) *device=(AUDIO_DEVICE_TYPE(TDevice) *)user;
  memset(out,0,frames*channels*sizeof *out);
  ++callbacks;
  assert(AUDIO_DEVICE_FUNC(Start)(device)==STATUS_CONST(BUSY));
  assert(AUDIO_DEVICE_FUNC(Stop)(device)==STATUS_CONST(BUSY));
  AUDIO_DEVICE_FUNC(Close)(device);
  assert(device->internal);
}
