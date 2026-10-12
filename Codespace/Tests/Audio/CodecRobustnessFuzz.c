#include "../../Cosmeron/Modules/Audio/CodecNative.h"
#include <stdio.h>
typedef struct Source { const uint8_t *data; size_t size,pos; } Source;
static OPSTATUS read_bytes(void *user, void *dst, size_t cap, size_t *got) {
  Source *s=(Source *)user;
  if (cap>11) cap=11;
  if (cap>s->size-s->pos) cap=s->size-s->pos;
  memcpy(dst,s->data+s->pos,cap); s->pos+=cap; *got=cap;
  return STATUS_CONST(SUCCESS);
}
static OPSTATUS seek_bytes(void *user,int64_t offset,AUDIO_CODEC_TYPE(TSeekOrigin) origin) {
  Source *s=(Source *)user;
  if (origin!=AUDIO_CODEC_CONST(SEEK_START) || offset<0 || (uint64_t)offset>s->size)
    return STATUS_CONST(OUT_OF_RANGE);
  s->pos=(size_t)offset; return STATUS_CONST(SUCCESS);
}
int LLVMFuzzerTestOneInput(const uint8_t *data,size_t size) {
  AUDIO_CODEC_TYPE(TDecoder) decoder={0};
  float pcm[128*8]; size_t got=0; unsigned int mode;
  if (size<4 || size>1024*1024) return 0;
  /* Only native recognizable input; never fuzz third-party fallbacks. */
  if (memcmp(data,"fLaC",4) && memcmp(data,"ID3",3) &&
      !(data[0]==0xff && (data[1]&0xe0)==0xe0)) return 0;
  for(mode=0;mode<3;++mode) {
    Source src={data,size,0};
    AUDIO_CODEC_TYPE(TReader) reader={&src,read_bytes,mode==2 ? NULL : seek_bytes};
    OPSTATUS status=mode==0 ? AUDIO_CODEC_FUNC(OpenMemory)(&decoder,data,size) :
        AUDIO_CODEC_FUNC(OpenReader)(&decoder,&reader);
    if (status==STATUS_CONST(SUCCESS)) {
      unsigned int i;
      for(i=0;i<4;++i) {
        if (AUDIO_CODEC_FUNC(ReadFrames)(&decoder,pcm,128,&got)!=STATUS_CONST(SUCCESS)) break;
      }
      (void)AUDIO_CODEC_FUNC(SeekFrame)(&decoder,0);
      (void)AUDIO_CODEC_FUNC(ReadFrames)(&decoder,pcm,128,&got);
      AUDIO_CODEC_FUNC(Close)(&decoder);
    }
  }
  return 0;
}
#ifndef COSMERON_CODEC_LIBFUZZER
int main(void) {
  const char *names[]={"Fixtures/Tone.mp3","Fixtures/Tone.flac"};
  unsigned int file;
  for(file=0;file<2;++file) {
    unsigned char bytes[65536], mutated[65536]; size_t size,i;
    FILE *f=fopen(names[file],"rb"); if (!f) return 1;
    size=fread(bytes,1,sizeof bytes,f); fclose(f);
    LLVMFuzzerTestOneInput(bytes,size);
    for(i=0;i<256;++i) {
      memcpy(mutated,bytes,size);
      mutated[(i*211U)%size]^=(unsigned char)(1U<<(i%8));
      LLVMFuzzerTestOneInput(mutated,size);
    }
    for(i=4;i<size;i+=113) LLVMFuzzerTestOneInput(bytes,i);
  }
  return 0;
}
#endif
