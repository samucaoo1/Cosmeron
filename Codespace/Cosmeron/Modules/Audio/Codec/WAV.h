#pragma once
/*
 * Pure C11 RIFF/WAVE PCM decoder and writer. No libm or pthread.
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

#include "Impl/WAVStream.inc"

AUDIO_CODEC_WAV_DECODE_MEMORY_PROTOTYPE {
  AUDIO_INS(TWAVView) view;
  OPSTATUS result;
  size_t frames = 0;
  if (!outBuffer) return STATUS_CONST(INVALID_ARGUMENT);
  if (outBuffer->samples) return STATUS_CONST(BUSY);
  result = AUDIO_INS(WAV_Parse)(data, size, &view);
  if (result != STATUS_CONST(SUCCESS)) return result;
  if (view.frameCount > SIZE_MAX / view.channels)
    return STATUS_CONST(ARITHMETIC_OVERFLOW);
  result = AUDIO_PCM_FUNC(Init)(outBuffer, (size_t)view.frameCount,
      view.channels, view.sampleRate);
  if (result != STATUS_CONST(SUCCESS)) return result;
  result = AUDIO_INS(WAV_ReadRange)(&view, 0, outBuffer->samples,
      (size_t)view.frameCount, &frames);
  if (result != STATUS_CONST(SUCCESS) || frames != view.frameCount) {
    AUDIO_PCM_FUNC(Destroy)(outBuffer);
    return result == STATUS_CONST(INVALID_SEQUENCE) ?
        STATUS_CONST(INVALID_ARGUMENT) : result;
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
