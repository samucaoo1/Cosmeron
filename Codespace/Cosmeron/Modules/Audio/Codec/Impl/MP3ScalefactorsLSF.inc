#pragma once

/* ISO/IEC 13818-3 Layer III low-sampling-frequency scalefactor
 * partitions. Rows 0..2 apply to ordinary stereo/mono, rows 3..5 to
 * right-channel intensity stereo. Columns: four slen groups.
 * The 21/36/33 transmitted scalefactors are placed into the existing
 * long/short/mixed arrays; any untransmitted terminal bands are zero. */
AUDIO_CODEC_MP3_SCALEFACTORS_LSF_PROTOTYPE {
  static const uint8_t partitions[6][3][4] = {
    {{6,5,5,5}, {9,9,9,9}, {6,9,9,9}},
    {{6,5,7,3}, {9,9,12,6}, {6,9,12,6}},
    {{11,10,0,0}, {18,18,0,0}, {15,18,0,0}},
    {{7,7,7,0}, {12,12,12,0}, {6,15,12,0}},
    {{6,6,6,3}, {12,9,9,6}, {6,12,9,6}},
    {{8,8,5,0}, {15,12,9,0}, {6,18,9,0}}
  };
  const AUDIO_CODEC_TYPE(TMP3Granule) *part;
  AUDIO_CODEC_TYPE(TMP3Scalefactors) result = {0};
  AUDIO_INS(TMP3Bits) bits;
  unsigned int code, family, kind, slen[4], group, n, i;
  uint8_t raw[36] = {0};
  size_t offset = 0, limit, at = 0;
  bool intensity;
  if (!mainData || !header || !side || !outScale || !outIntensityScale)
    return STATUS_CONST(INVALID_ARGUMENT);
  if ((header->mpegVersion != 2 && header->mpegVersion != 25) ||
      side->granules != 1 || side->channels != header->channels ||
      side->channels < 1 || side->channels > 2 ||
      channel >= side->channels || mainDataBytes > SIZE_MAX / 8)
    return STATUS_CONST(INVALID_ARGUMENT);
  for (i = 0; i < channel; ++i)
    offset += side->parts[0][i].part23Length;
  part = &side->parts[0][channel];
  if (part->scalefacCompress > 511 || part->mixedBlock > 1 ||
      (part->windowSwitching && (part->blockType == 0 ||
                                  part->blockType > 3)) ||
      (part->mixedBlock &&
          !(part->windowSwitching && part->blockType == 2)))
    return STATUS_CONST(INVALID_SEQUENCE);
  if (offset > mainDataBytes*8 ||
      part->part23Length > mainDataBytes*8 - offset)
    return STATUS_CONST(INCOMPLETE_SEQUENCE);
  bits.data = mainData;
  bits.bytes = mainDataBytes;
  bits.cursor = offset;
  limit = offset + part->part23Length;
  kind = part->windowSwitching && part->blockType == 2 ?
      (part->mixedBlock ? 2U : 1U) : 0U;
  intensity = header->intensityStereo && channel == 1;
  code = part->scalefacCompress;
  if (intensity) {
    code >>= 1;
    if (code < 180) {
      family = 3;
      slen[0] = code / 36;
      slen[1] = (code / 6) % 6;
      slen[2] = code % 6;
      slen[3] = 0;
    } else if (code < 244) {
      family = 4;
      code -= 180;
      slen[0] = code / 16;
      slen[1] = (code / 4) % 4;
      slen[2] = code % 4;
      slen[3] = 0;
    } else {
      family = 5;
      code -= 244;
      slen[0] = code / 3;
      slen[1] = code % 3;
      slen[2] = 0;
      slen[3] = 0;
    }
  } else if (code < 400) {
    family = 0;
    slen[0] = code / 80;
    slen[1] = (code / 16) % 5;
    slen[2] = (code / 4) % 4;
    slen[3] = code % 4;
  } else if (code < 500) {
    family = 1;
    code -= 400;
    slen[0] = code / 20;
    slen[1] = (code / 4) % 5;
    slen[2] = code % 4;
    slen[3] = 0;
  } else {
    family = 2;
    code -= 500;
    slen[0] = code / 3;
    slen[1] = code % 3;
    slen[2] = slen[3] = 0;
  }
  for (group = 0; group < 4; ++group) {
    n = partitions[family][kind][group];
    for (i = 0; i < n; ++i) {
      unsigned int v = 0;
      if (at >= 36 || bits.cursor > limit ||
          slen[group] > limit - bits.cursor ||
          !AUDIO_INS(MP3_ReadBits)(&bits, slen[group], &v))
        return STATUS_CONST(INVALID_SEQUENCE);
      raw[at++] = (uint8_t)v;
    }
  }
  if (kind == 0) {
    if (at != 21) return STATUS_CONST(INVALID_SEQUENCE);
    for (i = 0; i < 21; ++i) result.longBands[i] = raw[i];
  } else if (kind == 1) {
    if (at != 36) return STATUS_CONST(INVALID_SEQUENCE);
    for (i = 0; i < 36; ++i)
      result.shortBands[i/3][i%3] = raw[i];
  } else {
    if (at != 33) return STATUS_CONST(INVALID_SEQUENCE);
    for (i = 0; i < 6; ++i) result.longBands[i] = raw[i];
    for (i = 6; i < 33; ++i)
      result.shortBands[3U + (i-6U)/3U][(i-6U)%3U] = raw[i];
  }
  result.part2Bits = (uint16_t)(bits.cursor-offset);
  *outScale = result;
  *outIntensityScale = intensity ?
      (uint8_t)(part->scalefacCompress & 1U) : 0U;
  return STATUS_CONST(SUCCESS);
}
