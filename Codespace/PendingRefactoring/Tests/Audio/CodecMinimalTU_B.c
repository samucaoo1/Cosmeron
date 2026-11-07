#include "../../Cosmeron/Modules/Audio/CodecMinimal.h"
/* Second translation unit has its own static vendor math bootstrap. */
int CodecMinimalTU_Math(void) {
  if (!AUDIO_INS(PrepareMath)()) return 1;
  if (AUDIO_INS(Sqrt)(9.0) != 3.0) return 2;
  if (AUDIO_INS(Pow)(3.0, 2.0) != 9.0) return 3;
  if (AUDIO_INS(Log)(1.0) != 0.0) return 4;
  return 0;
}
