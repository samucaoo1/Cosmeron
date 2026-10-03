#include "../../Cosmeron/Modules/Audio/Audio.h"
#include <stdio.h>
/* cc -std=c11 PlayFile.c -o play-file
 * ./play-file song.mp3      press Enter to stop */
int main(int argc, char **argv) {
  AUDIO_PLAYER_TYPE(TPlayer) player = {0};
  AUDIO_PLAYER_TYPE(TConfig) config = AUDIO_PLAYER_FUNC(Config)();
  AUDIO_PLAYER_TYPE(TVoice) voice;
  OPSTATUS status;
  if (argc != 2) {
    fputs("Usage: play-file <WAV/MP3/FLAC>\n", stderr);
    return 1;
  }
  status = AUDIO_PLAYER_FUNC(Open)(&player, &config);
  if (status == STATUS_CONST(SUCCESS))
    status = AUDIO_PLAYER_FUNC(PlayFile)(&player, argv[1], false, &voice);
  if (status == STATUS_CONST(SUCCESS)) {
    puts("Playing. Press Enter to stop.");
    (void)getchar();
  }
  AUDIO_PLAYER_FUNC(Close)(&player);
  return status == STATUS_CONST(SUCCESS) ? 0 : 1;
}
