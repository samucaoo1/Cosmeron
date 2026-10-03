#pragma once
#define AUDIO_BEEP_PLAY_TONE_PROTOTYPE                                         \
  static inline OPSTATUS AUDIO_BEEP_FUNC(PlayTone)(                            \
      AUDIO_PLAYER_TYPE(TPlayer) * player, double frequency,                   \
      uint32_t durationMs, float volume, AUDIO_PLAYER_TYPE(TVoice) * outVoice)
#define AUDIO_BEEP_PLAY_SEQUENCE_PROTOTYPE                                     \
  static inline OPSTATUS AUDIO_BEEP_FUNC(PlaySequence)(                        \
      AUDIO_PLAYER_TYPE(TPlayer) * player,                                     \
      const AUDIO_SEQUENCE_TYPE(TEvent) * events, size_t eventCount,           \
      bool loop, AUDIO_PLAYER_TYPE(TVoice) * outVoice)
#define AUDIO_BEEP_PLAY_MML_PROTOTYPE                                          \
  static inline OPSTATUS AUDIO_BEEP_FUNC(PlayMML)(                             \
      AUDIO_PLAYER_TYPE(TPlayer) * player, const char *text, bool loop,        \
      AUDIO_PLAYER_TYPE(TVoice) * outVoice, size_t *outErrorOffset)
#define AUDIO_BEEP_PLAY_BLOCKING_PROTOTYPE                                     \
  static inline OPSTATUS AUDIO_BEEP_FUNC(PlayBlocking)(                        \
      AUDIO_PLAYER_TYPE(TPlayer) * player, const char *text,                   \
      size_t *outErrorOffset)

AUDIO_BEEP_PLAY_TONE_PROTOTYPE;
AUDIO_BEEP_PLAY_SEQUENCE_PROTOTYPE;
AUDIO_BEEP_PLAY_MML_PROTOTYPE;
AUDIO_BEEP_PLAY_BLOCKING_PROTOTYPE;

AUDIO_BEEP_PLAY_TONE_PROTOTYPE {
  AUDIO_PCM_TYPE(TBuffer) buffer = {0};
  OPSTATUS status;
  if (player == NULL || player->internal == NULL || outVoice == NULL)
    return STATUS_CONST(INVALID_ARGUMENT);
  status = AUDIO_BEEP_FUNC(RenderTone)(
      frequency, durationMs, volume,
      ((AUDIO_INS(Player) *)player->internal)->config.sampleRate, &buffer);
  if (status == STATUS_CONST(SUCCESS))
    status = AUDIO_PLAYER_FUNC(PlayBuffer)(player, &buffer, false, outVoice);
  AUDIO_PCM_FUNC(Destroy)(&buffer);
  return status;
}
AUDIO_BEEP_PLAY_SEQUENCE_PROTOTYPE {
  AUDIO_PCM_TYPE(TBuffer) buffer = {0};
  OPSTATUS status;
  if (player == NULL || player->internal == NULL || outVoice == NULL)
    return STATUS_CONST(INVALID_ARGUMENT);
  status = AUDIO_SEQUENCE_FUNC(RenderBuffer)(
      events, eventCount,
      ((AUDIO_INS(Player) *)player->internal)->config.sampleRate,
      AUDIO_OSCILLATOR_CONST(SQUARE), &buffer);
  if (status == STATUS_CONST(SUCCESS))
    status = AUDIO_PLAYER_FUNC(PlayBuffer)(player, &buffer, loop, outVoice);
  AUDIO_PCM_FUNC(Destroy)(&buffer);
  return status;
}
AUDIO_BEEP_PLAY_MML_PROTOTYPE {
  AUDIO_SEQUENCE_TYPE(TSequence) sequence = {0};
  OPSTATUS status;
  if (player == NULL || player->internal == NULL || outVoice == NULL)
    return STATUS_CONST(INVALID_ARGUMENT);
  status = AUDIO_SEQUENCE_FUNC(ParseMML)(text, &sequence, outErrorOffset);
  if (status == STATUS_CONST(SUCCESS))
    status = AUDIO_BEEP_FUNC(PlaySequence)(player, sequence.events,
                                           sequence.eventCount, loop, outVoice);
  AUDIO_SEQUENCE_FUNC(Destroy)(&sequence);
  return status;
}
AUDIO_BEEP_PLAY_BLOCKING_PROTOTYPE {
  AUDIO_PLAYER_TYPE(TVoice) voice;
  OPSTATUS status;
  if (player == NULL || player->internal == NULL)
    return STATUS_CONST(INVALID_ARGUMENT);
  if (((AUDIO_INS(Player) *)player->internal)->config.offline)
    return STATUS_CONST(NOT_SUPPORTED);
  status =
      AUDIO_BEEP_FUNC(PlayMML)(player, text, false, &voice, outErrorOffset);
  if (status != STATUS_CONST(SUCCESS))
    return status;
  while (AUDIO_PLAYER_FUNC(IsPlaying)(player, voice)) {
    if (!cosmeron_audio_vendor_ma_device_is_started(
            cosmeron_audio_vendor_ma_engine_get_device(
                &((AUDIO_INS(Player) *)player->internal)->native))) {
      (void)AUDIO_PLAYER_FUNC(Stop)(player, voice);
      return STATUS_CONST(NOT_AVAILABLE);
    }
    cosmeron_audio_vendor_ma_sleep(10);
  }
  return AUDIO_PLAYER_FUNC(Stop)(player, voice);
}
