#include "../../Cosmeron/Modules/Audio/Codec/MP3.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

int main(void) {
  AUDIO_CODEC_TYPE(TMP3FrameInfo) info = {0};
  const unsigned char mpeg1[4] = {0xff,0xfb,0x90,0x64};
  const unsigned char mpeg2[4] = {0xff,0xf3,0x80,0xc4};
  const unsigned char mpeg25[4] = {0xff,0xe3,0x20,0xc4};
  unsigned char tagged[850] = {0};
  size_t offset = 0;
  assert(AUDIO_CODEC_FUNC(MP3_ParseHeader)(mpeg1, 4, &info) ==
      STATUS_CONST(SUCCESS));
  assert(info.sampleRate == 44100 && info.bitRate == 128000 &&
      info.channels == 2 && info.mpegVersion == 1 &&
      info.frameBytes == 417 && info.samplesPerFrame == 1152 &&
      info.sideInfoBytes == 32 && !info.crcProtected);
  assert(AUDIO_CODEC_FUNC(MP3_ParseHeader)(mpeg2, 4, &info) ==
      STATUS_CONST(SUCCESS));
  assert(info.sampleRate == 22050 && info.bitRate == 64000 &&
      info.channels == 1 && info.mpegVersion == 2 &&
      info.frameBytes == 208 && info.samplesPerFrame == 576 &&
      info.sideInfoBytes == 9);
  assert(AUDIO_CODEC_FUNC(MP3_ParseHeader)(mpeg25, 4, &info) ==
      STATUS_CONST(SUCCESS));
  assert(info.sampleRate == 11025 && info.bitRate == 16000 &&
      info.channels == 1 && info.mpegVersion == 25 &&
      info.frameBytes == 104);
  {
    const unsigned char protectedFrame[4] = {0xff,0xfa,0x90,0x64};
    const unsigned char reservedVersion[4] = {0xff,0xeb,0x90,0x64};
    const unsigned char reservedRate[4] = {0xff,0xfb,0x9c,0x64};
    const unsigned char reservedBitrate[4] = {0xff,0xfb,0xf0,0x64};
    assert(AUDIO_CODEC_FUNC(MP3_ParseHeader)(protectedFrame, 4, &info) ==
        STATUS_CONST(SUCCESS) && info.crcProtected);
    assert(AUDIO_CODEC_FUNC(MP3_ParseHeader)(reservedVersion, 4, &info) !=
        STATUS_CONST(SUCCESS));
    assert(AUDIO_CODEC_FUNC(MP3_ParseHeader)(reservedRate, 4, &info) ==
        STATUS_CONST(INVALID_SEQUENCE));
    assert(AUDIO_CODEC_FUNC(MP3_ParseHeader)(reservedBitrate, 4, &info) ==
        STATUS_CONST(INVALID_SEQUENCE));
    assert(AUDIO_CODEC_FUNC(MP3_ParseHeader)(mpeg1, 3, &info) ==
        STATUS_CONST(INCOMPLETE_SEQUENCE));
  }
  memcpy(tagged, "ID3", 3);
  tagged[3] = 4;
  tagged[9] = 4;
  memcpy(tagged + 14, mpeg1, 4);
  memcpy(tagged + 431, mpeg1, 4);
  assert(AUDIO_CODEC_FUNC(MP3_SkipID3)(tagged, sizeof tagged, &offset) ==
      STATUS_CONST(SUCCESS) && offset == 14);
  assert(AUDIO_CODEC_FUNC(MP3_FindFrame)(tagged, sizeof tagged,
      0, &offset, &info) == STATUS_CONST(SUCCESS));
  assert(offset == 14 && info.frameBytes == 417);
  tagged[9] = 0x80;
  assert(AUDIO_CODEC_FUNC(MP3_SkipID3)(tagged, sizeof tagged, &offset) ==
      STATUS_CONST(INVALID_SEQUENCE));
  {
    FILE *file = fopen("Fixtures/Tone.mp3", "rb");
    unsigned char data[65536];
    size_t size;
    assert(file);
    size = fread(data, 1, sizeof data, file);
    assert(!fclose(file));
    assert(size > 1000);
    assert(AUDIO_CODEC_FUNC(MP3_FindFrame)(data, size, 0, &offset, &info) ==
        STATUS_CONST(SUCCESS));
    assert(info.sampleRate == 44100 && info.channels == 2 &&
        info.frameBytes >= 24 && offset < size);
  }
  return 0;
}
