#include "../../Cosmeron/Modules/Audio/Audio.h"
#include <assert.h>
#include <stdio.h>

int main(void) {
  AUDIO_PLAYER_TYPE(TPlayer) player = {0};
  AUDIO_PLAYER_TYPE(TConfig) config = AUDIO_PLAYER_FUNC(Config)();
  AUDIO_PLAYER_TYPE(TVoice) voice, second, old;
  AUDIO_DEVICE_TYPE(TDevice) device = {0};
  AUDIO_DEVICE_TYPE(TConfig) deviceConfig = {0};
  float samples[1024];
  size_t count, i;
  double energy = 0;
  config.offline = true;
  assert(AUDIO_PLAYER_FUNC(Open)(&player, &config) == STATUS_CONST(SUCCESS));
  assert(AUDIO_PLAYER_FUNC(Open)(&player, &config) == STATUS_CONST(BUSY));
  assert(AUDIO_BEEP_FUNC(PlayTone)(&player, 440, 100, 0.25f, &voice) ==
         STATUS_CONST(SUCCESS));
  old = voice;
  assert(AUDIO_BEEP_FUNC(PlayMML)(&player, "T120 O4 C D E", false, &second,
                                  NULL) == STATUS_CONST(SUCCESS));
  assert(AUDIO_PLAYER_FUNC(IsPlaying)(&player, voice));
  assert(AUDIO_MIXER_FUNC(SetMasterVolume)(&player, 0.5f) ==
         STATUS_CONST(SUCCESS));
  assert(AUDIO_MIXER_FUNC(SetVolume)(&player, voice, 0.5f) ==
         STATUS_CONST(SUCCESS));
  assert(AUDIO_MIXER_FUNC(SetPan)(&player, voice, -0.5f) ==
         STATUS_CONST(SUCCESS));
  assert(AUDIO_PLAYER_FUNC(Render)(&player, samples, 512, &count) ==
             STATUS_CONST(SUCCESS) &&
         count == 512);
  for (i = 0; i < 1024; ++i) {
    energy += samples[i] * samples[i];
    assert(samples[i] >= -1 && samples[i] <= 1);
  }
  assert(energy > 0.001);
  assert(AUDIO_PLAYER_FUNC(Pause)(&player, voice) == STATUS_CONST(SUCCESS));
  assert(!AUDIO_PLAYER_FUNC(IsPlaying)(&player, voice));
  assert(AUDIO_PLAYER_FUNC(Resume)(&player, voice) == STATUS_CONST(SUCCESS));
  assert(AUDIO_PLAYER_FUNC(SeekFrame)(&player, voice, 0) ==
         STATUS_CONST(SUCCESS));
  assert(AUDIO_PLAYER_FUNC(Stop)(&player, voice) == STATUS_CONST(SUCCESS));
  assert(AUDIO_PLAYER_FUNC(Stop)(&player, voice) == STATUS_CONST(NOT_FOUND));
  assert(AUDIO_PLAYER_FUNC(Stop)(&player, second) == STATUS_CONST(SUCCESS));
  assert(AUDIO_PLAYER_FUNC(PlayFile)(&player, "Fixtures/Tone.mp3", true,
                                     &voice) == STATUS_CONST(SUCCESS));
  for (i = 0; i < 30; ++i)
    assert(AUDIO_PLAYER_FUNC(Render)(&player, samples, 512, &count) ==
           STATUS_CONST(SUCCESS));
  assert(AUDIO_PLAYER_FUNC(IsPlaying)(&player, voice));
  assert(AUDIO_PLAYER_FUNC(Collect)(&player) == 0);
  assert(AUDIO_PLAYER_FUNC(Stop)(&player, voice) == STATUS_CONST(SUCCESS));
  assert(AUDIO_PLAYER_FUNC(PlayFile)(&player, "Fixtures/Tone.flac", false,
                                     &voice) == STATUS_CONST(SUCCESS));
  for (i = 0; i < 30; ++i)
    assert(AUDIO_PLAYER_FUNC(Render)(&player, samples, 512, &count) ==
           STATUS_CONST(SUCCESS));
  assert(AUDIO_PLAYER_FUNC(Collect)(&player) == 1);
  assert(AUDIO_BEEP_FUNC(PlayBlocking)(&player, "C", NULL) ==
         STATUS_CONST(NOT_SUPPORTED));
  AUDIO_PLAYER_FUNC(Close)(&player);
  AUDIO_PLAYER_FUNC(Close)(&player);
  assert(!AUDIO_PLAYER_FUNC(IsPlaying)(&player, old));
  assert(AUDIO_PLAYER_FUNC(Open)(&player, &config) == STATUS_CONST(SUCCESS));
  assert(AUDIO_BEEP_FUNC(PlayTone)(&player, 440, 100, 0.25f, &voice) ==
         STATUS_CONST(SUCCESS));
  assert(AUDIO_PLAYER_FUNC(Stop)(&player, old) == STATUS_CONST(NOT_FOUND));
  AUDIO_PLAYER_FUNC(Close)(&player);
  assert(AUDIO_DEVICE_FUNC(Open)(&device, &deviceConfig) ==
         STATUS_CONST(INVALID_ARGUMENT));
  assert(AUDIO_DEVICE_FUNC(Start)(&device) == STATUS_CONST(INVALID_ARGUMENT));
  assert(AUDIO_DEVICE_FUNC(Stop)(&device) == STATUS_CONST(INVALID_ARGUMENT));
  assert(!AUDIO_DEVICE_FUNC(IsStarted)(&device));
  AUDIO_DEVICE_FUNC(Close)(&device);
  puts("Audio offline player/mixer: OK");
  return 0;
}
