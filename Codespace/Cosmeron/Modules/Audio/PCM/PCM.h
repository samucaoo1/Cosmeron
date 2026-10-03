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

#include "Impl/PCM.impl"
