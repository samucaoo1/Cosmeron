#include "../../Cosmeron/Modules/Audio/Offline.h"
OPSTATUS OfflineTU_Render(float *out, size_t count) {
  AUDIO_OSCILLATOR_TYPE(TOscillator) osc;
  OPSTATUS result = AUDIO_OSCILLATOR_FUNC(Init)(&osc, 48000,
    AUDIO_OSCILLATOR_CONST(SQUARE), 440.0);
  if (result != STATUS_CONST(SUCCESS)) return result;
  return AUDIO_OSCILLATOR_FUNC(Render)(&osc, out, count);
}
