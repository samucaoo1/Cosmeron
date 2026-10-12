#pragma once
#include "../Audio.space"
#include "../PCM/PCM.h"
#include "Impl/FLACFrame.inc"

/* Public standalone lossless decoder. No miniaudio, libm or device backend.
 * The caller owns the decoded PCM and frees it with Audio_PCM_Destroy. */
#define AUDIO_CODEC_FLAC_DECODE_MEMORY_PROTOTYPE \
  static inline OPSTATUS AUDIO_CODEC_FUNC(FLAC_DecodeMemory)( \
      const void *data, size_t size, AUDIO_PCM_TYPE(TBuffer) *outBuffer)

AUDIO_CODEC_FLAC_DECODE_MEMORY_PROTOTYPE;

AUDIO_CODEC_FLAC_DECODE_MEMORY_PROTOTYPE {
  const unsigned char *bytes = (const unsigned char *)data;
  AUDIO_INS(TFLACStreamInfo) metadata = {0};
  float *scratch = NULL;
  size_t offset, total = 0, produced = 0, next, frames, maximum;
  OPSTATUS status;
  if (!data || !outBuffer) return STATUS_CONST(INVALID_ARGUMENT);
  if (outBuffer->samples) return STATUS_CONST(BUSY);
  status = AUDIO_INS(FLAC_ParseMetadata)(data, size, &metadata);
  if (status != STATUS_CONST(SUCCESS)) return status;
  if (metadata.maximumBlockSize > SIZE_MAX / metadata.channels / sizeof(float))
    return STATUS_CONST(ARITHMETIC_OVERFLOW);
  maximum = (size_t)metadata.maximumBlockSize;
  scratch = (float *)malloc(maximum * metadata.channels * sizeof(float));
  if (!scratch) return STATUS_CONST(OUT_OF_MEMORY);
  offset = metadata.frameOffset;
  while (offset < size) {
    frames = next = 0;
    status = AUDIO_INS(FLAC_DecodeFrame)(bytes, size, &metadata,
        offset, scratch, maximum, &frames, &next);
    if (status != STATUS_CONST(SUCCESS)) goto done;
    if (next <= offset || !frames ||
        frames > SIZE_MAX / metadata.channels / sizeof(float) - total) {
      status = STATUS_CONST(ARITHMETIC_OVERFLOW);
      goto done;
    }
    total += frames;
    offset = next;
  }
  if (metadata.totalSamples && metadata.totalSamples != total) {
    status = STATUS_CONST(INVALID_SEQUENCE);
    goto done;
  }
  status = AUDIO_PCM_FUNC(Init)(outBuffer, total,
      metadata.channels, metadata.sampleRate);
  if (status != STATUS_CONST(SUCCESS)) goto done;
  offset = metadata.frameOffset;
  while (offset < size) {
    frames = next = 0;
    status = AUDIO_INS(FLAC_DecodeFrame)(bytes, size, &metadata,
        offset, outBuffer->samples + produced * metadata.channels,
        total - produced, &frames, &next);
    if (status != STATUS_CONST(SUCCESS) || !frames ||
        next <= offset || frames > total - produced) {
      if (status == STATUS_CONST(SUCCESS))
        status = STATUS_CONST(INVALID_SEQUENCE);
      AUDIO_PCM_FUNC(Destroy)(outBuffer);
      goto done;
    }
    produced += frames;
    offset = next;
  }
  if (produced != total) {
    AUDIO_PCM_FUNC(Destroy)(outBuffer);
    status = STATUS_CONST(INVALID_SEQUENCE);
  }
done:
  free(scratch);
  return status;
}

#define AUDIO_CODEC_FLAC_VERIFY_MEMORY_PROTOTYPE \
  static inline OPSTATUS AUDIO_CODEC_FUNC(FLAC_VerifyMemory)( \
      const void *data, size_t size)
AUDIO_CODEC_FLAC_VERIFY_MEMORY_PROTOTYPE {
  AUDIO_INS(TFLACStreamInfo) info = {0};
  AUDIO_INS(TFLACMD5) digest;
  unsigned char actual[16], zero[16] = {0};
  float *scratch;
  size_t offset;
  uint64_t total = 0;
  OPSTATUS status = AUDIO_INS(FLAC_ParseMetadata)(data, size, &info);
  if (status != STATUS_CONST(SUCCESS)) return status;
  if (!memcmp(info.md5, zero, 16)) return STATUS_CONST(NOT_AVAILABLE);
  scratch = (float *)malloc((size_t)info.maximumBlockSize * info.channels * sizeof(float));
  if (!scratch) return STATUS_CONST(OUT_OF_MEMORY);
  AUDIO_INS(FLAC_MD5Init)(&digest);
  offset = info.frameOffset;
  while (offset < size) {
    size_t frames = 0, next = 0;
    status = AUDIO_INS(FLAC_DecodeFrameDigest)((const unsigned char *)data, size,
        &info, offset, scratch, info.maximumBlockSize, &frames, &next, &digest);
    if (status != STATUS_CONST(SUCCESS)) break;
    if (!frames || next <= offset) { status = STATUS_CONST(INVALID_SEQUENCE); break; }
    total += frames;
    offset = next;
  }
  if (status == STATUS_CONST(SUCCESS)) {
    AUDIO_INS(FLAC_MD5Final)(&digest, actual);
    if ((info.totalSamples && total != info.totalSamples) || memcmp(actual, info.md5, 16))
      status = STATUS_CONST(INVALID_SEQUENCE);
  }
  free(scratch);
  return status;
}
