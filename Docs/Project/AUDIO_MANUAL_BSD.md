# Audio manual playback: native BSD backends

**Entry point:** `Cosmeron/Modules/Audio/Device/Manual.h`.

Unlike the Miniaudio Device backend, this API has no worker thread:
`Manual_Open` configures the device, `Manual_Start` enables the state,
`Manual_Pump` renders and writes **blocking** signed 16-bit little-endian
PCM, and `Manual_Stop` / `Manual_Close` release it.

- FreeBSD and DragonFly BSD: OSS `/dev/dsp` and SNDCTL_DSP_* ioctls
- NetBSD: native `/dev/sound` and `AUDIO_SETINFO` / `AUDIO_GETINFO`
- OpenBSD: separate sndio integration needed; this backend returns
  `NOT_SUPPORTED` instead of silently accessing `/dev/audio`.
- Linux/macOS/Windows: returns `NOT_SUPPORTED`; Miniaudio is unaffected.

No `-lm`/`-pthread` or Miniaudio headers are required merely to compile
and link this module. The backend uses the selected OS's libc and driver;
it is not independent of system runtime services.

```c
#include "Cosmeron/Modules/Audio/Device/Manual.h"

static void render(void *ctx, float *dst, size_t frames, uint32_t channels) {
  /* Fill dst with frames*channels interleaved samples in [-1,1]. */
  (void)ctx;
  memset(dst, 0, frames*channels*sizeof(*dst));
}
int main(void) {
  AUDIO_DEVICE_TYPE(TManual) device = {0};
  AUDIO_DEVICE_TYPE(TManualConfig) config = {
    48000, 2, render, NULL, NULL
  };
  if (AUDIO_DEVICE_FUNC(Manual_Open)(&device, &config) != STATUS_CONST(SUCCESS))
    return 1;
  (void)AUDIO_DEVICE_FUNC(Manual_Start)(&device);
  (void)AUDIO_DEVICE_FUNC(Manual_Pump)(&device, 512);
  (void)AUDIO_DEVICE_FUNC(Manual_Stop)(&device);
  AUDIO_DEVICE_FUNC(Manual_Close)(&device);
  return 0;
}
```

The supplied CI tests **only compilation, integer conversion and linkage**,
without opening a sound device. A real hardware run is required to certify
rate negotiation, scheduling, underruns and permissions. The callback must
be quick and must not assume there is a dedicated audio thread.
