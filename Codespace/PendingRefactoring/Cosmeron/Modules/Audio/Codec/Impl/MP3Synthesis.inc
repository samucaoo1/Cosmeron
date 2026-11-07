#pragma once
#include "MP3SynthesisTables.inc"

/* MPEG Layer III 32-subband polyphase synthesis. One slot consumes
 * exactly 32 subband samples and emits 32 mono f32 PCM samples.
 * The 1024-sample delay line is owned by the zero-initialized state.
 * No libm, malloc or platform audio backend is required. */
AUDIO_CODEC_MP3_SYNTHESIS_SLOT_PROTOTYPE {
  float v[1024], pcm[32];
  unsigned int i, k, j, phase;
  if (!subbands || !state || !outPCM)
    return STATUS_CONST(INVALID_ARGUMENT);
  for (i = 0; i < 32; ++i)
    if (!AUDIO_INS(Finite)(subbands[i]))
      return STATUS_CONST(INVALID_SEQUENCE);
  for (i = 0; i < 1024; ++i)
    if (!AUDIO_INS(Finite)(state->delay[i]))
      return STATUS_CONST(INVALID_SEQUENCE);
  memcpy(v + 64, state->delay, 960 * sizeof(float));
  for (i = 0; i < 64; ++i) {
    float sum = 0;
    for (k = 0; k < 32; ++k)
      sum += subbands[k] * AUDIO_INS(MP3_SynthesisMatrix)[i][k];
    if (!AUDIO_INS(Finite)(sum))
      return STATUS_CONST(ARITHMETIC_OVERFLOW);
    v[i] = sum;
  }
  for (j = 0; j < 32; ++j) {
    float sum = 0;
    for (phase = 0; phase < 16; ++phase) {
      const unsigned int u = phase * 32U + j;
      const unsigned int b = (phase / 2U) * 128U +
          (phase % 2U ? 96U : 0U) + j;
      sum += v[b] * AUDIO_INS(MP3_SynthesisWindow)[u];
    }
    if (!AUDIO_INS(Finite)(sum))
      return STATUS_CONST(ARITHMETIC_OVERFLOW);
    pcm[j] = sum;
  }
  memcpy(outPCM, pcm, sizeof pcm);
  memcpy(state->delay, v, sizeof v);
  return STATUS_CONST(SUCCESS);
}

/* The preceding IMDCT returns [subband][time]. Process its 18 timeslots
 * in chronological order and produce 576 contiguous f32 PCM samples.
 * Output and history remain unchanged if any time slot is rejected. */
AUDIO_CODEC_MP3_SYNTHESIZE_PROTOTYPE {
  AUDIO_CODEC_TYPE(TMP3SynthesisState) work;
  float pcm[576], input[32], current[32];
  unsigned int time, sb;
  OPSTATUS status;
  if (!subbandTime || !state || !outPCM)
    return STATUS_CONST(INVALID_ARGUMENT);
  work = *state;
  for (time = 0; time < 18; ++time) {
    for (sb = 0; sb < 32; ++sb)
      input[sb] = subbandTime[18U * sb + time];
    status = AUDIO_CODEC_FUNC(MP3_SynthesisSlot)(input, &work, current);
    if (status != STATUS_CONST(SUCCESS)) return status;
    memcpy(pcm + 32U * time, current, sizeof current);
  }
  memcpy(outPCM, pcm, sizeof pcm);
  *state = work;
  return STATUS_CONST(SUCCESS);
}
