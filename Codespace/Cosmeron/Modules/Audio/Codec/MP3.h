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
  uint8_t channelMode;     /* 0=stereo, 1=joint, 2=dual, 3=mono. */
  uint8_t modeExtension;   /* meaningful only in joint stereo. */
  bool msStereo, intensityStereo;
  uint8_t mpegVersion; /* MPEG-1 = 1, MPEG-2 = 2, MPEG-2.5 = 25. */
  uint8_t sideInfoBytes;
  bool crcProtected;
} AUDIO_CODEC_TYPE(TMP3FrameInfo);

/* Optional Xing/Info metadata from an MPEG Layer III first frame.
 * Values are raw bitstream fields: they are not yet gapless output count. */
typedef struct AUDIO_CODEC_TYPE(TMP3XingInfo) {
  uint32_t encodedFrames, encodedBytes, vbrScale;
  uint16_t encoderDelay, endPadding;
  uint8_t flags;
  bool variableBitrate, hasTOC, hasGapless;
} AUDIO_CODEC_TYPE(TMP3XingInfo);

#define AUDIO_CODEC_MP3_PARSE_XING_PROTOTYPE                                 \
  static inline OPSTATUS AUDIO_CODEC_FUNC(MP3_ParseXing)(                    \
      const void *frame, size_t length,                                      \
      AUDIO_CODEC_TYPE(TMP3XingInfo) *outInfo)

AUDIO_CODEC_MP3_PARSE_XING_PROTOTYPE;

/* Hybrid/synthesis decoder priming beyond LAME's encoded delay field.
 * This is relevant to raw MPEG Layer III decode-to-gapless mapping. */
enum { AUDIO_CODEC_CONST(MP3_DECODER_PRIMING) = 529 };

#define AUDIO_CODEC_MP3_GAPLESS_SPAN_PROTOTYPE                                 \
  static inline OPSTATUS AUDIO_CODEC_FUNC(MP3_GaplessSpan)(                  \
      const AUDIO_CODEC_TYPE(TMP3XingInfo) *xing,                            \
      uint16_t samplesPerFrame, uint32_t metadataFrames,                     \
      uint64_t *outFirstPCM, uint64_t *outFrames)

AUDIO_CODEC_MP3_GAPLESS_SPAN_PROTOTYPE;

/* A granule stores the side-info parameters required by the subsequent
 * Huffman/scalefactor and synthesis stages; no PCM decoding occurs here. */
typedef struct AUDIO_CODEC_TYPE(TMP3Granule) {
  uint32_t part23Length;
  uint16_t bigValues, scalefacCompress;
  uint8_t globalGain, blockType, mixedBlock, windowSwitching;
  uint8_t tableSelect[3], subblockGain[3];
  uint8_t region0Count, region1Count;
  uint8_t preflag, scalefacScale, count1TableSelect;
} AUDIO_CODEC_TYPE(TMP3Granule);

typedef struct AUDIO_CODEC_TYPE(TMP3SideInfo) {
  uint16_t mainDataBegin;
  uint8_t privateBits, channels, granules;
  uint8_t scfsi[2][4];
  uint32_t totalPart23Bits, mainDataBytes;
  AUDIO_CODEC_TYPE(TMP3Granule) parts[2][2];
} AUDIO_CODEC_TYPE(TMP3SideInfo);

typedef struct AUDIO_CODEC_TYPE(TMP3Scalefactors) {
  uint8_t longBands[22];
  uint8_t shortBands[13][3];
  uint16_t part2Bits;
  /* LSF right-channel intensity positions after no-intensity sentinel
   * expansion: 255 means that the band carries ordinary L/R or MS.
   * The ordinary scalefactor arrays remain untouched for requantization. */
  uint8_t lsfIntensityLong[22];
  uint8_t lsfIntensityShort[13][3];
} AUDIO_CODEC_TYPE(TMP3Scalefactors);

#define AUDIO_CODEC_MP3_PARSE_SCALEFACTORS_PROTOTYPE                          \
  static inline OPSTATUS AUDIO_CODEC_FUNC(MP3_ParseScalefactors)(             \
      const unsigned char *mainData, size_t mainDataBytes,                    \
      const AUDIO_CODEC_TYPE(TMP3SideInfo) *side,                              \
      unsigned int granule, unsigned int channel,                              \
      const AUDIO_CODEC_TYPE(TMP3Scalefactors) *firstGranule,                  \
      AUDIO_CODEC_TYPE(TMP3Scalefactors) *outScale)

AUDIO_CODEC_MP3_PARSE_SCALEFACTORS_PROTOTYPE;

#define AUDIO_CODEC_MP3_SCALEFACTORS_LSF_PROTOTYPE                             \
  static inline OPSTATUS AUDIO_CODEC_FUNC(MP3_ParseScalefactorsLSF)(          \
      const unsigned char *mainData, size_t mainDataBytes,                    \
      const AUDIO_CODEC_TYPE(TMP3FrameInfo) *header,                           \
      const AUDIO_CODEC_TYPE(TMP3SideInfo) *side, unsigned int channel,       \
      AUDIO_CODEC_TYPE(TMP3Scalefactors) *outScale,                            \
      uint8_t *outIntensityScale)

AUDIO_CODEC_MP3_SCALEFACTORS_LSF_PROTOTYPE;

#define AUDIO_CODEC_MP3_HUFFMAN_PAIR_PROTOTYPE                                 \
  static inline OPSTATUS AUDIO_CODEC_FUNC(MP3_HuffmanPair)(                   \
      const unsigned char *data, size_t size, size_t *ioBit, size_t endBit,    \
      unsigned int table, int32_t *outX, int32_t *outY)
#define AUDIO_CODEC_MP3_HUFFMAN_COUNT1_PROTOTYPE                               \
  static inline OPSTATUS AUDIO_CODEC_FUNC(MP3_HuffmanCount1)(                 \
      const unsigned char *data, size_t size, size_t *ioBit, size_t endBit,    \
      unsigned int table, int32_t outQuad[4])

AUDIO_CODEC_MP3_HUFFMAN_PAIR_PROTOTYPE;
AUDIO_CODEC_MP3_HUFFMAN_COUNT1_PROTOTYPE;

/* 1024-entry synthesis delay, separate from the 32x18 IMDCT overlap.
 * The owner retains one zero-initialized state per audio channel. */
typedef struct AUDIO_CODEC_TYPE(TMP3SynthesisState) {
  float delay[1024];
} AUDIO_CODEC_TYPE(TMP3SynthesisState);

#define AUDIO_CODEC_MP3_SYNTHESIS_SLOT_PROTOTYPE                              \
  static inline OPSTATUS AUDIO_CODEC_FUNC(MP3_SynthesisSlot)(                \
      const float subbands[32],                                              \
      AUDIO_CODEC_TYPE(TMP3SynthesisState) *state, float outPCM[32])
#define AUDIO_CODEC_MP3_SYNTHESIZE_PROTOTYPE                                  \
  static inline OPSTATUS AUDIO_CODEC_FUNC(MP3_Synthesize)(                   \
      const float subbandTime[576],                                          \
      AUDIO_CODEC_TYPE(TMP3SynthesisState) *state, float outPCM[576])

AUDIO_CODEC_MP3_SYNTHESIS_SLOT_PROTOTYPE;
AUDIO_CODEC_MP3_SYNTHESIZE_PROTOTYPE;

/* One zero-initialized state per channel, retained across granules.
 * No Init/Free or opaque vendor object is required. */
typedef struct AUDIO_CODEC_TYPE(TMP3HybridState) {
  float overlap[32][18];
} AUDIO_CODEC_TYPE(TMP3HybridState);

#define AUDIO_CODEC_MP3_IMDCT_PROTOTYPE                                       \
  static inline OPSTATUS AUDIO_CODEC_FUNC(MP3_IMDCT)(                         \
      const float spectrum[576],                                              \
      const AUDIO_CODEC_TYPE(TMP3Granule) *part,                               \
      AUDIO_CODEC_TYPE(TMP3HybridState) *state, float outSubband[576])
#define AUDIO_CODEC_MP3_FREQUENCY_INVERT_PROTOTYPE                             \
  static inline OPSTATUS AUDIO_CODEC_FUNC(MP3_FrequencyInvert)(               \
      const float input[576], float output[576])

AUDIO_CODEC_MP3_IMDCT_PROTOTYPE;
AUDIO_CODEC_MP3_FREQUENCY_INVERT_PROTOTYPE;

#define AUDIO_CODEC_MP3_INTENSITY_BAND_PROTOTYPE                             \
  static inline OPSTATUS AUDIO_CODEC_FUNC(MP3_IntensityBand)(                 \
      const float left[576], const float right[576],                           \
      size_t first, size_t last, unsigned int isPosition,                     \
      float outLeft[576], float outRight[576])

AUDIO_CODEC_MP3_INTENSITY_BAND_PROTOTYPE;

#define AUDIO_CODEC_MP3_JOINT_STEREO_PROTOTYPE                                \
  static inline OPSTATUS AUDIO_CODEC_FUNC(MP3_JointStereo)(                   \
      const AUDIO_CODEC_TYPE(TMP3FrameInfo) *header,                          \
      const AUDIO_CODEC_TYPE(TMP3Granule) *rightPart,                         \
      const AUDIO_CODEC_TYPE(TMP3Scalefactors) *rightScale,                   \
      const float left[576], const float right[576],                           \
      float outLeft[576], float outRight[576])

AUDIO_CODEC_MP3_JOINT_STEREO_PROTOTYPE;

#define AUDIO_CODEC_MP3_REORDER_SHORT_PROTOTYPE                            \
  static inline OPSTATUS AUDIO_CODEC_FUNC(MP3_ReorderShort)(                  \
      const float input[576], const AUDIO_CODEC_TYPE(TMP3Granule) *part,     \
      uint32_t sampleRate, float output[576])
#define AUDIO_CODEC_MP3_STEREO_MS_PROTOTYPE                                \
  static inline OPSTATUS AUDIO_CODEC_FUNC(MP3_StereoMS)(                     \
      const float mid[576], const float side[576],                          \
      size_t first, size_t last, float outLeft[576], float outRight[576])
#define AUDIO_CODEC_MP3_ANTIALIAS_PROTOTYPE                                \
  static inline OPSTATUS AUDIO_CODEC_FUNC(MP3_Antialias)(                    \
      const float input[576], const AUDIO_CODEC_TYPE(TMP3Granule) *part,    \
      float output[576])

AUDIO_CODEC_MP3_REORDER_SHORT_PROTOTYPE;
AUDIO_CODEC_MP3_STEREO_MS_PROTOTYPE;
AUDIO_CODEC_MP3_ANTIALIAS_PROTOTYPE;

#define AUDIO_CODEC_MP3_REQUANTIZE_PROTOTYPE                                 \
  static inline OPSTATUS AUDIO_CODEC_FUNC(MP3_Requantize)(                    \
      const int32_t quantized[576],                                           \
      const AUDIO_CODEC_TYPE(TMP3Granule) *part,                              \
      const AUDIO_CODEC_TYPE(TMP3Scalefactors) *scales,                        \
      uint32_t sampleRate, float outSpectral[576])

AUDIO_CODEC_MP3_REQUANTIZE_PROTOTYPE;

#define AUDIO_CODEC_MP3_HUFFMAN_GRANULE_PROTOTYPE                              \
  static inline OPSTATUS AUDIO_CODEC_FUNC(MP3_HuffmanGranule)(                \
      const unsigned char *mainData, size_t mainDataBytes,                    \
      const AUDIO_CODEC_TYPE(TMP3SideInfo) *side,                              \
      const AUDIO_CODEC_TYPE(TMP3Scalefactors) *scales,                        \
      unsigned int granule, unsigned int channel, uint32_t sampleRate,        \
      int32_t outValues[576], size_t *outCount)

AUDIO_CODEC_MP3_HUFFMAN_GRANULE_PROTOTYPE;

/* Plain zero-initialized state; no init/free call, allocator or globals. */
typedef struct AUDIO_CODEC_TYPE(TMP3Reservoir) {
  unsigned char history[511];
  size_t historySize;
  unsigned char frame[2048];
  size_t frameSize;
} AUDIO_CODEC_TYPE(TMP3Reservoir);

#define AUDIO_CODEC_MP3_PARSE_SIDE_INFO_PROTOTYPE                             \
  static inline OPSTATUS AUDIO_CODEC_FUNC(MP3_ParseSideInfo)(                 \
      const void *frame, size_t bytes, AUDIO_CODEC_TYPE(TMP3SideInfo) *outSideInfo)
#define AUDIO_CODEC_MP3_RESERVOIR_FEED_PROTOTYPE                              \
  static inline OPSTATUS AUDIO_CODEC_FUNC(MP3_ReservoirFeed)(                 \
      AUDIO_CODEC_TYPE(TMP3Reservoir) *reservoir, const void *frame,          \
      size_t bytes, const unsigned char **outMainData, size_t *outBytes,       \
      AUDIO_CODEC_TYPE(TMP3SideInfo) *outSideInfo)

AUDIO_CODEC_MP3_PARSE_SIDE_INFO_PROTOTYPE;
AUDIO_CODEC_MP3_RESERVOIR_FEED_PROTOTYPE;

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
  frame.channelMode = (uint8_t)mode;
  frame.modeExtension = mode == 1 ?
      (uint8_t)((header >> 4) & 3U) : 0U;
  frame.msStereo = mode == 1 && (frame.modeExtension & 2U) != 0;
  frame.intensityStereo = mode == 1 &&
      (frame.modeExtension & 1U) != 0;
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

#include "Impl/MP3SideInfo.inc"
#include "Impl/MP3Scalefactors.inc"
#include "Impl/MP3ScalefactorsLSF.inc"
#include "Impl/MP3Huffman.inc"
#include "Impl/MP3Requant.inc"
#include "Impl/MP3Hybrid.inc"
#include "Impl/MP3IMDCT.inc"
#include "Impl/MP3Synthesis.inc"
#include "Impl/MP3Intensity.inc"
#include "Impl/MP3JointStereo.inc"
#include "Impl/MP3Gapless.inc"
