#pragma once

static inline bool AUDIO_INS(MP3_ReadScale)(
    AUDIO_INS(TMP3Bits) *bits, unsigned int length, size_t end, uint8_t *out) {
  unsigned int value = 0;
  if (!out || bits->cursor > end || length > end - bits->cursor ||
      !AUDIO_INS(MP3_ReadBits)(bits, length, &value))
    return false;
  *out = (uint8_t)value;
  return true;
}

/* Part 2 occupies the first bits of part2_3_length, before Huffman data.
 * This decoder handles MPEG-1 scalefactors; MPEG-2/2.5 use distinct
 * low-sampling-frequency scalefactor tables, handled in a later stage. */
AUDIO_CODEC_MP3_PARSE_SCALEFACTORS_PROTOTYPE {
  static const uint8_t slen1[16] = {
    0,0,0,0,3,1,1,1,2,2,2,3,3,3,4,4
  };
  static const uint8_t slen2[16] = {
    0,1,2,3,0,1,2,3,1,2,3,1,2,3,2,3
  };
  static const uint8_t groups[5] = {0,6,11,16,21};
  const AUDIO_CODEC_TYPE(TMP3Granule) *part;
  AUDIO_CODEC_TYPE(TMP3Scalefactors) result = {0};
  AUDIO_INS(TMP3Bits) bits;
  size_t offset = 0, limit, index, j, k;
  unsigned int l1, l2, group;
  if (!mainData || !side || !outScale ||
      side->channels < 1 || side->channels > 2 ||
      granule >= 2 || channel >= side->channels)
    return STATUS_CONST(INVALID_ARGUMENT);
  if (side->granules != 2) return STATUS_CONST(NOT_SUPPORTED);
  if (mainDataBytes > SIZE_MAX / 8) return STATUS_CONST(OUT_OF_RANGE);
  for (j = 0; j < granule; ++j)
    for (k = 0; k < side->channels; ++k)
      offset += side->parts[j][k].part23Length;
  for (k = 0; k < channel; ++k)
    offset += side->parts[granule][k].part23Length;
  part = &side->parts[granule][channel];
  if (part->scalefacCompress > 15 || offset > mainDataBytes * 8 ||
      part->part23Length > mainDataBytes * 8 - offset)
    return STATUS_CONST(INCOMPLETE_SEQUENCE);
  limit = offset + part->part23Length;
  bits.data = mainData;
  bits.bytes = mainDataBytes;
  bits.cursor = offset;
  l1 = slen1[part->scalefacCompress];
  l2 = slen2[part->scalefacCompress];
  if (part->windowSwitching && part->blockType == 2) {
    index = part->mixedBlock ? 3 : 0;
    if (part->mixedBlock)
      for (j = 0; j < 8; ++j)
        if (!AUDIO_INS(MP3_ReadScale)(&bits, l1, limit, &result.longBands[j]))
          return STATUS_CONST(INVALID_SEQUENCE);
    for (j = index; j < 12; ++j) {
      unsigned int width = j < 6 ? l1 : l2;
      for (k = 0; k < 3; ++k)
        if (!AUDIO_INS(MP3_ReadScale)(&bits, width, limit,
                                    &result.shortBands[j][k]))
          return STATUS_CONST(INVALID_SEQUENCE);
    }
  } else {
    for (group = 0; group < 4; ++group) {
      const bool reuse = granule == 1 && side->scfsi[channel][group] != 0;
      unsigned int width = group < 2 ? l1 : l2;
      if (reuse && !firstGranule) return STATUS_CONST(INVALID_ARGUMENT);
      for (j = groups[group]; j < groups[group + 1]; ++j) {
        if (reuse) result.longBands[j] = firstGranule->longBands[j];
        else if (!AUDIO_INS(MP3_ReadScale)(&bits, width, limit,
                                          &result.longBands[j]))
          return STATUS_CONST(INVALID_SEQUENCE);
      }
    }
  }
  result.part2Bits = (uint16_t)(bits.cursor - offset);
  *outScale = result;
  return STATUS_CONST(SUCCESS);
}
