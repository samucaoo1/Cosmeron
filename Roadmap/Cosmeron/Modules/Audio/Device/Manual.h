#pragma once
/*
 * Pull-based native playback: OSS (FreeBSD/DragonFly), audio(4) (NetBSD),
 * and sndio (OpenBSD, loaded dynamically at runtime). No libm,
 * pthread or extra user link flags. The caller owns the event loop and must
 * pump regularly to avoid underruns. No background worker is created.
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
#elif defined(__OpenBSD__)
#include <sndio.h>
#include <dlfcn.h>
#define AUDIO_MANUAL_BSD_BACKEND 1
#define AUDIO_MANUAL_SNDIO_BACKEND 1
#else
#define AUDIO_MANUAL_BSD_BACKEND 0
#endif

typedef void (*AUDIO_DEVICE_TYPE(TManualRender))(void *userData,
                                                 float *outSamples,
                                                 size_t frameCount,
                                                 uint32_t channels);
/* A sink receives S16LE bytes and reports how many bytes were accepted.
 * Returning SUCCESS with zero or an excessive count is an I/O error.
 * Implementations may accept partial buffers. The callback is synchronous,
 * runs on the calling thread, and must not retain the passed byte pointer. */
typedef OPSTATUS (*AUDIO_DEVICE_TYPE(TManualWrite))(
    void *userData, const void *bytes, size_t capacity, size_t *outWritten);
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
#define AUDIO_DEVICE_MANUAL_OPEN_SINK_PROTOTYPE                                \
  static inline OPSTATUS AUDIO_DEVICE_FUNC(Manual_OpenSink)(                  \
      AUDIO_DEVICE_TYPE(TManual) *device,                                      \
      const AUDIO_DEVICE_TYPE(TManualConfig) *config,                         \
      AUDIO_DEVICE_TYPE(TManualWrite) writeSink, void *sinkUserData)
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
AUDIO_DEVICE_MANUAL_OPEN_SINK_PROTOTYPE;
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
  AUDIO_DEVICE_TYPE(TManualWrite) writeSink;
  void *sinkUserData;
#if defined(AUDIO_MANUAL_SNDIO_BACKEND)
  void *library;
  struct sio_hdl *stream;
  int (*startStream)(struct sio_hdl *);
  int (*stopStream)(struct sio_hdl *);
  size_t (*writeStream)(struct sio_hdl *, const void *, size_t);
  void (*closeStream)(struct sio_hdl *);
#endif
} AUDIO_INS(ManualState_InternalType_);

#if defined(AUDIO_MANUAL_SNDIO_BACKEND)
static inline bool AUDIO_INS(Manual_LoadSymbol)(void *handle, const char *name,
                                                void *destination, size_t bytes) {
  void *symbol = dlsym(handle, name);
  if (symbol == NULL || bytes != sizeof symbol) return false;
  memcpy(destination, &symbol, bytes);
  return true;
}
#endif

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
    AUDIO_INS(ManualState_InternalType_) *state =
      (AUDIO_INS(ManualState_InternalType_) *)calloc(1, sizeof(*state));
    if (state == NULL) return STATUS_CONST(OUT_OF_MEMORY);
    state->fd = -1;
#if defined(AUDIO_MANUAL_SNDIO_BACKEND)
    {
      /* OpenBSD's dlopen/dlsym are in libc. -lsndio is not required;
       * libsndio remains a required RUNTIME dependency for playback. */
      const char *names[] = {"libsndio.so", "libsndio.so.7.0",
                             "libsndio.so.7.1", "libsndio.so.8.0"};
      size_t k;
      struct sio_hdl *(*openStream)(const char *, unsigned int, int) = NULL;
      int (*setParameters)(struct sio_hdl *, struct sio_par *) = NULL;
      int (*getParameters)(struct sio_hdl *, struct sio_par *) = NULL;
      void (*initParameters)(struct sio_par *) = NULL;
      struct sio_par par;
      for (k = 0; k < sizeof names / sizeof names[0]; ++k) {
        state->library = dlopen(names[k], RTLD_NOW | RTLD_LOCAL);
        if (state->library != NULL) break;
      }
      if (state->library == NULL) { free(state); return STATUS_CONST(NOT_AVAILABLE); }
      if (!AUDIO_INS(Manual_LoadSymbol)(state->library, "sio_open",
          &openStream, sizeof openStream) ||
          !AUDIO_INS(Manual_LoadSymbol)(state->library, "sio_setpar",
          &setParameters, sizeof setParameters) ||
          !AUDIO_INS(Manual_LoadSymbol)(state->library, "sio_getpar",
          &getParameters, sizeof getParameters) ||
          !AUDIO_INS(Manual_LoadSymbol)(state->library, "sio_initpar",
          &initParameters, sizeof initParameters) ||
          !AUDIO_INS(Manual_LoadSymbol)(state->library, "sio_start",
          &state->startStream, sizeof state->startStream) ||
          !AUDIO_INS(Manual_LoadSymbol)(state->library, "sio_stop",
          &state->stopStream, sizeof state->stopStream) ||
          !AUDIO_INS(Manual_LoadSymbol)(state->library, "sio_write",
          &state->writeStream, sizeof state->writeStream) ||
          !AUDIO_INS(Manual_LoadSymbol)(state->library, "sio_close",
          &state->closeStream, sizeof state->closeStream)) {
        dlclose(state->library); free(state);
        return STATUS_CONST(NOT_AVAILABLE);
      }
      state->stream = openStream(config->path, SIO_PLAY, 0);
      if (state->stream == NULL) {
        dlclose(state->library); free(state);
        return STATUS_CONST(NOT_AVAILABLE);
      }
      initParameters(&par);
      par.bits = 16;
      par.bps = 2;
      par.sig = 1;
      par.le = 1;
      par.rate = config->sampleRate;
      par.pchan = config->channels;
      if (!setParameters(state->stream, &par) ||
          !getParameters(state->stream, &par) ||
          par.bits != 16 || par.bps != 2 || par.sig != 1 || par.le != 1 ||
          par.rate != config->sampleRate || par.pchan != config->channels) {
        state->closeStream(state->stream);
        dlclose(state->library); free(state);
        return STATUS_CONST(NOT_SUPPORTED);
      }
    }
#else
    {
      const char *path = config->path != NULL ? config->path :
#if defined(__NetBSD__)
        "/dev/sound";
#else
        "/dev/dsp";
#endif
      state->fd = open(path, O_WRONLY);
      if (state->fd < 0) {
        free(state);
        return STATUS_CONST(NOT_AVAILABLE);
      }
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
#endif /* no sndio */
    state->config = *config;
    device->internal = state;
    return STATUS_CONST(SUCCESS);
  }
#else
  (void)config;
  return STATUS_CONST(NOT_SUPPORTED);
#endif
}

AUDIO_DEVICE_MANUAL_OPEN_SINK_PROTOTYPE {
  AUDIO_INS(ManualState_InternalType_) *state;
  if (device == NULL || config == NULL || config->render == NULL ||
      writeSink == NULL ||
      !AUDIO_INS(ValidFormat)(config->sampleRate, config->channels))
    return STATUS_CONST(INVALID_ARGUMENT);
  if (device->internal != NULL)
    return STATUS_CONST(BUSY);
  state = (AUDIO_INS(ManualState_InternalType_) *)calloc(1, sizeof(*state));
  if (state == NULL)
    return STATUS_CONST(OUT_OF_MEMORY);
  state->fd = -1;
  state->config = *config;
  state->writeSink = writeSink;
  state->sinkUserData = sinkUserData;
  device->internal = state;
  return STATUS_CONST(SUCCESS);
}

AUDIO_DEVICE_MANUAL_START_PROTOTYPE {
  if (device == NULL || device->internal == NULL)
    return STATUS_CONST(INVALID_ARGUMENT);
  {
    AUDIO_INS(ManualState_InternalType_) *state =
      (AUDIO_INS(ManualState_InternalType_) *)device->internal;
#if defined(AUDIO_MANUAL_SNDIO_BACKEND)
    if (!state->started && state->stream != NULL &&
        !state->startStream(state->stream))
      return STATUS_CONST(GENERIC_ERROR);
#endif
    state->started = true;
  }
  return STATUS_CONST(SUCCESS);
}
AUDIO_DEVICE_MANUAL_STOP_PROTOTYPE {
  if (device == NULL || device->internal == NULL)
    return STATUS_CONST(INVALID_ARGUMENT);
  {
    AUDIO_INS(ManualState_InternalType_) *state =
      (AUDIO_INS(ManualState_InternalType_) *)device->internal;
#if defined(AUDIO_MANUAL_SNDIO_BACKEND)
    if (state->started && state->stream != NULL &&
        !state->stopStream(state->stream))
      return STATUS_CONST(GENERIC_ERROR);
#endif
    state->started = false;
  }
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
        if (state->writeSink != NULL) {
          size_t n = 0;
          OPSTATUS status = state->writeSink(state->sinkUserData,
                              pcm + written, bytes - written, &n);
          if (status != STATUS_CONST(SUCCESS))
            return status;
          if (n == 0 || n > bytes - written)
            return STATUS_CONST(GENERIC_ERROR);
          written += n;
        } else {
#if defined(AUDIO_MANUAL_SNDIO_BACKEND)
          size_t n = state->writeStream(state->stream, pcm + written,
                                        bytes - written);
          if (n == 0 || n > bytes - written)
            return STATUS_CONST(GENERIC_ERROR);
          written += n;
#elif AUDIO_MANUAL_BSD_BACKEND
          ssize_t n = write(state->fd, pcm + written, bytes - written);
          if (n < 0 && errno == EINTR) continue;
          if (n <= 0) return STATUS_CONST(GENERIC_ERROR);
          written += (size_t)n;
#else
          return STATUS_CONST(NOT_SUPPORTED);
#endif
        }
      }
      frames -= count;
    }
  }
  return STATUS_CONST(SUCCESS);
}

AUDIO_DEVICE_MANUAL_CLOSE_PROTOTYPE {
  if (device != NULL && device->internal != NULL) {
    AUDIO_INS(ManualState_InternalType_) *state =
      (AUDIO_INS(ManualState_InternalType_) *)device->internal;
    device->internal = NULL;
#if AUDIO_MANUAL_BSD_BACKEND
#if defined(AUDIO_MANUAL_SNDIO_BACKEND)
    if (state->stream != NULL) state->closeStream(state->stream);
    if (state->library != NULL) (void)dlclose(state->library);
#else
    if (state->fd >= 0) (void)close(state->fd);
#endif
#endif
    free(state);
  }
}
