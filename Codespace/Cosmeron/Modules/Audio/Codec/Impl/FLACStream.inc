#pragma once
#include "../../Audio.space"

typedef struct AUDIO_INS(TFLACStreamInfo) {
  uint32_t sampleRate;
  uint32_t channels;
  uint32_t bitsPerSample;
  uint32_t minimumBlockSize;
  uint32_t maximumBlockSize;
  uint64_t totalSamples;
  size_t frameOffset;
  unsigned char md5[16];
} AUDIO_INS(TFLACStreamInfo);

static inline uint32_t AUDIO_INS(FLAC_BigEndian24)(const unsigned char *p) {
  return ((uint32_t)p[0] << 16) | ((uint32_t)p[1] << 8) | p[2];
}
static inline uint64_t AUDIO_INS(FLAC_BigEndian64)(const unsigned char *p) {
  uint64_t value = 0;
  unsigned int i;
  for (i = 0; i < 8; ++i) value = (value << 8) | p[i];
  return value;
}

/* Parses all FLAC metadata without allocating or invoking the vendor codec.
 * The returned frameOffset points to the first audio frame after the final
 * metadata block; the caller keeps the input memory alive while using it. */
static inline OPSTATUS AUDIO_INS(FLAC_ParseMetadata)(
    const void *source, size_t size, AUDIO_INS(TFLACStreamInfo) *outInfo) {
  const unsigned char *data = (const unsigned char *)source;
  AUDIO_INS(TFLACStreamInfo) result = {0};
  size_t offset = 4;
  bool foundStreamInfo = false, last = false;
  if (!source || !outInfo) return STATUS_CONST(INVALID_ARGUMENT);
  if (size < 4) return STATUS_CONST(INCOMPLETE_SEQUENCE);
  if (memcmp(data, "fLaC", 4))
    return STATUS_CONST(NOT_SUPPORTED);
  do {
    uint8_t kind;
    uint32_t length;
    const unsigned char *block;
    if (size - offset < 4) return STATUS_CONST(INCOMPLETE_SEQUENCE);
    last = (data[offset] & 0x80U) != 0;
    kind = data[offset] & 0x7fU;
    length = AUDIO_INS(FLAC_BigEndian24)(data + offset + 1);
    offset += 4;
    if (length > size - offset) return STATUS_CONST(INCOMPLETE_SEQUENCE);
    block = data + offset;
    if (!foundStreamInfo && kind != 0)
      return STATUS_CONST(INVALID_SEQUENCE);
    if (kind == 0) {
      uint64_t packed;
      if (foundStreamInfo || length != 34)
        return STATUS_CONST(INVALID_SEQUENCE);
      result.minimumBlockSize = ((uint32_t)block[0] << 8) | block[1];
      result.maximumBlockSize = ((uint32_t)block[2] << 8) | block[3];
      packed = AUDIO_INS(FLAC_BigEndian64)(block + 10);
      result.sampleRate = (uint32_t)((packed >> 44) & UINT64_C(0xfffff));
      result.channels = (uint32_t)(((packed >> 41) & 7U) + 1U);
      result.bitsPerSample = (uint32_t)(((packed >> 36) & 31U) + 1U);
      result.totalSamples = packed & UINT64_C(0xfffffffff);
      memcpy(result.md5, block + 18, sizeof result.md5);
      if (result.minimumBlockSize < 16 ||
          result.maximumBlockSize < result.minimumBlockSize ||
          result.bitsPerSample < 4 || result.bitsPerSample > 32)
        return STATUS_CONST(INVALID_SEQUENCE);
      if (!AUDIO_INS(ValidFormat)(result.sampleRate, result.channels))
        return STATUS_CONST(NOT_SUPPORTED);
      foundStreamInfo = true;
    } else if (kind == 127) {
      return STATUS_CONST(INVALID_SEQUENCE);
    }
    offset += length;
  } while (!last);
  if (!foundStreamInfo) return STATUS_CONST(INVALID_SEQUENCE);
  result.frameOffset = offset;
  *outInfo = result;
  return STATUS_CONST(SUCCESS);
}
