#pragma once
#include "MP3RequantTables.inc"

/* MPEG-1 spectral dequantization before short-window reordering,
 * stereo processing, antialiasing, IMDCT and subband synthesis.
 * All transforms are float LUT-based: no -lm or other linking required.
 * Outputs are committed only if every coefficient is valid. */
static inline OPSTATUS AUDIO_INS(MP3_RequantBand)(
    const int32_t *quantized, size_t first, size_t count, float gain,
    float *output) {
  size_t i;
  if (first > 576 || count > 576 - first)
    return STATUS_CONST(OUT_OF_RANGE);
  for (i = first; i < first + count; ++i) {
    int64_t magnitude = quantized[i] < 0 ?
        -(int64_t)quantized[i] : (int64_t)quantized[i];
    if (magnitude > 8206) return STATUS_CONST(OUT_OF_RANGE);
    output[i] = AUDIO_INS(MP3_Pow43)[(size_t)magnitude] * gain;
    if (quantized[i] < 0) output[i] = -output[i];
  }
  return STATUS_CONST(SUCCESS);
}

AUDIO_CODEC_MP3_REQUANTIZE_PROTOTYPE {
  float output[576];
  unsigned int rate = 9, sfFactor, band, window;
  float common;
  size_t first, last, j;
  bool shortBlock;
  OPSTATUS status;
  if (!quantized || !part || !scales || !outSpectral)
    return STATUS_CONST(INVALID_ARGUMENT);
  for (rate = 0; rate < 9; ++rate)
    if (AUDIO_INS(MP3_LongRates)[rate] == sampleRate) break;
  if (rate == 9) return STATUS_CONST(NOT_SUPPORTED);
  if (part->scalefacScale > 1 || part->preflag > 1 ||
      part->blockType > 3 || part->mixedBlock > 1)
    return STATUS_CONST(INVALID_ARGUMENT);
  sfFactor = 1U + part->scalefacScale;
  common = AUDIO_INS(MP3_Gain)[part->globalGain];
  shortBlock = part->windowSwitching && part->blockType == 2;
  if (!shortBlock) {
    for (band = 0; band < 22; ++band) {
      unsigned int sf = scales->longBands[band];
      unsigned int pre = part->preflag ? AUDIO_INS(MP3_Pretab)[band] : 0;
      unsigned int attenuation = (sf + pre) * sfFactor;
      if (sf > (rate < 3 ? 15U : 31U) || attenuation >= 64)
        return STATUS_CONST(INVALID_ARGUMENT);
      first = AUDIO_INS(MP3_LongBands)[rate][band];
      last = AUDIO_INS(MP3_LongBands)[rate][band+1];
      status = AUDIO_INS(MP3_RequantBand)(
          quantized, first, last-first,
          common * AUDIO_INS(MP3_HalfPower)[attenuation], output);
      if (status != STATUS_CONST(SUCCESS)) return status;
    }
  } else {
    first = 0;
    if (part->mixedBlock) {
      for (band = 0; band < (rate < 3 ? 8U : 6U); ++band) {
        unsigned int sf = scales->longBands[band];
        unsigned int pre = part->preflag ? AUDIO_INS(MP3_Pretab)[band] : 0;
        unsigned int attenuation = (sf + pre) * sfFactor;
        if (sf > 15 || attenuation >= 64)
          return STATUS_CONST(INVALID_ARGUMENT);
        last = AUDIO_INS(MP3_LongBands)[rate][band+1];
        status = AUDIO_INS(MP3_RequantBand)(
            quantized, first, last-first,
            common * AUDIO_INS(MP3_HalfPower)[attenuation], output);
        if (status != STATUS_CONST(SUCCESS)) return status;
        first = last;
      }
    }
    for (band = part->mixedBlock ? 3U : 0U; band < 13; ++band) {
      size_t width = AUDIO_INS(MP3_ShortBands)[rate][band+1] -
                     AUDIO_INS(MP3_ShortBands)[rate][band];
      for (window = 0; window < 3; ++window) {
        unsigned int sf = scales->shortBands[band][window];
        unsigned int attenuation = sf * sfFactor;
        if (sf > (rate < 3 ? 15U : 31U) || attenuation >= 64 ||
            part->subblockGain[window] > 7)
          return STATUS_CONST(INVALID_ARGUMENT);
        status = AUDIO_INS(MP3_RequantBand)(
            quantized, first, width,
            common * AUDIO_INS(MP3_HalfPower)[attenuation] *
            AUDIO_INS(MP3_SubblockGain)[part->subblockGain[window]], output);
        if (status != STATUS_CONST(SUCCESS)) return status;
        first += width;
      }
    }
    if (first != 576) return STATUS_CONST(INVALID_SEQUENCE);
  }
  for (j = 0; j < 576; ++j) {
    if (!AUDIO_INS(Finite)(output[j]))
      return STATUS_CONST(ARITHMETIC_OVERFLOW);
  }
  memcpy(outSpectral, output, sizeof output);
  return STATUS_CONST(SUCCESS);
}
