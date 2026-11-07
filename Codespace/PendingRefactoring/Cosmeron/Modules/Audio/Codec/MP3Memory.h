#pragma once
/* Opt-in MPEG Layer III memory stream. The source buffer is borrowed;
 * no vendor decoder, linking library or full-file PCM copy is required.
 * Production Audio_Codec_Open* remains unchanged. */
#include "MP3Experimental.h"
#include <stdlib.h>
#include <string.h>

typedef struct AUDIO_CODEC_TYPE(TMP3Memory) {
  void *internal;
} AUDIO_CODEC_TYPE(TMP3Memory);

#define AUDIO_CODEC_MP3_MEMORY_OPEN_PROTOTYPE                                 \
  static inline OPSTATUS AUDIO_CODEC_FUNC(MP3_MemoryOpen)(                    \
      AUDIO_CODEC_TYPE(TMP3Memory) *stream, const void *bytes, size_t size)
#define AUDIO_CODEC_MP3_MEMORY_INFO_PROTOTYPE                                 \
  static inline OPSTATUS AUDIO_CODEC_FUNC(MP3_MemoryInfo)(                    \
      const AUDIO_CODEC_TYPE(TMP3Memory) *stream, uint32_t *outRate,           \
      uint32_t *outChannels, uint64_t *outFrames)
#define AUDIO_CODEC_MP3_MEMORY_READ_PROTOTYPE                                 \
  static inline OPSTATUS AUDIO_CODEC_FUNC(MP3_MemoryRead)(                    \
      AUDIO_CODEC_TYPE(TMP3Memory) *stream, float *outInterleaved,             \
      size_t capacity, size_t *outFrames)
#define AUDIO_CODEC_MP3_MEMORY_SEEK_PROTOTYPE                                 \
  static inline OPSTATUS AUDIO_CODEC_FUNC(MP3_MemorySeek)(                    \
      AUDIO_CODEC_TYPE(TMP3Memory) *stream, uint64_t frameIndex)
#define AUDIO_CODEC_MP3_MEMORY_CLOSE_PROTOTYPE                                \
  static inline void AUDIO_CODEC_FUNC(MP3_MemoryClose)(                       \
      AUDIO_CODEC_TYPE(TMP3Memory) *stream)

AUDIO_CODEC_MP3_MEMORY_OPEN_PROTOTYPE;
AUDIO_CODEC_MP3_MEMORY_INFO_PROTOTYPE;
AUDIO_CODEC_MP3_MEMORY_READ_PROTOTYPE;
AUDIO_CODEC_MP3_MEMORY_SEEK_PROTOTYPE;
AUDIO_CODEC_MP3_MEMORY_CLOSE_PROTOTYPE;

#include "Impl/MP3Memory.inc"
