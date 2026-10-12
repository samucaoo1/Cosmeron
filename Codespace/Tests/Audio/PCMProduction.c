#include "../../Cosmeron/Modules/Audio/PCM/PCM.h"
#include <assert.h>
int main(void) {
  float pcm[6]={-2,-1,0,0.5f,1,2};
  int16_t s16[6]; int32_t s32[6]; unsigned char s24[18];
  const unsigned char edges[18]={0,0,0x80,0,0,0x80,0,0,0,0,0,0x40,0xff,0xff,0x7f,0xff,0xff,0x7f};
  assert(AUDIO_PCM_FUNC(ToS16)(pcm,6,s16)==STATUS_CONST(SUCCESS));
  assert(s16[0]==INT16_MIN && s16[1]==INT16_MIN && s16[2]==0 && s16[3]==16384 && s16[4]==INT16_MAX);
  assert(AUDIO_PCM_FUNC(ToS32)(pcm,6,s32)==STATUS_CONST(SUCCESS));
  assert(s32[0]==INT32_MIN && s32[3]==1073741824 && s32[4]==INT32_MAX);
  assert(AUDIO_PCM_FUNC(ToS24LE)(pcm,6,s24)==STATUS_CONST(SUCCESS));
  assert(!memcmp(s24,edges,sizeof edges));
  assert(AUDIO_PCM_FUNC(FromS24LE)(s24,6,pcm)==STATUS_CONST(SUCCESS));
  assert(pcm[0]==-1 && pcm[3]==0.5f);
  pcm[0]=-FLT_MAX; pcm[1]=FLT_MAX;
  assert(AUDIO_PCM_FUNC(Normalize)(pcm,6,0.5f)==STATUS_CONST(SUCCESS));
  assert(pcm[0]==-0.5f && pcm[1]==0.5f);
  return 0;
}
