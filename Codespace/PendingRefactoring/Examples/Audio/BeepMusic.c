#include "../../Cosmeron/Modules/Audio/Audio.h"
#include <stdio.h>

/* cc -std=c11 BeepMusic.c -o beep-music
 * ./beep-music             exports melody.wav without opening a sound device
 * ./beep-music --play      also plays it through the default desktop device */
int main(int argc, char **argv) {
  const char *melody = "T140 O5 L8 V24 E D C D E E E4 P8 D D D4 E G G4";
  AUDIO_PCM_TYPE(TBuffer) buffer = {0};
  OPSTATUS status = AUDIO_BEEP_FUNC(RenderMML)(
      melody, AUDIO_CONST(DEFAULT_SAMPLE_RATE), &buffer, NULL);
  if (status != STATUS_CONST(SUCCESS))
    return 1;
  status = AUDIO_CODEC_FUNC(WriteWAV)("melody.wav", &buffer);
  AUDIO_PCM_FUNC(Destroy)(&buffer);
  if (status != STATUS_CONST(SUCCESS))
    return 1;
  puts("Created melody.wav");
  if (argc > 1 && strcmp(argv[1], "--play") == 0) {
    AUDIO_PLAYER_TYPE(TPlayer) player = {0};
    AUDIO_PLAYER_TYPE(TConfig) config = AUDIO_PLAYER_FUNC(Config)();
    status = AUDIO_PLAYER_FUNC(Open)(&player, &config);
    if (status == STATUS_CONST(SUCCESS))
      status = AUDIO_BEEP_FUNC(PlayBlocking)(&player, melody, NULL);
    AUDIO_PLAYER_FUNC(Close)(&player);
    if (status != STATUS_CONST(SUCCESS)) {
      fputs("Audio device unavailable or playback failed.\n", stderr);
      return 1;
    }
  }
  return 0;
}
