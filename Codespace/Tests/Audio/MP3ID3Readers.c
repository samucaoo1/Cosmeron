#define AUDIO_CODEC_EXPERIMENTAL_NATIVE_MP3 1
#include "../../Cosmeron/Modules/Audio/CodecMinimal.h"
#include <assert.h>
#include <limits.h>
#include <stdint.h>
#include <string.h>

typedef struct ID3Reader {
  const unsigned char *data;
  size_t size, position;
} ID3Reader;

static OPSTATUS readBytes(void *user, void *destination, size_t capacity,
                          size_t *outBytes) {
  ID3Reader *r = (ID3Reader *)user;
  size_t n;
  if (r->position > r->size) return STATUS_CONST(OUT_OF_RANGE);
  n = r->size - r->position;
  if (n > capacity) n = capacity;
  if (n > 3) n = 3; /* force arbitrary short reads */
  if (n) memcpy(destination, r->data + r->position, n);
  r->position += n;
  *outBytes = n;
  return STATUS_CONST(SUCCESS);
}

static OPSTATUS seekBytes(void *user, int64_t offset,
                          AUDIO_CODEC_TYPE(TSeekOrigin) origin) {
  ID3Reader *r = (ID3Reader *)user;
  size_t base;
  if (origin == AUDIO_CODEC_CONST(SEEK_START)) base = 0;
  else if (origin == AUDIO_CODEC_CONST(SEEK_CURRENT)) base = r->position;
  else if (origin == AUDIO_CODEC_CONST(SEEK_END)) base = r->size;
  else return STATUS_CONST(INVALID_ARGUMENT);
  if (offset < 0) {
    uint64_t magnitude = (uint64_t)(-(offset + 1)) + 1U;
    if (magnitude > base) return STATUS_CONST(OUT_OF_RANGE);
    r->position = base - (size_t)magnitude;
  } else {
    if ((uint64_t)offset > SIZE_MAX - base) return STATUS_CONST(OUT_OF_RANGE);
    r->position = base + (size_t)offset;
  }
  return STATUS_CONST(SUCCESS);
}

static void checkOne(unsigned int version, unsigned int flags, int withFooter,
                     int withV1) {
  unsigned char buffer[10 + 6 + 10 + 3*417 + 128] = {0};
  size_t start = 16U + (withFooter ? 10U : 0U);
  size_t end = start + 3U * 417U + (withV1 ? 128U : 0U);
  AUDIO_CODEC_TYPE(TDecoder) stream = {0};
  AUDIO_CODEC_TYPE(TDecoder) memory = {0};
  AUDIO_CODEC_TYPE(TInfo) info = {0};
  AUDIO_CODEC_TYPE(TReader) reader;
  ID3Reader source = {buffer, end, 0};
  float pcm[19] = {0};
  size_t got = 0, i;
  memcpy(buffer,"ID3",3);
  buffer[3] = (unsigned char)version;
  buffer[5] = (unsigned char)flags;
  buffer[9] = 6;
  if (withFooter) {
    memcpy(buffer+16, "3DI",3);
    memcpy(buffer+19, buffer+3,7);
  }
  for (i = start; i < start + 3U*417U; i += 417) {
    buffer[i] = 0xff;buffer[i+1] = 0xfb;
    buffer[i+2] = 0x90;buffer[i+3] = 0xc4;
  }
  if (withV1) memcpy(buffer+start+3U*417U,"TAG",3);
  assert(AUDIO_CODEC_FUNC(MP3_SkipID3)(buffer, end, &got) ==
      STATUS_CONST(SUCCESS) && got == start);
  assert(AUDIO_CODEC_FUNC(OpenMemory)(&memory,buffer,end) ==
      STATUS_CONST(SUCCESS));
  reader.userData = &source;
  reader.read = readBytes;
  reader.seek = seekBytes;
  assert(AUDIO_CODEC_FUNC(OpenReader)(&stream,&reader) ==
      STATUS_CONST(SUCCESS));
  assert(AUDIO_CODEC_FUNC(GetInfo)(&stream,&info) ==
      STATUS_CONST(SUCCESS));
  assert(info.sampleRate==44100 && info.channels==1 &&
      info.lengthKnown && info.frameCount == 3456);
  assert(AUDIO_CODEC_FUNC(ReadFrames)(&stream,pcm,19,&got) ==
      STATUS_CONST(SUCCESS) && got == 19);
  for(i=0;i<got;++i)assert(pcm[i]==0);
  assert(AUDIO_CODEC_FUNC(SeekFrame)(&stream,1900) ==
      STATUS_CONST(SUCCESS));
  assert(AUDIO_CODEC_FUNC(ReadFrames)(&stream,pcm,19,&got) ==
      STATUS_CONST(SUCCESS) && got == 19);
  for(i=0;i<got;++i)assert(pcm[i]==0);
  AUDIO_CODEC_FUNC(Close)(&stream);
  AUDIO_CODEC_FUNC(Close)(&memory);
}

static void checkInvalid(unsigned int version, unsigned int flags,
                         int footerChange, int shortTag) {
  unsigned char buf[10 + 6 + 10 + 3*417] = {0};
  AUDIO_CODEC_TYPE(TDecoder) readerHandle = {0};
  AUDIO_CODEC_TYPE(TDecoder) memoryHandle = {0};
  AUDIO_CODEC_TYPE(TReader) reader;
  ID3Reader source;
  size_t start = 16U+(version==4 && (flags&16U) ? 10U : 0U);
  size_t len = start+3U*417U;
  size_t i;
  memcpy(buf,"ID3",3);
  buf[3]=(unsigned char)version;
  buf[5]=(unsigned char)flags;
  buf[9] = shortTag ? 0x7f : 6;
  if(version==4&&(flags&16U)){
    memcpy(buf+16,"3DI",3);
    memcpy(buf+19,buf+3,7);
    if(footerChange)buf[16]='X';
  }
  for(i=start;i<start+3U*417U;i+=417){
    buf[i]=0xff;buf[i+1]=0xfb;buf[i+2]=0x90;buf[i+3]=0xc4;
  }
  source.data=buf;source.size=len;source.position=0;
  reader.userData=&source;reader.read=readBytes;reader.seek=seekBytes;
  assert(AUDIO_CODEC_FUNC(OpenMemory)(&memoryHandle,buf,len) !=
      STATUS_CONST(SUCCESS));
  assert(AUDIO_CODEC_FUNC(OpenReader)(&readerHandle,&reader) !=
      STATUS_CONST(SUCCESS));
  assert(memoryHandle.internal==NULL && readerHandle.internal==NULL);
}

int main(void) {
  checkOne(2,0x40,0,0);
  checkOne(3,0x20,0,1);
  checkOne(4,0x10,1,1);
  checkOne(4,0,0,0);
  checkInvalid(2,0x01,0,0);
  checkInvalid(3,0x10,0,0);
  checkInvalid(4,0x01,0,0);
  checkInvalid(4,0x10,1,0);
  checkInvalid(4,0x10,0,1);
  return 0;
}
