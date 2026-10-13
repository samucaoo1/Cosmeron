#define AUDIO_DEVICE_MINIAUDIO_FALLBACK 0
#include "../../Cosmeron/Modules/Audio/Device/Device.h"
#include <assert.h>
void CrossTU_Render(void *,float *,size_t,uint32_t);
size_t CrossTU_Count(void);
int main(void) {
#if defined(__linux__)
  AUDIO_DEVICE_TYPE(TDevice) device={0};
  AUDIO_DEVICE_TYPE(TConfig) config={48000,2,CrossTU_Render,&device};
  OPSTATUS status = AUDIO_DEVICE_FUNC(OpenNamed)(&device,&config,"null");
  if (status == STATUS_CONST(NOT_AVAILABLE)) return 0;
  assert(status == STATUS_CONST(SUCCESS));
  assert(AUDIO_DEVICE_FUNC(Start)(&device)==STATUS_CONST(SUCCESS));
  (void)poll(NULL,0,30);
  assert(AUDIO_DEVICE_FUNC(Stop)(&device)==STATUS_CONST(SUCCESS));
  assert(CrossTU_Count());
  AUDIO_DEVICE_FUNC(Close)(&device);
  assert(!device.internal);
#endif
  return 0;
}
