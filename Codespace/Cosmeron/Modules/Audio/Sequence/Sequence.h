#pragma once

#include "../Audio.space"
#include "../Oscillator/Oscillator.h"
#include "../PCM/PCM.h"
#include <ctype.h>

typedef struct AUDIO_SEQUENCE_TYPE(TEvent) {
  double frequency; /* Hz; zero is a rest. */
  uint32_t ticks;
  uint32_t bpm;
  float volume;
} AUDIO_SEQUENCE_TYPE(TEvent);
/* Owns only arrays produced by ParseMML. Zero initialize, destroy once. */
typedef struct AUDIO_SEQUENCE_TYPE(TSequence) {
  AUDIO_SEQUENCE_TYPE(TEvent) * events;
  size_t eventCount;
} AUDIO_SEQUENCE_TYPE(TSequence);
/* Borrows events, which must remain alive and unchanged while rendering. */
typedef struct AUDIO_SEQUENCE_TYPE(TCursor) {
  const AUDIO_SEQUENCE_TYPE(TEvent) * events;
  size_t eventCount, eventIndex;
  uint64_t frameInEvent, eventFrames;
  double fractionalFrames;
  AUDIO_OSCILLATOR_TYPE(TOscillator) oscillator;
} AUDIO_SEQUENCE_TYPE(TCursor);
#define AUDIO_SEQUENCE_NOTE_FREQUENCY_PROTOTYPE                                \
  static inline OPSTATUS AUDIO_SEQUENCE_FUNC(NoteFrequency)(                   \
      uint32_t midiNote, double *outFrequency)

#define AUDIO_SEQUENCE_PARSE_MML_PROTOTYPE                                     \
  static inline OPSTATUS AUDIO_SEQUENCE_FUNC(ParseMML)(                        \
      const char *text, AUDIO_SEQUENCE_TYPE(TSequence) * outSequence,          \
      size_t *outErrorOffset)

#define AUDIO_SEQUENCE_DESTROY_PROTOTYPE                                       \
  static inline void AUDIO_SEQUENCE_FUNC(Destroy)(                             \
      AUDIO_SEQUENCE_TYPE(TSequence) * sequence)

#define AUDIO_SEQUENCE_INIT_PROTOTYPE                                          \
  static inline OPSTATUS AUDIO_SEQUENCE_FUNC(Init)(                            \
      AUDIO_SEQUENCE_TYPE(TCursor) * cursor,                                   \
      const AUDIO_SEQUENCE_TYPE(TEvent) * events, size_t eventCount,           \
      uint32_t sampleRate, AUDIO_OSCILLATOR_TYPE(TWaveform) waveform)

#define AUDIO_SEQUENCE_FINISHED_PROTOTYPE                                      \
  static inline bool AUDIO_SEQUENCE_FUNC(Finished)(                            \
      const AUDIO_SEQUENCE_TYPE(TCursor) * cursor)

#define AUDIO_SEQUENCE_RENDER_PROTOTYPE                                        \
  static inline OPSTATUS AUDIO_SEQUENCE_FUNC(Render)(                          \
      AUDIO_SEQUENCE_TYPE(TCursor) * cursor, float *outSamples,                \
      size_t frameCount, size_t *outFrames)

#define AUDIO_SEQUENCE_RENDER_BUFFER_PROTOTYPE                                 \
  static inline OPSTATUS AUDIO_SEQUENCE_FUNC(RenderBuffer)(                    \
      const AUDIO_SEQUENCE_TYPE(TEvent) * events, size_t eventCount,           \
      uint32_t sampleRate, AUDIO_OSCILLATOR_TYPE(TWaveform) waveform,          \
      AUDIO_PCM_TYPE(TBuffer) * outBuffer)

AUDIO_SEQUENCE_NOTE_FREQUENCY_PROTOTYPE;
AUDIO_SEQUENCE_PARSE_MML_PROTOTYPE;
AUDIO_SEQUENCE_DESTROY_PROTOTYPE;
AUDIO_SEQUENCE_INIT_PROTOTYPE;
AUDIO_SEQUENCE_FINISHED_PROTOTYPE;
AUDIO_SEQUENCE_RENDER_PROTOTYPE;
AUDIO_SEQUENCE_RENDER_BUFFER_PROTOTYPE;

#include "Impl/Sequence.impl"
