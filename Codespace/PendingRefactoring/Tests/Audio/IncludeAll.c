#define COSMERON_NAMESPACE Lab
#define COSMERON_NAMESPACE_CONST LAB
#include "../../Cosmeron/Modules/Audio/Audio.h"
#include <assert.h>
int main(void) {
  Lab_Audio_Oscillator_TOscillator oscillator;
  assert(Lab_Audio_Oscillator_Init(&oscillator, LAB_AUDIO_DEFAULT_SAMPLE_RATE,
                                   LAB_AUDIO_OSCILLATOR_SINE,
                                   440) == STATUS_CONST(SUCCESS));
  return 0;
}
