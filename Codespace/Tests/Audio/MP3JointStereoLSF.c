#include "../../Cosmeron/Modules/Audio/Codec/MP3.h"
#include <assert.h>
#include <string.h>
static int near(float a,float b) {
  float d=a-b;return d>-0.00005f && d<0.00005f;
}
int main(void) {
  AUDIO_CODEC_TYPE(TMP3FrameInfo) h={0};
  AUDIO_CODEC_TYPE(TMP3Granule) g={0};
  AUDIO_CODEC_TYPE(TMP3Scalefactors) sf={0};
  float l[576]={0},r[576]={0},a[576],b[576];
  h.mpegVersion=2;h.channels=2;h.sampleRate=22050;
  h.intensityStereo=true;
  memset(sf.lsfIntensityLong,255,sizeof sf.lsfIntensityLong);
  memset(sf.lsfIntensityShort,255,sizeof sf.lsfIntensityShort);
  sf.lsfIntensityLong[5]=0;
  sf.lsfIntensityLong[6]=1;
  sf.lsfIntensityLong[7]=2;
  l[30]=4.0f;l[36]=4.0f;l[44]=4.0f;
  assert(AUDIO_CODEC_FUNC(MP3_JointStereoLSF)(
      &h,&g,&sf,0,l,r,a,b)==STATUS_CONST(SUCCESS));
  assert(near(a[30],4)&&near(b[30],4));
  assert(near(a[36],4*0.8408964153f)&&near(b[36],4));
  assert(near(a[44],4)&&near(b[44],4*0.8408964153f));
  assert(AUDIO_CODEC_FUNC(MP3_JointStereoLSF)(
      &h,&g,&sf,1,l,r,a,b)==STATUS_CONST(SUCCESS));
  assert(near(a[36],4*0.70710678118f)&&near(b[36],4));
  assert(near(a[44],4)&&near(b[44],4*0.70710678118f));
  h.msStereo=true;
  assert(AUDIO_CODEC_FUNC(MP3_JointStereoLSF)(
      &h,&g,&sf,0,l,r,a,b)==STATUS_CONST(SUCCESS));
  assert(near(a[30],4));
  assert(near(b[44],4*0.8408964153f));
  l[0]=2;r[0]=2;
  assert(AUDIO_CODEC_FUNC(MP3_JointStereoLSF)(
      &h,&g,&sf,0,l,r,a,b)==STATUS_CONST(SUCCESS));
  assert(near(a[0],2*1.41421356237f)&&near(b[0],0));
  h.sampleRate=0;a[0]=999;
  assert(AUDIO_CODEC_FUNC(MP3_JointStereoLSF)(
      &h,&g,&sf,0,l,r,a,b)==STATUS_CONST(NOT_SUPPORTED));
  assert(a[0]==999);
  return 0;
}
