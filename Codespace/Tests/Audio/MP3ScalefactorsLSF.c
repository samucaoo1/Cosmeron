#include "../../Cosmeron/Modules/Audio/Codec/MP3.h"
#include <assert.h>
#include <string.h>

static void check(unsigned int compress, unsigned int channels,
    unsigned int channel, unsigned int blockType, unsigned int mixed,
    unsigned int expectedBits, unsigned int firstLong,
    unsigned int firstShort, unsigned int intensityScale) {
  unsigned char data[512];
  AUDIO_CODEC_TYPE(TMP3FrameInfo) header = {0};
  AUDIO_CODEC_TYPE(TMP3SideInfo) side = {0};
  AUDIO_CODEC_TYPE(TMP3Scalefactors) out;
  uint8_t scale = 99;
  unsigned int off = 0, i;
  OPSTATUS status;
  memset(data, 0xff, sizeof data);
  memset(&out, 0x79, sizeof out);
  header.mpegVersion = 2;
  header.channels = (uint8_t)channels;
  header.sampleRate = 22050;
  header.intensityStereo = channels == 2 && channel == 1;
  side.channels = (uint8_t)channels;
  side.granules = 1;
  side.parts[0][channel].scalefacCompress = (uint16_t)compress;
  side.parts[0][channel].windowSwitching = blockType == 2;
  side.parts[0][channel].blockType = (uint8_t)blockType;
  side.parts[0][channel].mixedBlock = (uint8_t)mixed;
  side.parts[0][channel].part23Length = expectedBits;
  if (channel == 1) {
    side.parts[0][0].part23Length = 5;
    off = 5;
  }
  status = AUDIO_CODEC_FUNC(MP3_ParseScalefactorsLSF)(
      data, sizeof data, &header, &side, channel, &out, &scale);
  assert(status == STATUS_CONST(SUCCESS));
  assert(out.part2Bits == expectedBits);
  assert(scale == intensityScale);
  assert(out.longBands[0] == firstLong);
  assert(out.shortBands[mixed ? 3U : 0U][0] == firstShort);
  assert(out.longBands[21] == 0 && out.shortBands[12][0] == 0);
  for (i = 0; i < 22; ++i)
    assert(out.longBands[i] <= 31);
  for (i = 0; i < 13; ++i)
    assert(out.shortBands[i][0] <= 31);
  if (expectedBits > 0) {
    out.longBands[0] = 67;
    scale = 99;
    side.parts[0][channel].part23Length = expectedBits-1;
    status = AUDIO_CODEC_FUNC(MP3_ParseScalefactorsLSF)(
        data, sizeof data, &header, &side, channel, &out, &scale);
    assert(status == STATUS_CONST(INVALID_SEQUENCE));
    assert(out.longBands[0] == 67 && scale == 99);
  }
  /* Reject data that ends before the declared part2_3_length. */
  side.parts[0][channel].part23Length = expectedBits;
  status = AUDIO_CODEC_FUNC(MP3_ParseScalefactorsLSF)(
      data, (off+expectedBits)/8, &header, &side, channel, &out, &scale);
  if (((off+expectedBits)&7U) != 0)
    assert(status == STATUS_CONST(INCOMPLETE_SEQUENCE));
}
int main(void) {
  /* MPEG-2/2.5 non-intensity LSF partition families 0,1,2. */
  check(0,1,0,0,0,0,0,0,0);
  check(399,1,0,0,0,74,15,0,0); /* 6x4 + 5x4 + 5x3 + 5x3 */
  check(400,1,0,0,0,0,0,0,0);
  check(499,1,0,0,0,65,15,0,0); /* 6x4 + 5x4 + 7x3 */
  check(500,1,0,0,0,0,0,0,0);
  check(511,1,0,0,0,53,7,0,0); /* 11x3 + 10x2 */
  check(399,1,0,2,0,126,0,15); /* 9x4 + 9x4 + 9x3 + 9x3 */
  check(399,1,0,2,1,114,15,15); /* 6x4 + 9x4 + 9x3 + 9x3 */

  /* Intensity-stereo right channel halves scalefac_compress, leaving
   * the low bit as intensity_scale. All-ones values are preserved raw. */
  check(1,2,1,0,0,0,0,0,1);
  check(359,2,1,0,0,98,15,0,1); /* 7x4 + 7x5 + 7x5 */
  check(360,2,1,0,0,0,0,0,0);
  check(487,2,1,0,0,54,7,0,1); /* 6x3 + 6x3 + 6x3 */
  check(488,2,1,0,0,0,0,0,0);
  /* MPEG2.5 has identical LSF scalefactor compression tables. */
  {
    AUDIO_CODEC_TYPE(TMP3FrameInfo) hdr = {0};
    AUDIO_CODEC_TYPE(TMP3SideInfo) side = {0};
    AUDIO_CODEC_TYPE(TMP3Scalefactors) scale = {0};
    unsigned char bits[8] = {0};
    uint8_t intensity = 21;
    hdr.mpegVersion = 25; hdr.channels = 1; hdr.sampleRate = 11025;
    side.granules = 1; side.channels = 1;
    assert(AUDIO_CODEC_FUNC(MP3_ParseScalefactorsLSF)(
        bits,sizeof bits,&hdr,&side,0,&scale,&intensity) ==
        STATUS_CONST(SUCCESS));
    assert(scale.part2Bits == 0 && intensity == 0);
  }
  return 0;
}
