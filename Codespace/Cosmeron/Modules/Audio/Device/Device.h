#pragma once

#include "../Audio.space"
#include "../PCM/PCM.h"
#ifndef AUDIO_DEVICE_MINIAUDIO_FALLBACK
#define AUDIO_DEVICE_MINIAUDIO_FALLBACK 1
#endif
#if AUDIO_DEVICE_MINIAUDIO_FALLBACK
#include "../Internal/Backend.inc"
#endif

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

typedef OPSTATUS (*AUDIO_DEVICE_TYPE(TEnumerateCallback))(
    void *userData, const char *id, const char *name);
#define AUDIO_DEVICE_ENUMERATE_PROTOTYPE \
  static inline OPSTATUS AUDIO_DEVICE_FUNC(Enumerate)( \
      AUDIO_DEVICE_TYPE(TEnumerateCallback) callback, void *userData)
AUDIO_DEVICE_ENUMERATE_PROTOTYPE;
/* Native names are ALSA PCM names, waveOut numeric IDs CoreAudio UIDs or OSS paths.
 * NULL selects the default output. */
#define AUDIO_DEVICE_OPEN_NAMED_PROTOTYPE \
  static inline OPSTATUS AUDIO_DEVICE_FUNC(OpenNamed)( \
      AUDIO_DEVICE_TYPE(TDevice) *device, \
      const AUDIO_DEVICE_TYPE(TConfig) *config, const char *id)
#define AUDIO_DEVICE_GET_STATUS_PROTOTYPE \
  static inline OPSTATUS AUDIO_DEVICE_FUNC(GetStatus)( \
      const AUDIO_DEVICE_TYPE(TDevice) *device)
#define AUDIO_DEVICE_GET_FORMAT_PROTOTYPE \
  static inline OPSTATUS AUDIO_DEVICE_FUNC(GetFormat)( \
      const AUDIO_DEVICE_TYPE(TDevice) *device, uint32_t *outRate, uint32_t *outChannels)
AUDIO_DEVICE_OPEN_NAMED_PROTOTYPE;
AUDIO_DEVICE_GET_STATUS_PROTOTYPE;
AUDIO_DEVICE_GET_FORMAT_PROTOTYPE;
#include "Impl/Native.inc"
#include "Impl/Enumeration.inc"
#include "Impl/Device.impl"
