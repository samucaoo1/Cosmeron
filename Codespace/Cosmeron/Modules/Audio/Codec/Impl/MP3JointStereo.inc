#pragma once

/* MPEG-1 Layer III joint-stereo band routing. Intensity applies only
 * above the right channel's highest coded nonzero scalefactor band;
 * ordinary mid/side processing applies everywhere else. Band positions
 * are determined in the Huffman (pre-reorder) spectral layout.
 *
 * For the untransmitted final long/short scalefactor band, the preceding
 * position is inherited unless that band contains transmitted right
 * coefficients, in which case the default position is 3 (centre).
 *
 * All computations take place in temporaries so failures never publish
 * partial stereo channels. */
/* LSF-specific stereo scalings: is_pos=0 shares channel energy equally
 * only in the MS-normalized representation when requested; 255 disables
 * intensity. Use base 2^(-1/4) and 2^(-1/2) factors, without libm. */
AUDIO_CODEC_MP3_JOINT_STEREO_LSF_PROTOTYPE {
  static const float quarter[4] = {1.0f,0.8408964153f,
      0.7071067812f,0.5946035575f};
  float l[576], r[576];
  uint8_t pos[576];
  int lastLong = -1, lastShort[3] = {-1,-1,-1};
  unsigned int rate, band, win, nLong, shortStart;
  size_t at = 0, i;
  bool mixed, shortBlock;
  if (!header || !part || !scale || !left || !right ||
      !outLeft || !outRight || outLeft == outRight || intensityScale > 1)
    return STATUS_CONST(INVALID_ARGUMENT);
  if (header->mpegVersion == 1 || header->channels != 2)
    return STATUS_CONST(NOT_SUPPORTED);
  for (rate=0;rate<9;++rate)
    if (AUDIO_INS(MP3_LongRates)[rate] == header->sampleRate) break;
  if (rate==9) return STATUS_CONST(NOT_SUPPORTED);
  shortBlock = part->windowSwitching && part->blockType==2;
  mixed = shortBlock && part->mixedBlock;
  if (part->mixedBlock > 1 || (part->mixedBlock && !shortBlock))
    return STATUS_CONST(INVALID_ARGUMENT);
  for (i=0;i<576;++i)
    if (!AUDIO_INS(Finite)(left[i]) || !AUDIO_INS(Finite)(right[i]))
      return STATUS_CONST(INVALID_SEQUENCE);
  memset(pos,255,sizeof pos);
  nLong = mixed ? 6U : shortBlock ? 0U : 22U;
  shortStart = mixed ? 3U : 0U;
  for (band=0;band<nLong;++band)
    for (i=AUDIO_INS(MP3_LongBands)[rate][band];
         i<AUDIO_INS(MP3_LongBands)[rate][band+1];++i)
      if (right[i]!=0) {lastLong=(int)band; break;}
  if (shortBlock) {
    at = mixed ? 3U*AUDIO_INS(MP3_ShortBands)[rate][3] : 0U;
    for (band=shortStart;band<13;++band) {
      size_t width=AUDIO_INS(MP3_ShortBands)[rate][band+1]-
          AUDIO_INS(MP3_ShortBands)[rate][band];
      if (width>(576U-at)/3U) return STATUS_CONST(INVALID_SEQUENCE);
      for (win=0;win<3;++win)
        for (i=0;i<width;++i)
          if (right[at+win*width+i]!=0) {
            lastShort[win]=(int)band; break;
          }
      at+=3U*width;
    }
    if (at!=576) return STATUS_CONST(INVALID_SEQUENCE);
  }
  for (band=0;band<nLong;++band) {
    unsigned int p=band==21 ?
        scale->lsfIntensityLong[20] : scale->lsfIntensityLong[band];
    if ((int)band>lastLong && p!=255)
      for (i=AUDIO_INS(MP3_LongBands)[rate][band];
           i<AUDIO_INS(MP3_LongBands)[rate][band+1];++i)
        pos[i]=(uint8_t)p;
  }
  if (shortBlock) {
    at=mixed ? 3U*AUDIO_INS(MP3_ShortBands)[rate][3] : 0U;
    for (band=shortStart;band<13;++band) {
      size_t width=AUDIO_INS(MP3_ShortBands)[rate][band+1]-
          AUDIO_INS(MP3_ShortBands)[rate][band];
      for (win=0;win<3;++win) {
        unsigned int p=band==12 ? scale->lsfIntensityShort[11][win] :
            scale->lsfIntensityShort[band][win];
        if ((int)band>lastShort[win] && p!=255)
          for (i=0;i<width;++i) pos[at+win*width+i]=(uint8_t)p;
      }
      at+=width*3U;
    }
  }
  for (i=0;i<576;++i) {
    float a=left[i],b=right[i];
    if (pos[i]!=255) {
      unsigned int position=pos[i];
      unsigned int exp=(position+1U)/2U << intensityScale;
      float q=1.0f;
      unsigned int z;
      /* Every four quarter exponents lower the value by one half. */
      for (z=0;z<exp/4U;++z) q*=0.5f;
      q*=quarter[exp%4U];
      if (position&1U) {l[i]=a*q; r[i]=a;}
      else {l[i]=a; r[i]=a*q;}
      /* Cosmeron's MS path already uses the 1/sqrt(2) normalization.
       * Unlike the unnormalized reference's mid/side transform, the
       * intensity path must not receive an additional sqrt(2) gain. */
      if (b!=0) return STATUS_CONST(INVALID_SEQUENCE);
    } else if (header->msStereo) {
      l[i]=(a+b)*0.70710678118f;
      r[i]=(a-b)*0.70710678118f;
    } else {l[i]=a;r[i]=b;}
    if (!AUDIO_INS(Finite)(l[i]) || !AUDIO_INS(Finite)(r[i]))
      return STATUS_CONST(ARITHMETIC_OVERFLOW);
  }
  memcpy(outLeft,l,sizeof l);memcpy(outRight,r,sizeof r);
  return STATUS_CONST(SUCCESS);
}

AUDIO_CODEC_MP3_JOINT_STEREO_PROTOTYPE {
  static const float panLeft[7] = {
    0.0f, 0.2113248654f, 0.3660254038f, 0.5f,
    0.6339745962f, 0.7886751346f, 1.0f
  };
  static const float panRight[7] = {
    1.0f, 0.7886751346f, 0.6339745962f, 0.5f,
    0.3660254038f, 0.2113248654f, 0.0f
  };
  const float invSqrt2 = 0.7071067811865475244f;
  const float sqrt2 = 1.4142135623730950488f;
  const bool switched = rightPart && rightPart->windowSwitching &&
      rightPart->blockType == 2;
  const bool mixed = switched && rightPart->mixedBlock;
  float l[576], r[576];
  uint8_t positions[576]; /* 255 denotes ordinary L/R or M/S. */
  unsigned int rate = 0, band, window;
  int highestLong = -1, highestShort[3] = {-1,-1,-1};
  bool anyShort = false;
  size_t cursor = 0, i;
  if (!header || !rightPart || !rightScale || !left || !right ||
      !outLeft || !outRight || outLeft == outRight)
    return STATUS_CONST(INVALID_ARGUMENT);
  if (header->mpegVersion != 1 || header->channels != 2)
    return STATUS_CONST(NOT_SUPPORTED);
  if (rightPart->mixedBlock > 1 ||
      (rightPart->windowSwitching && (rightPart->blockType == 0 ||
                                     rightPart->blockType > 3)) ||
      (rightPart->mixedBlock && !switched))
    return STATUS_CONST(INVALID_ARGUMENT);
  for (i = 0; i < 576; ++i)
    if (!AUDIO_INS(Finite)(left[i]) || !AUDIO_INS(Finite)(right[i]))
      return STATUS_CONST(INVALID_SEQUENCE);
  if (!header->intensityStereo && !header->msStereo) {
    memmove(outLeft, left, sizeof l);
    memmove(outRight, right, sizeof r);
    return STATUS_CONST(SUCCESS);
  }
  if (header->intensityStereo) {
    for (rate = 0; rate < 3; ++rate)
      if (AUDIO_INS(MP3_LongRates)[rate] == header->sampleRate) break;
    if (rate == 3) return STATUS_CONST(NOT_SUPPORTED);
  }
  memset(positions, 255, sizeof positions);
  if (header->intensityStereo) {
    const unsigned int longCount = mixed ? 8U : switched ? 0U : 22U;
    const unsigned int shortStart = mixed ? 3U : 0U;
    /* First locate transmitted right-side information. Zero bands below
     * a higher occupied band are never intensity-coded. */
    for (band = 0; band < longCount; ++band) {
      size_t a = AUDIO_INS(MP3_LongBands)[rate][band];
      size_t b = AUDIO_INS(MP3_LongBands)[rate][band+1];
      for (i = a; i < b; ++i)
        if (right[i] != 0.0f) { highestLong = (int)band; break; }
    }
    if (switched) {
      cursor = mixed ? 36U : 0U;
      for (band = shortStart; band <= 12; ++band) {
        size_t width = AUDIO_INS(MP3_ShortBands)[rate][band+1] -
                       AUDIO_INS(MP3_ShortBands)[rate][band];
        if (width > (576U-cursor)/3U)
          return STATUS_CONST(INVALID_SEQUENCE);
        for (window = 0; window < 3; ++window)
          for (i = 0; i < width; ++i)
            if (right[cursor + window*width + i] != 0.0f) {
              highestShort[window] = (int)band;
              anyShort = true;
              break;
            }
        cursor += width * 3U;
      }
      if (cursor != 576) return STATUS_CONST(INVALID_SEQUENCE);
    }
    for (band = 0; band < longCount; ++band) {
      unsigned int pos = band == 21 ?
          (highestLong >= 20 ? 3U : rightScale->longBands[20]) :
          rightScale->longBands[band];
      bool eligible = (int)band > highestLong && (!mixed || !anyShort);
      if (eligible && pos < 7)
        for (i = AUDIO_INS(MP3_LongBands)[rate][band];
             i < AUDIO_INS(MP3_LongBands)[rate][band+1]; ++i)
          positions[i] = (uint8_t)pos;
    }
    if (switched) {
      cursor = mixed ? 36U : 0U;
      for (band = shortStart; band <= 12; ++band) {
        size_t width = AUDIO_INS(MP3_ShortBands)[rate][band+1] -
                       AUDIO_INS(MP3_ShortBands)[rate][band];
        for (window = 0; window < 3; ++window) {
          unsigned int pos = band == 12 ?
              (highestShort[window] >= 11 ? 3U :
               rightScale->shortBands[11][window]) :
              rightScale->shortBands[band][window];
          if ((int)band > highestShort[window] && pos < 7)
            for (i = 0; i < width; ++i)
              positions[cursor + window*width + i] = (uint8_t)pos;
        }
        cursor += 3U*width;
      }
    }
  }
  for (i = 0; i < 576; ++i) {
    const float a = left[i], b = right[i];
    if (positions[i] != 255) {
      unsigned int p = positions[i];
      if (b != 0.0f) return STATUS_CONST(INVALID_SEQUENCE);
      l[i] = a * panLeft[p] * (header->msStereo ? sqrt2 : 1.0f);
      r[i] = a * panRight[p] * (header->msStereo ? sqrt2 : 1.0f);
    } else if (header->msStereo) {
      l[i] = (a+b)*invSqrt2;
      r[i] = (a-b)*invSqrt2;
    } else {
      l[i] = a;
      r[i] = b;
    }
    if (!AUDIO_INS(Finite)(l[i]) || !AUDIO_INS(Finite)(r[i]))
      return STATUS_CONST(ARITHMETIC_OVERFLOW);
  }
  memcpy(outLeft, l, sizeof l);
  memcpy(outRight, r, sizeof r);
  return STATUS_CONST(SUCCESS);
}
