#define AUDIO_CODEC_EXPERIMENTAL_NATIVE_MP3 1
#include "../../Cosmeron/Modules/Audio/CodecNative.h"
#include <assert.h>
#include <stdio.h>

typedef struct Source { const unsigned char *p; size_t size, pos, reads, seeks; } Source;
static OPSTATUS read_bytes(void *user, void *dst, size_t n, size_t *got) {
  Source *s=(Source *)user;
  ++s->reads;
  if (n>7) n=7;
  if (n>s->size-s->pos) n=s->size-s->pos;
  memcpy(dst,s->p+s->pos,n); s->pos+=n; *got=n;
  return STATUS_CONST(SUCCESS);
}
static OPSTATUS seek_bytes(void *user, int64_t off, AUDIO_CODEC_TYPE(TSeekOrigin) origin) {
  Source *s=(Source *)user;
  ++s->seeks;
  if (origin!=AUDIO_CODEC_CONST(SEEK_START) || off<0 || (uint64_t)off>s->size)
    return STATUS_CONST(OUT_OF_RANGE);
  s->pos=(size_t)off; return STATUS_CONST(SUCCESS);
}
int main(int argc, char **argv) {
  unsigned char *bytes;
  size_t size=417*256, n, total=0, baseReads;
  float a[257*2], b[257*2];
  AUDIO_CODEC_TYPE(TDecoder) mem={0}, pipe={0}, indexed={0};
  AUDIO_CODEC_TYPE(TInfo) info;
  Source src, indexedSrc;
  AUDIO_CODEC_TYPE(TReader) reader;
  if (argc==2) {
    FILE *f=fopen(argv[1],"rb"); long len;
    assert(f && !fseek(f,0,SEEK_END)); len=ftell(f);
    assert(len>0 && !fseek(f,0,SEEK_SET)); size=(size_t)len;
    bytes=(unsigned char *)malloc(size); assert(bytes);
    assert(fread(bytes,1,size,f)==size); fclose(f);
  } else {
    bytes=(unsigned char *)calloc(1,size); assert(bytes);
    for(n=0;n<size;n+=417) { bytes[n]=0xff; bytes[n+1]=0xfb; bytes[n+2]=0x90; bytes[n+3]=0xc0; }
  }
  src=(Source){bytes,size,0,0,0}; indexedSrc=src;
  reader=(AUDIO_CODEC_TYPE(TReader)){&src,read_bytes,NULL};
  assert(AUDIO_CODEC_FUNC(OpenMemory)(&mem,bytes,size)==STATUS_CONST(SUCCESS));
  assert(AUDIO_CODEC_FUNC(OpenReader)(&pipe,&reader)==STATUS_CONST(SUCCESS));
  assert(AUDIO_CODEC_FUNC(GetInfo)(&mem,&info)==STATUS_CONST(SUCCESS));
  while(total<info.frameCount) {
    size_t got=0, cap=info.frameCount-total>257 ? 257 : (size_t)(info.frameCount-total);
    assert(AUDIO_CODEC_FUNC(ReadFrames)(&mem,a,cap,&n)==STATUS_CONST(SUCCESS));
    assert(AUDIO_CODEC_FUNC(ReadFrames)(&pipe,b,cap,&got)==STATUS_CONST(SUCCESS));
    assert(n==cap && got==n && !memcmp(a,b,n*info.channels*sizeof(float))); total+=n;
  }
  assert(!src.seeks);
  assert(AUDIO_CODEC_FUNC(ReadFrames)(&pipe,b,257,&n)==STATUS_CONST(SUCCESS) && !n);
  assert(AUDIO_CODEC_FUNC(GetInfo)(&pipe,&info)==STATUS_CONST(SUCCESS) && info.lengthKnown);
  assert(AUDIO_CODEC_FUNC(SeekFrame)(&pipe,0)==STATUS_CONST(NOT_SUPPORTED));
  reader=(AUDIO_CODEC_TYPE(TReader)){&indexedSrc,read_bytes,seek_bytes};
  assert(AUDIO_CODEC_FUNC(OpenReader)(&indexed,&reader)==STATUS_CONST(SUCCESS));
  total=0;
  while(total<info.frameCount) {
    assert(AUDIO_CODEC_FUNC(ReadFrames)(&indexed,b,257,&n)==STATUS_CONST(SUCCESS));
    assert(n); total+=n;
  }
  if (info.frameCount>1152*160) {
    uint64_t target=1152*130+13;
    baseReads=indexedSrc.reads;
    assert(AUDIO_CODEC_FUNC(SeekFrame)(&indexed,target)==STATUS_CONST(SUCCESS));
    assert(indexedSrc.reads-baseReads<1000); /* bounded replay, not 130 frames */
    assert(AUDIO_CODEC_FUNC(SeekFrame)(&mem,target)==STATUS_CONST(SUCCESS));
    assert(AUDIO_CODEC_FUNC(ReadFrames)(&mem,a,257,&n)==STATUS_CONST(SUCCESS));
    assert(AUDIO_CODEC_FUNC(ReadFrames)(&indexed,b,257,&total)==STATUS_CONST(SUCCESS));
    assert(total==n && !memcmp(a,b,n*info.channels*sizeof(float)));
  }
  AUDIO_CODEC_FUNC(Close)(&mem); AUDIO_CODEC_FUNC(Close)(&pipe); AUDIO_CODEC_FUNC(Close)(&indexed);
  free(bytes); return 0;
}
