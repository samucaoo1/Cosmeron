#pragma once

#include "../Audio.space"

typedef struct AUDIO_PCM_TYPE(TBuffer) {
  float *samples;
  size_t frameCount;
  uint32_t channels;
  uint32_t sampleRate;
} AUDIO_PCM_TYPE(TBuffer);
/* Owning object. Zero initialize; do not copy an initialized buffer. */
#define AUDIO_PCM_INIT_PROTOTYPE                                               \
  static inline OPSTATUS AUDIO_PCM_FUNC(Init)(                                 \
      AUDIO_PCM_TYPE(TBuffer) * buffer, size_t frameCount, uint32_t channels,  \
      uint32_t sampleRate)

#define AUDIO_PCM_DESTROY_PROTOTYPE                                            \
  static inline void AUDIO_PCM_FUNC(Destroy)(AUDIO_PCM_TYPE(TBuffer) * buffer)

#define AUDIO_PCM_FROM_S16_PROTOTYPE                                           \
  static inline OPSTATUS AUDIO_PCM_FUNC(FromS16)(                              \
      const int16_t *samples, size_t sampleCount, float *outSamples)

#define AUDIO_PCM_TO_S16_PROTOTYPE                                             \
  static inline OPSTATUS AUDIO_PCM_FUNC(ToS16)(                                \
      const float *samples, size_t sampleCount, int16_t *outSamples)

AUDIO_PCM_INIT_PROTOTYPE;
AUDIO_PCM_DESTROY_PROTOTYPE;
AUDIO_PCM_FROM_S16_PROTOTYPE;
AUDIO_PCM_TO_S16_PROTOTYPE;

#define AUDIO_PCM_FROM_S32_PROTOTYPE \
  static inline OPSTATUS AUDIO_PCM_FUNC(FromS32)(const int32_t *samples, size_t sampleCount, float *outSamples)
#define AUDIO_PCM_TO_S32_PROTOTYPE \
  static inline OPSTATUS AUDIO_PCM_FUNC(ToS32)(const float *samples, size_t sampleCount, int32_t *outSamples)
#define AUDIO_PCM_FROM_S24LE_PROTOTYPE \
  static inline OPSTATUS AUDIO_PCM_FUNC(FromS24LE)(const unsigned char *samples, size_t sampleCount, float *outSamples)
#define AUDIO_PCM_TO_S24LE_PROTOTYPE \
  static inline OPSTATUS AUDIO_PCM_FUNC(ToS24LE)(const float *samples, size_t sampleCount, unsigned char *outSamples)
#define AUDIO_PCM_NORMALIZE_PROTOTYPE \
  static inline OPSTATUS AUDIO_PCM_FUNC(Normalize)(float *samples, size_t sampleCount, float peak)
#define AUDIO_PCM_SANITIZE_PROTOTYPE \
  static inline OPSTATUS AUDIO_PCM_FUNC(Sanitize)(float *samples, size_t sampleCount)
AUDIO_PCM_FROM_S32_PROTOTYPE;
AUDIO_PCM_TO_S32_PROTOTYPE;
AUDIO_PCM_FROM_S24LE_PROTOTYPE;
AUDIO_PCM_TO_S24LE_PROTOTYPE;
AUDIO_PCM_NORMALIZE_PROTOTYPE;
AUDIO_PCM_SANITIZE_PROTOTYPE;
#include "Impl/PCM.impl"
