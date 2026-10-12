#include "../../Cosmeron/Modules/Audio/Codec/MP3.h"
#include <assert.h>
#include <string.h>

/* Region switching requires the first nonzero Huffman pair to be read
 * from table 1 at precisely the scalefactor-band-derived line. */
static void testBoundary(uint32_t sampleRate, bool mixed, size_t region0) {
  AUDIO_CODEC_TYPE(TMP3SideInfo) side = {0};
  AUDIO_CODEC_TYPE(TMP3Scalefactors) scales = {0};
  AUDIO_INS(TMP3Book) codebook;
  unsigned int linbits = 0;
  int32_t out[576];
  unsigned char bytes[8] = {0};
  size_t count = 0, i;
  unsigned int sym = 2; /* book 1: x=1,y=0 */
  unsigned int length;
  assert(region0 % 2U == 0 && region0 < 576);
  assert(AUDIO_INS(MP3_BookFor)(1, &codebook, &linbits));
  assert(codebook.alphabet == 2 && codebook.lengths[sym] > 0);
  length = codebook.lengths[sym];
  for (i = 0; i < length; ++i)
    if ((codebook.codes[sym] >> (length - i - 1U)) & 1U)
      bytes[i/8] |= (unsigned char)(0x80U >> (i & 7U));
  side.channels = 1;
  side.granules = 1;
  side.parts[0][0].windowSwitching = 1;
  side.parts[0][0].blockType = 2;
  side.parts[0][0].mixedBlock = (uint8_t)mixed;
  side.parts[0][0].tableSelect[0] = 0; /* 0,0 without any bits */
  side.parts[0][0].tableSelect[1] = 1; /* 1,0 with a sign bit */
  side.parts[0][0].bigValues = (uint16_t)(region0 / 2U + 1U);
  side.parts[0][0].part23Length = length + 1U;
  assert(AUDIO_CODEC_FUNC(MP3_HuffmanGranule)(
      bytes, sizeof bytes, &side, &scales, 0, 0, sampleRate,
      out, &count) == STATUS_CONST(SUCCESS));
  assert(count == region0 + 2U);
  for (i = 0; i < region0; ++i) assert(out[i] == 0);
  assert(out[region0] == 1 && out[region0 + 1] == 0);
}
int main(void) {
  testBoundary(44100, false, 198);
  testBoundary(22050, false, 222);
  testBoundary(11025, false, 240);
  testBoundary(8000, false, 480);
  testBoundary(44100, true, 36);
  testBoundary(22050, true, 72);
  testBoundary(8000, true, 156);
  return 0;
}
