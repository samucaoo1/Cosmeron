#pragma once
/* Portable offline synthesis: no desktop audio vendor or OS threads. */
#include "../Audio.space"
#include "../Sequence/Sequence.h"

#define AUDIO_BEEP_RENDER_TONE_PROTOTYPE                                       \
  static inline OPSTATUS AUDIO_BEEP_FUNC(RenderTone)(                          \
      double frequency, uint32_t durationMs, float volume,                     \
      uint32_t sampleRate, AUDIO_PCM_TYPE(TBuffer) * outBuffer)

#define AUDIO_BEEP_RENDER_MML_PROTOTYPE                                        \
  static inline OPSTATUS AUDIO_BEEP_FUNC(RenderMML)(                           \
      const char *text, uint32_t sampleRate,                                   \
      AUDIO_PCM_TYPE(TBuffer) * outBuffer, size_t *outErrorOffset)

AUDIO_BEEP_RENDER_TONE_PROTOTYPE;
AUDIO_BEEP_RENDER_MML_PROTOTYPE;

#include "Impl/Beep.impl"
