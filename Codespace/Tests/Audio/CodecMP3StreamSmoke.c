#define AUDIO_CODEC_EXPERIMENTAL_NATIVE_MP3 1
#include "../../Cosmeron/Modules/Audio/CodecMinimal.h"
#include <assert.h>
#include <limits.h>
#include <string.h>
#include <stdio.h>

static OPSTATUS readCallback(void *data,void *bytes,size_t capacity,size_t *got){
  FILE *file=(FILE*)data;
  if(capacity>3)capacity=3;
  *got=fread(bytes,1,capacity,file);
  return ferror(file)?STATUS_CONST(GENERIC_ERROR):STATUS_CONST(SUCCESS);
}
static OPSTATUS seekCallback(void *data,int64_t offset,
                            AUDIO_CODEC_TYPE(TSeekOrigin) origin){
  int how=origin==AUDIO_CODEC_CONST(SEEK_START)?SEEK_SET:
          origin==AUDIO_CODEC_CONST(SEEK_CURRENT)?SEEK_CUR:SEEK_END;
  if(offset>LONG_MAX || offset<LONG_MIN)return STATUS_CONST(OUT_OF_RANGE);
  return fseek((FILE*)data,(long)offset,how)==0?
      STATUS_CONST(SUCCESS):STATUS_CONST(GENERIC_ERROR);
}
int main(void){
  const char *path="mp3_native_reader_smoke.tmp";
  unsigned char data[3*417]={0};
  AUDIO_CODEC_TYPE(TDecoder) decoder={0};
  AUDIO_CODEC_TYPE(TInfo) info={0};
  AUDIO_CODEC_TYPE(TReader) reader;
  float samples[8]={0};
  FILE *file;
  size_t i,got=999;
  for(i=0;i<sizeof data;i+=417){
    data[i]=0xff;data[i+1]=0xfb;data[i+2]=0x90;data[i+3]=0xc4;
  }
  file=fopen(path,"wb");
  assert(file);
  assert(fwrite(data,1,sizeof data,file)==sizeof data);
  assert(fclose(file)==0);
  assert(AUDIO_CODEC_FUNC(OpenFile)(
      &decoder,path)==STATUS_CONST(SUCCESS));
  assert(AUDIO_CODEC_FUNC(GetInfo)(
      &decoder,&info)==STATUS_CONST(SUCCESS));
  assert(info.lengthKnown && info.frameCount==3456 &&
      info.sampleRate==44100 && info.channels==1);
  assert(AUDIO_CODEC_FUNC(ReadFrames)(
      &decoder,samples,8,&got)==STATUS_CONST(SUCCESS) && got==8);
  for(i=0;i<8;++i)assert(samples[i]==0.f);
  assert(AUDIO_CODEC_FUNC(SeekFrame)(
      &decoder,2203)==STATUS_CONST(SUCCESS));
  assert(AUDIO_CODEC_FUNC(ReadFrames)(
      &decoder,samples,8,&got)==STATUS_CONST(SUCCESS) && got==8);
  for(i=0;i<8;++i)assert(samples[i]==0.f);
  AUDIO_CODEC_FUNC(Close)(&decoder);
  file=fopen(path,"rb");
  assert(file);
  reader.userData=file;reader.read=readCallback;reader.seek=seekCallback;
  assert(AUDIO_CODEC_FUNC(OpenReader)(
      &decoder,&reader)==STATUS_CONST(SUCCESS));
  assert(AUDIO_CODEC_FUNC(SeekFrame)(
      &decoder,3456)==STATUS_CONST(SUCCESS));
  assert(AUDIO_CODEC_FUNC(ReadFrames)(
      &decoder,samples,8,&got)==STATUS_CONST(SUCCESS) && !got);
  assert(AUDIO_CODEC_FUNC(SeekFrame)(
      &decoder,0)==STATUS_CONST(SUCCESS));
  assert(AUDIO_CODEC_FUNC(ReadFrames)(
      &decoder,samples,8,&got)==STATUS_CONST(SUCCESS) && got==8);
  AUDIO_CODEC_FUNC(Close)(&decoder);
  assert(fclose(file)==0);
  /* Test an ID3v2 payload deliberately larger than the ten-byte
   * probe buffer, plus the optional fixed 128-byte ID3v1 footer. */
  {
    unsigned char header[18]={'I','D','3',3,0,0,0,0,0,8};
    unsigned char footer[128]={0};
    memcpy(footer,"TAG",3);
    file=fopen(path,"wb");
    assert(file);
    assert(fwrite(header,1,sizeof header,file)==sizeof header);
    assert(fwrite(data,1,sizeof data,file)==sizeof data);
    assert(fwrite(footer,1,sizeof footer,file)==sizeof footer);
    assert(fclose(file)==0);
    assert(AUDIO_CODEC_FUNC(OpenFile)(
        &decoder,path)==STATUS_CONST(SUCCESS));
    assert(AUDIO_CODEC_FUNC(GetInfo)(
        &decoder,&info)==STATUS_CONST(SUCCESS));
    assert(info.frameCount==3456);
    AUDIO_CODEC_FUNC(Close)(&decoder);
    file=fopen(path,"rb");
    assert(file);
    reader.userData=file;
    assert(AUDIO_CODEC_FUNC(OpenReader)(
        &decoder,&reader)==STATUS_CONST(SUCCESS));
    AUDIO_CODEC_FUNC(Close)(&decoder);
    assert(fclose(file)==0);
    /* Truncated footer must never be mistaken for a shorter track. */
    file=fopen(path,"wb");
    assert(file);
    assert(fwrite(header,1,sizeof header,file)==sizeof header);
    assert(fwrite(data,1,sizeof data,file)==sizeof data);
    assert(fwrite(footer,1,sizeof footer-1,file)==sizeof footer-1);
    assert(fclose(file)==0);
    assert(AUDIO_CODEC_FUNC(OpenFile)(
        &decoder,path)!=STATUS_CONST(SUCCESS));
    assert(decoder.internal==NULL);
  }
  assert(remove(path)==0);
  return 0;
}
