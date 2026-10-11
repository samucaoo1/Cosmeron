#include "../../Cosmeron/Modules/Audio/Codec/MP3.h"
#include <assert.h>
#include <string.h>
static bool near(float actual, float expected) {
  return actual > expected - 0.001f && actual < expected + 0.001f;
}
int main(void) {
  int32_t q[576] = {0};
  float spectrum[576];
  AUDIO_CODEC_TYPE(TMP3Granule) part = {0};
  AUDIO_CODEC_TYPE(TMP3Scalefactors) scale = {0};
  unsigned int i;
  for (i = 0; i < 576; ++i) spectrum[i] = 777.f;
  part.globalGain = 210;
  q[0] = 8;
  q[1] = -27;
  q[4] = 8;
  q[5] = 64;
  scale.longBands[1] = 2; /* -1 octave in scalefac_scale 0 */
  assert(AUDIO_CODEC_FUNC(MP3_Requantize)(q, &part, &scale, 44100,
      spectrum) == STATUS_CONST(SUCCESS));
  assert(near(spectrum[0],16.f) && near(spectrum[1],-81.f));
  assert(near(spectrum[4],8.f) && near(spectrum[5],128.f));
  assert(near(spectrum[10],0.f));
  part.globalGain = 214;
  assert(AUDIO_CODEC_FUNC(MP3_Requantize)(q, &part, &scale, 44100,
      spectrum) == STATUS_CONST(SUCCESS));
  assert(near(spectrum[0],32.f));
  part.globalGain = 210;
  part.scalefacScale = 1;
  assert(AUDIO_CODEC_FUNC(MP3_Requantize)(q, &part, &scale, 44100,
      spectrum) == STATUS_CONST(SUCCESS));
  assert(near(spectrum[4],4.f));

  memset(q,0,sizeof q);
  memset(&scale,0,sizeof scale);
  memset(&part,0,sizeof part);
  part.globalGain = 210;
  part.preflag = 1;
  q[62] = 8; /* long band 11 begins at 62 for 44.1 kHz */
  assert(AUDIO_CODEC_FUNC(MP3_Requantize)(q, &part, &scale, 44100,
      spectrum) == STATUS_CONST(SUCCESS));
  assert(near(spectrum[62],16.f * 0.707106781f));

  memset(q,0,sizeof q);
  memset(&part,0,sizeof part);
  part.globalGain = 210;
  part.windowSwitching = 1;
  part.blockType = 2;
  part.subblockGain[1] = 1;
  scale.shortBands[0][2] = 2;
  q[0] = 8;
  q[4] = 8;
  q[8] = 8;
  assert(AUDIO_CODEC_FUNC(MP3_Requantize)(q, &part, &scale, 44100,
      spectrum) == STATUS_CONST(SUCCESS));
  assert(near(spectrum[0],16.f) && near(spectrum[4],4.f) &&
      near(spectrum[8],8.f));

  /* Mixed block: first 36 long coefficients, then short windows from
   * scalefactor band 3. */
  memset(q,0,sizeof q);
  memset(&scale,0,sizeof scale);
  part.mixedBlock = 1;
  q[0] = 8;
  q[36] = 8;
  q[40] = 8;
  q[44] = 8;
  scale.longBands[0] = 2;
  scale.shortBands[3][2] = 2;
  assert(AUDIO_CODEC_FUNC(MP3_Requantize)(q, &part, &scale, 44100,
      spectrum) == STATUS_CONST(SUCCESS));
  assert(near(spectrum[0],8.f) && near(spectrum[36],16.f) &&
      near(spectrum[40],4.f) && near(spectrum[44],8.f));

  /* Invalid parameters must not publish partially computed results. */
  spectrum[0] = 789.f;
  q[575] = 8207;
  assert(AUDIO_CODEC_FUNC(MP3_Requantize)(q, &part, &scale, 44100,
      spectrum) == STATUS_CONST(OUT_OF_RANGE));
  assert(spectrum[0] == 789.f);
  q[575] = 0;
  assert(AUDIO_CODEC_FUNC(MP3_Requantize)(q, &part, &scale, 22050,
      spectrum) == STATUS_CONST(NOT_SUPPORTED));
  assert(spectrum[0] == 789.f);
  part.scalefacScale = 2;
  assert(AUDIO_CODEC_FUNC(MP3_Requantize)(q, &part, &scale, 44100,
      spectrum) == STATUS_CONST(INVALID_ARGUMENT));
  return 0;
}
