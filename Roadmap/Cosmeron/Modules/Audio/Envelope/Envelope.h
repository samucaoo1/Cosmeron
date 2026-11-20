#pragma once

#include "../Audio.space"

typedef enum AUDIO_ENVELOPE_TYPE(TStage) {
  AUDIO_ENVELOPE_CONST(IDLE),
  AUDIO_ENVELOPE_CONST(ATTACK),
  AUDIO_ENVELOPE_CONST(DECAY),
  AUDIO_ENVELOPE_CONST(SUSTAIN),
  AUDIO_ENVELOPE_CONST(RELEASE)
} AUDIO_ENVELOPE_TYPE(TStage);
typedef struct AUDIO_ENVELOPE_TYPE(TEnvelope) {
  uint64_t attackFrames, decayFrames, releaseFrames, position;
  float sustain, level, releaseLevel;
  AUDIO_ENVELOPE_TYPE(TStage) stage;
} AUDIO_ENVELOPE_TYPE(TEnvelope);
#define AUDIO_ENVELOPE_INIT_PROTOTYPE                                          \
  static inline OPSTATUS AUDIO_ENVELOPE_FUNC(Init)(                            \
      AUDIO_ENVELOPE_TYPE(TEnvelope) * envelope, uint64_t attackFrames,        \
      uint64_t decayFrames, float sustain, uint64_t releaseFrames)

#define AUDIO_ENVELOPE_TRIGGER_PROTOTYPE                                       \
  static inline void AUDIO_ENVELOPE_FUNC(Trigger)(                             \
      AUDIO_ENVELOPE_TYPE(TEnvelope) * envelope)

#define AUDIO_ENVELOPE_RELEASE_PROTOTYPE                                       \
  static inline void AUDIO_ENVELOPE_FUNC(Release)(                             \
      AUDIO_ENVELOPE_TYPE(TEnvelope) * envelope)

#define AUDIO_ENVELOPE_APPLY_PROTOTYPE                                         \
  static inline OPSTATUS AUDIO_ENVELOPE_FUNC(Apply)(                           \
      AUDIO_ENVELOPE_TYPE(TEnvelope) * envelope, float *samples,               \
      size_t frameCount, uint32_t channels)

AUDIO_ENVELOPE_INIT_PROTOTYPE;
AUDIO_ENVELOPE_TRIGGER_PROTOTYPE;
AUDIO_ENVELOPE_RELEASE_PROTOTYPE;
AUDIO_ENVELOPE_APPLY_PROTOTYPE;

#include "Impl/Envelope.impl"
