#define AUDIO_DEVICE_MINIAUDIO_FALLBACK 0
#include "../../Cosmeron/Modules/Audio/Device/Device.h"
#include <assert.h>
static void render(void *user, float *out, size_t frames, uint32_t channels) {
  AUDIO_DEVICE_TYPE(TDevice) *device=(AUDIO_DEVICE_TYPE(TDevice) *)user;
  size_t i;
  for(i=0;i<frames*channels;++i) out[i]=(i%2) ? 2.0f : -2.0f;
  /* Reentrant teardown requests stop, does not join/free this callback. */
  assert(AUDIO_DEVICE_FUNC(Stop)(device)==STATUS_CONST(BUSY));
  AUDIO_DEVICE_FUNC(Close)(device);
  assert(device->internal);
}
static OPSTATUS listed(void *user, const char *id, const char *name) {
  size_t *count=(size_t *)user;
  assert(id && *id && name && *name); ++*count;
  return STATUS_CONST(SUCCESS);
}
int main(void) {
  size_t count=0;
  AUDIO_DEVICE_TYPE(TDevice) device={0};
  AUDIO_DEVICE_TYPE(TConfig) config={48000,2,render,&device};
  uint32_t rate, channels;
  assert(AUDIO_DEVICE_FUNC(Open)(NULL,&config)==STATUS_CONST(INVALID_ARGUMENT));
  assert(AUDIO_DEVICE_FUNC(Start)(&device)==STATUS_CONST(INVALID_ARGUMENT));
  assert(AUDIO_DEVICE_FUNC(OpenNamed)(&device,&config,"cosmeron-does-not-exist")==STATUS_CONST(NOT_AVAILABLE));
  assert(!device.internal);
  assert(AUDIO_DEVICE_FUNC(Enumerate)(listed,&count)==STATUS_CONST(SUCCESS));
  assert(count);
#if defined(__linux__)
  assert(AUDIO_DEVICE_FUNC(OpenNamed)(&device,&config,"null")==STATUS_CONST(SUCCESS));
  assert(AUDIO_DEVICE_FUNC(GetFormat)(&device,&rate,&channels)==STATUS_CONST(SUCCESS));
  assert(rate==48000 && channels==2);
  assert(AUDIO_DEVICE_FUNC(Start)(&device)==STATUS_CONST(SUCCESS));
  /* Stop joins even if the worker requested close from its own callback. */
  (void)poll(NULL,0,30);
  assert(AUDIO_DEVICE_FUNC(Stop)(&device)==STATUS_CONST(SUCCESS));
  assert(!AUDIO_DEVICE_FUNC(IsStarted)(&device));
  assert(AUDIO_DEVICE_FUNC(GetStatus)(&device)==STATUS_CONST(SUCCESS));
  assert(AUDIO_DEVICE_FUNC(Start)(&device)==STATUS_CONST(SUCCESS));
  (void)poll(NULL,0,30);
  AUDIO_DEVICE_FUNC(Close)(&device);
  assert(!device.internal);
#else
  (void)rate; (void)channels;
#endif
  AUDIO_DEVICE_FUNC(Close)(&device);
  return 0;
}
