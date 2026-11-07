#include "../../Cosmeron/Modules/Audio/CodecOnly.h"
#include <assert.h>

/* One frame of signed 16-bit PCM WAV, 8 kHz mono. Use a real decoder call
 * so linker probes cannot accidentally pass when the vendor is dead-stripped.
 * This test never initializes an audio device. */
static const unsigned char wav[] = {
  'R','I','F','F', 38,0,0,0, 'W','A','V','E',
  'f','m','t',' ', 16,0,0,0, 1,0, 1,0,
  0x40,0x1f,0,0, 0x80,0x3e,0,0, 2,0, 16,0,
  'd','a','t','a', 2,0,0,0, 0,0
};
int main(void) {
  AUDIO_CODEC_TYPE(TDecoder) decoder = {0};
  AUDIO_CODEC_TYPE(TInfo) info = {0};
  AUDIO_PCM_TYPE(TBuffer) tone = {0};
  float sample = 0.25f;
  size_t frames = 0;
  assert(AUDIO_CODEC_FUNC(OpenMemory)(&decoder, wav, sizeof wav) ==
         STATUS_CONST(SUCCESS));
  assert(AUDIO_CODEC_FUNC(GetInfo)(&decoder, &info) ==
         STATUS_CONST(SUCCESS));
  assert(info.sampleRate == 8000 && info.channels == 1);
  assert(AUDIO_CODEC_FUNC(ReadFrames)(&decoder, &sample, 1, &frames) ==
         STATUS_CONST(SUCCESS));
  assert(frames == 1 && sample == 0.0f);
  AUDIO_CODEC_FUNC(Close)(&decoder);
  assert(AUDIO_BEEP_FUNC(RenderTone)(440, 5, 0.2f, 48000, &tone) ==
         STATUS_CONST(SUCCESS));
  assert(tone.frameCount > 0);
  AUDIO_PCM_FUNC(Destroy)(&tone);
  return 0;
}
