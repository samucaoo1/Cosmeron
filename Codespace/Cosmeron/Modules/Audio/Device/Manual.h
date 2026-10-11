#pragma once
/*
 * Pull-based device playback, native OSS (FreeBSD/DragonFly) or
 * audio(4) (NetBSD). No Miniaudio, libm, pthread, dlopen or user link flags.
 * The caller owns the event loop and must pump regularly to avoid underruns.
 * OpenBSD/sndio is a separate optional integration; the default unsupported
 * implementation does not pretend to provide hardware playback.
 */
#include "../Audio.space"
#include <errno.h>
#include <limits.h>

#if defined(__FreeBSD__) || defined(__DragonFly__) || defined(__NetBSD__)
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#if defined(__FreeBSD__) || defined(__DragonFly__)
#include <sys/soundcard.h>
#elif defined(__NetBSD__)
#include <sys/audioio.h>
#endif
#define AUDIO_MANUAL_BSD_BACKEND 1
#else
#define AUDIO_MANUAL_BSD_BACKEND 0
#endif

typedef void (*AUDIO_DEVICE_TYPE(TManualRender))(void *userData,
                                                 float *outSamples,
                                                 size_t frameCount,
                                                 uint32_t channels);
typedef struct AUDIO_DEVICE_TYPE(TManualConfig) {
  uint32_t sampleRate, channels;
  AUDIO_DEVICE_TYPE(TManualRender) render;
  void *userData;
  const char *path; /* NULL uses the platform-default output device */
} AUDIO_DEVICE_TYPE(TManualConfig);
typedef struct AUDIO_DEVICE_TYPE(TManual) {
  void *internal; /* zero-initialize; non-copyable while open */
} AUDIO_DEVICE_TYPE(TManual);

#define AUDIO_DEVICE_MANUAL_OPEN_PROTOTYPE                                     \
  static inline OPSTATUS AUDIO_DEVICE_FUNC(Manual_Open)(                      \
      AUDIO_DEVICE_TYPE(TManual) *device,                                     \
      const AUDIO_DEVICE_TYPE(TManualConfig) *config)
#define AUDIO_DEVICE_MANUAL_START_PROTOTYPE                                    \
  static inline OPSTATUS AUDIO_DEVICE_FUNC(Manual_Start)(                     \
      AUDIO_DEVICE_TYPE(TManual) *device)
#define AUDIO_DEVICE_MANUAL_STOP_PROTOTYPE                                     \
  static inline OPSTATUS AUDIO_DEVICE_FUNC(Manual_Stop)(                      \
      AUDIO_DEVICE_TYPE(TManual) *device)
#define AUDIO_DEVICE_MANUAL_PUMP_PROTOTYPE                                     \
  static inline OPSTATUS AUDIO_DEVICE_FUNC(Manual_Pump)(                      \
      AUDIO_DEVICE_TYPE(TManual) *device, size_t frames)
#define AUDIO_DEVICE_MANUAL_IS_STARTED_PROTOTYPE                               \
  static inline bool AUDIO_DEVICE_FUNC(Manual_IsStarted)(                     \
      const AUDIO_DEVICE_TYPE(TManual) *device)
#define AUDIO_DEVICE_MANUAL_CLOSE_PROTOTYPE                                    \
  static inline void AUDIO_DEVICE_FUNC(Manual_Close)(                         \
      AUDIO_DEVICE_TYPE(TManual) *device)
#define AUDIO_DEVICE_MANUAL_ENCODE_PROTOTYPE                                   \
  static inline OPSTATUS AUDIO_DEVICE_FUNC(Manual_EncodeS16LE)(               \
      const float *samples, size_t sampleCount, unsigned char *outBytes)

AUDIO_DEVICE_MANUAL_OPEN_PROTOTYPE;
AUDIO_DEVICE_MANUAL_START_PROTOTYPE;
AUDIO_DEVICE_MANUAL_STOP_PROTOTYPE;
AUDIO_DEVICE_MANUAL_PUMP_PROTOTYPE;
AUDIO_DEVICE_MANUAL_IS_STARTED_PROTOTYPE;
AUDIO_DEVICE_MANUAL_CLOSE_PROTOTYPE;
AUDIO_DEVICE_MANUAL_ENCODE_PROTOTYPE;

typedef struct AUDIO_INS(ManualState_InternalType_) {
  int fd;
  AUDIO_DEVICE_TYPE(TManualConfig) config;
  bool started;
} AUDIO_INS(ManualState_InternalType_);

AUDIO_DEVICE_MANUAL_ENCODE_PROTOTYPE {
  size_t i;
  if ((samples == NULL || outBytes == NULL) && sampleCount != 0)
    return STATUS_CONST(INVALID_ARGUMENT);
  if (sampleCount > SIZE_MAX / 2)
    return STATUS_CONST(ARITHMETIC_OVERFLOW);
  for (i = 0; i < sampleCount; ++i) {
    float value = samples[i];
    int16_t pcm;
    if (!AUDIO_INS(Finite)(value)) value = 0.0f;
    value = AUDIO_INS(Clamp)(value);
    pcm = value <= -1.0f ? INT16_MIN : (int16_t)(value * 32767.0f);
    outBytes[i * 2] = (unsigned char)((uint16_t)pcm);
    outBytes[i * 2 + 1] = (unsigned char)((uint16_t)pcm >> 8);
  }
  return STATUS_CONST(SUCCESS);
}

AUDIO_DEVICE_MANUAL_OPEN_PROTOTYPE {
  if (device == NULL || config == NULL || config->render == NULL ||
      !AUDIO_INS(ValidFormat)(config->sampleRate, config->channels))
    return STATUS_CONST(INVALID_ARGUMENT);
  if (device->internal != NULL)
    return STATUS_CONST(BUSY);
#if AUDIO_MANUAL_BSD_BACKEND
  {
    const char *path = config->path != NULL ? config->path :
#if defined(__NetBSD__)
      "/dev/sound";
#else
      "/dev/dsp";
#endif
    AUDIO_INS(ManualState_InternalType_) *state =
      (AUDIO_INS(ManualState_InternalType_) *)calloc(1, sizeof(*state));
    if (state == NULL)
      return STATUS_CONST(OUT_OF_MEMORY);
    state->fd = open(path, O_WRONLY);
    if (state->fd < 0) {
      free(state);
      return STATUS_CONST(NOT_AVAILABLE);
    }
#if defined(__NetBSD__)
    {
      audio_info_t info;
      audio_info_t effective;
      AUDIO_INITINFO(&info);
      info.play.sample_rate = config->sampleRate;
      info.play.channels = config->channels;
      info.play.precision = 16;
      info.play.encoding = AUDIO_ENCODING_SLINEAR_LE;
      info.mode = AUMODE_PLAY;
      if (ioctl(state->fd, AUDIO_SETINFO, &info) == -1 ||
          ioctl(state->fd, AUDIO_GETINFO, &effective) == -1 ||
          effective.play.sample_rate != config->sampleRate ||
          effective.play.channels != config->channels ||
          effective.play.precision != 16 ||
          effective.play.encoding != AUDIO_ENCODING_SLINEAR_LE) {
        close(state->fd); free(state);
        return STATUS_CONST(NOT_SUPPORTED);
      }
    }
#else
    {
      int format = AFMT_S16_LE;
      int channels = (int)config->channels;
      int rate = (int)config->sampleRate;
      if (ioctl(state->fd, SNDCTL_DSP_SETFMT, &format) == -1 ||
          format != AFMT_S16_LE ||
          ioctl(state->fd, SNDCTL_DSP_CHANNELS, &channels) == -1 ||
          channels != (int)config->channels ||
          ioctl(state->fd, SNDCTL_DSP_SPEED, &rate) == -1 ||
          rate != (int)config->sampleRate) {
        close(state->fd); free(state);
        return STATUS_CONST(NOT_SUPPORTED);
      }
    }
#endif
    state->config = *config;
    device->internal = state;
    return STATUS_CONST(SUCCESS);
  }
#else
  (void)config;
  return STATUS_CONST(NOT_SUPPORTED);
#endif
}

AUDIO_DEVICE_MANUAL_START_PROTOTYPE {
  if (device == NULL || device->internal == NULL)
    return STATUS_CONST(INVALID_ARGUMENT);
  ((AUDIO_INS(ManualState_InternalType_) *)device->internal)->started = true;
  return STATUS_CONST(SUCCESS);
}
AUDIO_DEVICE_MANUAL_STOP_PROTOTYPE {
  if (device == NULL || device->internal == NULL)
    return STATUS_CONST(INVALID_ARGUMENT);
  ((AUDIO_INS(ManualState_InternalType_) *)device->internal)->started = false;
  return STATUS_CONST(SUCCESS);
}
AUDIO_DEVICE_MANUAL_IS_STARTED_PROTOTYPE {
  return device != NULL && device->internal != NULL &&
    ((const AUDIO_INS(ManualState_InternalType_) *)device->internal)->started;
}

AUDIO_DEVICE_MANUAL_PUMP_PROTOTYPE {
  AUDIO_INS(ManualState_InternalType_) *state;
  if (device == NULL || device->internal == NULL)
    return STATUS_CONST(INVALID_ARGUMENT);
  state = (AUDIO_INS(ManualState_InternalType_) *)device->internal;
  if (!state->started)
    return STATUS_CONST(NOT_AVAILABLE);
  if (frames == 0)
    return STATUS_CONST(SUCCESS);
#if AUDIO_MANUAL_BSD_BACKEND
  {
    float samples[512u * AUDIO_CONST(MAX_CHANNELS)];
    unsigned char pcm[512u * AUDIO_CONST(MAX_CHANNELS) * 2u];
    while (frames != 0) {
      size_t count = frames < 512 ? frames : 512;
      size_t sampleCount = count * (size_t)state->config.channels;
      size_t bytes = sampleCount * 2u, written = 0;
      memset(samples, 0, sampleCount * sizeof(float));
      state->config.render(state->config.userData, samples, count,
                           state->config.channels);
      if (AUDIO_DEVICE_FUNC(Manual_EncodeS16LE)(samples, sampleCount, pcm) !=
          STATUS_CONST(SUCCESS))
        return STATUS_CONST(GENERIC_ERROR);
      while (written < bytes) {
        ssize_t n = write(state->fd, pcm + written, bytes - written);
        if (n < 0 && errno == EINTR) continue;
        if (n <= 0) return STATUS_CONST(GENERIC_ERROR);
        written += (size_t)n;
      }
      /* The render callback runs on the CALLING thread; never on a worker. */
      frames -= count;
    }
    return STATUS_CONST(SUCCESS);
  }
#else
  (void)state;
  return STATUS_CONST(NOT_SUPPORTED);
#endif
}

AUDIO_DEVICE_MANUAL_CLOSE_PROTOTYPE {
  if (device != NULL && device->internal != NULL) {
    AUDIO_INS(ManualState_InternalType_) *state =
      (AUDIO_INS(ManualState_InternalType_) *)device->internal;
    device->internal = NULL;
#if AUDIO_MANUAL_BSD_BACKEND
    if (state->fd >= 0) (void)close(state->fd);
#endif
    free(state);
  }
}
