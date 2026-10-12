#include "../../Cosmeron/Modules/Audio/Codec/MP3Experimental.h"
#include <assert.h>
int main(void) {
  /* Independently computed protected mono header + zero side information. */
  unsigned char frame[2048]={0xff,0xfa,0x90,0xc0};
  AUDIO_CODEC_TYPE(TMP3Experimental) state={0}, before;
  AUDIO_CODEC_TYPE(TMP3FrameInfo) info;
  float pcm[1152]; size_t got=0;
  frame[4]=0; frame[5]=0;
  /* Golden CRC is inserted by the host fixture author below. */
  frame[4]=0xd0; frame[5]=0xbf;
  assert(AUDIO_CODEC_FUNC(MP3_CheckCRC)(frame,417)==STATUS_CONST(SUCCESS));
  assert(AUDIO_CODEC_FUNC(MP3_ExperimentFrame)(&state,frame,417,pcm,1152,&got,&info)==STATUS_CONST(SUCCESS) && got==1152);
  before=state; frame[8]^=1;
  assert(AUDIO_CODEC_FUNC(MP3_CheckCRC)(frame,417)==STATUS_CONST(INVALID_SEQUENCE));
  assert(AUDIO_CODEC_FUNC(MP3_ExperimentFrame)(&state,frame,417,pcm,1152,&got,&info)==STATUS_CONST(INVALID_SEQUENCE));
  assert(!memcmp(&state,&before,sizeof state));
  return 0;
}
