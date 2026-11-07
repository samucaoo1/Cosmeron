#define AUDIO_NO_DESKTOP
#include "../../Cosmeron/Modules/Audio/Audio.h"
#include <stdio.h>
/* Synthesizes mono signed 16-bit little-endian PCM to stdout.
 * Replace fwrite with your DAC/I2S driver on an embedded target. */
int main(void) {
  AUDIO_OSCILLATOR_TYPE(TOscillator) oscillator;
  float samples[AUDIO_CONST(RENDER_BLOCK_FRAMES)];
  int16_t pcm[AUDIO_CONST(RENDER_BLOCK_FRAMES)];
  size_t offset = 0;
  if (AUDIO_OSCILLATOR_FUNC(Init)(&oscillator, AUDIO_CONST(DEFAULT_SAMPLE_RATE),
                                  AUDIO_OSCILLATOR_CONST(SINE),
                                  440) != STATUS_CONST(SUCCESS))
    return 1;
  while (offset < AUDIO_CONST(DEFAULT_SAMPLE_RATE)) {
    size_t i, count = AUDIO_CONST(DEFAULT_SAMPLE_RATE) - offset;
    if (count > AUDIO_CONST(RENDER_BLOCK_FRAMES))
      count = AUDIO_CONST(RENDER_BLOCK_FRAMES);
    if (AUDIO_OSCILLATOR_FUNC(Render)(&oscillator, samples, count) !=
        STATUS_CONST(SUCCESS))
      return 1;
    if (AUDIO_PCM_FUNC(ToS16)(samples, count, pcm) != STATUS_CONST(SUCCESS))
      return 1;
    for (i = 0; i < count; ++i) {
      unsigned char bytes[2];
      uint16_t sample = (uint16_t)pcm[i];
      bytes[0] = (unsigned char)sample;
      bytes[1] = (unsigned char)(sample >> 8);
      if (fwrite(bytes, 1, 2, stdout) != 2)
        return 1;
    }
    offset += count;
  }
  return 0;
}
