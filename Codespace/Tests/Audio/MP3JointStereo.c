#include "../../Cosmeron/Modules/Audio/Codec/MP3.h"
#include <assert.h>
#include <string.h>

static bool near(float a, float b) {
  const float d = a - b;
  return d > -0.00005f && d < 0.00005f;
}
static void longBlocks(void) {
  AUDIO_CODEC_TYPE(TMP3FrameInfo) hdr = {0};
  AUDIO_CODEC_TYPE(TMP3Granule) part = {0};
  AUDIO_CODEC_TYPE(TMP3Scalefactors) scales = {0};
  float left[576] = {0}, right[576] = {0}, l[576], r[576];
  size_t i;
  hdr.mpegVersion = 1; hdr.channels = 2; hdr.sampleRate = 44100;
  hdr.intensityStereo = true;
  scales.longBands[12] = 3;
  scales.longBands[20] = 5; /* inherited by untransmitted band 21 */
  left[13] = 2; right[13] = 4;
  left[74] = 8; /* band 12, position 3 => 4 / 4 */
  left[419] = 8; /* band 21 inherits position 5 */
  assert(AUDIO_CODEC_FUNC(MP3_JointStereo)(&hdr, &part, &scales,
      left, right, l, r) == STATUS_CONST(SUCCESS));
  assert(l[13] == 2 && r[13] == 4);
  assert(near(l[74],4) && near(r[74],4));
  assert(near(l[419],8*0.7886751346f));
  assert(near(r[419],8*0.2113248654f));
  hdr.msStereo = true;
  assert(AUDIO_CODEC_FUNC(MP3_JointStereo)(&hdr, &part, &scales,
      left, right, l, r) == STATUS_CONST(SUCCESS));
  assert(near(l[13],6*0.70710678118f));
  assert(near(r[13],-2*0.70710678118f));
  assert(near(l[74],4*1.41421356237f));
  assert(near(r[74],4*1.41421356237f));
  assert(near(l[419],8*0.7886751346f*1.41421356237f));

  /* An intensity position of 7 marks a normal/MS frequency band. */
  scales.longBands[12] = 7;
  assert(AUDIO_CODEC_FUNC(MP3_JointStereo)(&hdr, &part, &scales,
      left, right, l, r) == STATUS_CONST(SUCCESS));
  assert(near(l[74],8*0.70710678118f));
  assert(near(r[74],8*0.70710678118f));
  scales.longBands[12] = 3;

  /* In-place buffers must preserve both channels. */
  memcpy(l, left, sizeof l);
  memcpy(r, right, sizeof r);
  assert(AUDIO_CODEC_FUNC(MP3_JointStereo)(&hdr, &part, &scales,
      l, r, l, r) == STATUS_CONST(SUCCESS));
  assert(near(l[74],4*1.41421356237f));

  /* Preserve both outputs atomically after a rejected input. */
  right[74] = 9.0f;
  l[0] = 1234.0f;
  assert(AUDIO_CODEC_FUNC(MP3_JointStereo)(&hdr, &part, &scales,
      left, right, l, r) == STATUS_CONST(SUCCESS));
  /* Right coefficients in this band make it normal/MS, not intensity. */
  assert(near(l[74],(8+9)*0.70710678118f));
  right[74] = 0;
  hdr.sampleRate = 24000;
  l[0] = 1234.0f;
  assert(AUDIO_CODEC_FUNC(MP3_JointStereo)(&hdr, &part, &scales,
      left, right, l, r) == STATUS_CONST(NOT_SUPPORTED));
  assert(l[0] == 1234.0f);
  hdr.sampleRate = 44100;
  assert(AUDIO_CODEC_FUNC(MP3_JointStereo)(&hdr, &part, &scales,
      left, right, l, l) == STATUS_CONST(INVALID_ARGUMENT));
  for (i = 0; i < 576; ++i)
    assert(left[i] == (i == 13 ? 2.f : i == 74 || i == 419 ? 8.f : 0.f));
}
static void shortAndMixed(void) {
  AUDIO_CODEC_TYPE(TMP3FrameInfo) hdr = {0};
  AUDIO_CODEC_TYPE(TMP3Granule) part = {0};
  AUDIO_CODEC_TYPE(TMP3Scalefactors) scale = {0};
  float left[576] = {0}, right[576] = {0}, l[576], r[576];
  hdr.mpegVersion = 1; hdr.channels = 2; hdr.sampleRate = 44100;
  hdr.intensityStereo = true;
  part.windowSwitching = 1; part.blockType = 2;
  /* sfb4 begins at 3*16=48 and has length 6 per window; encode order:
   * window0 [48..53], window1 [54..59], window2 [60..65]. */
  left[48] = 6; left[54] = 6; left[60] = 6;
  scale.shortBands[4][0] = 0;
  scale.shortBands[4][1] = 3;
  scale.shortBands[4][2] = 6;
  /* Right transmission in window 1 of band 5 must disable intensity
   * in window 1 for band 4, but not in the two other windows. */
  right[75] = 3; /* sfb5: offset 66, window1 inside [74..81] */
  assert(AUDIO_CODEC_FUNC(MP3_JointStereo)(&hdr, &part, &scale,
      left,right,l,r) == STATUS_CONST(SUCCESS));
  assert(near(l[48],0) && near(r[48],6));
  assert(near(l[54],6) && near(r[54],0)); /* ordinary raw L/R */
  assert(near(l[60],6) && near(r[60],0));
  hdr.msStereo = true;
  assert(AUDIO_CODEC_FUNC(MP3_JointStereo)(&hdr, &part, &scale,
      left,right,l,r) == STATUS_CONST(SUCCESS));
  assert(near(l[48],0) && near(r[48],6*1.41421356237f));
  assert(near(l[54],6*0.70710678118f));
  assert(near(r[54],6*0.70710678118f));
  assert(near(l[60],6*1.41421356237f));
  /* Last sfb 12 inherits the position of band 11 if no higher right
   * information was transmitted in that specific window. */
  scale.shortBands[11][2] = 3;
  left[575] = 10; /* last short band, window 2 */
  assert(AUDIO_CODEC_FUNC(MP3_JointStereo)(&hdr, &part, &scale,
      left,right,l,r) == STATUS_CONST(SUCCESS));
  assert(near(l[575],5*1.41421356237f));
  assert(near(r[575],5*1.41421356237f));

  /* Mixed blocks preserve first 36 long lines, short coding starts at
   * scalefactor band 3, with the first short band at coefficient 36. */
  memset(left, 0, sizeof left);
  memset(right, 0, sizeof right);
  memset(&scale, 0, sizeof scale);
  part.mixedBlock = 1;
  left[0] = 5; right[0] = 2; /* transmitted long band 0 */
  left[36] = 8;
  scale.shortBands[3][0] = 3;
  assert(AUDIO_CODEC_FUNC(MP3_JointStereo)(&hdr, &part, &scale,
      left,right,l,r) == STATUS_CONST(SUCCESS));
  assert(near(l[0],7*0.70710678118f));
  assert(near(r[0],3*0.70710678118f));
  assert(near(l[36],4*1.41421356237f));
  assert(near(r[36],4*1.41421356237f));
}
int main(void) {
  longBlocks();
  shortAndMixed();
  return 0;
}
