/*
 * Opt-in physical audio QA. Compiled during CI but NEVER RUN AUTOMATICALLY.
 *
 * Run manually on FreeBSD/DragonFly/NetBSD/OpenBSD with an accessible
 * playback device: make -C Codespace/Tests/Audio hardware-test
 *
 * Audible 440 Hz low-volume square tone, ~0.5 sec, no -lm or -pthread.
 */
#include "../../Cosmeron/Modules/Audio/Device/Manual.h"
#include <stdio.h>
typedef struct {
  uint32_t phase;
} TSound;
static void Tone(void *argument, float *dst, size_t frames,
                 uint32_t channels) {
  TSound *sound = (TSound *)argument;
  size_t frame;
  for (frame = 0; frame < frames; ++frame) {
    uint32_t channel;
    float sample = sound->phase < 24000u ? 0.06f : -0.06f;
    for (channel = 0; channel < channels; ++channel)
      dst[frame * channels + channel] = sample;
    sound->phase += 440u;
    if (sound->phase >= 48000u) sound->phase -= 48000u;
  }
}
int main(int argc, char **argv) {
  AUDIO_DEVICE_TYPE(TManual) device = {0};
  TSound sound = {0};
  AUDIO_DEVICE_TYPE(TManualConfig) config = {
      48000, 2, Tone, &sound, argc > 1 ? argv[1] : NULL
  };
  OPSTATUS result = AUDIO_DEVICE_FUNC(Manual_Open)(&device, &config);
  if (result != STATUS_CONST(SUCCESS)) {
    fprintf(stderr, "Manual_Open failed: %d (device/permissions/backend)\n",
            (int)result);
    return 1;
  }
  result = AUDIO_DEVICE_FUNC(Manual_Start)(&device);
  if (result == STATUS_CONST(SUCCESS))
    result = AUDIO_DEVICE_FUNC(Manual_Pump)(&device, 24000);
  if (result == STATUS_CONST(SUCCESS))
    result = AUDIO_DEVICE_FUNC(Manual_Stop)(&device);
  AUDIO_DEVICE_FUNC(Manual_Close)(&device);
  if (result != STATUS_CONST(SUCCESS)) {
    fprintf(stderr, "Manual playback failed: %d\n", (int)result);
    return 2;
  }
  puts("Manual audio playback request completed. Confirm sound was audible.");
  return 0;
}
