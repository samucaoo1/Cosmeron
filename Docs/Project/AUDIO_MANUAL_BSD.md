# Audio manual playback: native BSD backends

**Entry point:** `Cosmeron/Modules/Audio/Device/Manual.h`.

Unlike the Miniaudio Device backend, this API has no worker thread:
`Manual_Open` configures the device, `Manual_Start` enables the state,
`Manual_Pump` renders and writes **blocking** signed 16-bit little-endian
PCM, and `Manual_Stop` / `Manual_Close` release it.

- FreeBSD and DragonFly BSD: OSS `/dev/dsp` and SNDCTL_DSP_* ioctls
- NetBSD: native `/dev/sound` and `AUDIO_SETINFO` / `AUDIO_GETINFO`
- OpenBSD: sndio via a **runtime-loaded** `libsndio.so` and its documented
  `sio_*` API. No `-lsndio` is used, but the library/server is required
  **at runtime**. If it is missing, `Manual_Open` returns `NOT_AVAILABLE`.
- Linux/macOS/Windows: returns `NOT_SUPPORTED`; Miniaudio is unaffected.

No `-lm`/`-pthread`/`-lsndio` or Miniaudio headers are required to
compile and link this module. `sndio.h` is required on OpenBSD. The backend uses the selected OS's libc and driver;
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

## Simulated PCM sink and end-to-end tests

`Manual_OpenSink(device, config, writeSink, userData)` opens a manual stream
without touching audio hardware on any operating system. The sink callback
receives signed 16-bit little-endian PCM and reports the number of accepted
bytes. Partial writes are supported, but returning zero bytes with a success
status is treated as an I/O error. Its buffer may not be retained after the
callback returns; the caller owns synchronization and the sink context.

`Codespace/Tests/Audio/ManualSink.c` calls the actual `Manual_Pump` function
for 513 stereo frames, forces partial 37-byte writes, checks every output
sample, and verifies lifecycle and error cases. Passing this suite does not
certify any physical audio device.

## Manual hardware acceptance test (not run by CI)

The optional `ManualHardware.c` builds under strict C11 as part of the
Audio test suite. It is **not automatically executed** by CI. On a BSD
host with a reachable audio device, run:

```sh
make -C Codespace/Tests/Audio hardware-test CC=cc
# Or explicitly choose the device with:
Codespace/Tests/Audio/.build/ManualHardware /dev/dsp
```

It requests a low-volume 440 Hz tone for half a second, checks the OS
calls, closes resources and asks the person running it to confirm audio was
heard without underruns. This is not appropriate for unattended CI runners
without speaker/permissions. OpenBSD's optional argument denotes a sndio
server/device name rather than a /dev/dsp path.
