#define AUDIO_CODEC_EXPERIMENTAL_NATIVE_MP3 1
#include "../../Cosmeron/Modules/Audio/CodecMinimal.h"
#include <assert.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>

/* Deliberately short reads verify that a callback is not assumed to
 * deliver a full header or MPEG frame in one call. */
static OPSTATUS shortRead(void *user, void *bytes, size_t n, size_t *got) {
  FILE *file = (FILE *)user;
  if (n > 7) n = 7;
  *got = fread(bytes, 1, n, file);
  return ferror(file) ? STATUS_CONST(GENERIC_ERROR) : STATUS_CONST(SUCCESS);
}
static OPSTATUS shortSeek(
    void *user, int64_t offset, AUDIO_CODEC_TYPE(TSeekOrigin) origin) {
  int how = origin == AUDIO_CODEC_CONST(SEEK_START) ? SEEK_SET :
            origin == AUDIO_CODEC_CONST(SEEK_CURRENT) ? SEEK_CUR : SEEK_END;
  if (offset > LONG_MAX || offset < LONG_MIN) return STATUS_CONST(OUT_OF_RANGE);
  return fseek((FILE *)user,(long)offset,how) == 0 ?
      STATUS_CONST(SUCCESS) : STATUS_CONST(GENERIC_ERROR);
}
static void run(const char *path, int callbacks) {
  FILE *file = fopen(path,"rb");
  AUDIO_CODEC_TYPE(TReader) reader;
  AUDIO_CODEC_TYPE(TDecoder) stream = {0};
  AUDIO_CODEC_TYPE(TInfo) info = {0};
  AUDIO_CODEC_TYPE(TMP3Memory) memory = {0};
  float a[193*2], b[193*2];
  uint32_t rate=0, channels=0;
  uint64_t length=0, at, positions[4];
  unsigned char *bytes;
  long total;
  size_t got, expected, j;
  assert(file && fseek(file,0,SEEK_END) == 0);
  total = ftell(file);
  assert(total > 0 && total < 2000000 && fseek(file,0,SEEK_SET)==0);
  bytes = (unsigned char*)malloc((size_t)total);
  assert(bytes && fread(bytes,1,(size_t)total,file)==(size_t)total);
  assert(AUDIO_CODEC_FUNC(MP3_MemoryOpen)(
      &memory,bytes,(size_t)total)==STATUS_CONST(SUCCESS));
  assert(AUDIO_CODEC_FUNC(MP3_MemoryInfo)(
      &memory,&rate,&channels,&length)==STATUS_CONST(SUCCESS));
  if (callbacks) {
    reader.userData=file;
    reader.read=shortRead;
    reader.seek=shortSeek;
    assert(AUDIO_CODEC_FUNC(OpenReader)(
        &stream,&reader)==STATUS_CONST(SUCCESS));
  } else {
    /* File-backed stream must remain valid independently of our
     * reference FILE handle and borrowed input bytes. */
    assert(AUDIO_CODEC_FUNC(OpenFile)(
        &stream,path)==STATUS_CONST(SUCCESS));
  }
  assert(AUDIO_CODEC_FUNC(GetInfo)(
      &stream,&info)==STATUS_CONST(SUCCESS));
  assert(info.lengthKnown && info.frameCount == length &&
      info.channels==channels && info.sampleRate==rate);
  positions[0]=0;
  positions[1]=length/2;
  positions[2]=length>193 ? length-193 : 0;
  positions[3]=length;
  for(j=0;j<4;++j) {
    assert(AUDIO_CODEC_FUNC(SeekFrame)(
        &stream,positions[j])==STATUS_CONST(SUCCESS));
    assert(AUDIO_CODEC_FUNC(MP3_MemorySeek)(
        &memory,positions[j])==STATUS_CONST(SUCCESS));
    at=positions[j];
    while(at<length) {
      size_t n=at%3==0 ? 31 : 193, readA=0,readB=0;
      if(n>length-at)n=(size_t)(length-at);
      assert(AUDIO_CODEC_FUNC(ReadFrames)(
          &stream,a,n,&readA)==STATUS_CONST(SUCCESS));
      assert(AUDIO_CODEC_FUNC(MP3_MemoryRead)(
          &memory,b,n,&readB)==STATUS_CONST(SUCCESS));
      assert(readA==n && readB==n);
      assert(memcmp(a,b,n*channels*sizeof(float))==0);
      at+=n;
    }
    got=999;
    assert(AUDIO_CODEC_FUNC(ReadFrames)(
        &stream,a,193,&got)==STATUS_CONST(SUCCESS) && !got);
  }
  expected=(size_t)(length+1);
  assert(AUDIO_CODEC_FUNC(SeekFrame)(
      &stream,expected)==STATUS_CONST(OUT_OF_RANGE));
  AUDIO_CODEC_FUNC(Close)(&stream);
  AUDIO_CODEC_FUNC(Close)(&stream);
  AUDIO_CODEC_FUNC(MP3_MemoryClose)(&memory);
  fclose(file);
  free(bytes);
}
int main(int argc,char **argv) {
  assert(argc==2);
  run(argv[1],0);
  run(argv[1],1);
  return 0;
}
