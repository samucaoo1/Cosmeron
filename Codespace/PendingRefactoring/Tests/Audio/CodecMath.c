#include "../../Cosmeron/Modules/Audio/CodecMinimal.h"
/* Exercise all five math symbols resolved by the decoder-only runtime.
 * The test is deliberately linked without -lm/-pthread on BSD. */
int main(void) {
  if (!AUDIO_INS(PrepareMath)()) return 1;
  if (AUDIO_INS(Sin)(0.0) != 0.0) return 2;
  if (AUDIO_INS(Exp)(0.0) != 1.0) return 3;
  if (AUDIO_INS(Log)(1.0) != 0.0) return 4;
  if (AUDIO_INS(Sqrt)(4.0) != 2.0) return 5;
  if (AUDIO_INS(Pow)(2.0, 3.0) != 8.0) return 6;
  return 0;
}
