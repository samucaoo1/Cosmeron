#include "../../Cosmeron/Modules/Audio/Codec/MP3.h"
#include <assert.h>
#include <float.h>
#include <string.h>

static bool approx(float a, float b) {
  float error = a-b;
  return error < 0.00005f && error > -0.00005f;
}
static void impulseAndOverlap(void) {
  AUDIO_CODEC_TYPE(TMP3HybridState) state = {0};
  AUDIO_CODEC_TYPE(TMP3Granule) part = {0};
  float input[576] = {0}, result[576], previousOverlap, zeros[576] = {0};
  size_t i;
  input[0] = 1.0f;
  assert(AUDIO_CODEC_FUNC(MP3_IMDCT)(
      input, &part, &state, result) == STATUS_CONST(SUCCESS));
  assert(approx(result[0], 0.0294688310f));
  for (i = 18; i < 576; ++i) assert(result[i] == 0.0f);
  previousOverlap = state.overlap[0][0];
  assert(approx(previousOverlap, -0.7365756122f));
  assert(AUDIO_CODEC_FUNC(MP3_IMDCT)(
      zeros, &part, &state, result) == STATUS_CONST(SUCCESS));
  assert(approx(result[0], previousOverlap));
  for (i = 0; i < 32; ++i)
    for (size_t n = 0; n < 18; ++n)
      assert(state.overlap[i][n] == 0.0f);
  assert(AUDIO_CODEC_FUNC(MP3_IMDCT)(
      zeros, &part, &state, result) == STATUS_CONST(SUCCESS));
  for (i = 0; i < 576; ++i) assert(result[i] == 0);
}
static void shortAndMixed(void) {
  AUDIO_CODEC_TYPE(TMP3HybridState) state = {0};
  AUDIO_CODEC_TYPE(TMP3Granule) part = {0};
  float input[576] = {0}, result[576];
  size_t i;
  part.windowSwitching = 1;
  part.blockType = 2;
  input[0] = 1.0f;
  assert(AUDIO_CODEC_FUNC(MP3_IMDCT)(
      input, &part, &state, result) == STATUS_CONST(SUCCESS));
  for (i = 0; i < 6; ++i) assert(result[i] == 0);
  assert(approx(result[6], 0.0794593113f));
  memset(&state, 0, sizeof state);
  part.mixedBlock = 1;
  input[0] = 1;
  input[36] = 1;
  assert(AUDIO_CODEC_FUNC(MP3_IMDCT)(
      input, &part, &state, result) == STATUS_CONST(SUCCESS));
  assert(approx(result[0], 0.0294688310f));
  for (i = 36; i < 42; ++i) assert(result[i] == 0);
  assert(approx(result[42], 0.0794593113f));
  /* Aliased input/output is permitted because the original spectrum is
   * fully consumed before result is published. */
  memset(&state, 0, sizeof state);
  assert(AUDIO_CODEC_FUNC(MP3_IMDCT)(
      input, &part, &state, input) == STATUS_CONST(SUCCESS));
  assert(approx(input[42], 0.0794593113f));
}
static void lsfMixedEightKhz(void) {
  AUDIO_CODEC_TYPE(TMP3HybridState) state = {0};
  AUDIO_CODEC_TYPE(TMP3Granule) part = {0};
  float input[576] = {0}, output[576], shortInput[576] = {0};
  float aliasOutput[576], ordinaryAlias[576], aliasIn[576] = {0};
  size_t i;
  part.windowSwitching = 1;
  part.blockType = 2;
  part.mixedBlock = 1;
  input[36] = 1.0f; /* subband 2 is long in 8-kHz mixed, short otherwise */
  input[72] = 1.0f; /* subband 4 remains short */
  assert(AUDIO_INS(MP3_IMDCTMixedSubbands)(
      input, &part, &state, output, 4U) == STATUS_CONST(SUCCESS));
  assert(approx(output[36], 0.0294688310f));
  assert(approx(output[72+6], 0.0794593113f));
  memset(&state, 0, sizeof state);
  shortInput[36] = 1.0f;
  assert(AUDIO_CODEC_FUNC(MP3_IMDCT)(
      shortInput, &part, &state, output) == STATUS_CONST(SUCCESS));
  for (i = 36; i < 42; ++i) assert(output[i] == 0);
  assert(approx(output[42], 0.0794593113f));

  /* Boundary 18, 36 and 54 are aliased before short subband 4.
   * Default MPEG-1 mixed antialiasing only transforms boundary 18. */
  aliasIn[53] = 1.0f;
  aliasIn[54] = 1.0f;
  assert(AUDIO_CODEC_FUNC(MP3_Antialias)(
      aliasIn, &part, ordinaryAlias) == STATUS_CONST(SUCCESS));
  assert(AUDIO_INS(MP3_AntialiasMixedBoundaries)(
      aliasIn, &part, aliasOutput, 3U) == STATUS_CONST(SUCCESS));
  assert(ordinaryAlias[53] == 1.0f && ordinaryAlias[54] == 1.0f);
  assert(aliasOutput[53] != 1.0f && aliasOutput[54] != 1.0f);
}

static void frequencyAndSafety(void) {
  float input[576] = {0}, out[576];
  AUDIO_CODEC_TYPE(TMP3HybridState) state = {0};
  AUDIO_CODEC_TYPE(TMP3Granule) part = {0};
  size_t i;
  for (i = 0; i < 576; ++i) input[i] = (float)(i+1);
  assert(AUDIO_CODEC_FUNC(MP3_FrequencyInvert)(
      input, out) == STATUS_CONST(SUCCESS));
  assert(out[0] == 1 && out[18] == 19 && out[19] == -20);
  assert(out[37] == 38 && out[55] == -56);
  assert(out[576-1] == -576);
  assert(AUDIO_CODEC_FUNC(MP3_FrequencyInvert)(
      out, out) == STATUS_CONST(SUCCESS));
  assert(!memcmp(out, input, sizeof out));
  part.windowSwitching = 1;
  part.blockType = 0;
  out[0] = 997;
  assert(AUDIO_CODEC_FUNC(MP3_IMDCT)(
      input,&part,&state,out) == STATUS_CONST(INVALID_ARGUMENT));
  assert(out[0] == 997 && state.overlap[0][0] == 0);
  part.windowSwitching = 0;
  part.blockType = 0;
  for (i = 0; i < 18; ++i) input[i] = FLT_MAX;
  assert(AUDIO_CODEC_FUNC(MP3_IMDCT)(
      input,&part,&state,out) == STATUS_CONST(ARITHMETIC_OVERFLOW));
  assert(out[0] == 997 && state.overlap[0][0] == 0);
}
int main(void) {
  impulseAndOverlap();
  shortAndMixed();
  lsfMixedEightKhz();
  frequencyAndSafety();
  return 0;
}
