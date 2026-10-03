#pragma once

#include "../Audio.space"
#include "../Internal/Backend.inc"
#include "../PCM/PCM.h"

typedef struct AUDIO_PLAYER_TYPE(TPlayer) {
  void *internal;
  uint64_t generation;
} AUDIO_PLAYER_TYPE(TPlayer);
typedef struct AUDIO_PLAYER_TYPE(TConfig) {
  uint32_t sampleRate, channels;
  bool offline;
} AUDIO_PLAYER_TYPE(TConfig);
/* Copyable token; never a pointer to a freed sound. */
typedef struct AUDIO_PLAYER_TYPE(TVoice) {
  const AUDIO_PLAYER_TYPE(TPlayer) * player;
  uint64_t generation, id;
} AUDIO_PLAYER_TYPE(TVoice);
#define AUDIO_PLAYER_CONFIG_PROTOTYPE                                          \
  static inline AUDIO_PLAYER_TYPE(TConfig) AUDIO_PLAYER_FUNC(Config)(void)

#define AUDIO_PLAYER_OPEN_PROTOTYPE                                            \
  static inline OPSTATUS AUDIO_PLAYER_FUNC(Open)(                              \
      AUDIO_PLAYER_TYPE(TPlayer) * player,                                     \
      const AUDIO_PLAYER_TYPE(TConfig) * config)

#define AUDIO_PLAYER_PLAY_FILE_PROTOTYPE                                       \
  static inline OPSTATUS AUDIO_PLAYER_FUNC(PlayFile)(                          \
      AUDIO_PLAYER_TYPE(TPlayer) * player, const char *path, bool loop,        \
      AUDIO_PLAYER_TYPE(TVoice) * outVoice)

#define AUDIO_PLAYER_PLAY_BUFFER_PROTOTYPE                                     \
  static inline OPSTATUS AUDIO_PLAYER_FUNC(PlayBuffer)(                        \
      AUDIO_PLAYER_TYPE(TPlayer) * player,                                     \
      const AUDIO_PCM_TYPE(TBuffer) * buffer, bool loop,                       \
      AUDIO_PLAYER_TYPE(TVoice) * outVoice)

#define AUDIO_PLAYER_PAUSE_PROTOTYPE                                           \
  static inline OPSTATUS AUDIO_PLAYER_FUNC(Pause)(                             \
      AUDIO_PLAYER_TYPE(TPlayer) * player, AUDIO_PLAYER_TYPE(TVoice) voice)

#define AUDIO_PLAYER_RESUME_PROTOTYPE                                          \
  static inline OPSTATUS AUDIO_PLAYER_FUNC(Resume)(                            \
      AUDIO_PLAYER_TYPE(TPlayer) * player, AUDIO_PLAYER_TYPE(TVoice) voice)

#define AUDIO_PLAYER_SEEK_FRAME_PROTOTYPE                                      \
  static inline OPSTATUS AUDIO_PLAYER_FUNC(SeekFrame)(                         \
      AUDIO_PLAYER_TYPE(TPlayer) * player, AUDIO_PLAYER_TYPE(TVoice) voice,    \
      uint64_t frameIndex)

#define AUDIO_PLAYER_IS_PLAYING_PROTOTYPE                                      \
  static inline bool AUDIO_PLAYER_FUNC(IsPlaying)(                             \
      const AUDIO_PLAYER_TYPE(TPlayer) * player,                               \
      AUDIO_PLAYER_TYPE(TVoice) voice)

#define AUDIO_PLAYER_STOP_PROTOTYPE                                            \
  static inline OPSTATUS AUDIO_PLAYER_FUNC(Stop)(                              \
      AUDIO_PLAYER_TYPE(TPlayer) * player, AUDIO_PLAYER_TYPE(TVoice) voice)

#define AUDIO_PLAYER_COLLECT_PROTOTYPE                                         \
  static inline size_t AUDIO_PLAYER_FUNC(Collect)(AUDIO_PLAYER_TYPE(TPlayer) * \
                                                  player)

#define AUDIO_PLAYER_RENDER_PROTOTYPE                                          \
  static inline OPSTATUS AUDIO_PLAYER_FUNC(Render)(                            \
      AUDIO_PLAYER_TYPE(TPlayer) * player, float *outSamples,                  \
      size_t frameCount, size_t *outFrames)

#define AUDIO_PLAYER_CLOSE_PROTOTYPE                                           \
  static inline void AUDIO_PLAYER_FUNC(Close)(AUDIO_PLAYER_TYPE(TPlayer) *     \
                                              player)

AUDIO_PLAYER_CONFIG_PROTOTYPE;
AUDIO_PLAYER_OPEN_PROTOTYPE;
AUDIO_PLAYER_PLAY_FILE_PROTOTYPE;
AUDIO_PLAYER_PLAY_BUFFER_PROTOTYPE;
AUDIO_PLAYER_PAUSE_PROTOTYPE;
AUDIO_PLAYER_RESUME_PROTOTYPE;
AUDIO_PLAYER_SEEK_FRAME_PROTOTYPE;
AUDIO_PLAYER_IS_PLAYING_PROTOTYPE;
AUDIO_PLAYER_STOP_PROTOTYPE;
AUDIO_PLAYER_COLLECT_PROTOTYPE;
AUDIO_PLAYER_RENDER_PROTOTYPE;
AUDIO_PLAYER_CLOSE_PROTOTYPE;

#include "Impl/Player.impl"
