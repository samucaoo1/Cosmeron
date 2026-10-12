#pragma once

#include "../Audio.space"
#if !defined(AUDIO_CODEC_NATIVE_ONLY)
#include "../Internal/Backend.inc"
#endif
#include "../PCM/PCM.h"
#include "WAV.h"
#include "FLAC.h"
#include "MP3.h"
/* Native MP3 is preferred. The vendor backend remains an explicit opt-out
 * for comparison and compatibility with inputs outside native coverage. */
#ifndef AUDIO_CODEC_NATIVE_MP3
#define AUDIO_CODEC_NATIVE_MP3 1
#endif
#if AUDIO_CODEC_NATIVE_MP3 && !defined(AUDIO_CODEC_EXPERIMENTAL_NATIVE_MP3)
#define AUDIO_CODEC_EXPERIMENTAL_NATIVE_MP3 1
#endif
#if defined(AUDIO_CODEC_EXPERIMENTAL_NATIVE_MP3)
#include "MP3Memory.h"
#endif
#include <stdio.h>

typedef struct AUDIO_CODEC_TYPE(TDecoder) {
  void *internal;
} AUDIO_CODEC_TYPE(TDecoder);
typedef struct AUDIO_CODEC_TYPE(TInfo) {
  uint32_t sampleRate, channels;
  uint64_t frameCount;
  bool lengthKnown;
} AUDIO_CODEC_TYPE(TInfo);
typedef enum AUDIO_CODEC_TYPE(TSeekOrigin) {
  AUDIO_CODEC_CONST(SEEK_START),
  AUDIO_CODEC_CONST(SEEK_CURRENT),
  AUDIO_CODEC_CONST(SEEK_END)
} AUDIO_CODEC_TYPE(TSeekOrigin);
typedef OPSTATUS (*AUDIO_CODEC_TYPE(TReadCallback))(void *userData,
                                                    void *outBytes,
                                                    size_t capacity,
                                                    size_t *outBytesRead);
typedef OPSTATUS (*AUDIO_CODEC_TYPE(TSeekCallback))(
    void *userData, int64_t offset, AUDIO_CODEC_TYPE(TSeekOrigin) origin);
typedef struct AUDIO_CODEC_TYPE(TReader) {
  void *userData;
  AUDIO_CODEC_TYPE(TReadCallback) read;
  AUDIO_CODEC_TYPE(TSeekCallback) seek;
} AUDIO_CODEC_TYPE(TReader);
#define AUDIO_CODEC_OPEN_FILE_PROTOTYPE                                        \
  static inline OPSTATUS AUDIO_CODEC_FUNC(OpenFile)(                           \
      AUDIO_CODEC_TYPE(TDecoder) * decoder, const char *path)

#define AUDIO_CODEC_OPEN_MEMORY_PROTOTYPE                                      \
  static inline OPSTATUS AUDIO_CODEC_FUNC(OpenMemory)(                         \
      AUDIO_CODEC_TYPE(TDecoder) * decoder, const void *data, size_t size)

#define AUDIO_CODEC_OPEN_READER_PROTOTYPE                                      \
  static inline OPSTATUS AUDIO_CODEC_FUNC(OpenReader)(                         \
      AUDIO_CODEC_TYPE(TDecoder) * decoder,                                    \
      const AUDIO_CODEC_TYPE(TReader) * reader)

#define AUDIO_CODEC_GET_INFO_PROTOTYPE                                         \
  static inline OPSTATUS AUDIO_CODEC_FUNC(GetInfo)(                            \
      const AUDIO_CODEC_TYPE(TDecoder) * decoder,                              \
      AUDIO_CODEC_TYPE(TInfo) * outInfo)

#define AUDIO_CODEC_READ_FRAMES_PROTOTYPE                                      \
  static inline OPSTATUS AUDIO_CODEC_FUNC(ReadFrames)(                         \
      AUDIO_CODEC_TYPE(TDecoder) * decoder, float *outSamples,                 \
      size_t frameCapacity, size_t *outFrames)

#define AUDIO_CODEC_SEEK_FRAME_PROTOTYPE                                       \
  static inline OPSTATUS AUDIO_CODEC_FUNC(SeekFrame)(                          \
      AUDIO_CODEC_TYPE(TDecoder) * decoder, uint64_t frameIndex)

#define AUDIO_CODEC_CLOSE_PROTOTYPE                                            \
  static inline void AUDIO_CODEC_FUNC(Close)(AUDIO_CODEC_TYPE(TDecoder) *      \
                                             decoder)

#define AUDIO_CODEC_WRITE_WAV_PROTOTYPE                                        \
  static inline OPSTATUS AUDIO_CODEC_FUNC(WriteWAV)(                           \
      const char *path, const AUDIO_PCM_TYPE(TBuffer) * buffer)

AUDIO_CODEC_OPEN_FILE_PROTOTYPE;
AUDIO_CODEC_OPEN_MEMORY_PROTOTYPE;
AUDIO_CODEC_OPEN_READER_PROTOTYPE;
AUDIO_CODEC_GET_INFO_PROTOTYPE;
AUDIO_CODEC_READ_FRAMES_PROTOTYPE;
AUDIO_CODEC_SEEK_FRAME_PROTOTYPE;
AUDIO_CODEC_CLOSE_PROTOTYPE;
AUDIO_CODEC_WRITE_WAV_PROTOTYPE;
#define AUDIO_CODEC_VERIFY_INTEGRITY_PROTOTYPE \
  static inline OPSTATUS AUDIO_CODEC_FUNC(VerifyIntegrity)(AUDIO_CODEC_TYPE(TDecoder) *decoder)
AUDIO_CODEC_VERIFY_INTEGRITY_PROTOTYPE;
#define AUDIO_CODEC_BUILD_SEEK_INDEX_PROTOTYPE \
  static inline OPSTATUS AUDIO_CODEC_FUNC(BuildSeekIndex)(AUDIO_CODEC_TYPE(TDecoder) *decoder)
AUDIO_CODEC_BUILD_SEEK_INDEX_PROTOTYPE;

#include "Impl/Codec.impl"
