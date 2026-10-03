#pragma once

#include "../Audio.space"
#include "../Player/Player.h"

#define AUDIO_MIXER_SET_MASTER_VOLUME_PROTOTYPE                                \
  static inline OPSTATUS AUDIO_MIXER_FUNC(SetMasterVolume)(                    \
      AUDIO_PLAYER_TYPE(TPlayer) * player, float volume)

#define AUDIO_MIXER_SET_VOLUME_PROTOTYPE                                       \
  static inline OPSTATUS AUDIO_MIXER_FUNC(SetVolume)(                          \
      AUDIO_PLAYER_TYPE(TPlayer) * player, AUDIO_PLAYER_TYPE(TVoice) voice,    \
      float volume)

#define AUDIO_MIXER_SET_PAN_PROTOTYPE                                          \
  static inline OPSTATUS AUDIO_MIXER_FUNC(SetPan)(                             \
      AUDIO_PLAYER_TYPE(TPlayer) * player, AUDIO_PLAYER_TYPE(TVoice) voice,    \
      float pan)

AUDIO_MIXER_SET_MASTER_VOLUME_PROTOTYPE;
AUDIO_MIXER_SET_VOLUME_PROTOTYPE;
AUDIO_MIXER_SET_PAN_PROTOTYPE;

#include "Impl/Mixer.impl"
