#pragma once

/* All bit offsets are local to the MPEG Layer III side-information block.
 * This parser never interprets quantized Huffman values as PCM samples. */
typedef struct AUDIO_INS(TMP3Bits) {
  const unsigned char *data;
  size_t bytes;
  size_t cursor;
} AUDIO_INS(TMP3Bits);

static inline bool AUDIO_INS(MP3_ReadBits)(
    AUDIO_INS(TMP3Bits) *bits, unsigned int count, unsigned int *outValue) {
  unsigned int value = 0, n;
  if (!bits || !outValue || count > 16 ||
      bits->bytes > SIZE_MAX / 8 ||
      bits->cursor > bits->bytes * 8 ||
      count > bits->bytes * 8 - bits->cursor) return false;
  for (n = 0; n < count; ++n) {
    size_t position = bits->cursor++;
    value = (value << 1) |
        ((bits->data[position / 8] >> (7U - (position & 7U))) & 1U);
  }
  *outValue = value;
  return true;
}

#define AUDIO_INS_MP3_READ(WIDTH, FIELD)                                      \
  do {                                                                        \
    unsigned int _mp3BitsValue;                                               \
    if (!AUDIO_INS(MP3_ReadBits)(&bits, (WIDTH), &_mp3BitsValue))              \
      return STATUS_CONST(INCOMPLETE_SEQUENCE);                               \
    (FIELD) = _mp3BitsValue;                                                  \
  } while (0)

AUDIO_CODEC_MP3_PARSE_SIDE_INFO_PROTOTYPE {
  const unsigned char *p = (const unsigned char *)frame;
  AUDIO_CODEC_TYPE(TMP3FrameInfo) header;
  AUDIO_CODEC_TYPE(TMP3SideInfo) result = {0};
  AUDIO_INS(TMP3Bits) bits;
  size_t sideStart, required;
  unsigned int gr, ch, band;
  OPSTATUS status;
  if (!p || !outSideInfo) return STATUS_CONST(INVALID_ARGUMENT);
  status = AUDIO_CODEC_FUNC(MP3_ParseHeader)(frame, bytes, &header);
  if (status != STATUS_CONST(SUCCESS)) return status;
  sideStart = 4U + (header.crcProtected ? 2U : 0U);
  required = sideStart + header.sideInfoBytes;
  if (bytes < required) return STATUS_CONST(INCOMPLETE_SEQUENCE);
  bits.data = p + sideStart;
  bits.bytes = header.sideInfoBytes;
  bits.cursor = 0;
  result.granules = header.mpegVersion == 1 ? 2 : 1;
  result.channels = header.channels;
  if (header.mpegVersion == 1) {
    AUDIO_INS_MP3_READ(9, result.mainDataBegin);
    AUDIO_INS_MP3_READ(header.channels == 1 ? 5U : 3U, result.privateBits);
    for (ch = 0; ch < header.channels; ++ch)
      for (band = 0; band < 4; ++band)
        AUDIO_INS_MP3_READ(1, result.scfsi[ch][band]);
  } else {
    AUDIO_INS_MP3_READ(8, result.mainDataBegin);
    AUDIO_INS_MP3_READ(header.channels == 1 ? 1U : 2U, result.privateBits);
  }
  for (gr = 0; gr < result.granules; ++gr) {
    for (ch = 0; ch < result.channels; ++ch) {
      AUDIO_CODEC_TYPE(TMP3Granule) *part = &result.parts[gr][ch];
      unsigned int switched = 0, value;
      AUDIO_INS_MP3_READ(12, part->part23Length);
      AUDIO_INS_MP3_READ(9, part->bigValues);
      AUDIO_INS_MP3_READ(8, part->globalGain);
      AUDIO_INS_MP3_READ(header.mpegVersion == 1 ? 4U : 9U,
                         part->scalefacCompress);
      AUDIO_INS_MP3_READ(1, switched);
      part->windowSwitching = switched != 0;
      if (part->windowSwitching) {
        AUDIO_INS_MP3_READ(2, part->blockType);
        AUDIO_INS_MP3_READ(1, part->mixedBlock);
        AUDIO_INS_MP3_READ(5, part->tableSelect[0]);
        AUDIO_INS_MP3_READ(5, part->tableSelect[1]);
        for (band = 0; band < 3; ++band)
          AUDIO_INS_MP3_READ(3, part->subblockGain[band]);
        part->region0Count = part->blockType == 2 &&
            !part->mixedBlock ? 8 : 7;
        part->region1Count = 20U - part->region0Count;
        if (part->blockType == 0)
          return STATUS_CONST(INVALID_SEQUENCE);
      } else {
        for (band = 0; band < 3; ++band)
          AUDIO_INS_MP3_READ(5, part->tableSelect[band]);
        AUDIO_INS_MP3_READ(4, part->region0Count);
        AUDIO_INS_MP3_READ(3, part->region1Count);
      }
      if (header.mpegVersion == 1)
        AUDIO_INS_MP3_READ(1, part->preflag);
      AUDIO_INS_MP3_READ(1, part->scalefacScale);
      AUDIO_INS_MP3_READ(1, part->count1TableSelect);
      if (part->bigValues > 288 ||
          part->tableSelect[0] == 4 || part->tableSelect[0] == 14 ||
          part->tableSelect[1] == 4 || part->tableSelect[1] == 14 ||
          (!part->windowSwitching &&
            (part->tableSelect[2] == 4 || part->tableSelect[2] == 14)))
        return STATUS_CONST(INVALID_SEQUENCE);
      value = part->part23Length;
      if (result.totalPart23Bits > UINT32_MAX - value)
        return STATUS_CONST(ARITHMETIC_OVERFLOW);
      result.totalPart23Bits += value;
    }
  }
  if (bits.cursor != bits.bytes * 8)
    return STATUS_CONST(INVALID_SEQUENCE);
  result.mainDataBytes = header.frameBytes - (uint32_t)required;
  *outSideInfo = result;
  return STATUS_CONST(SUCCESS);
}

#undef AUDIO_INS_MP3_READ

/* Retain at most 511 bytes of past main data (255 for MPEG-2/2.5).
 * A new frame is assembled as: referenced history + this frame's payload.
 * The returned pointer aliases reservoir.frame and is invalid after the
 * next Feed operation. History advances even when the stream starts mid-
 * reservoir and the current frame cannot be decoded yet. */
AUDIO_CODEC_MP3_RESERVOIR_FEED_PROTOTYPE {
  AUDIO_CODEC_TYPE(TMP3SideInfo) side;
  AUDIO_CODEC_TYPE(TMP3FrameInfo) header;
  const unsigned char *p = (const unsigned char *)frame;
  size_t payloadOffset, payload, used, retain;
  OPSTATUS status;
  if (!reservoir || !frame || !outMainData || !outBytes || !outSideInfo)
    return STATUS_CONST(INVALID_ARGUMENT);
  status = AUDIO_CODEC_FUNC(MP3_ParseHeader)(frame, bytes, &header);
  if (status != STATUS_CONST(SUCCESS)) return status;
  if (bytes < header.frameBytes) return STATUS_CONST(INCOMPLETE_SEQUENCE);
  status = AUDIO_CODEC_FUNC(MP3_ParseSideInfo)(frame, bytes, &side);
  if (status != STATUS_CONST(SUCCESS)) return status;
  payloadOffset = 4U + (header.crcProtected ? 2U : 0U) + header.sideInfoBytes;
  payload = header.frameBytes - payloadOffset;
  if (side.mainDataBegin > sizeof reservoir->history ||
      payload > sizeof reservoir->frame - side.mainDataBegin)
    return STATUS_CONST(OUT_OF_RANGE);
  if (side.mainDataBegin <= reservoir->historySize) {
    memcpy(reservoir->frame,
           reservoir->history + reservoir->historySize - side.mainDataBegin,
           side.mainDataBegin);
    if (payload) memcpy(reservoir->frame + side.mainDataBegin,
                        p + payloadOffset, payload);
    reservoir->frameSize = side.mainDataBegin + payload;
  } else reservoir->frameSize = 0;
  used = reservoir->historySize;
  retain = sizeof reservoir->history;
  if (payload >= retain) {
    memcpy(reservoir->history, p + payloadOffset + payload - retain, retain);
    reservoir->historySize = retain;
  } else {
    size_t preserve = used > retain - payload ? retain - payload : used;
    if (preserve)
      memmove(reservoir->history, reservoir->history + used - preserve, preserve);
    if (payload) memcpy(reservoir->history + preserve, p + payloadOffset, payload);
    reservoir->historySize = preserve + payload;
  }
  *outSideInfo = side;
  if (side.mainDataBegin > used) {
    *outMainData = NULL;
    *outBytes = 0;
    return STATUS_CONST(INCOMPLETE_SEQUENCE);
  }
  *outMainData = reservoir->frame;
  *outBytes = reservoir->frameSize;
  return STATUS_CONST(SUCCESS);
}
