#pragma once
/*
 * Pure C11 RIFF/WAVE PCM decoder and writer. No Miniaudio, libm, pthread.
 * Decodes unsigned PCM8, signed PCM16/24/32 and IEEE float32 to interleaved
 * f32. The writer deliberately retains PCM16 output for compatibility.
 * Intentionally rejects unsupported formats instead of guessing.
 */
#include "../Audio.space"
#include "../PCM/PCM.h"
#include <stdio.h>
#include <float.h>

#define AUDIO_CODEC_WAV_DECODE_MEMORY_PROTOTYPE                                \
  static inline OPSTATUS AUDIO_CODEC_FUNC(WAV_DecodeMemory)(                  \
      const void *data, size_t size, AUDIO_PCM_TYPE(TBuffer) *outBuffer)
#define AUDIO_CODEC_WAV_WRITE_FILE_PROTOTYPE                                   \
  static inline OPSTATUS AUDIO_CODEC_FUNC(WAV_WriteFile)(                     \
      const char *path, const AUDIO_PCM_TYPE(TBuffer) *buffer)

AUDIO_CODEC_WAV_DECODE_MEMORY_PROTOTYPE;
AUDIO_CODEC_WAV_WRITE_FILE_PROTOTYPE;

static inline uint16_t AUDIO_INS(WAV_U16)(const unsigned char *p) {
  return (uint16_t)((uint16_t)p[0] | ((uint16_t)p[1] << 8));
}
static inline uint32_t AUDIO_INS(WAV_U32)(const unsigned char *p) {
  return (uint32_t)p[0] | ((uint32_t)p[1] << 8) |
         ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24);
}
static inline void AUDIO_INS(WAV_Store16)(unsigned char *p, uint16_t n) {
  p[0] = (unsigned char)n;
  p[1] = (unsigned char)(n >> 8);
}
static inline void AUDIO_INS(WAV_Store32)(unsigned char *p, uint32_t n) {
  p[0] = (unsigned char)n;
  p[1] = (unsigned char)(n >> 8);
  p[2] = (unsigned char)(n >> 16);
  p[3] = (unsigned char)(n >> 24);
}

AUDIO_CODEC_WAV_DECODE_MEMORY_PROTOTYPE {
  const unsigned char *p = (const unsigned char *)data;
  const unsigned char *audioData = NULL;
  size_t offset = 12, riffEnd, audioBytes = 0, frames, count, i;
  uint16_t format = 0, channels = 0, bits = 0, align = 0;
  uint32_t rate = 0;
  bool foundFmt = false, foundData = false;
  OPSTATUS result;
  if (p == NULL || size < 12 || outBuffer == NULL)
    return STATUS_CONST(INVALID_ARGUMENT);
  if (outBuffer->samples != NULL)
    return STATUS_CONST(BUSY);
  if (memcmp(p, "RIFF", 4) != 0 || memcmp(p + 8, "WAVE", 4) != 0)
    return STATUS_CONST(INVALID_ARGUMENT);
  if (AUDIO_INS(WAV_U32)(p + 4) < 4u ||
      (size_t)AUDIO_INS(WAV_U32)(p + 4) > size - 8u)
    return STATUS_CONST(INVALID_ARGUMENT);
  riffEnd = (size_t)AUDIO_INS(WAV_U32)(p + 4) + 8u;
  while (offset < riffEnd) {
    size_t n, advance;
    if (riffEnd - offset < 8u) return STATUS_CONST(INVALID_ARGUMENT);
    n = (size_t)AUDIO_INS(WAV_U32)(p + offset + 4u);
    offset += 8u;
    if (n > riffEnd - offset) return STATUS_CONST(INVALID_ARGUMENT);
    if (memcmp(p + offset - 8u, "fmt ", 4) == 0 && !foundFmt) {
      if (n < 16u) return STATUS_CONST(INVALID_ARGUMENT);
      format = AUDIO_INS(WAV_U16)(p + offset);
      channels = AUDIO_INS(WAV_U16)(p + offset + 2);
      rate = AUDIO_INS(WAV_U32)(p + offset + 4);
      align = AUDIO_INS(WAV_U16)(p + offset + 12);
      bits = AUDIO_INS(WAV_U16)(p + offset + 14);
      foundFmt = true;
    } else if (memcmp(p + offset - 8u, "data", 4) == 0 && !foundData) {
      audioData = p + offset;
      audioBytes = n;
      foundData = true;
    }
    advance = n + (n & 1u);
    if (advance > riffEnd - offset) {
      if (n == riffEnd - offset) advance = n; /* tolerate unpadded last chunk */
      else return STATUS_CONST(INVALID_ARGUMENT);
    }
    offset += advance;
  }
  if (!foundFmt || !foundData)
    return STATUS_CONST(INVALID_ARGUMENT);
  if (format == 1u) {
    if (bits != 8u && bits != 16u && bits != 24u && bits != 32u)
      return STATUS_CONST(NOT_SUPPORTED);
  } else if (format == 3u) {
    /* IEEE binary32 only. Do not reinterpret arbitrary host float layouts. */
    if (bits != 32u || sizeof(float) != 4 ||
        FLT_RADIX != 2 || FLT_MANT_DIG != 24 || FLT_MAX_EXP != 128)
      return STATUS_CONST(NOT_SUPPORTED);
  } else {
    return STATUS_CONST(NOT_SUPPORTED);
  }
  if (!AUDIO_INS(ValidFormat)(rate, channels) ||
      align != channels * (uint16_t)(bits / 8u) ||
      audioBytes % align != 0)
    return STATUS_CONST(INVALID_ARGUMENT);
  frames = audioBytes / align;
  if (frames > SIZE_MAX / channels)
    return STATUS_CONST(ARITHMETIC_OVERFLOW);
  count = frames * channels;
  result = AUDIO_PCM_FUNC(Init)(outBuffer, frames, channels, rate);
  if (result != STATUS_CONST(SUCCESS)) return result;
  for (i = 0; i < count; ++i) {
    if (format == 3u) {
      uint32_t raw = AUDIO_INS(WAV_U32)(audioData + 4u * i);
      float decoded;
      memcpy(&decoded, &raw, sizeof decoded);
      if (!AUDIO_INS(Finite)(decoded)) {
        AUDIO_PCM_FUNC(Destroy)(outBuffer);
        return STATUS_CONST(INVALID_ARGUMENT);
      }
      outBuffer->samples[i] = decoded;
    } else if (bits == 8u) {
      outBuffer->samples[i] = ((float)audioData[i] - 128.0f) / 128.0f;
    } else if (bits == 16u) {
      uint16_t raw = AUDIO_INS(WAV_U16)(audioData + 2u * i);
      int32_t v = raw >= 0x8000u ? (int32_t)raw - 65536 : (int32_t)raw;
      outBuffer->samples[i] = (float)v / 32768.0f;
    } else if (bits == 24u) {
      const unsigned char *q = audioData + 3u * i;
      uint32_t raw = (uint32_t)q[0] | ((uint32_t)q[1] << 8) |
                     ((uint32_t)q[2] << 16);
      int32_t v = (raw & 0x800000u) ? (int32_t)raw - 16777216 : (int32_t)raw;
      outBuffer->samples[i] = (float)v / 8388608.0f;
    } else { /* signed PCM32: use int64_t to avoid implementation-defined casts */
      uint32_t raw = AUDIO_INS(WAV_U32)(audioData + 4u * i);
      int64_t v = raw >= 0x80000000u ?
        (int64_t)raw - INT64_C(4294967296) : (int64_t)raw;
      outBuffer->samples[i] = (float)v / 2147483648.0f;
    }
  }
  return STATUS_CONST(SUCCESS);
}

AUDIO_CODEC_WAV_WRITE_FILE_PROTOTYPE {
  unsigned char header[44] = {0};
  FILE *out;
  size_t count, i;
  uint32_t bytes;
  bool ok;
  if (path == NULL || buffer == NULL ||
      !AUDIO_INS(ValidFormat)(buffer->sampleRate, buffer->channels) ||
      (buffer->frameCount != 0 && buffer->samples == NULL))
    return STATUS_CONST(INVALID_ARGUMENT);
  if (buffer->frameCount > (UINT32_MAX - 36u) /
                               (size_t)buffer->channels / 2u ||
      buffer->sampleRate > UINT32_MAX / buffer->channels / 2u)
    return STATUS_CONST(OUT_OF_RANGE);
  count = buffer->frameCount * buffer->channels;
  for (i = 0; i < count; ++i)
    if (!AUDIO_INS(Finite)(buffer->samples[i]))
      return STATUS_CONST(INVALID_ARGUMENT);
  bytes = (uint32_t)(count * 2u);
  memcpy(header, "RIFF", 4);
  AUDIO_INS(WAV_Store32)(header + 4, bytes + 36u);
  memcpy(header + 8, "WAVEfmt ", 8);
  AUDIO_INS(WAV_Store32)(header + 16, 16);
  AUDIO_INS(WAV_Store16)(header + 20, 1);
  AUDIO_INS(WAV_Store16)(header + 22, (uint16_t)buffer->channels);
  AUDIO_INS(WAV_Store32)(header + 24, buffer->sampleRate);
  AUDIO_INS(WAV_Store32)(header + 28,
      buffer->sampleRate * buffer->channels * 2u);
  AUDIO_INS(WAV_Store16)(header + 32, (uint16_t)(buffer->channels * 2u));
  AUDIO_INS(WAV_Store16)(header + 34, 16);
  memcpy(header + 36, "data", 4);
  AUDIO_INS(WAV_Store32)(header + 40, bytes);
  out = fopen(path, "wb");
  if (out == NULL) return STATUS_CONST(GENERIC_ERROR);
  ok = fwrite(header, 1, sizeof header, out) == sizeof header;
  for (i = 0; ok && i < count; ++i) {
    unsigned char pair[2];
    float s = AUDIO_INS(Clamp)(buffer->samples[i]);
    int16_t pcm = s <= -1.0f ? INT16_MIN : (int16_t)(s * 32767.0f);
    AUDIO_INS(WAV_Store16)(pair, (uint16_t)pcm);
    ok = fwrite(pair, 1, 2, out) == 2;
  }
  if (fclose(out) != 0) ok = false;
  return ok ? STATUS_CONST(SUCCESS) : STATUS_CONST(GENERIC_ERROR);
}
