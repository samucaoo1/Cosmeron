#include "../../Cosmeron/Modules/Audio/CodecNative.h"
#include <assert.h>
#include <stdio.h>
int main(int argc, char **argv) {
  const unsigned char expected[16]={0x90,0x01,0x50,0x98,0x3c,0xd2,0x4f,0xb0,0xd6,0x96,0x3f,0x7d,0x28,0xe1,0x7f,0x72};
  AUDIO_INS(TFLACMD5) state;
  unsigned char digest[16], *bytes;
  FILE *f; long length;
  AUDIO_INS(FLAC_MD5Init)(&state);
  AUDIO_INS(FLAC_MD5Update)(&state,(const unsigned char *)"a",1);
  AUDIO_INS(FLAC_MD5Update)(&state,(const unsigned char *)"bc",2);
  AUDIO_INS(FLAC_MD5Final)(&state,digest); assert(!memcmp(digest,expected,16));
  f=fopen(argc==2 ? argv[1] : "Fixtures/Tone.flac","rb"); assert(f);
  assert(!fseek(f,0,SEEK_END)); length=ftell(f); assert(length>42);
  assert(!fseek(f,0,SEEK_SET)); bytes=(unsigned char *)malloc((size_t)length); assert(bytes);
  assert(fread(bytes,1,(size_t)length,f)==(size_t)length); fclose(f);
  assert(AUDIO_CODEC_FUNC(FLAC_VerifyMemory)(bytes,(size_t)length)==STATUS_CONST(SUCCESS));
  {
    AUDIO_CODEC_TYPE(TDecoder) memory={0}, file={0};
    AUDIO_CODEC_TYPE(TInfo) info;
    AUDIO_PCM_TYPE(TBuffer) expectedPCM={0};
    float chunk[64*8]; size_t got=0;
    assert(AUDIO_CODEC_FUNC(FLAC_DecodeMemory)(bytes,(size_t)length,&expectedPCM)==STATUS_CONST(SUCCESS));
    assert(AUDIO_CODEC_FUNC(OpenMemory)(&memory,bytes,(size_t)length)==STATUS_CONST(SUCCESS));
    assert(AUDIO_CODEC_FUNC(OpenFile)(&file,argc==2 ? argv[1] : "Fixtures/Tone.flac")==STATUS_CONST(SUCCESS));
    assert(AUDIO_CODEC_FUNC(GetInfo)(&memory,&info)==STATUS_CONST(SUCCESS));
    assert(AUDIO_CODEC_FUNC(ReadFrames)(&memory,chunk,7,&got)==STATUS_CONST(SUCCESS) && got==7);
    assert(AUDIO_CODEC_FUNC(VerifyIntegrity)(&memory)==STATUS_CONST(SUCCESS));
    assert(AUDIO_CODEC_FUNC(ReadFrames)(&memory,chunk,64,&got)==STATUS_CONST(SUCCESS));
    assert(!memcmp(chunk,expectedPCM.samples+7*info.channels,got*info.channels*sizeof(float)));
    assert(AUDIO_CODEC_FUNC(BuildSeekIndex)(&file)==STATUS_CONST(SUCCESS));
    assert(AUDIO_CODEC_FUNC(SeekFrame)(&file,info.frameCount-12)==STATUS_CONST(SUCCESS));
    assert(AUDIO_CODEC_FUNC(VerifyIntegrity)(&file)==STATUS_CONST(SUCCESS));
    assert(AUDIO_CODEC_FUNC(ReadFrames)(&file,chunk,64,&got)==STATUS_CONST(SUCCESS) && got==12);
    assert(!memcmp(chunk,expectedPCM.samples+(info.frameCount-12)*info.channels,got*info.channels*sizeof(float)));
    AUDIO_CODEC_FUNC(Close)(&memory); AUDIO_CODEC_FUNC(Close)(&file);
    AUDIO_PCM_FUNC(Destroy)(&expectedPCM);
  }
  bytes[26]^=1;
  assert(AUDIO_CODEC_FUNC(FLAC_VerifyMemory)(bytes,(size_t)length)==STATUS_CONST(INVALID_SEQUENCE));
  memset(bytes+26,0,16);
  assert(AUDIO_CODEC_FUNC(FLAC_VerifyMemory)(bytes,(size_t)length)==STATUS_CONST(NOT_AVAILABLE));
  free(bytes); return 0;
}
