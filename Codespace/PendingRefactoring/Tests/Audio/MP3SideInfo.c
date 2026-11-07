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
      mainData[114] == 0x42 && mainData[115] == 0x99 &&
      mainData[511] == 0x55);
  {
    unsigned char stereo[417] = {0xff, 0xfb, 0x90, 0x64};
    size_t bit = 0;
    /* MPEG1 stereo: 9-bit reservoir ref, 3 private, 8 scfsi. */
    putBits(stereo + 4, &bit, 511, 9);
    putBits(stereo + 4, &bit, 5, 3);
    putBits(stereo + 4, &bit, 0xa5, 8);
    /* First granule/channel: short switched block with two Huffman tables. */
    putBits(stereo + 4, &bit, 123, 12);
    putBits(stereo + 4, &bit, 88, 9);
    putBits(stereo + 4, &bit, 190, 8);
    putBits(stereo + 4, &bit, 7, 4);
    putBits(stereo + 4, &bit, 1, 1);
    putBits(stereo + 4, &bit, 2, 2);
    putBits(stereo + 4, &bit, 1, 1);
    putBits(stereo + 4, &bit, 1, 5);
    putBits(stereo + 4, &bit, 2, 5);
    putBits(stereo + 4, &bit, 3, 3);
    putBits(stereo + 4, &bit, 2, 3);
    putBits(stereo + 4, &bit, 1, 3);
    putBits(stereo + 4, &bit, 1, 1);
    putBits(stereo + 4, &bit, 0, 1);
    putBits(stereo + 4, &bit, 1, 1);
    assert(bit == 20 + 59);
    assert(AUDIO_CODEC_FUNC(MP3_ParseSideInfo)(stereo, sizeof stereo, &info) ==
        STATUS_CONST(SUCCESS));
    assert(info.channels == 2 && info.granules == 2 &&
        info.mainDataBegin == 511 && info.privateBits == 5 &&
        info.scfsi[0][0] == 1 && info.scfsi[1][1] == 1 &&
        info.parts[0][0].part23Length == 123 &&
        info.parts[0][0].bigValues == 88 &&
        info.parts[0][0].windowSwitching &&
        info.parts[0][0].blockType == 2 &&
        info.parts[0][0].mixedBlock == 1 &&
        info.parts[0][0].tableSelect[0] == 1 &&
        info.parts[0][0].tableSelect[1] == 2 &&
        info.parts[0][0].subblockGain[0] == 3 &&
        info.parts[0][0].region0Count == 7 &&
        info.parts[0][0].region1Count == 13 &&
        info.parts[0][0].preflag == 1 &&
        info.parts[0][0].count1TableSelect == 1);
    /* window_switching_flag == 1 with block_type == 0 is forbidden. */
    {
      size_t blockBit = 20 + 12 + 9 + 8 + 4 + 1;
      stereo[4 + blockBit / 8] &=
          (unsigned char)~(0x80U >> (blockBit & 7U));
    }
    assert(AUDIO_CODEC_FUNC(MP3_ParseSideInfo)(stereo, sizeof stereo, &info) ==
        STATUS_CONST(INVALID_SEQUENCE));
  }
  {
    unsigned char low[208] = {0xff, 0xf3, 0x80, 0xc4};
    size_t cursor = 0;
    putBits(low + 4, &cursor, 128, 8);
    putBits(low + 4, &cursor, 1, 1);
    putBits(low + 4, &cursor, 100, 12);
    putBits(low + 4, &cursor, 20, 9);
    putBits(low + 4, &cursor, 180, 8);
    putBits(low + 4, &cursor, 300, 9);
    putBits(low + 4, &cursor, 0, 1);
    putBits(low + 4, &cursor, 1, 5);
    putBits(low + 4, &cursor, 2, 5);
    putBits(low + 4, &cursor, 3, 5);
    putBits(low + 4, &cursor, 5, 4);
    putBits(low + 4, &cursor, 2, 3);
    putBits(low + 4, &cursor, 1, 1);
    putBits(low + 4, &cursor, 0, 1);
    assert(cursor == 72);
    assert(AUDIO_CODEC_FUNC(MP3_ParseSideInfo)(low, sizeof low, &info) ==
        STATUS_CONST(SUCCESS));
    assert(info.granules == 1 && info.channels == 1 &&
        info.mainDataBegin == 128 &&
        info.parts[0][0].scalefacCompress == 300 &&
        info.parts[0][0].tableSelect[2] == 3 &&
        info.parts[0][0].preflag == 0);
  }
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
