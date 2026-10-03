#define AUDIO_NO_DESKTOP
#include "../../Cosmeron/Modules/Audio/Audio.h"
#include <assert.h>
#include <stdio.h>

static float absFloat(float value) { return value < 0 ? -value : value; }
static void testPCM(void) {
  AUDIO_PCM_TYPE(TBuffer) buffer = {0};
  int16_t input[] = {INT16_MIN, 0, INT16_MAX}, output[3];
  float values[3];
  assert(AUDIO_PCM_FUNC(Init)(&buffer, 32, 2, 48000) == STATUS_CONST(SUCCESS));
  assert(buffer.frameCount == 32 && buffer.samples[63] == 0);
  assert(AUDIO_PCM_FUNC(Init)(&buffer, 32, 2, 48000) == STATUS_CONST(BUSY));
  AUDIO_PCM_FUNC(Destroy)(&buffer);
  AUDIO_PCM_FUNC(Destroy)(&buffer);
  assert(AUDIO_PCM_FUNC(Init)(&buffer, SIZE_MAX, 2, 48000) ==
         STATUS_CONST(ARITHMETIC_OVERFLOW));
  assert(AUDIO_PCM_FUNC(FromS16)(input, 3, values) == STATUS_CONST(SUCCESS));
  assert(values[0] == -1 && values[1] == 0 && values[2] < 1);
  values[2] = 2;
  assert(AUDIO_PCM_FUNC(ToS16)(values, 3, output) == STATUS_CONST(SUCCESS));
  assert(output[0] == INT16_MIN && output[1] == 0 && output[2] == INT16_MAX);
}
static void testOscillator(void) {
  AUDIO_OSCILLATOR_TYPE(TOscillator) a, b;
  float whole[1024], split[1024], table[] = {0, 1, 0, -1};
  size_t i;
  int wave;
  for (wave = AUDIO_OSCILLATOR_CONST(SINE);
       wave <= AUDIO_OSCILLATOR_CONST(NOISE); ++wave) {
    assert(AUDIO_OSCILLATOR_FUNC(Init)(&a, 48000,
                                       (AUDIO_OSCILLATOR_TYPE(TWaveform))wave,
                                       440) == STATUS_CONST(SUCCESS));
    b = a;
    assert(AUDIO_OSCILLATOR_FUNC(Render)(&a, whole, 1024) ==
           STATUS_CONST(SUCCESS));
    assert(AUDIO_OSCILLATOR_FUNC(Render)(&b, split, 17) ==
           STATUS_CONST(SUCCESS));
    assert(AUDIO_OSCILLATOR_FUNC(Render)(&b, split + 17, 1007) ==
           STATUS_CONST(SUCCESS));
    assert(memcmp(whole, split, sizeof(whole)) == 0);
    for (i = 0; i < 1024; ++i)
      assert(absFloat(whole[i]) <= 0.251f);
  }
  assert(AUDIO_OSCILLATOR_FUNC(SetFrequency)(&a, 24000) ==
         STATUS_CONST(INVALID_ARGUMENT));
  assert(AUDIO_OSCILLATOR_FUNC(SetFrequency)(&a, 12000) ==
         STATUS_CONST(SUCCESS));
  assert(AUDIO_OSCILLATOR_FUNC(SetAmplitude)(&a, 1) == STATUS_CONST(SUCCESS));
  assert(AUDIO_OSCILLATOR_FUNC(SetDutyCycle)(&a, 0) ==
         STATUS_CONST(INVALID_ARGUMENT));
  assert(AUDIO_OSCILLATOR_FUNC(SetDutyCycle)(&a, 0.25f) ==
         STATUS_CONST(SUCCESS));
  assert(AUDIO_OSCILLATOR_FUNC(SetWaveform)(&a, AUDIO_OSCILLATOR_CONST(SINE)) ==
         STATUS_CONST(SUCCESS));
  assert(AUDIO_OSCILLATOR_FUNC(ResetPhase)(&a, 0) == STATUS_CONST(SUCCESS));
  assert(AUDIO_OSCILLATOR_FUNC(Render)(&a, whole, 4) == STATUS_CONST(SUCCESS));
  assert(whole[0] == 0 && absFloat(whole[1] - 1) < 0.000001f && whole[2] == 0 &&
         absFloat(whole[3] + 1) < 0.000001f);
  assert(AUDIO_OSCILLATOR_FUNC(SetWavetable)(&a, table, 4) ==
         STATUS_CONST(SUCCESS));
  assert(AUDIO_OSCILLATOR_FUNC(ResetPhase)(&a, 0) == STATUS_CONST(SUCCESS));
  assert(AUDIO_OSCILLATOR_FUNC(Render)(&a, whole, 4) == STATUS_CONST(SUCCESS));
  assert(memcmp(whole, table, sizeof(table)) == 0);
  assert(AUDIO_OSCILLATOR_FUNC(SetWaveform)(
             &a, AUDIO_OSCILLATOR_CONST(NOISE)) == STATUS_CONST(SUCCESS));
  assert(AUDIO_OSCILLATOR_FUNC(SetQuality)(
             &a, AUDIO_OSCILLATOR_CONST(POLYBLEP)) == STATUS_CONST(SUCCESS));
  assert(AUDIO_OSCILLATOR_FUNC(SetWaveform)(&a, AUDIO_OSCILLATOR_CONST(SAW)) ==
         STATUS_CONST(SUCCESS));
  assert(AUDIO_OSCILLATOR_FUNC(ResetPhase)(&a, 0) == STATUS_CONST(SUCCESS));
  assert(AUDIO_OSCILLATOR_FUNC(Render)(&a, whole, 4) == STATUS_CONST(SUCCESS));
  assert(whole[0] == 0); /* Corrected discontinuity, not the naive -1. */
  assert(AUDIO_OSCILLATOR_FUNC(SetWaveform)(
             &a, AUDIO_OSCILLATOR_CONST(NOISE)) == STATUS_CONST(SUCCESS));
  AUDIO_OSCILLATOR_FUNC(Seed)(&a, 123);
  b = a;
  assert(AUDIO_OSCILLATOR_FUNC(Render)(&a, whole, 1024) ==
         STATUS_CONST(SUCCESS));
  assert(AUDIO_OSCILLATOR_FUNC(Render)(&b, split, 1024) ==
         STATUS_CONST(SUCCESS));
  assert(memcmp(whole, split, sizeof(whole)) == 0);
}
static void testEnvelope(void) {
  AUDIO_ENVELOPE_TYPE(TEnvelope) envelope;
  float values[] = {1, 1, 1, 1, 1, 1, 1, 1};
  assert(AUDIO_ENVELOPE_FUNC(Init)(&envelope, 2, 2, 0.5f, 2) ==
         STATUS_CONST(SUCCESS));
  AUDIO_ENVELOPE_FUNC(Trigger)(&envelope);
  assert(AUDIO_ENVELOPE_FUNC(Apply)(&envelope, values, 4, 2) ==
         STATUS_CONST(SUCCESS));
  assert(values[0] == 0.5f && values[1] == 0.5f && values[2] == 1 &&
         values[4] == 0.75f && values[6] == 0.5f);
  AUDIO_ENVELOPE_FUNC(Release)(&envelope);
  values[0] = values[1] = values[2] = 1;
  assert(AUDIO_ENVELOPE_FUNC(Apply)(&envelope, values, 3, 1) ==
         STATUS_CONST(SUCCESS));
  assert(values[0] == 0.25f && values[1] == 0 && values[2] == 0);
  assert(AUDIO_ENVELOPE_FUNC(Init)(&envelope, 0, 0, 1, 0) ==
         STATUS_CONST(SUCCESS));
  AUDIO_ENVELOPE_FUNC(Trigger)(&envelope);
  values[0] = 1;
  assert(AUDIO_ENVELOPE_FUNC(Apply)(&envelope, values, 1, 1) ==
         STATUS_CONST(SUCCESS));
  assert(values[0] == 1);
  AUDIO_ENVELOPE_FUNC(Release)(&envelope);
  assert(AUDIO_ENVELOPE_FUNC(Apply)(&envelope, values, 1, 1) ==
         STATUS_CONST(SUCCESS));
  assert(values[0] == 0);
}
static void testSequence(void) {
  AUDIO_SEQUENCE_TYPE(TSequence) sequence = {0};
  AUDIO_SEQUENCE_TYPE(TCursor) cursor;
  AUDIO_PCM_TYPE(TBuffer) buffer = {0}, tone = {0};
  float block[127];
  size_t error = SIZE_MAX, count, offset = 0;
  double frequency = 0;
  assert(AUDIO_SEQUENCE_FUNC(NoteFrequency)(69, &frequency) ==
         STATUS_CONST(SUCCESS));
  assert(frequency > 439.999 && frequency < 440.001);
  assert(AUDIO_SEQUENCE_FUNC(NoteFrequency)(128, &frequency) ==
         STATUS_CONST(INVALID_ARGUMENT));
  assert(AUDIO_SEQUENCE_FUNC(ParseMML)("T137 O4 L8 V32 C# D- E. P >C <B",
                                       &sequence,
                                       &error) == STATUS_CONST(SUCCESS));
  assert(sequence.eventCount == 6 && sequence.events[3].frequency == 0);
  assert(sequence.events[0].frequency == sequence.events[1].frequency);
  assert(sequence.events[2].ticks == 720);
  assert(AUDIO_SEQUENCE_FUNC(RenderBuffer)(
             sequence.events, sequence.eventCount, 44100,
             AUDIO_OSCILLATOR_CONST(SQUARE), &buffer) == STATUS_CONST(SUCCESS));
  assert(buffer.frameCount == (size_t)(3120.0 * 44100 * 60 / (137 * 960)));
  assert(AUDIO_SEQUENCE_FUNC(Init)(
             &cursor, sequence.events, sequence.eventCount, 44100,
             AUDIO_OSCILLATOR_CONST(SQUARE)) == STATUS_CONST(SUCCESS));
  while (!AUDIO_SEQUENCE_FUNC(Finished)(&cursor)) {
    assert(AUDIO_SEQUENCE_FUNC(Render)(&cursor, block, 127, &count) ==
           STATUS_CONST(SUCCESS));
    assert(offset + count <= buffer.frameCount);
    assert(memcmp(block, buffer.samples + offset, count * sizeof(float)) == 0);
    offset += count;
  }
  assert(offset == buffer.frameCount);
  assert(AUDIO_SEQUENCE_FUNC(Render)(&cursor, block, 127, &count) ==
             STATUS_CONST(SUCCESS) &&
         count == 0);
  AUDIO_SEQUENCE_FUNC(Destroy)(&sequence);
  AUDIO_PCM_FUNC(Destroy)(&buffer);
  assert(AUDIO_SEQUENCE_FUNC(ParseMML)("C L0 D", &sequence, &error) ==
         STATUS_CONST(INVALID_SEQUENCE));
  assert(error == 2 && sequence.events == NULL);
  assert(AUDIO_SEQUENCE_FUNC(ParseMML)("T42949672960 C", &sequence, &error) ==
         STATUS_CONST(INVALID_SEQUENCE));
  assert(AUDIO_SEQUENCE_FUNC(ParseMML)("O9 B", &sequence, &error) ==
         STATUS_CONST(INVALID_SEQUENCE));
  assert(AUDIO_BEEP_FUNC(RenderMML)("T120 O4 L4 C D E P", 48000, &buffer,
                                    &error) == STATUS_CONST(SUCCESS));
  assert(buffer.frameCount == 96000 && buffer.samples[0] == 0 &&
         buffer.samples[95999] == 0);
  AUDIO_PCM_FUNC(Destroy)(&buffer);
  assert(AUDIO_BEEP_FUNC(RenderTone)(440, 100, 0.2f, 48000, &tone) ==
         STATUS_CONST(SUCCESS));
  assert(tone.frameCount == 4800 && tone.samples[0] == 0 &&
         tone.samples[4799] == 0);
  AUDIO_PCM_FUNC(Destroy)(&tone);
  assert(AUDIO_BEEP_FUNC(RenderTone)(0, 100, 0.2f, 48000, &tone) ==
         STATUS_CONST(SUCCESS));
  for (offset = 0; offset < tone.frameCount; ++offset)
    assert(tone.samples[offset] == 0);
  AUDIO_PCM_FUNC(Destroy)(&tone);
}
int main(void) {
  testPCM();
  testOscillator();
  testEnvelope();
  testSequence();
  puts("Audio synthesis: OK");
  return 0;
}
