#pragma once

#include "../Audio.space"

typedef enum AUDIO_OSCILLATOR_TYPE(TWaveform) {
  AUDIO_OSCILLATOR_CONST(SINE),
  AUDIO_OSCILLATOR_CONST(SQUARE),
  AUDIO_OSCILLATOR_CONST(TRIANGLE),
  AUDIO_OSCILLATOR_CONST(SAW),
  AUDIO_OSCILLATOR_CONST(NOISE),
  AUDIO_OSCILLATOR_CONST(WAVETABLE)
} AUDIO_OSCILLATOR_TYPE(TWaveform);

typedef enum AUDIO_OSCILLATOR_TYPE(TQuality) {
  AUDIO_OSCILLATOR_CONST(BASIC),
  AUDIO_OSCILLATOR_CONST(POLYBLEP)
} AUDIO_OSCILLATOR_TYPE(TQuality);

typedef struct AUDIO_OSCILLATOR_TYPE(TOscillator) {
  AUDIO_OSCILLATOR_TYPE(TQuality) quality;
  double phase;
  double frequency;
  float amplitude;
  float dutyCycle;
  uint32_t sampleRate;
  uint32_t noiseState;
  AUDIO_OSCILLATOR_TYPE(TWaveform) waveform;
  const float *table;
  size_t tableSize;
} AUDIO_OSCILLATOR_TYPE(TOscillator);
#define AUDIO_OSCILLATOR_INIT_PROTOTYPE                                        \
  static inline OPSTATUS AUDIO_OSCILLATOR_FUNC(Init)(                          \
      AUDIO_OSCILLATOR_TYPE(TOscillator) * oscillator, uint32_t sampleRate,    \
      AUDIO_OSCILLATOR_TYPE(TWaveform) waveform, double frequency)

#define AUDIO_OSCILLATOR_SET_FREQUENCY_PROTOTYPE                               \
  static inline OPSTATUS AUDIO_OSCILLATOR_FUNC(SetFrequency)(                  \
      AUDIO_OSCILLATOR_TYPE(TOscillator) * oscillator, double frequency)

#define AUDIO_OSCILLATOR_SET_AMPLITUDE_PROTOTYPE                               \
  static inline OPSTATUS AUDIO_OSCILLATOR_FUNC(SetAmplitude)(                  \
      AUDIO_OSCILLATOR_TYPE(TOscillator) * oscillator, float amplitude)

#define AUDIO_OSCILLATOR_SET_DUTY_CYCLE_PROTOTYPE                              \
  static inline OPSTATUS AUDIO_OSCILLATOR_FUNC(SetDutyCycle)(                  \
      AUDIO_OSCILLATOR_TYPE(TOscillator) * oscillator, float dutyCycle)

#define AUDIO_OSCILLATOR_SET_WAVEFORM_PROTOTYPE                                \
  static inline OPSTATUS AUDIO_OSCILLATOR_FUNC(SetWaveform)(                   \
      AUDIO_OSCILLATOR_TYPE(TOscillator) * oscillator,                         \
      AUDIO_OSCILLATOR_TYPE(TWaveform) waveform)

#define AUDIO_OSCILLATOR_SET_WAVETABLE_PROTOTYPE                               \
  static inline OPSTATUS AUDIO_OSCILLATOR_FUNC(SetWavetable)(                  \
      AUDIO_OSCILLATOR_TYPE(TOscillator) * oscillator, const float *table,     \
      size_t tableSize)

#define AUDIO_OSCILLATOR_RESET_PHASE_PROTOTYPE                                 \
  static inline OPSTATUS AUDIO_OSCILLATOR_FUNC(ResetPhase)(                    \
      AUDIO_OSCILLATOR_TYPE(TOscillator) * oscillator, double phase)

#define AUDIO_OSCILLATOR_SEED_PROTOTYPE                                        \
  static inline void AUDIO_OSCILLATOR_FUNC(Seed)(                              \
      AUDIO_OSCILLATOR_TYPE(TOscillator) * oscillator, uint32_t seed)

#define AUDIO_OSCILLATOR_RENDER_PROTOTYPE                                      \
  static inline OPSTATUS AUDIO_OSCILLATOR_FUNC(Render)(                        \
      AUDIO_OSCILLATOR_TYPE(TOscillator) * oscillator, float *outSamples,      \
      size_t frameCount)

AUDIO_OSCILLATOR_INIT_PROTOTYPE;
AUDIO_OSCILLATOR_SET_FREQUENCY_PROTOTYPE;
AUDIO_OSCILLATOR_SET_AMPLITUDE_PROTOTYPE;
AUDIO_OSCILLATOR_SET_DUTY_CYCLE_PROTOTYPE;
AUDIO_OSCILLATOR_SET_WAVEFORM_PROTOTYPE;
AUDIO_OSCILLATOR_SET_WAVETABLE_PROTOTYPE;
AUDIO_OSCILLATOR_RESET_PHASE_PROTOTYPE;
AUDIO_OSCILLATOR_SEED_PROTOTYPE;
AUDIO_OSCILLATOR_RENDER_PROTOTYPE;

#define AUDIO_OSCILLATOR_SET_QUALITY_PROTOTYPE                                 \
  static inline OPSTATUS AUDIO_OSCILLATOR_FUNC(SetQuality)(                    \
      AUDIO_OSCILLATOR_TYPE(TOscillator) * oscillator,                         \
      AUDIO_OSCILLATOR_TYPE(TQuality) quality)
AUDIO_OSCILLATOR_SET_QUALITY_PROTOTYPE;

#include "Impl/Oscillator.impl"
