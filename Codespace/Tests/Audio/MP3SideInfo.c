#include "../../Cosmeron/Modules/Audio/Codec/MP3.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

static void putBits(unsigned char *data, size_t *cursor,
                    unsigned int value, unsigned int width) {
  unsigned int bit;
  for (bit = width; bit; --bit) {
    size_t at = (*cursor)++;
    if ((value >> (bit - 1)) & 1U)
      data[at >> 3] |= (unsigned char)(0x80U >> (at & 7U));
  }
}
static void makeMono(unsigned char *frame, unsigned int mainDataBegin,
                     unsigned char pattern) {
  size_t cursor = 0, n;
  /* MPEG-1, 128 kbps, 44100 Hz, mono, unprotected: 417-byte frame. */
  memset(frame, 0, 417);
  frame[0] = 0xff; frame[1] = 0xfb; frame[2] = 0x90; frame[3] = 0xc4;
  putBits(frame + 4, &cursor, mainDataBegin, 9);
  putBits(frame + 4, &cursor, 0, 5); /* private bits */
  putBits(frame + 4, &cursor, 0, 4); /* scfsi */
  putBits(frame + 4, &cursor, 0, 12); /* gr 0: part2_3 */
  putBits(frame + 4, &cursor, 0, 9); /* big values */
  putBits(frame + 4, &cursor, 210, 8); /* global gain */
  putBits(frame + 4, &cursor, 0, 4); /* scalefac compress */
  putBits(frame + 4, &cursor, 0, 1); /* no switched window */
  putBits(frame + 4, &cursor, 0, 15); /* Huffman tables */
  putBits(frame + 4, &cursor, 0, 4); /* region 0 */
  putBits(frame + 4, &cursor, 0, 3); /* region 1 */
  putBits(frame + 4, &cursor, 0, 3); /* flags */
  putBits(frame + 4, &cursor, 0, 12); /* gr 1: part2_3 */
  putBits(frame + 4, &cursor, 0, 9);
  putBits(frame + 4, &cursor, 211, 8);
  putBits(frame + 4, &cursor, 0, 4);
  putBits(frame + 4, &cursor, 0, 1);
  putBits(frame + 4, &cursor, 0, 15);
  putBits(frame + 4, &cursor, 0, 4);
  putBits(frame + 4, &cursor, 0, 3);
  putBits(frame + 4, &cursor, 0, 3);
  assert(cursor == 17 * 8);
  for (n = 21; n < 417; ++n) frame[n] = pattern;
}
int main(void) {
  AUDIO_CODEC_TYPE(TMP3SideInfo) info = {0};
  AUDIO_CODEC_TYPE(TMP3Reservoir) reservoir = {0};
  AUDIO_CODEC_TYPE(TMP3FrameInfo) header;
  unsigned char a[417], b[417], c[417];
  const unsigned char *mainData = NULL;
  size_t mainSize = 0;
  makeMono(a, 0, 0x42);
  makeMono(b, 12, 0x99);
  makeMono(c, 511, 0x55);
  assert(AUDIO_CODEC_FUNC(MP3_ParseSideInfo)(a, sizeof a, &info) ==
      STATUS_CONST(SUCCESS));
  assert(info.channels == 1 && info.granules == 2 &&
      info.mainDataBytes == 396 && !info.mainDataBegin &&
      info.parts[0][0].globalGain == 210 &&
      info.parts[1][0].globalGain == 211);
  assert(AUDIO_CODEC_FUNC(MP3_ReservoirFeed)(&reservoir, a, sizeof a,
      &mainData, &mainSize, &info) == STATUS_CONST(SUCCESS));
  assert(mainSize == 396 && mainData[0] == 0x42 &&
      reservoir.historySize == 396);
  assert(AUDIO_CODEC_FUNC(MP3_ReservoirFeed)(&reservoir, b, sizeof b,
      &mainData, &mainSize, &info) == STATUS_CONST(SUCCESS));
  assert(mainSize == 408 && info.mainDataBegin == 12);
  assert(mainData[0] == 0x42 && mainData[11] == 0x42 &&
      mainData[12] == 0x99 && reservoir.historySize == 511);
  assert(AUDIO_CODEC_FUNC(MP3_ReservoirFeed)(&reservoir, c, sizeof c,
      &mainData, &mainSize, &info) == STATUS_CONST(SUCCESS));
  assert(mainSize == 907 && mainData[0] == 0x42 &&
      mainData[115] == 0x42 && mainData[116] == 0x99 &&
      mainData[511] == 0x55);
  memset(&reservoir, 0, sizeof reservoir);
  assert(AUDIO_CODEC_FUNC(MP3_ReservoirFeed)(&reservoir, b, sizeof b,
      &mainData, &mainSize, &info) == STATUS_CONST(INCOMPLETE_SEQUENCE));
  assert(mainData == NULL && mainSize == 0 && reservoir.historySize == 396);
  assert(AUDIO_CODEC_FUNC(MP3_ReservoirFeed)(&reservoir, b, 200,
      &mainData, &mainSize, &info) == STATUS_CONST(INCOMPLETE_SEQUENCE));
  assert(AUDIO_CODEC_FUNC(MP3_ParseSideInfo)(a, 19, &info) ==
      STATUS_CONST(INCOMPLETE_SEQUENCE));
  /* Low-sampling-frequency mono has one granule, 9 bytes side information. */
  {
    unsigned char low[208] = {0xff,0xf3,0x80,0xc4};
    assert(AUDIO_CODEC_FUNC(MP3_ParseSideInfo)(low, sizeof low, &info) ==
        STATUS_CONST(SUCCESS));
    assert(info.channels == 1 && info.granules == 1 &&
        info.mainDataBytes == 195);
  }
  {
    FILE *file = fopen("Fixtures/Tone.mp3", "rb");
    unsigned char buffer[65536];
    size_t length, offset = 0, frames = 0, valid = 0;
    assert(file);
    length = fread(buffer, 1, sizeof buffer, file);
    assert(!fclose(file));
    assert(length > 1000);
    assert(AUDIO_CODEC_FUNC(MP3_FindFrame)(buffer, length,
        0, &offset, &header) == STATUS_CONST(SUCCESS));
    memset(&reservoir, 0, sizeof reservoir);
    while (length - offset >= 4 && frames < 100) {
      OPSTATUS status = AUDIO_CODEC_FUNC(MP3_ParseHeader)(
          buffer + offset, length - offset, &header);
      if (status != STATUS_CONST(SUCCESS) ||
          header.frameBytes > length - offset) break;
      status = AUDIO_CODEC_FUNC(MP3_ReservoirFeed)(&reservoir,
          buffer + offset, header.frameBytes, &mainData, &mainSize, &info);
      assert(status == STATUS_CONST(SUCCESS) ||
             status == STATUS_CONST(INCOMPLETE_SEQUENCE));
      assert(info.channels == header.channels &&
          info.granules == (header.mpegVersion == 1 ? 2 : 1));
      if (status == STATUS_CONST(SUCCESS)) {
        assert(mainSize >= info.mainDataBegin);
        ++valid;
      }
      offset += header.frameBytes;
      ++frames;
    }
    assert(frames >= 3 && valid >= 1);
  }
  return 0;
}
