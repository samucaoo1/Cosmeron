#pragma once

#include "../Audio.space"
#include "../Internal/Backend.inc"

typedef void (*AUDIO_DEVICE_TYPE(TRenderCallback))(void *userData,
                                                   float *outSamples,
                                                   size_t frameCount,
                                                   uint32_t channels);
typedef struct AUDIO_DEVICE_TYPE(TConfig) {
  uint32_t sampleRate, channels;
  AUDIO_DEVICE_TYPE(TRenderCallback) render;
  void *userData;
} AUDIO_DEVICE_TYPE(TConfig);
typedef struct AUDIO_DEVICE_TYPE(TDevice) {
  void *internal;
} AUDIO_DEVICE_TYPE(TDevice);
#define AUDIO_DEVICE_OPEN_PROTOTYPE                                            \
  static inline OPSTATUS AUDIO_DEVICE_FUNC(Open)(                              \
      AUDIO_DEVICE_TYPE(TDevice) * device,                                     \
      const AUDIO_DEVICE_TYPE(TConfig) * config)

#define AUDIO_DEVICE_START_PROTOTYPE                                           \
  static inline OPSTATUS AUDIO_DEVICE_FUNC(Start)(AUDIO_DEVICE_TYPE(TDevice) * \
                                                  device)

#define AUDIO_DEVICE_STOP_PROTOTYPE                                            \
  static inline OPSTATUS AUDIO_DEVICE_FUNC(Stop)(AUDIO_DEVICE_TYPE(TDevice) *  \
                                                 device)

#define AUDIO_DEVICE_IS_STARTED_PROTOTYPE                                      \
  static inline bool AUDIO_DEVICE_FUNC(IsStarted)(                             \
      const AUDIO_DEVICE_TYPE(TDevice) * device)

#define AUDIO_DEVICE_CLOSE_PROTOTYPE                                           \
  static inline void AUDIO_DEVICE_FUNC(Close)(AUDIO_DEVICE_TYPE(TDevice) *     \
                                              device)

AUDIO_DEVICE_OPEN_PROTOTYPE;
AUDIO_DEVICE_START_PROTOTYPE;
AUDIO_DEVICE_STOP_PROTOTYPE;
AUDIO_DEVICE_IS_STARTED_PROTOTYPE;
AUDIO_DEVICE_CLOSE_PROTOTYPE;

#include "Impl/Device.impl"
