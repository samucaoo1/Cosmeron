#include "../../Cosmeron/Modules/Audio/CodecNative.h"
#include <assert.h>
#include <stdio.h>
typedef struct Source { uint64_t size,pos; size_t reads; } Source;
static OPSTATUS read_bytes(void *user,void *dst,size_t cap,size_t *got) {
  static const unsigned char header[4]={0xff,0xfb,0x90,0xc0};
  Source *s=(Source *)user; unsigned char *out=(unsigned char *)dst; size_t i;
  ++s->reads;
  if((uint64_t)cap>s->size-s->pos) cap=(size_t)(s->size-s->pos);
  for(i=0;i<cap;++i) { uint64_t at=(s->pos+i)%417; out[i]=at<4 ? header[at] : 0; }
  s->pos+=cap; *got=cap; return STATUS_CONST(SUCCESS);
}
static OPSTATUS seek_bytes(void *user,int64_t off,AUDIO_CODEC_TYPE(TSeekOrigin) origin) {
  Source *s=(Source *)user;
  if(origin!=AUDIO_CODEC_CONST(SEEK_START) || off<0 || (uint64_t)off>s->size)
    return STATUS_CONST(OUT_OF_RANGE);
  s->pos=(uint64_t)off; return STATUS_CONST(SUCCESS);
}
int main(void) {
  uint64_t frames=SIZE_MAX>UINT32_MAX ? 10400000 : 1000000;
  Source src={frames*417,0,0}; float pcm[16]; size_t got,i,baseReads;
  AUDIO_CODEC_TYPE(TDecoder) decoder={0}; AUDIO_CODEC_TYPE(TInfo) info;
  AUDIO_CODEC_TYPE(TReader) reader={&src,read_bytes,seek_bytes};
  assert(AUDIO_CODEC_FUNC(OpenReader)(&decoder,&reader)==STATUS_CONST(SUCCESS));
  assert(AUDIO_CODEC_FUNC(GetInfo)(&decoder,&info)==STATUS_CONST(SUCCESS));
  assert(info.lengthKnown && info.frameCount==frames*1152);
  baseReads=src.reads;
  assert(AUDIO_CODEC_FUNC(SeekFrame)(&decoder,info.frameCount-113)==STATUS_CONST(SUCCESS));
  assert(src.reads-baseReads<3000); /* cold near-end seek, no DSP replay of ten million frames */
  assert(AUDIO_CODEC_FUNC(ReadFrames)(&decoder,pcm,16,&got)==STATUS_CONST(SUCCESS) && got==16);
  for(i=0;i<got;++i) assert(pcm[i]==0);
  printf("virtual bytes=%llu PCM frames=%llu cold seek reads=%zu\n",
      (unsigned long long)src.size,(unsigned long long)info.frameCount,src.reads-baseReads);
  AUDIO_CODEC_FUNC(Close)(&decoder); return 0;
}
