#include "../../Cosmeron/Modules/Audio/Codec/MP3.h"
#include <assert.h>
#include <float.h>
#include <string.h>

static float absolute(float v) { return v < 0 ? -v : v; }
static bool almost(float a, float b) {
  return absolute(a-b) < (0.000008f + 0.00006f * absolute(b));
}

static void impulseFixture(void) {
  const struct { size_t index; float value; } golden[] = {
    {0,0.0f}, {1,-0.0000102471764f}, {31,0.000266426587f},
    {63,0.00235165152f}, {95,0.00410911775f}, {127,0.0233243129f},
    {191,0.0806571248f}, {255,0.847860821f}, {319,0.0673725549f},
    {383,0.0226120339f}, {447,0.00246471169f}, {575,0.0f}
  };
  AUDIO_CODEC_TYPE(TMP3SynthesisState) state = {0};
  float subbandTime[576] = {0}, pcm[576] = {0};
  size_t i;
  subbandTime[0] = 1;
  assert(AUDIO_CODEC_FUNC(MP3_Synthesize)(
      subbandTime, &state, pcm) == STATUS_CONST(SUCCESS));
  for (i = 0; i < sizeof golden / sizeof golden[0]; ++i)
    assert(almost(pcm[golden[i].index], golden[i].value));
  /* State survives calls: a 1024-entry delay lasts multiple 32-sample
   * slots. The next empty granule has no contribution from the impulse. */
  memset(subbandTime, 0, sizeof subbandTime);
  assert(AUDIO_CODEC_FUNC(MP3_Synthesize)(
      subbandTime, &state, pcm) == STATUS_CONST(SUCCESS));
  for (i = 0; i < 576; ++i) assert(almost(pcm[i], 0.f));
}

static void slotAndBatchParity(void) {
  AUDIO_CODEC_TYPE(TMP3SynthesisState) a = {0}, b = {0};
  float subbandTime[576], expected[576], individual[576], slice[32];
  float input[32];
  unsigned int t, sb;
  size_t i;
  for (i = 0; i < 576; ++i)
    subbandTime[i] = (float)((int)(i%31)-15) * 0.003f;
  assert(AUDIO_CODEC_FUNC(MP3_Synthesize)(
      subbandTime, &a, expected) == STATUS_CONST(SUCCESS));
  for (t = 0; t < 18; ++t) {
    for (sb = 0; sb < 32; ++sb)
      input[sb] = subbandTime[18U * sb + t];
    assert(AUDIO_CODEC_FUNC(MP3_SynthesisSlot)(
        input, &b, slice) == STATUS_CONST(SUCCESS));
    memcpy(individual + 32U*t, slice, sizeof slice);
  }
  assert(!memcmp(individual, expected, sizeof expected));
  assert(!memcmp(&a, &b, sizeof a));
  /* Verify both routines support input/output aliasing. */
  memcpy(individual, subbandTime, sizeof individual);
  memset(&a,0,sizeof a);
  assert(AUDIO_CODEC_FUNC(MP3_Synthesize)(
      individual, &a, individual) == STATUS_CONST(SUCCESS));
  assert(!memcmp(individual, expected, sizeof expected));
}

static void errorAtomicity(void) {
  AUDIO_CODEC_TYPE(TMP3SynthesisState) state = {0}, snapshot;
  float input[576] = {0}, output[576];
  size_t i;
  for (i = 0; i < 576; ++i) output[i] = 819.f;
  input[17] = FLT_MAX;
  input[18U * 4 + 17] = FLT_MAX;
  snapshot = state;
  assert(AUDIO_CODEC_FUNC(MP3_Synthesize)(
      input, &state, output) == STATUS_CONST(ARITHMETIC_OVERFLOW));
  for (i = 0; i < 576; ++i) assert(output[i] == 819.f);
  assert(!memcmp(&state, &snapshot, sizeof state));
  input[17] = 0;
  input[18U * 4 + 17] = 0;
  assert(AUDIO_CODEC_FUNC(MP3_Synthesize)(
      input, &state, output) == STATUS_CONST(SUCCESS));
  for (i = 0; i < 576; ++i) assert(output[i] == 0.f);
}
int main(void) {
  impulseFixture();
  slotAndBatchParity();
  errorAtomicity();
  return 0;
}
