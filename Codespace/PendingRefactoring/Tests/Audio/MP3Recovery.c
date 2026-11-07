#define AUDIO_CODEC_MP3_RECOVER_ERRORS 1
#include "../../Cosmeron/Modules/Audio/CodecNative.h"
#include <assert.h>
typedef struct Source { const unsigned char *bytes; size_t size,pos,failAt; bool failed; } Source;
static OPSTATUS read_bytes(void *user,void *dst,size_t cap,size_t *got) {
  Source *s=(Source *)user;
  if(cap>3) cap=3;
  if(cap>s->size-s->pos) cap=s->size-s->pos;
  if(s->failAt && !s->failed && s->pos<s->failAt && cap>s->failAt-s->pos) cap=s->failAt-s->pos;
  memcpy(dst,s->bytes+s->pos,cap); s->pos+=cap; *got=cap;
  if(s->failAt && !s->failed && s->pos==s->failAt) { s->failed=true; return STATUS_CONST(NOT_AVAILABLE); }
  return STATUS_CONST(SUCCESS);
}
static OPSTATUS seek_bytes(void *user,int64_t off,AUDIO_CODEC_TYPE(TSeekOrigin) origin) {
  Source *s=(Source *)user;
  if(origin!=AUDIO_CODEC_CONST(SEEK_START) || off<0 || (uint64_t)off>s->size)
    return STATUS_CONST(OUT_OF_RANGE);
  s->pos=(size_t)off; return STATUS_CONST(SUCCESS);
}
int main(void) {
  unsigned char bytes[417*4+9]={0}; size_t i,mode,scenario;
  for(i=0;i<4;++i) {
    size_t at=i*417+(i ? 9 : 0);
    bytes[at]=0xff; bytes[at+1]=0xfb; bytes[at+2]=0x90; bytes[at+3]=0xc0;
  }
  memcpy(bytes+417,"broken..!",9);
  bytes[427]=0xfa; bytes[430]=0xd0; bytes[431]=0xbe; /* bad protected CRC */
  for(scenario=0;scenario<2;++scenario) {
    if(scenario) bytes[428]|=4; /* incompatible rate: discard complete damaged frame */
    for(mode=0;mode<4;++mode) {
      Source src={bytes,sizeof bytes,0,mode==3 ? 430 : 0,false}; float pcm[127]; size_t got,total=0;
      uint64_t dropped,concealed;
      AUDIO_CODEC_TYPE(TInfo) info;
      AUDIO_CODEC_TYPE(TDecoder) decoder={0};
      AUDIO_CODEC_TYPE(TReader) reader={&src,read_bytes,mode>=2 ? NULL : seek_bytes};
      OPSTATUS status=mode==0 ? AUDIO_CODEC_FUNC(OpenMemory)(&decoder,bytes,sizeof bytes) :
          AUDIO_CODEC_FUNC(OpenReader)(&decoder,&reader);
      assert(status==STATUS_CONST(SUCCESS));
      for(;;) {
        status=AUDIO_CODEC_FUNC(ReadFrames)(&decoder,pcm,127,&got);
        for(i=0;i<got;++i) assert(pcm[i]==0);
        total+=got;
        if(status==STATUS_CONST(NOT_AVAILABLE)) { assert(mode==3 && src.failed); continue; }
        assert(status==STATUS_CONST(SUCCESS));
        if(!got) break;
      }
      assert(total==(scenario ? 3456 : 4608));
      assert(AUDIO_CODEC_FUNC(GetInfo)(&decoder,&info)==STATUS_CONST(SUCCESS));
      assert(info.lengthKnown && info.frameCount==total);
      assert(AUDIO_CODEC_FUNC(GetRecoveryCounts)(&decoder,&dropped,&concealed)==STATUS_CONST(SUCCESS));
      assert(dropped==(scenario ? 426 : 9) && concealed==(scenario ? 0 : 1152));
      AUDIO_CODEC_FUNC(Close)(&decoder);
    }
  }
  return 0;
}
