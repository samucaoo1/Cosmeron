#pragma once

/* MPEG-1 Layer III intensity-stereo ratios for is_pos 0..6.
 * For is_pos = 7, the band is not intensity coded: copy unchanged.
 * The specified interval is supplied by the caller after identifying
 * intensity-coded scalefactor bands of the right channel. Do NOT apply
 * mid/side processing to this same interval. */
AUDIO_CODEC_MP3_INTENSITY_BAND_PROTOTYPE {
  static const float leftFactor[7] = {
    0.0f, 0.2113248654f, 0.3660254038f, 0.5f,
    0.6339745962f, 0.7886751346f, 1.0f
  };
  static const float rightFactor[7] = {
    1.0f, 0.7886751346f, 0.6339745962f, 0.5f,
    0.3660254038f, 0.2113248654f, 0.0f
  };
  float l[576], r[576];
  size_t i;
  if (!left || !right || !outLeft || !outRight || outLeft == outRight ||
      first > last || last > 576 || isPosition > 7)
    return STATUS_CONST(INVALID_ARGUMENT);
  memcpy(l, left, sizeof l);
  memcpy(r, right, sizeof r);
  if (isPosition != 7U) {
    for (i = first; i < last; ++i) {
      /* Intensity stereo is only meaningful for regions whose right
       * spectral coefficients were not transmitted. Replacing a nonzero
       * right input would destroy actual encoded channel information. */
      if (!AUDIO_INS(Finite)(l[i]) || !AUDIO_INS(Finite)(r[i]))
        return STATUS_CONST(INVALID_SEQUENCE);
      if (r[i] != 0.0f) return STATUS_CONST(INVALID_SEQUENCE);
      l[i] = left[i] * leftFactor[isPosition];
      r[i] = left[i] * rightFactor[isPosition];
      if (!AUDIO_INS(Finite)(l[i]) || !AUDIO_INS(Finite)(r[i]))
        return STATUS_CONST(ARITHMETIC_OVERFLOW);
    }
  }
  memcpy(outLeft, l, sizeof l);
  memcpy(outRight, r, sizeof r);
  return STATUS_CONST(SUCCESS);
}
