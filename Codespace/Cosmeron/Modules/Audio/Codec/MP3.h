#pragma once
/* Native MPEG Layer III framing. This is not yet an MP3 PCM decoder.
 * C11, header-only, independent of vendor decoders and external linking. */
#include "../Audio.space"

typedef struct AUDIO_CODEC_TYPE(TMP3FrameInfo) {
  uint32_t sampleRate;
  uint32_t bitRate;
  uint32_t frameBytes;
  uint16_t samplesPerFrame;
  uint8_t channels;
  uint8_t mpegVersion; /* MPEG-1 = 1, MPEG-2 = 2, MPEG-2.5 = 25. */
  uint8_t sideInfoBytes;
  bool crcProtected;
} AUDIO_CODEC_TYPE(TMP3FrameInfo);

#define AUDIO_CODEC_MP3_PARSE_HEADER_PROTOTYPE                                 \
  static inline OPSTATUS AUDIO_CODEC_FUNC(MP3_ParseHeader)(                   \
      const void *bytes, size_t size, AUDIO_CODEC_TYPE(TMP3FrameInfo) *outInfo)
#define AUDIO_CODEC_MP3_SKIP_ID3_PROTOTYPE                                     \
  static inline OPSTATUS AUDIO_CODEC_FUNC(MP3_SkipID3)(                        \
      const void *bytes, size_t size, size_t *outStart)
#define AUDIO_CODEC_MP3_FIND_FRAME_PROTOTYPE                                   \
  static inline OPSTATUS AUDIO_CODEC_FUNC(MP3_FindFrame)(                     \
      const void *bytes, size_t size, size_t start, size_t *outOffset,          \
      AUDIO_CODEC_TYPE(TMP3FrameInfo) *outInfo)

AUDIO_CODEC_MP3_PARSE_HEADER_PROTOTYPE;
AUDIO_CODEC_MP3_SKIP_ID3_PROTOTYPE;
AUDIO_CODEC_MP3_FIND_FRAME_PROTOTYPE;

AUDIO_CODEC_MP3_PARSE_HEADER_PROTOTYPE {
  static const uint16_t mpeg1Bitrates[16] = {
    0,32,40,48,56,64,80,96,112,128,160,192,224,256,320,0
  };
  static const uint16_t mpeg2Bitrates[16] = {
    0,8,16,24,32,40,48,56,64,80,96,112,128,144,160,0
  };
  static const uint32_t rates[3] = {44100,48000,32000};
  const unsigned char *p = (const unsigned char *)bytes;
  AUDIO_CODEC_TYPE(TMP3FrameInfo) frame = {0};
  uint32_t header, frameBytes;
  unsigned int versionBits, layer, bitIndex, rateIndex, mode, padding;
  uint32_t kiloBits;
  if (!p || !outInfo) return STATUS_CONST(INVALID_ARGUMENT);
  if (size < 4) return STATUS_CONST(INCOMPLETE_SEQUENCE);
  header = ((uint32_t)p[0] << 24) | ((uint32_t)p[1] << 16) |
           ((uint32_t)p[2] << 8) | (uint32_t)p[3];
  if ((header & UINT32_C(0xffe00000)) != UINT32_C(0xffe00000))
    return STATUS_CONST(INVALID_SEQUENCE);
  versionBits = (header >> 19) & 3U;
  layer = (header >> 17) & 3U;
  bitIndex = (header >> 12) & 15U;
  rateIndex = (header >> 10) & 3U;
  padding = (header >> 9) & 1U;
  mode = (header >> 6) & 3U;
  if (versionBits == 1 || !layer || rateIndex == 3 || bitIndex == 15)
    return STATUS_CONST(INVALID_SEQUENCE);
  if (layer != 1 || bitIndex == 0) return STATUS_CONST(NOT_SUPPORTED);
  frame.mpegVersion = versionBits == 3 ? 1 : versionBits == 2 ? 2 : 25;
  frame.sampleRate = rates[rateIndex] /
      (versionBits == 3 ? 1U : versionBits == 2 ? 2U : 4U);
  kiloBits = versionBits == 3 ?
      mpeg1Bitrates[bitIndex] : mpeg2Bitrates[bitIndex];
  frame.bitRate = kiloBits * 1000U;
  frame.samplesPerFrame = versionBits == 3 ? 1152 : 576;
  frameBytes = (versionBits == 3 ? UINT32_C(144000) : UINT32_C(72000)) *
               kiloBits / frame.sampleRate + padding;
  frame.channels = mode == 3 ? 1 : 2;
  frame.sideInfoBytes = versionBits == 3 ?
      (frame.channels == 1 ? 17 : 32) :
      (frame.channels == 1 ? 9 : 17);
  frame.crcProtected = ((header >> 16) & 1U) == 0U;
  if (frameBytes < 4U + (frame.crcProtected ? 2U : 0U) + frame.sideInfoBytes)
    return STATUS_CONST(INVALID_SEQUENCE);
  frame.frameBytes = frameBytes;
  *outInfo = frame;
  return STATUS_CONST(SUCCESS);
}

/* The ID3v2 size excludes the ten-byte tag header; v2.4 optionally adds
 * a ten-byte footer. Accept only synchsafe size octets and versions 2-4. */
AUDIO_CODEC_MP3_SKIP_ID3_PROTOTYPE {
  const unsigned char *p = (const unsigned char *)bytes;
  size_t total, length;
  unsigned int i;
  if (!p || !outStart) return STATUS_CONST(INVALID_ARGUMENT);
  if (size < 3 || memcmp(p, "ID3", 3)) {
    *outStart = 0;
    return STATUS_CONST(SUCCESS);
  }
  if (size < 10) return STATUS_CONST(INCOMPLETE_SEQUENCE);
  if (p[3] < 2 || p[3] > 4 || p[4] == 0xff)
    return STATUS_CONST(NOT_SUPPORTED);
  length = 0;
  for (i = 6; i < 10; ++i) {
    if (p[i] & 0x80U) return STATUS_CONST(INVALID_SEQUENCE);
    length = (length << 7) | p[i];
  }
  total = 10 + length + ((p[3] == 4 && (p[5] & 0x10U)) ? 10U : 0U);
  if (total > size) return STATUS_CONST(INCOMPLETE_SEQUENCE);
  *outStart = total;
  return STATUS_CONST(SUCCESS);
}

/* Recover from optional ID3v2 and non-audio padding without false
 * claims of decoding: a candidate must fit a complete MPEG III frame. */
AUDIO_CODEC_MP3_FIND_FRAME_PROTOTYPE {
  const unsigned char *p = (const unsigned char *)bytes;
  size_t at;
  if (!p || !outOffset || !outInfo || start > size)
    return STATUS_CONST(INVALID_ARGUMENT);
  if (start == 0) {
    OPSTATUS status = AUDIO_CODEC_FUNC(MP3_SkipID3)(bytes, size, &start);
    if (status != STATUS_CONST(SUCCESS)) return status;
  }
  for (at = start; at <= size && size - at >= 4; ++at) {
    AUDIO_CODEC_TYPE(TMP3FrameInfo) candidate;
    if (p[at] != 0xff || (p[at + 1] & 0xe0U) != 0xe0U) continue;
    if (AUDIO_CODEC_FUNC(MP3_ParseHeader)(p + at, size - at, &candidate) !=
        STATUS_CONST(SUCCESS)) continue;
    if (candidate.frameBytes > size - at) continue;
    *outOffset = at;
    *outInfo = candidate;
    return STATUS_CONST(SUCCESS);
  }
  return STATUS_CONST(NOT_FOUND);
}
