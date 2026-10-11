#pragma once
#include "MP3IMDCTTables.inc"

/* Stateful MPEG Layer III hybrid filterbank: 18 spectral inputs -> 36
 * windowed time values per subband, overlapping 18 with prior granule.
 * The output is 32 x 18 subband time samples, NOT final PCM. */
AUDIO_CODEC_MP3_IMDCT_PROTOTYPE {
  float subband[576], overlap[32][18], y[36];
  unsigned int sb, block, n, k, win;
  size_t i;
  if (!spectrum || !part || !state || !outSubband)
    return STATUS_CONST(INVALID_ARGUMENT);
  if ((part->windowSwitching && (part->blockType == 0 ||
                                 part->blockType > 3)) ||
      part->mixedBlock > 1 || (part->mixedBlock &&
          !(part->windowSwitching && part->blockType == 2)))
    return STATUS_CONST(INVALID_ARGUMENT);
  for (i = 0; i < 576; ++i)
    if (!AUDIO_INS(Finite)(spectrum[i]))
      return STATUS_CONST(INVALID_SEQUENCE);
  for (sb = 0; sb < 32; ++sb) {
    for (n = 0; n < 18; ++n)
      if (!AUDIO_INS(Finite)(state->overlap[sb][n]))
        return STATUS_CONST(INVALID_SEQUENCE);
  }
  for (sb = 0; sb < 32; ++sb) {
    block = part->windowSwitching ? part->blockType : 0U;
    if (part->mixedBlock && sb < 2) block = 0;
    for (n = 0; n < 36; ++n) y[n] = 0.0f;
    if (block == 2U) {
      for (win = 0; win < 3; ++win) {
        for (n = 0; n < 12; ++n) {
          float sum = 0.0f;
          for (k = 0; k < 6; ++k)
            sum += spectrum[sb * 18 + k * 3 + win] *
                AUDIO_INS(MP3_IMDCTShort)[n][k];
          y[6 + 6 * win + n] +=
              sum * AUDIO_INS(MP3_IMDCTWindows)[2][n];
        }
      }
    } else {
      for (n = 0; n < 36; ++n) {
        float sum = 0.0f;
        for (k = 0; k < 18; ++k)
          sum += spectrum[sb * 18 + k] *
              AUDIO_INS(MP3_IMDCTLong)[n][k];
        y[n] = sum * AUDIO_INS(MP3_IMDCTWindows)[block][n];
      }
    }
    for (n = 0; n < 18; ++n) {
      subband[sb * 18 + n] = state->overlap[sb][n] + y[n];
      overlap[sb][n] = y[n + 18];
      if (!AUDIO_INS(Finite)(subband[sb * 18 + n]) ||
          !AUDIO_INS(Finite)(overlap[sb][n]))
        return STATUS_CONST(ARITHMETIC_OVERFLOW);
    }
  }
  memcpy(outSubband, subband, sizeof subband);
  memcpy(state->overlap, overlap, sizeof overlap);
  return STATUS_CONST(SUCCESS);
}

/* Standard MPEG Layer III frequency inversion: negate odd time positions
 * in odd subbands. May operate in-place. */
AUDIO_CODEC_MP3_FREQUENCY_INVERT_PROTOTYPE {
  float temporary[576];
  unsigned int sb, sample;
  size_t i;
  if (!input || !output) return STATUS_CONST(INVALID_ARGUMENT);
  for (i = 0; i < 576; ++i)
    if (!AUDIO_INS(Finite)(input[i]))
      return STATUS_CONST(INVALID_SEQUENCE);
  memcpy(temporary, input, sizeof temporary);
  for (sb = 1; sb < 32; sb += 2)
    for (sample = 1; sample < 18; sample += 2)
      temporary[18U * sb + sample] = -temporary[18U * sb + sample];
  memcpy(output, temporary, sizeof temporary);
  return STATUS_CONST(SUCCESS);
}
