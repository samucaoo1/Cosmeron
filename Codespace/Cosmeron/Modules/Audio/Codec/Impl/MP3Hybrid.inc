#pragma once

/* MPEG Layer III short-block reorder: bitstream order is
 * [band][window][frequency]. Hybrid IMDCT expects
 * [band][frequency][window]. Mixed blocks preserve long lines 0..35.
 * Caller may provide identical input/output buffers. */
AUDIO_CODEC_MP3_REORDER_SHORT_PROTOTYPE {
  float reordered[576];
  unsigned int rate, band, win;
  size_t inputStart = 0, src, dst, line, width, i;
  if (!input || !part || !output) return STATUS_CONST(INVALID_ARGUMENT);
  if ((part->windowSwitching && part->blockType == 0) ||
      part->mixedBlock > 1)
    return STATUS_CONST(INVALID_ARGUMENT);
  if (!(part->windowSwitching && part->blockType == 2)) {
    if (part->mixedBlock) return STATUS_CONST(INVALID_ARGUMENT);
    memmove(output, input, sizeof reordered);
    return STATUS_CONST(SUCCESS);
  }
  for (rate = 0; rate < 9; ++rate)
    if (AUDIO_INS(MP3_LongRates)[rate] == sampleRate) break;
  if (rate == 9) return STATUS_CONST(NOT_SUPPORTED);
  if (part->mixedBlock) {
    inputStart = 3U * AUDIO_INS(MP3_ShortBands)[rate][3];
    if (inputStart > 576) return STATUS_CONST(INVALID_SEQUENCE);
    memcpy(reordered, input, inputStart * sizeof(float));
  }
  for (band = part->mixedBlock ? 3U : 0U; band < 13; ++band) {
    const size_t bandStart = 3U * AUDIO_INS(MP3_ShortBands)[rate][band];
    width = AUDIO_INS(MP3_ShortBands)[rate][band + 1] -
            AUDIO_INS(MP3_ShortBands)[rate][band];
    if (!width || width > 192 || inputStart > 576 - 3 * width ||
        bandStart > 576 - 3 * width)
      return STATUS_CONST(INVALID_SEQUENCE);
    for (win = 0; win < 3; ++win) {
      for (line = 0; line < width; ++line) {
        src = inputStart + (size_t)win * width + line;
        dst = bandStart + 3U * line + win;
        reordered[dst] = input[src];
      }
    }
    inputStart += 3U * width;
  }
  if (inputStart != 576) return STATUS_CONST(INVALID_SEQUENCE);
  for (i = 0; i < 576; ++i)
    if (!AUDIO_INS(Finite)(reordered[i]))
      return STATUS_CONST(INVALID_SEQUENCE);
  memcpy(output, reordered, sizeof reordered);
  return STATUS_CONST(SUCCESS);
}

/* MPEG-1 mid/side is an orthonormal rotation, not (M +/- S)/2.
 * Intensity-stereo bands must be handled separately before requesting MS
 * over the remaining spectral lines. The caller chooses the active range. */
AUDIO_CODEC_MP3_STEREO_MS_PROTOTYPE {
  float l[576], r[576];
  const float invSqrt2 = 0.70710678118654752440f;
  size_t i;
  if (!mid || !side || !outLeft || !outRight || outLeft == outRight ||
      first > last || last > 576)
    return STATUS_CONST(INVALID_ARGUMENT);
  memcpy(l, mid, sizeof l);
  memcpy(r, side, sizeof r);
  for (i = first; i < last; ++i) {
    const float m = mid[i], s = side[i];
    if (!AUDIO_INS(Finite)(m) || !AUDIO_INS(Finite)(s))
      return STATUS_CONST(INVALID_SEQUENCE);
    l[i] = (m + s) * invSqrt2;
    r[i] = (m - s) * invSqrt2;
    if (!AUDIO_INS(Finite)(l[i]) || !AUDIO_INS(Finite)(r[i]))
      return STATUS_CONST(ARITHMETIC_OVERFLOW);
  }
  memcpy(outLeft, l, sizeof l);
  memcpy(outRight, r, sizeof r);
  return STATUS_CONST(SUCCESS);
}

/* ISO/IEC 11172-3 alias-reduction butterfly on adjacent polyphase
 * subbands, with cs=1/sqrt(1+ci^2) and ca=ci*cs (Table B.9).
 * Pure short: no butterflies; mixed: one long-band boundary at line 18;
 * other block types: all 31 subband boundaries. */
static inline OPSTATUS AUDIO_INS(MP3_AntialiasMixedBoundaries)(
    const float input[576], const AUDIO_CODEC_TYPE(TMP3Granule) *part,
    float output[576], unsigned int mixedBoundaries) {
  static const float cs[8] = {
    0.8574929257f, 0.8817419973f, 0.9496286491f, 0.9833145925f, 0.9955178161f, 0.9991605582f, 0.9998991952f, 0.9999931551f
  };
  static const float ca[8] = {
    -0.5144957554f, -0.4717319686f, -0.3133774542f, -0.1819131996f, -0.0945741925f, -0.0409655829f, -0.0141985686f, -0.0036999747f
  };
  float transformed[576];
  unsigned int sb, butterfly, boundaries;
  size_t i;
  if (!input || !part || !output) return STATUS_CONST(INVALID_ARGUMENT);
  if (part->mixedBlock > 1 ||
      (part->windowSwitching &&
          (part->blockType == 0 || part->blockType > 3)))
    return STATUS_CONST(INVALID_ARGUMENT);
  if (mixedBoundaries > 31) return STATUS_CONST(INVALID_ARGUMENT);
  for (i = 0; i < 576; ++i)
    if (!AUDIO_INS(Finite)(input[i]))
      return STATUS_CONST(INVALID_SEQUENCE);
  memcpy(transformed, input, sizeof transformed);
  boundaries = part->windowSwitching && part->blockType == 2 ?
      (part->mixedBlock ? mixedBoundaries : 0U) : 31U;
  for (sb = 1; sb <= boundaries; ++sb) {
    for (butterfly = 0; butterfly < 8; ++butterfly) {
      const size_t lower = 18U * sb - 1U - butterfly;
      const size_t upper = 18U * sb + butterfly;
      const float a = transformed[lower], b = transformed[upper];
      transformed[lower] = a * cs[butterfly] - b * ca[butterfly];
      transformed[upper] = b * cs[butterfly] + a * ca[butterfly];
      if (!AUDIO_INS(Finite)(transformed[lower]) ||
          !AUDIO_INS(Finite)(transformed[upper]))
        return STATUS_CONST(ARITHMETIC_OVERFLOW);
    }
  }
  memcpy(output, transformed, sizeof transformed);
  return STATUS_CONST(SUCCESS);
}

AUDIO_CODEC_MP3_ANTIALIAS_PROTOTYPE {
  return AUDIO_INS(MP3_AntialiasMixedBoundaries)(input,part,output,1U);
}
